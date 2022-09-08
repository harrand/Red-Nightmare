#ifndef REDNIGHTMARE_QUADTREE_HPP
#define REDNIGHTMARE_QUADTREE_HPP
#include "box.hpp"
#include <array>
#include <memory>
#include <vector>
#include <optional>

namespace game
{
	namespace detail
	{
		struct QuadtreeHelper
		{
			static Box compute_quadrant_box(const Box& parent_box, std::size_t quadrant_id);
			static std::optional<std::size_t> find_quadrant_id(const Box& node_box, const Box& value_box);
		};
	}

	template<typename T>
	concept BoxedThing = requires(T a)
	{
		{a.get_box()} -> std::convertible_to<const Box&>;
	};

	template<BoxedThing T>
	class Quadtree
	{
	public:
		using IntersectionState = std::vector<std::pair<T, T>>;
		Quadtree(Box global_boundary);
		void add(const T& value);
		void remove(const T& value);
		void clear();
		std::vector<std::pair<T, T>> find_all_intersections() const;
	private:
		static constexpr std::size_t threshold = 16;
		static constexpr std::size_t max_depth = 8;

		// All nodes can store values. Value goes into the smallest node that can completely store it.
		struct Node
		{
			std::array<std::unique_ptr<Node>, 4> children{nullptr, nullptr, nullptr, nullptr};
			std::vector<T> values = {};

			bool is_leaf() const
			{
				return this->children[0] == nullptr;
			}
		};

		void node_add_value(Node* node, std::size_t depth, const Box& box, const T& value);
		void node_split(Node* node, const Box& box);
		bool node_remove(Node* node, const Box& box, const T& value);
		void remove_value(Node* node, const T& value);
		bool try_merge(Node* node);
		void node_find_intersections(const Node* node, std::vector<std::pair<T, T>>& intersections) const;
		void node_find_descendent_intersections(const Node* node, const T& value, std::vector<std::pair<T, T>>& intersections) const;

		Box global_boundary;
		Node root;
	};
}
#include "quadtree.inl"

#endif // REDNIGHTMARE_QUADTREE_HPP
