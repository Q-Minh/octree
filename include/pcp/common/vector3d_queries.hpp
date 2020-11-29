#pragma once

#include <numeric>
#include <cmath>
#include <iterator>
#include <pcp/traits/vector3d_traits.hpp>
#include <type_traits>

namespace pcp {
namespace common {

template <class T>
bool floating_point_equals(T v1, T v2, T eps = static_cast<T>(1e-5))
{
    T const d = std::abs(v1 - v2);
    return d < eps;
}

/**
 * @brief Equality test of 2 points by coordinates within given precision.
 * @tparam Vector3d1 Type satisfying PointView concept
 * @tparam Vector3d2 Type satisfying PointView concept
 * @param v1 Vector 1
 * @param v2 Vector3 2
 * @param eps Precision (margin of error)
 * @return
 */
template <class Vector3d1, class Vector3d2>
bool are_vectors_equal(
    Vector3d1 const& v1,
    Vector3d2 const& v2,
    typename Vector3d1::component_type eps = static_cast<typename Vector3d1::component_type>(1e-5))
{
    // TODO:
    // These requirements are too restrictive. We only want to compare x,y,z components.
    // Maybe move this function to a point_queries.hpp file and have type safety check 
    // for only points or point_views.
    /*static_assert(traits::is_vector3d_v<Vector3d1>, "Vector3d1 must satisfy Vector3d concept");
    static_assert(traits::is_vector3d_v<Vector3d2>, "Vector3d2 must satisfy Vector3d concept");*/

    using component_type = typename Vector3d1::component_type;
    bool const equals    = floating_point_equals(v1.x(), v2.x(), eps) &&
                        floating_point_equals(v1.y(), v2.y(), eps) &&
                        floating_point_equals(v1.z(), v2.z(), eps);
    return equals;
}

template <
    class ForwardIter,
    class Vector3d = typename std::iterator_traits<ForwardIter>::value_type>
Vector3d center_of_geometry(ForwardIter begin, ForwardIter end)
{
    static_assert(traits::is_vector3d_v<Vector3d>, "Vector3d must satisfy Vector3d concept");
    static_assert(
        std::is_convertible_v<typename ForwardIter::value_type, Vector3d>,
        "Type of dereferenced ForwardIter must be convertible to Vector3d");
    auto const n   = std::distance(begin, end);
    // Note: Can be parallelized
    auto const sum = std::reduce(begin, end);
    return sum / n;
}

} // namespace common
} // namespace pcp
