#pragma once

#include "flight.hpp"

#include <chrono>
#include <vector>

inline std::vector<flight> demo_flights() {
  using namespace std::chrono;
  const auto seats = std::vector{
      seat{"1A", type_seat::BUSINESS, status_seat::AVAILABLE},
      seat{"1B", type_seat::BUSINESS, status_seat::AVAILABLE},
      seat{"2A", type_seat::ECONOMY, status_seat::AVAILABLE},
      seat{"2B", type_seat::ECONOMY, status_seat::AVAILABLE},
  };
  const Aircraft aircraft{"F-DEMO", "Avion de démonstration", 4};
  const auto date = sys_days{year{2027} / January / 15};
  return {
      {"AM101", "CDG", "NCE", date + hours{8}, date + hours{10},
       Flightstatus::ON_TIME, aircraft, seats},
      {"AM102", "NCE", "CDG", date + hours{11}, date + hours{13},
       Flightstatus::DELAYED, aircraft, seats},
      {"AM103", "CDG", "NCE", date + hours{15}, date + hours{17},
       Flightstatus::CANCELLED, aircraft, seats},
      {"AM104", "CDG", "NCE", date + days{1} + hours{8},
       date + days{1} + hours{10}, Flightstatus::ON_TIME, aircraft, seats},
  };
}
