#pragma once

#include "flight.hpp"

#include <algorithm>
#include <chrono>
#include <functional>
#include <optional>
#include <string_view>
#include <vector>

inline std::vector<std::reference_wrapper<const flight>>
search_flights(const std::vector<flight> &flights, std::string_view source,
               std::string_view destination,
               std::optional<std::chrono::sys_days> date = std::nullopt) {
  std::vector<std::reference_wrapper<const flight>> matches;
  for (const auto &trip : flights) {
    if (trip.get_source() == source && trip.get_dest() == destination &&
        (!date || std::chrono::floor<std::chrono::days>(trip.get_date_depart()) ==
                      *date)) {
      matches.emplace_back(trip);
    }
  }

  std::ranges::stable_sort(matches, {}, [](const flight &trip) {
    return trip.get_date_depart();
  });
  return matches;
}
