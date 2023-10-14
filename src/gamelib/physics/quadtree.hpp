#ifndef RN_GAMELIB_PHYSICS_QUADTREE_HPP
#define RN_GAMELIB_PHYSICS_QUADTREE_HPP
#include "gamelib/physics/aabb.hpp"
#include <array>
#include <memory>
#include <vector>
#include <optional>

namespace game::physics
{
	namespace detail
	{
		struct quadtree_helper
		{
			static aabb compute_quadrant_aabb(const aabb& parent_aabb, std::size_t quadrant_id);
			static std::optional<std::size_t> find_quadrant_id(const aabb& node_aabb, const aabb& value_aabb);
		};
	}

	template<typename T>
	concept boxed_t = requires(T a)
	{
		{a.get_aabb()} -> std::convertible_to<const aabb&>;
	};

	template<boxed_t T>
	class quadtree
	{
	public:
		using intersection_state_t = std::vector<std::pair<T, T>>;
		quadtree(aabb global_boundary);
		void add(const T& value);
		void remove(const T& value);
		void clear();
		std::vector<std::pair<T, T>> find_all_intersections() const;
		void dbgui();
	private:
		static constexpr std::size_t threshold = 16;
		static constexpr std::size_t max_depth = 8;

		// All nodes can store values. Value goes into the smallest node that can completely store it.
		struct node
		{
			std::array<std::unique_ptr<node>, 4> children{nullptr, nullptr, nullptr, nullptr};
			std::vector<T> values = {};

			bool is_leaf() const
			{
				return std::all_of(this->children.begin(), this->children.end(), [](const auto& ptr){return ptr == nullptr;});
			}
		};

		void node_add_value(node* node, std::size_t depth, const aabb& aabb, const T& value);
		void node_split(node* node, const aabb& aabb);
		bool node_remove(node* node, const aabb& aabb, const T& value);
		void remove_value(node* node, const T& value);
		bool try_merge(node* node);
		void node_find_intersections(const node* node, std::vector<std::pair<T, T>>& intersections) const;
		void node_find_descendent_intersections(const node* node, const T& value, std::vector<std::pair<T, T>>& intersections) const;

		aabb global_boundary;
		node root;
		bool debug_draw_enabled = false;
	};


	struct actor_quadtree_node
	{
		std::size_t uuid = std::numeric_limits<std::size_t>::max();
		aabb bounding_aabb = {{}, {0.0f, 0.0f}};
		const aabb& get_aabb() const{return this->bounding_aabb;}
		bool operator==(const actor_quadtree_node& rhs) const{return this->uuid == rhs.uuid;}
	};

	using actor_quadtree = quadtree<actor_quadtree_node>;
}

#include "gamelib/physics/quadtree.inl"
#endif // RN_GAMELIB_PHYSICS_QUADTREE_HPP