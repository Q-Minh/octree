#pragma once

#include <type_traits>

namespace pcp {
namespace traits {

template <class Normal, class = void>
struct is_normal : std::false_type
{
};

/**
 * Normal requirements:
 * - component_type type member
 * - default constructible
 * - copy constructible
 * - move constructible
 * - parameter constructor from x,y,z
 * - copy assignable
 * - move assignable
 * - x,y,z getters
 * - x,y,z setters
 * - equality/inequality comparable
 * - *,/,+,- operators
 */
template <class Normal>
struct is_normal<
    Normal,
    std::void_t<
        typename Normal::component_type,
        decltype(std::declval<Normal&>().x()),
        decltype(std::declval<Normal&>().y()),
        decltype(std::declval<Normal&>().z()),
        decltype(std::declval<Normal&>().x(std::declval<typename Normal::component_type>())),
        decltype(std::declval<Normal&>().y(std::declval<typename Normal::component_type>())),
        decltype(std::declval<Normal&>().z(std::declval<typename Normal::component_type>())),
        decltype(std::declval<typename Normal::component_type>() * std::declval<Normal&>()),
        decltype(std::declval<Normal&>() / std::declval<typename Normal::component_type>()),
        decltype(std::declval<Normal&>() + std::declval<Normal&>()),
        decltype(std::declval<Normal&>() - std::declval<Normal&>()),
        decltype(std::declval<Normal&>() == std::declval<Normal&>()),
        decltype(std::declval<Normal&>() != std::declval<Normal&>())>> : std::true_type
{
    static_assert(std::is_default_constructible_v<Normal>, "Normal must be default constructible");
    static_assert(std::is_copy_constructible_v<Normal>, "Normal must be copy constructible");
    static_assert(std::is_move_constructible_v<Normal>, "Normal must be move constructible");
    static_assert(std::is_copy_assignable_v<Normal>, "Normal must be copy assignable");
    static_assert(std::is_move_assignable_v<Normal>, "Normal must be move assignable");
    static_assert(
        std::is_constructible_v<
            Normal,
            typename Normal::component_type,
            typename Normal::component_type,
            typename Normal::component_type>,
        "Normal must be constructible from x,y,z components");
};

template <class Normal>
static constexpr bool is_normal_v = is_normal<Normal>::value;

} // namespace traits
} // namespace pcp