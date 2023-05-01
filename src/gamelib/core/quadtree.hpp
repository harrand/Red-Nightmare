#ifndef RNLIB_CORE_QUADTREE_HPP
#define RNLIB_CORE_QUADTREE_HPP
#include "gamelib/core/box.hpp"
#include "gamelib/core/mount.hpp"
#include "gamelib/render/quad_renderer.hpp"
#include "gamelib/render/image.hpp"
#include <array>
#include <memory>
#include <vector>
#include <optional>

namespace rnlib
{
	namespace detail
	{
		struct quadtree_helper
		{
			static box compute_quadrant_box(const box& parent_box, std::size_t quadrant_id);
			static std::optional<std::size_t> find_quadrant_id(const box& node_box, const box& value_box);
		};
	}

	template<typename T>
	concept boxed_t = requires(T a)
	{
		{a.get_box()} -> std::convertible_to<const box&>;
	};

	template<boxed_t T>
	class quadtree
	{
	public:
		using intersection_state_t = std::vector<std::pair<T, T>>;
		quadtree(box global_boundary);
		void add(const T& value);
		void remove(const T& value);
		void clear();
		std::vector<std::pair<T, T>> find_all_intersections() const;
		mount_result debug_mount(std::span<quad_renderer::quad_data> quads);
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

		void node_add_value(node* node, std::size_t depth, const box& box, const T& value);
		void node_split(node* node, const box& box);
		bool node_remove(node* node, const box& box, const T& value);
		void remove_value(node* node, const T& value);
		bool try_merge(node* node);
		void node_find_intersections(const node* node, std::vector<std::pair<T, T>>& intersections) const;
		void node_find_descendent_intersections(const node* node, const T& value, std::vector<std::pair<T, T>>& intersections) const;

		box global_boundary;
		node root;
	};


	struct actor_quadtree_node
	{
		std::size_t uuid = std::numeric_limits<std::size_t>::max();
		box bounding_box = {{}, {0.0f, 0.0f}};
		const box& get_box() const{return this->bounding_box;}
		bool operator==(const actor_quadtree_node& rhs) const{return this->uuid == rhs.uuid;}
	};

	using actor_quadtree = quadtree<actor_quadtree_node>;
}

#include "gamelib/core/quadtree.inl"
#endif // RNLIB_CORE_QUADTREE_HPP
