#pragma once

#include <string>
class passenger {
  std::string id;
  std::string name;
  std::string email;

public:
  passenger(std::string id, std::string name, std::string email)
      : id(id), name(name), email(email) {}
  const std::string &get_name() const { return name; }
  const std::string &get_email() const { return email; }
  std::string get_id() const { return id; }
};
