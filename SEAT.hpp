#pragma once

#include <string>

enum class type_seat {
  ECONOMY,
  pre_economy,
  BUSINESS,
  CLASS_first

};

enum class status_seat {
  AVAILABLE,
  RESERVED,
  OCCUPEED

};

class seat {
private:
  std::string seat_name;
  type_seat type;
  status_seat status;

public:
  seat(std::string seat, type_seat t, status_seat s)
      : seat_name(seat), type(t), status(s) {}
  std::string get_seat_name() const { return seat_name; }

  type_seat get_type() const { return type; }
  status_seat get_status() const { return status; }

  void reserve() { status = status_seat::RESERVED; }
  void release() { status = status_seat::AVAILABLE; }
  bool booked() const {
    return status == status_seat::RESERVED || status == status_seat::OCCUPEED;
  }
};
