#include <catch2/catch.hpp>
#include <pcp/common/points/point.hpp>
#include <pcp/kdtree/linked_kdtree.hpp>

SCENARIO("KNN searches on linked kdtrees", "[kdtree]")
{
    auto const coordinate_map = [](pcp::point_t const& p) {
        return std::array<float, 3u>{p.x(), p.y(), p.z()};
    };

    using kdtree_type = pcp::basic_linked_kdtree_t<pcp::point_t, 3u, decltype(coordinate_map)>;

    GIVEN("a kdtree with 1 point in each octant")
    {
        pcp::kdtree::construction_params_t params;
        params.construction = pcp::kdtree::construction_t::nth_element;
        params.max_depth    = 12u;

        std::vector<pcp::point_t> points;
        points.push_back({-.5f, -.5f, -.5f}); // 000
        points.push_back({.5f, -.5f, -.5f});  // 100
        points.push_back({.5f, .5f, -.5f});   // 110
        points.push_back({-.5f, .5f, -.5f});  // 010
        points.push_back({-.5f, -.5f, .5f});  // 001
        points.push_back({.5f, -.5f, .5f});   // 101
        points.push_back({.5f, .5f, .5f});    // 111
        points.push_back({-.5f, .5f, .5f});   // 011

        kdtree_type kdtree{points.begin(), points.end(), coordinate_map, params};

        WHEN("searching for k nearest neighbors with k=1")
        {
            auto const k = 1u;

            auto const reference111                 = pcp::point_t{.51f, .51f, .51f};
            auto const& nearest_neighbors_octant111 = kdtree.nearest_neighbours(reference111, k);

            auto const reference000                 = pcp::point_t{-.51f, -.51f, -.51f};
            auto const& nearest_neighbors_octant000 = kdtree.nearest_neighbours(reference000, k);

            auto const reference110                 = pcp::point_t{.51f, .51f, -.51f};
            auto const& nearest_neighbors_octant110 = kdtree.nearest_neighbours(reference110, k);

            auto const reference011                 = pcp::point_t{-.51f, .51f, .51f};
            auto const& nearest_neighbors_octant011 = kdtree.nearest_neighbours(reference011, k);

            THEN("returns the nearest neighbor")
            {
                REQUIRE(nearest_neighbors_octant111.size() == k);
                REQUIRE(nearest_neighbors_octant000.size() == k);
                REQUIRE(nearest_neighbors_octant110.size() == k);
                REQUIRE(nearest_neighbors_octant011.size() == k);
            }
        }
    }
    GIVEN(
        "a kdtree with 1 point in 7 octants "
        "and 4 points in the other octant")
    {
        std::vector<pcp::point_t> points;
        points.push_back({-.5f, -.5f, -.5f}); // 000
        points.push_back({.5f, -.5f, -.5f});  // 100
        points.push_back({-.5f, .5f, -.5f});  // 010
        points.push_back({-.5f, -.5f, .5f});  // 001
        points.push_back({.5f, -.5f, .5f});   // 101
        points.push_back({.5f, .5f, .5f});    // 111
        points.push_back({-.5f, .5f, .5f});   // 011

        pcp::point_t const reference = {.5f, .5f, -.5f};

        pcp::point_t const first_nearest{.51f, .51f, -.51f};
        pcp::point_t const second_nearest{.61f, .51f, -.51f};
        pcp::point_t const third_nearest{.41f, .31f, -.51f};
        pcp::point_t const fourth_nearest{.71f, .21f, -.51f};

        points.push_back(first_nearest); // 110
        points.push_back(second_nearest);
        points.push_back(third_nearest);
        points.push_back(fourth_nearest);

        pcp::kdtree::construction_params_t params;
        params.construction = pcp::kdtree::construction_t::nth_element;
        params.max_depth    = 12u;
        kdtree_type kdtree{points.begin(), points.end(), coordinate_map, params};

        WHEN("searching for k nearest neighbors with k=4 in the octant with 4 points")
        {
            auto const k                  = 4u;
            auto const& nearest_neighbors = kdtree.nearest_neighbours(reference, k);

            THEN("nearest points are the 4 points in that same octant")
            {
                REQUIRE(nearest_neighbors.size() == k);
                REQUIRE(pcp::common::are_vectors_equal(nearest_neighbors[0], first_nearest));
                REQUIRE(pcp::common::are_vectors_equal(nearest_neighbors[1], second_nearest));
                REQUIRE(pcp::common::are_vectors_equal(nearest_neighbors[2], third_nearest));
                REQUIRE(pcp::common::are_vectors_equal(nearest_neighbors[3], fourth_nearest));
            }
        }
        WHEN("searching for k nearest neighbors with k=3 in the octant with 4 points")
        {
            auto const k                  = 3u;
            auto const& nearest_neighbors = kdtree.nearest_neighbours(reference, k);

            THEN("nearest points are the 3 nearest points in that same octant")
            {
                REQUIRE(nearest_neighbors.size() == k);
                REQUIRE(pcp::common::are_vectors_equal(nearest_neighbors[0], first_nearest));
                REQUIRE(pcp::common::are_vectors_equal(nearest_neighbors[1], second_nearest));
                REQUIRE(pcp::common::are_vectors_equal(nearest_neighbors[2], third_nearest));
            }
        }
    }
    /*GIVEN("a randomly constructed octree")
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> coordinate_distribution(-0.95f, 0.95f);
        std::uniform_real_distribution<float> near_coordinate_distribution(-.99f, -0.96f);
        std::uniform_real_distribution<float> far_coordinate_distribution(0.96f, .99f);
        std::uniform_int_distribution<std::size_t> size_distribution(1'000, 100'000);
        std::uniform_int_distribution<std::size_t> k_distribution(1u, 10u);

        pcp::octree_parameters_t<pcp::point_t> params;
        params.node_capacity = node_capacity;
        params.max_depth     = static_cast<std::uint8_t>(max_depth);
        params.voxel_grid    = pcp::axis_aligned_bounding_box_t<pcp::point_t>{
            pcp::point_t{-2.f, -2.f, -2.f},
            pcp::point_t{2.f, 2.f, 2.f}};

        pcp::linked_octree_t octree(params);

        auto const size = size_distribution(gen);
        for (std::size_t i = 0; i < size; ++i)
        {
            octree.insert(
                pcp::point_t{
                    coordinate_distribution(gen),
                    coordinate_distribution(gen),
                    coordinate_distribution(gen)},
                point_map);
        }

        REQUIRE(octree.size() == size);

        WHEN("inserting in the octree k nearest points to the reference point")
        {
            pcp::point_t const reference = {-1.f, 1.f, 1.f};
            std::vector<pcp::point_t> k_inserted_points;

            auto const k = k_distribution(gen);
            for (std::size_t i = 0; i < k; ++i)
            {
                k_inserted_points.push_back(pcp::point_t{
                    near_coordinate_distribution(gen),
                    far_coordinate_distribution(gen),
                    far_coordinate_distribution(gen)});
            }

            octree.insert(k_inserted_points.cbegin(), k_inserted_points.cend(), point_map);

            THEN("k nearest neighbour search returns those k points")
            {
                std::vector<pcp::point_t> nearest_neighbours =
                    octree.nearest_neighbours(reference, k, point_map);

                REQUIRE(nearest_neighbours.size() == k);

                auto const contains = [&k_inserted_points](pcp::point_t const& p) -> bool {
                    return std::find_if(
                               k_inserted_points.cbegin(),
                               k_inserted_points.cend(),
                               [&p](auto const& other) {
                                   return pcp::common::are_vectors_equal(p, other);
                               }) != k_inserted_points.cend();
                };

                bool const found_nearest_points =
                    std::all_of(nearest_neighbours.cbegin(), nearest_neighbours.cend(), contains);

                REQUIRE(found_nearest_points);
            }
        }
    }*/
}
