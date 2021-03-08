#ifndef PCP_ALGORITHM_HIERARCHY_SIMPLIFICATION_HPP
#define PCP_ALGORITHM_HIERARCHY_SIMPLIFICATION_HPP

/**
 * @file
 * @ingroup algorithm
 */

#include "covariance.hpp"
#include "pcp/common/norm.hpp"
#include "pcp/traits/output_iterator_traits.hpp"
#include "pcp/traits/point_map.hpp"

#include <algorithm>
#include <queue>

namespace pcp {
namespace algorithm {
namespace hierarchy {

struct params_t
{
    std::size_t cluster_size = 0u; ///< Maximum size of subdivided clusters
    double var_max =
        1. / 3.; ///< Maximum variation permitted after which a cluster will be subdivided
};

} // namespace hierarchy

template <class RandomAccessIter, class OutputIter, class PointMap>
OutputIter hierarchy_simplification(
    RandomAccessIter begin,
    RandomAccessIter end,
    OutputIter out_begin,
    PointMap const& point_map,
    hierarchy::params_t const& params)
{
    using input_element_type = typename std::iterator_traits<RandomAccessIter>::value_type;
    static_assert(
        traits::is_point_map_v<PointMap, input_element_type>,
        "point_map must satisfy PointMap concept");

    using point_type          = std::invoke_result_t<PointMap, input_element_type>;
    using scalar_type         = typename point_type::coordinate_type;
    using iterator_type       = RandomAccessIter;
    using output_element_type = typename xstd::output_iterator_traits<OutputIter>::value_type;

    static_assert(
        traits::is_point_v<output_element_type>,
        "out_begin must be iterator to a type satisfying Point concept");

    assert(params.cluster_size > 0u);
    assert(params.var_max >= 0. && params.var_max <= (1. / 3.));

    struct cluster_t
    {
        iterator_type begin, end;
    };

    std::vector<output_element_type> points_to_keep{};
    std::queue<cluster_t> recursion_queue{};
    recursion_queue.push(cluster_t{begin, end});
    while (!recursion_queue.empty())
    {
        cluster_t const& cluster = recursion_queue.front();
        recursion_queue.pop();

        std::size_t const N = static_cast<std::size_t>(std::distance(cluster.begin, cluster.end));

        auto const [mu, Cov] = covariance(cluster.begin, cluster.end, point_map);
        auto const [eigen_values, eigen_vectors] = eigen_sorted(Cov);

        scalar_type const var =
            eigen_values(0) / (eigen_values(0) + eigen_values(1) + eigen_values(2));

        if (N > params.cluster_size /* || var > params.var_max*/)
        {
            auto const n     = eigen_vectors.col(2);
            auto const d     = mu.dot(n);
            auto const plane = [&](input_element_type const& e) {
                point_type const& p        = point_map(e);
                scalar_type const f        = p.x() * n.x() + p.y() * n.y() + p.z() * n.z() - d;
                scalar_type constexpr zero = static_cast<scalar_type>(0.);
                return f <= zero;
            };

            auto it = std::partition(cluster.begin, cluster.end, plane);
            recursion_queue.push(cluster_t{cluster.begin, it});
            recursion_queue.push(cluster_t{it, cluster.end});
        }
        else
        {
            point_type const centroid{mu.x(), mu.y(), mu.z()};
            auto const min = std::min_element(
                cluster.begin,
                cluster.end,
                [&](input_element_type const& e1, input_element_type const& e2) {
                    auto const& p1 = point_map(e1);
                    auto const& p2 = point_map(e2);
                    auto const d1  = common::squared_distance(p1, centroid);
                    auto const d2  = common::squared_distance(p2, centroid);
                    return d1 < d2;
                });

            // std::size_t const index = static_cast<std::size_t>(std::distance(begin, min));
            // points_to_keep.push_back(index);
            output_element_type const point{min->x(), min->y(), min->z()};
            points_to_keep.push_back(point);
        }
    }

    auto out_it = std::copy(points_to_keep.begin(), points_to_keep.end(), out_begin);

    return out_it;
}

} // namespace algorithm
} // namespace pcp

#endif // PCP_ALGORITHM_HIERARCHY_SIMPLIFICATION_HPP