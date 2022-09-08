#include "tz/core/assert.hpp"
namespace game
{
	template<BoxedThing T>
	Quadtree<T>::Quadtree(Box global_boundary):
	global_boundary(global_boundary),
	root(){}

	template<BoxedThing T>
	void Quadtree<T>::add(const T& value)
	{
		node_add_value(&this->root, 0, this->global_boundary, value);
	}

	template<BoxedThing T>
	void Quadtree<T>::remove(const T& value)
	{
		node_remove(&this->root, this->global_boundary, value);
	}

	template<BoxedThing T>
	void Quadtree<T>::clear()
	{
		for(auto& child_ptr : this->root.children)
		{
			child_ptr = nullptr;
		}
		this->root.values.clear();
	}

	template<BoxedThing T>
	std::vector<std::pair<T, T>> Quadtree<T>::find_all_intersections() const
	{
		std::vector<std::pair<T, T>> ret;
		node_find_intersections(&this->root, ret);
		return ret;
	}

	template<BoxedThing T>
	void Quadtree<T>::node_add_value(Node* node, std::size_t depth, const Box& box, const T& value)
	{
		tz_assert(node != nullptr, "Passed nullptr to node_add_value");
		tz_assert(box.contains(value.get_box()), "Passed box which cannot contain value's box boundary. Depth = %zu", depth);
		if(node->is_leaf())
		{
			// node: should the first clause be inversed?
			if(depth >= Quadtree<T>::max_depth || node->values.size() < Quadtree<T>::threshold)
			{
				node->values.push_back(value);
			}
			else
			{
				// On failure, split and try again.
				node_split(node, box);
				node_add_value(node, depth, box, value);
			}
		}
		else
		{
			// Find a child node which completely contains the value, and add it there.
			auto i = detail::QuadtreeHelper::find_quadrant_id(box, value.get_box());
			if(i.has_value())
			{
				node_add_value(node->children[i.value()].get(), depth + 1, detail::QuadtreeHelper::compute_quadrant_box(box, i.value()), value);
			}
			else
			{
				node->values.push_back(value);
			}
		}
	}

	template<BoxedThing T>
	void Quadtree<T>::node_split(Node* node, const Box& box)
	{
		tz_assert(node != nullptr, "Attempted to split a nullptr node.");
		tz_assert(node->is_leaf(), "Only leaf-nodes can be split");
		// Create children.
		for(auto& node_ptr : node->children)
		{
			node_ptr = std::make_unique<Node>();
		}
		std::vector<T> new_vals;
		for(const T& value : node->values)
		{
			auto i = detail::QuadtreeHelper::find_quadrant_id(box, value.get_box());
			if(i.has_value())
			{
				node->children[i.value()]->values.push_back(value);
			}
			else
			{
				new_vals.push_back(value);
			}
		}
		node->values = std::move(new_vals);
	}

	template<BoxedThing T>
	bool Quadtree<T>::node_remove(Node* node, const Box& box, const T& value)
	{
		tz_assert(node != nullptr, "Cannot remove node that's nullptr");
		tz_assert(box.contains(value.get_box()), "Cannot remove node that doesn't contain the value's bounding box");
		if(node->is_leaf())
		{
			remove_value(node, value);
			return true;
		}
		else
		{
			auto id = detail::QuadtreeHelper::find_quadrant_id(box, value.get_box());
			if(id.has_value())
			{
				if(node_remove(node->children[id.value()].get(), detail::QuadtreeHelper::compute_quadrant_box(box, id.value()), value))
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

	template<BoxedThing T>
	void Quadtree<T>::remove_value(Node* node, const T& value)
	{
		auto it = std::find(node->values.begin(), node->values.end(), value);
		tz_assert(it != node->values.end(), "Cannot remove a value from a node if it doesn't already exist in the node.");
		*it = std::move(node->values.back());
		node->values.pop_back();
	}

	template<BoxedThing T>
	bool Quadtree<T>::try_merge(Node* node)
	{
		tz_assert(node != nullptr, "Cannot merge a nullptr node");
		tz_assert(!node->is_leaf(), "Leaf nodes cannot be merged.");
		std::size_t value_count = node->values.size();
		for(const auto& child_ptr : node->children)
		{
			if(!child_ptr->is_leaf())
			{
				return false;
			}
			value_count += child_ptr->values.size();
		}
		if(value_count <= Quadtree<T>::threshold)
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
	
	template<BoxedThing T>
	void Quadtree<T>::node_find_intersections(const Node* node, std::vector<std::pair<T, T>>& intersections) const
	{
		for(std::size_t i = 0; i < node->values.size(); i++)
		{
			for(std::size_t j = 0; j < i; j++)
			{
				if(node->values[i].get_box().intersects(node->values[j].get_box()))
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

	template<BoxedThing T>
	void Quadtree<T>::node_find_descendent_intersections(const Node* node, const T& value, std::vector<std::pair<T, T>>& intersections) const
	{
		for(const T& other : node->values)
		{
			if(value.get_box().intersects(other.get_box()))
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
