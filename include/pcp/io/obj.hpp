#ifndef PCP_IO_OBJ_HPP
#define PCP_IO_OBJ_HPP

/**
 * @file
 * @ingroup io
 */

#include "pcp/traits/normal_traits.hpp"
#include "pcp/traits/point_traits.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace pcp {
namespace io {

/**
 * @ingroup io-obj
 * @brief
 * Load an obj point cloud file to memory from an input stream in obj format.
 * @tparam Point Type of points to return satisfying Point concept
 * @tparam Normal Type of normals to return satisfying Normal concept
 * @param is The input stream in obj format
 * @return A pair of vector of points and vector of normals if there were any
 */
template <class Point, class Normal>
inline auto read_obj(std::istream& is) -> std::tuple<std::vector<Point>, std::vector<Normal>>
{
    static_assert(traits::is_point_v<Point>, "Point must satisfy Point concept");
    static_assert(traits::is_normal_v<Normal>, "Normal must satisfy Normal concept");

    using point_type      = Point;
    using normal_type     = Normal;
    using coordinate_type = typename point_type::coordinate_type;
    using component_type  = typename normal_type::component_type;

    std::vector<point_type> points;
    std::vector<normal_type> normals;

    std::string line;
    while (std::getline(is, line))
    {
        std::string type = line.substr(0, 2);

        if (type == "v ")
        {
            std::istringstream s(line.substr(2));
            coordinate_type x, y, z;
            s >> x;
            s >> y;
            s >> z;
            point_type v{x, y, z};
            points.push_back(v);
        }
        else if (type == "vn")
        {
            std::istringstream s(line.substr(2));
            component_type x, y, z;
            s >> x;
            s >> y;
            s >> z;
            normal_type vn{x, y, z};
            normals.push_back(vn);
        }
    }

    return std::make_tuple(points, normals);
}

/**
 * @ingroup io-obj
 * @brief
 * Load an obj point cloud file to memory from a file path in obj format.
 * @tparam Point Type of points to return satisfying Point concept
 * @tparam Normal Type of normals to return satisfying Normal concept
 * @param path The input obj file path
 * @return A pair of vector of points and vector of normals if there were any
 */
template <class Point, class Normal>
inline auto read_obj(std::filesystem::path const& path)
    -> std::tuple<std::vector<Point>, std::vector<Normal>>
{
    if (!path.has_filename())
        return {};

    if (!path.has_extension() || path.extension() != ".obj")
        return {};

    if (!std::filesystem::exists(path))
        return {};

    std::ifstream ifs{path.c_str(), std::ios::binary};

    if (!ifs.is_open())
        return {};

    return read_obj<Point, Normal>(ifs);
}

/**
 * @ingroup io-obj
 * @brief
 * Export a point cloud with or without normals to an obj file at the specified path.
 * @tparam Point Type of points in the point cloud
 * @tparam Normal Type of normals associated with the point cloud
 * @param path Path to the file to write to
 * @param points The point cloud's points
 * @param normals The point cloud's normals
 */
template <class Point, class Normal>
inline void write_obj(
    std::filesystem::path const& path,
    std::vector<Point> const& points,
    std::vector<Normal> const& normals)
{
    if (!path.has_extension() || path.extension() != ".obj")
        return;

    if (points.empty())
        return;

    if (points.size() != normals.size())
        return;

    std::ofstream ofs{path.c_str(), std::ios::binary};

    if (!ofs.is_open())
        return;

    write_obj(points, normals, ofs);
}

/**
 * @ingroup io-obj
 * @brief
 * Export a point cloud with or without normals to an output stream.
 * @tparam Point Type of points in the point cloud
 * @tparam Normal Type of normals associated with the point cloud
 * @param os The output stream to write to
 * @param points The point cloud's points
 * @param normals The point cloud's normals
 */
template <class Point, class Normal>
inline void
write_obj(std::ostream& os, std::vector<Point> const& points, std::vector<Normal> const& normals)
{
    bool const has_normals = !normals.empty();
    for (std::size_t i = 0; i < points.size(); ++i)
    {
        auto const& p = points[i];

        std::string const v = "v " + std::to_string(p.x()) + " " + std::to_string(p.y()) + " " +
                              std::to_string(p.z()) + "\n";

        os << v;

        if (!has_normals)
            continue;

        auto const& n = normals[i];

        std::string const vn = "vn " + std::to_string(n.nx()) + " " + std::to_string(n.ny()) + " " +
                               std::to_string(n.nz()) + "\n";

        os << vn;
    }
}

} // namespace io
} // namespace pcp

#endif // PCP_IO_OBJ_HPP