#pragma once

#include "point_traits.hpp"
#include "normal_traits.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace pcp {
namespace io {

template <class Point, class Normal>
inline auto read_obj(std::istream& is)
    -> std::tuple<std::vector<Point>, std::vector<Normal>>
{
    static_assert(traits::is_point_v<Point>, "Point must satisfy Point concept");
    static_assert(traits::is_normal_v<Normal>, "Normal must satisfy Normal concept");

    using point_type  = Point;
    using normal_type = Normal;

    std::vector<point_type> points;
    std::vector<normal_type> normals;

    std::string line;
    while (std::getline(is, line))
    {
        std::string type = line.substr(0, 2);

        if (type == "v ")
        {
            std::istringstream s(line.substr(2));
            point_type v;
            s >> v.x();
            s >> v.y();
            s >> v.z();
            points.push_back(v);
        }
        else if (type == "vn")
        {
            std::istringstream s(line.substr(2));
            normal_type vn;
            s >> vn.x();
            s >> vn.y();
            s >> vn.z();
            normals.push_back(vn);
        }
    }

    return std::make_tuple(points, normals);
}

template <class Point, class Normal>
inline auto read_obj(std::filesystem::path const& path)
    -> std::tuple<std::vector<Point>, std::vector<Normal>>
{
    if (!path.has_filename())
        return {};

    if (!path.has_extension() || path.extension() != "obj")
        return {};

    if (!std::filesystem::exists(path))
        return {};

    std::ifstream ifs{path.c_str()};

    if (!ifs.is_open())
        return {};

    return read_obj<Point, Normal>(ifs);
}

template <class Point, class Normal>
inline void write_obj(
    std::vector<Point> const& points,
    std::vector<Normal> const& normals,
    std::filesystem::path const& path)
{
    if (!path.has_extension() || path.extension() != "obj")
        return;

    if (points.empty())
        return;

    if (points.size() != normals.size())
        return;

    std::ofstream ofs{path.c_str()};

    if (!ofs.is_open())
        return;

    write_obj(points, normals, ofs);
}

template <class Point, class Normal>
inline void write_obj(
    std::vector<Point> const& points,
    std::vector<Normal> const& normals,
    std::ostream& os)
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

        std::string const vn = "vn " + std::to_string(n.x()) + " " + std::to_string(n.y()) + " " +
                               std::to_string(n.z()) + "\n";

        os << vn;
    }
}

} // namespace io
} // namespace pcp