#pragma once

#include "AIR_carft.hpp"
#include "SEAT.hpp"
#include "passenger.hpp"

#include <algorithm>
#include <chrono>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

enum class Flightstatus { ON_TIME, DELAYED, CANCELLED };

enum class ReservationResult {
  SUCCESS,
  FLIGHT_CANCELLED,
  UNKNOWN_SEAT,
  SEAT_UNAVAILABLE,
  PASSENGER_ALREADY_BOOKED,
  NO_RESERVATION
};

class flight {
  std::string flight_number;
  std::string source;
  std::string dest;
  std::chrono::sys_seconds date_depart;
  std::chrono::sys_seconds date_arrivee;
  Flightstatus status;
  Aircraft aircraft;
  std::vector<seat> seats;
  std::map<std::string, passenger> reservations;

public:
  flight(std::string flight_number, std::string source, std::string dest,
         std::chrono::sys_seconds date_depart,
         std::chrono::sys_seconds date_arrivee, Flightstatus status,
         Aircraft aircraft, std::vector<seat> seats)
      : flight_number(std::move(flight_number)), source(std::move(source)),
        dest(std::move(dest)), date_depart(date_depart),
        date_arrivee(date_arrivee), status(status), aircraft(std::move(aircraft)),
        seats(std::move(seats)) {
    if (this->flight_number.empty() || this->source.empty() ||
        this->dest.empty() || this->source == this->dest ||
        date_arrivee <= date_depart || this->aircraft.get_total_seat() <= 0 ||
        this->seats.size() !=
            static_cast<std::size_t>(this->aircraft.get_total_seat())) {
      throw std::invalid_argument("Vol ou capacité invalide");
    }

    std::set<std::string> names;
    for (const auto &place : this->seats) {
      const auto name = place.get_seat_name();
      if (name.empty() || !names.insert(name).second) {
        throw std::invalid_argument("Nom de siège vide ou dupliqué");
      }
    }
  }

  const std::string &get_flight_number() const { return flight_number; }
  const std::string &get_source() const { return source; }
  const std::string &get_dest() const { return dest; }
  std::chrono::sys_seconds get_date_depart() const { return date_depart; }
  std::chrono::sys_seconds get_date_arrivee() const { return date_arrivee; }
  Flightstatus get_status() const { return status; }
  const Aircraft &get_aircraft() const { return aircraft; }
  const std::vector<seat> &get_seats() const { return seats; }

  auto available_seats() const {
    return std::ranges::count_if(seats, [](const seat &place) {
      return !place.booked();
    });
  }

  ReservationResult reserve(const std::string &seat_name,
                            const passenger &traveler) {
    if (status == Flightstatus::CANCELLED) {
      return ReservationResult::FLIGHT_CANCELLED;
    }

    const auto place = std::ranges::find(seats, seat_name, &seat::get_seat_name);
    if (place == seats.end()) {
      return ReservationResult::UNKNOWN_SEAT;
    }
    if (place->booked()) {
      return ReservationResult::SEAT_UNAVAILABLE;
    }
    if (std::ranges::any_of(reservations, [&traveler](const auto &entry) {
          return entry.second.get_id() == traveler.get_id();
        })) {
      return ReservationResult::PASSENGER_ALREADY_BOOKED;
    }

    reservations.emplace(seat_name, traveler);
    place->reserve();
    return ReservationResult::SUCCESS;
  }

  ReservationResult cancel(const std::string &seat_name,
                           const std::string &passenger_id) {
    const auto place = std::ranges::find(seats, seat_name, &seat::get_seat_name);
    if (place == seats.end()) {
      return ReservationResult::UNKNOWN_SEAT;
    }

    const auto booking = reservations.find(seat_name);
    if (booking == reservations.end() ||
        booking->second.get_id() != passenger_id) {
      return ReservationResult::NO_RESERVATION;
    }

    reservations.erase(booking);
    place->release();
    return ReservationResult::SUCCESS;
  }
};
