#pragma once

#include <string>

class Aircraft {
private:
  std::string tail_number;
  std::string model;
  int total_seat;

public:
  Aircraft(std::string tail_number, std::string model, int total_seat)
      : tail_number(tail_number), model(model), total_seat(total_seat) {}
  const std::string &get_model() const { return model; }
  int get_total_seat() const { return total_seat; }
  std::string get_tail_number() const { return tail_number; }
};
