#include <catch2/catch.hpp>
#include <pcp/octree/flat_octree.hpp>

SCENARIO("flat octree find", "[flat_octree]")
{
    auto depth = GENERATE(1u, 3u, 11u, 15u, 21u);

    GIVEN("a randomly filled flat octree")
    {
        float const min = -1000.f;
        float const max = 1000.f;

        pcp::flat_octree_parameters_t<pcp::point_t> params;
        params.voxel_grid    = {{min, min, min}, {max, max, max}};
        params.depth         = static_cast<std::uint8_t>(depth);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> coordinate_distribution(min + 1.f, max - 1.f);
        std::uniform_int_distribution<std::uint32_t> size_distribution(1'000u, 100'000u);

        std::uint32_t const size = size_distribution(gen);
        std::uniform_int_distribution<std::size_t> index_distribution(0u, size - 1u);

        std::vector<pcp::point_t> points;
        for (std::uint32_t i = 0u; i < size; ++i)
        {
            points.emplace_back(pcp::point_t{
                coordinate_distribution(gen),
                coordinate_distribution(gen),
                coordinate_distribution(gen)});
        }

        auto const point_map = [](pcp::point_t const& p) {
            return p;
        };

        pcp::flat_octree_t octree(points.cbegin(), points.cend(), point_map, params);

        REQUIRE(octree.size() == points.size());

        WHEN("searching for an inserted point in the flat octree")
        {
            std::size_t const idx = index_distribution(gen);
            auto const& p         = points[idx];
            auto const it         = octree.find(p, point_map);

            THEN("the corresponding iterator is returned")
            {
                REQUIRE(it != octree.cend());
                REQUIRE(pcp::common::are_vectors_equal(point_map(*it), p));
            }
        }
        WHEN(
            "searching for a point that is contained in the flat "
            "octree's voxel grid but that was not inserted in the octree")
        {
            pcp::point_t const p{min, max, min};
            auto const it = octree.find(p, point_map);

            THEN("the end iterator is returned") { REQUIRE(it == octree.cend()); }
        }
        WHEN("searching for a point that is not contained in the flat octree's voxel grid")
        {
            pcp::point_t const p{min - 1.f, min - 1.f, min - 1.f};
            auto const it = octree.find(p, point_map);

            THEN("the end iterator is returned") { REQUIRE(it == octree.cend()); }
        }
    }
}