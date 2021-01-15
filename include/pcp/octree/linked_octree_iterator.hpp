#ifndef PCP_OCTREE_LINKED_OCTREE_ITERATOR_HPP
#define PCP_OCTREE_LINKED_OCTREE_ITERATOR_HPP

/**
 * @file
 * @ingroup octree
 */

#include <stack>

namespace pcp {

template <class PointView, class ParamsType>
class basic_linked_octree_node_t;

/**
 * @ingroup octree
 * @brief
 * Read-only forward iterator for points in the octree.
 * Uses a stack of ancestor nodes under the hood to
 * traverse the octree in postorder.
 * @tparam PointView Type satisfying PointView concept
 * @tparam ParamsType Type containg the octree parameters
 */
template <class PointView, class ParamsType>
class linked_octree_iterator_t
{
    using octree_node_type = basic_linked_octree_node_t<PointView, ParamsType>;

  public:
    using value_type        = typename octree_node_type::value_type;
    using difference_type   = std::size_t;
    using reference         = typename octree_node_type::reference;
    using pointer           = typename octree_node_type::pointer;
    using iterator_category = std::forward_iterator_tag;

    using const_reference = typename octree_node_type::const_reference;

  private:
    using self_type             = linked_octree_iterator_t<PointView, ParamsType>;
    using point_iterator        = typename octree_node_type::points_type::iterator;
    using const_point_iterator  = typename octree_node_type::points_type::const_iterator;
    using octant_iterator       = typename octree_node_type::octants_type::iterator;
    using const_octant_iterator = typename octree_node_type::octants_type::const_iterator;

  public:
    friend class basic_linked_octree_node_t<PointView, ParamsType>;

    linked_octree_iterator_t() : octree_node_(nullptr), it_() {}

    explicit linked_octree_iterator_t(octree_node_type* octree_node) : octree_node_(nullptr), it_()
    {
        octree_node_ = get_next_node(octree_node, octree_node->octants_.cbegin());
        it_          = octree_node_->points_.begin();
    }

    linked_octree_iterator_t(self_type const& other)     = default;
    linked_octree_iterator_t(self_type&& other) noexcept = default;

    self_type& operator=(self_type const& other) = default;

    /**
     * @brief Get the root of the octree
     * @return
     */
    octree_node_type const* root() const
    {
        using stack_type = decltype(ancestor_octree_nodes_);

        if (ancestor_octree_nodes_.empty())
            return octree_node_;

        decltype(ancestor_octree_nodes_) copy;
        while (!ancestor_octree_nodes_.empty())
        {
            copy.push(ancestor_octree_nodes_.top());
            const_cast<stack_type&>(ancestor_octree_nodes_).pop();
        }

        auto const* root = copy.top();

        while (!copy.empty())
        {
            const_cast<stack_type&>(ancestor_octree_nodes_).push(copy.top());
            copy.pop();
        }

        return root;
    }

    const_reference operator*() const { return *it_; }
    reference operator*() { return *it_; }

    self_type& operator++()
    {
        /*
         * There are still points in this octree node, just
         * return the next point in line.
         */
        if (++it_ != octree_node_->points_.cend())
            return *this;

        /*
         * If there are no ancestors, the current octree node
         * is the root node. If we have exhausted all points
         * of the root node, then we have reached the end
         * iterator.
         */
        if (ancestor_octree_nodes_.empty())
        {
            octree_node_ = nullptr;
            it_          = decltype(it_){};
            return *this;
        }

        /*
         * If there are no more points in this octree node,
         * but there are still points remaining in the tree,
         * we move to the next node in the sequence.
         */
        move_to_next_node();
        return *this;
    }

    self_type const& operator++() const { return ++(const_cast<self_type&>(*this)); }

    self_type operator++(int)
    {
        self_type previous{*this};
        ++(const_cast<self_type&>(*this));
        return previous;
    }

    self_type operator++(int) const { return const_cast<self_type&>(*this)++; }

    bool operator==(self_type const& other) const
    {
        return (octree_node_ == other.octree_node_) && (it_ == other.it_);
    }

    bool operator!=(self_type const& other) const { return !(*this == other); }

    PointView const* operator->() const { return &(*it_); }
    PointView* operator->() { return &(*it_); }

  private:
    /**
     * @brief
     * Returns the next node in post-order sequence.
     * @param octree Node to start looking from.
     * @param begin Iterator to the current node's child to start looking from.
     * @return The next node to move to.
     */
    octree_node_type* get_next_node(octree_node_type* octree, const_octant_iterator begin)
    {
        auto const end = octree->octants_.cend();

        for (auto octree_child_node_it = begin; octree_child_node_it != end; ++octree_child_node_it)
        {
            auto const& octree_child_node = *octree_child_node_it;
            if (!octree_child_node)
                continue;

            ancestor_octree_nodes_.push(octree);
            return get_next_node(octree_child_node.get(), octree_child_node->octants_.begin());
        }

        /*
         * If there were no more children to visit from this octree,
         * it means this octree node is the next node to visit in
         * post-order sequence.
         */
        return octree;
    }

    void move_to_next_node()
    {
        /*
         * If we've exhausted all points of this octree node,
         * then it's time to go back to our parent and look
         * for the next point from there.
         */
        auto* parent = ancestor_octree_nodes_.top();
        ancestor_octree_nodes_.pop();

        auto const is_same_octant =
            [target = octree_node_](
                typename std::iterator_traits<const_octant_iterator>::reference octant) -> bool {
            return octant.get() == target;
        };

        /*
         * Find our next non-null sibling
         */
        const_octant_iterator next_octant =
            std::find_if(parent->octants_.cbegin(), parent->octants_.cend(), is_same_octant);

        octree_node_ = get_next_node(parent, ++next_octant);

        /*
         * Once we've found the next octree node in the
         * sequence, we initialize our internal iterator
         * to the first point of that octree node.
         */
        it_ = octree_node_->points_.begin();
    }

    octree_node_type* octree_node_;
    std::stack<octree_node_type*> ancestor_octree_nodes_;
    point_iterator it_;
};

} // namespace pcp

#endif // PCP_OCTREE_LINKED_OCTREE_ITERATOR_HPP