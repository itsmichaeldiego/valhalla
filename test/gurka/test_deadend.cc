#include "gurka.h"
#include <gtest/gtest.h>

using namespace valhalla;

TEST(Standalone, BirectionalDeadend) {

  const std::string ascii_map = R"(
    A-B-C-D-E
        |
        |1
        F···G
        |
        |2
        H
  )";

  const gurka::ways ways = {
      {"AB", {{"highway", "primary"}}}, {"BC", {{"highway", "primary"}}},
      {"CD", {{"highway", "primary"}}}, {"DE", {{"highway", "primary"}}},
      {"CF", {{"highway", "service"}}}, {"FH", {{"highway", "service"}}},
      {"FG", {{"highway", "footway"}}},
  };

  const auto layout = gurka::detail::map_to_coordinates(ascii_map, 100);

  auto map = gurka::buildtiles(layout, ways, {}, {}, "test/data/deadend");

  // Origin and destination edges should not be connected, otherwise it will be considered a trivial
  // route and handled by a* instead of bidirectional
  auto result = gurka::route(map, {"A", "1", "E"}, "auto",
                             {{"/locations/1/type", "break_through"}, {"/locations/1/radius", "0"}});

  // Verify path
  gurka::assert::raw::expect_path(result, {"AB", "BC", "CF", "CF", "FH", "FH", "CF", "CD", "DE"});

  // Verify maneuver types
  gurka::assert::raw::expect_maneuvers(result, {DirectionsLeg_Maneuver_Type_kStart,
                                                DirectionsLeg_Maneuver_Type_kRight,
                                                DirectionsLeg_Maneuver_Type_kDestinationLeft,
                                                DirectionsLeg_Maneuver_Type_kStartLeft,
                                                DirectionsLeg_Maneuver_Type_kUturnRight,
                                                DirectionsLeg_Maneuver_Type_kRight,
                                                DirectionsLeg_Maneuver_Type_kDestination});
}
