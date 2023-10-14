#include "tz/core/debug.hpp"
#include <deque>

namespace game::physics
{
	template<boxed_t T>
	quadtree<T>::quadtree(aabb global_boundary):
	global_boundary(global_boundary),
	root(){}

	template<boxed_t T>
	void quadtree<T>::add(const T& value)
	{
		node_add_value(&this->root, 0, this->global_boundary, value);
	}

	template<boxed_t T>
	void quadtree<T>::remove(const T& value)
	{
		node_remove(&this->root, this->global_boundary, value);
	}

	template<boxed_t T>
	void quadtree<T>::clear()
	{
		this->root = {};
	}

	template<boxed_t T>
	std::vector<std::pair<T, T>> quadtree<T>::find_all_intersections() const
	{
		std::vector<std::pair<T, T>> ret;
		node_find_intersections(&this->root, ret);
		return ret;
	}

	template<boxed_t T>
	void quadtree<T>::dbgui()
	{
		ImGui::CheckBox("Debug Draw", &this->debug_draw_enabled);
		static tz::vec2 position = tz::vec2::filled(0.0f), size = tz::vec2::filled(10.0f);
		if(ImGui::InputFloat2("Position", position.data().data()) || ImGui::InputFloat2("Scale", size.data().data()))
		{
			this->global_boundary = {position - (size * 0.5f), position + (size * 0.5f)};
			this->clear();
		}
	}

	template<boxed_t T>
	void quadtree<T>::node_add_value(node* node, std::size_t depth, const aabb& aabb, const T& value)
	{
		tz::assert(node != nullptr, "Passed nullptr to node_add_value");
		if(!aabb.contains(value.get_aabb()))
		{
			// tried to add something that can't be added.
			// crashing here is super extreme, just means that intersections cannot be queried for it.
			// ideally should warn, but in our use case this warn is super spammy. so errr.... looking for a bug with intersection sometimes not working? put an assert here!
			// TODO: write safe code
			tz::report("Warning: Quadtree node out of bounds!");
			return;
		}
		if(node->is_leaf())
		{
			// node: should the first clause be inversed?
			if(depth >= quadtree<T>::max_depth || node->values.size() < quadtree<T>::threshold)
			{
				node->values.push_back(value);
			}
			else
			{
				// On failure, split and try again.
				node_split(node, aabb);
				node_add_value(node, depth, aabb, value);
			}
		}
		else
		{
			// Find a child node which completely contains the value, and add it there.
			auto i = detail::quadtree_helper::find_quadrant_id(aabb, value.get_aabb());
			if(i.has_value())
			{
				node_add_value(node->children[i.value()].get(), depth + 1, detail::quadtree_helper::compute_quadrant_aabb(aabb, i.value()), value);
			}
			else
			{
				node->values.push_back(value);
			}
		}
	}

	template<boxed_t T>
	void quadtree<T>::node_split(node* my_node, const aabb& aabb)
	{
		tz::assert(my_node != nullptr, "Attempted to split a nullptr node.");
		tz::assert(my_node->is_leaf(), "Only leaf-nodes can be split");
		// Create children.
		for(auto& node_ptr : my_node->children)
		{
			node_ptr = std::make_unique<node>();
		}
		std::vector<T> new_vals;
		for(const T& value : my_node->values)
		{
			auto i = detail::quadtree_helper::find_quadrant_id(aabb, value.get_aabb());
			if(i.has_value())
			{
				my_node->children[i.value()]->values.push_back(value);
			}
			else
			{
				new_vals.push_back(value);
			}
		}
		my_node->values = std::move(new_vals);
	}

	template<boxed_t T>
	bool quadtree<T>::node_remove(node* node, const aabb& aabb, const T& value)
	{
		tz::assert(node != nullptr, "Cannot remove node that's nullptr");
		tz::assert(aabb.contains(value.get_aabb()), "Cannot remove node that doesn't contain the value's bounding aabb");
		if(node->is_leaf())
		{
			remove_value(node, value);
			return true;
		}
		else
		{
			auto id = detail::quadtree_helper::find_quadrant_id(aabb, value.get_aabb());
			if(id.has_value())
			{
				if(node_remove(node->children[id.value()].get(), detail::quadtree_helper::compute_quadrant_aabb(aabb, id.value()), value))
				{
					return try_merge(node);
				}
			}
			else
			{
				remove_value(node, value);
			}
			return false;
		}
	}

	template<boxed_t T>
	void quadtree<T>::remove_value(node* node, const T& value)
	{
		auto it = std::find(node->values.begin(), node->values.end(), value);
		tz::assert(it != node->values.end(), "Cannot remove a value from a node if it doesn't already exist in the node.");
		*it = std::move(node->values.back());
		node->values.pop_back();
	}

	template<boxed_t T>
	bool quadtree<T>::try_merge(node* node)
	{
		tz::assert(node != nullptr, "Cannot merge a nullptr node");
		tz::assert(!node->is_leaf(), "Leaf nodes cannot be merged.");
		std::size_t value_count = node->values.size();
		for(const auto& child_ptr : node->children)
		{
			if(!child_ptr->is_leaf())
			{
				return false;
			}
			value_count += child_ptr->values.size();
		}
		if(value_count <= quadtree<T>::threshold)
		{
			node->values.reserve(value_count);
			for(const auto& child_ptr : node->children)
			{
				for(const T& val : child_ptr->values)
				{
					node->values.push_back(val);
				}
			}
			// Remove children.
			for(auto& child_ptr : node->children)
			{
				child_ptr = nullptr;
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	
	template<boxed_t T>
	void quadtree<T>::node_find_intersections(const node* node, std::vector<std::pair<T, T>>& intersections) const
	{
		for(std::size_t i = 0; i < node->values.size(); i++)
		{
			for(std::size_t j = 0; j < i; j++)
			{
				if(node->values[i].get_aabb().intersects(node->values[j].get_aabb()))
				{
					intersections.emplace_back(node->values[i], node->values[j]);
				}
			}
		}
		if(!node->is_leaf())
		{
			// Values in this node can intersects descendents.
			for(const auto& child_ptr : node->children)
			{
				for(const T& value : node->values)
				{
					node_find_descendent_intersections(child_ptr.get(), value, intersections);
				}
			}
			// Find intersections in children
			for(const auto& child_ptr : node->children)
			{
				node_find_intersections(child_ptr.get(), intersections);
			}
		}
	}

	template<boxed_t T>
	void quadtree<T>::node_find_descendent_intersections(const node* node, const T& value, std::vector<std::pair<T, T>>& intersections) const
	{
		for(const T& other : node->values)
		{
			if(value.get_aabb().intersects(other.get_aabb()))
			{
				intersections.emplace_back(value, other);
			}
		}
		if(!node->is_leaf())
		{
			for(const auto& child_ptr : node->children)
			{
				node_find_descendent_intersections(child_ptr.get(), value, intersections);
			}
		}
	}
}