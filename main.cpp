#include "booking_labels.hpp"
#include "demo_flights.hpp"
#include "flight_search.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <chrono>
#include <cctype>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace {
using namespace std::chrono;

std::string upper(std::string value) {
  for (char &letter : value) {
    letter = static_cast<char>(std::toupper(static_cast<unsigned char>(letter)));
  }
  return value;
}

std::optional<sys_days> parse_date(std::string_view value) {
  if (value.size() != 10 || value[4] != '-' || value[7] != '-') {
    return std::nullopt;
  }
  std::array<unsigned, 3> fields{};
  const std::array offsets{0, 5, 8};
  const std::array widths{4, 2, 2};
  for (std::size_t i = 0; i < fields.size(); ++i) {
    const char *first = value.data() + offsets[i];
    const char *last = first + widths[i];
    const auto [end, error] = std::from_chars(first, last, fields[i]);
    if (end != last || error != std::errc{}) {
      return std::nullopt;
    }
  }
  const auto date = year{static_cast<int>(fields[0])} / month{fields[1]} /
                    day{fields[2]};
  if (!date.ok() || fields[0] == 0) {
    return std::nullopt;
  }
  return sys_days{date};
}

void print_time(sys_seconds time) {
  const auto timestamp = system_clock::to_time_t(time);
  if (const auto *utc = std::gmtime(&timestamp)) {
    std::cout << std::put_time(utc, "%Y-%m-%d %H:%M") << " UTC";
  }
}

void print_flight(const flight &trip) {
  std::cout << trip.get_flight_number() << "  " << trip.get_source() << " → "
            << trip.get_dest() << "  ";
  print_time(trip.get_date_depart());
  std::cout << " → ";
  print_time(trip.get_date_arrivee());
  std::cout << "  " << label(trip.get_status()) << "  "
            << trip.get_aircraft().get_model() << "  ";
  if (trip.get_status() != Flightstatus::CANCELLED) {
    std::cout << trip.available_seats() << " siège(s) disponible(s)";
  }
  std::cout << '\n';
}

bool read(std::string_view prompt, std::string &value) {
  std::cout << prompt;
  if (!std::getline(std::cin, value)) {
    return false;
  }
  const auto first = value.find_first_not_of(" \t\r");
  if (first == std::string::npos) {
    value.clear();
  } else {
    value = value.substr(first, value.find_last_not_of(" \t\r") - first + 1);
  }
  return true;
}

bool search(const std::vector<flight> &flights) {
  std::string source, destination, input;
  if (!read("Départ (code aéroport) : ", source) ||
      !read("Destination (code aéroport) : ", destination) ||
      !read("Date de départ UTC (AAAA-MM-JJ, facultative) : ", input)) {
    return false;
  }

  const auto date = parse_date(input);
  if (!input.empty() && !date) {
    std::cout << "Date invalide. Utilisez le format AAAA-MM-JJ.\n";
    return true;
  }
  const auto matches = search_flights(flights, upper(source), upper(destination), date);
  if (matches.empty()) {
    std::cout << "Aucun vol trouvé.\n";
  }
  for (const flight &trip : matches) {
    print_flight(trip);
  }
  return true;
}

bool manage_flight(std::vector<flight> &flights, const std::string &action) {
  std::string number;
  if (!read("Numéro de vol : ", number)) {
    return false;
  }
  const auto trip = std::ranges::find(flights, upper(number), &flight::get_flight_number);
  if (trip == flights.end()) {
    std::cout << "Vol introuvable.\n";
    return true;
  }
  if (action == "3") {
    print_flight(*trip);
    for (const auto &place : trip->get_seats()) {
      std::cout << place.get_seat_name() << "  " << label(place.get_type())
                << "  " << label(place.get_status()) << '\n';
    }
    return true;
  }
  if (action == "4" && trip->get_status() == Flightstatus::CANCELLED) {
    std::cout << label(ReservationResult::FLIGHT_CANCELLED) << '\n';
    return true;
  }

  std::string seat_name, id;
  if (!read("Siège : ", seat_name) || !read("Identifiant du passager : ", id)) {
    return false;
  }
  if (seat_name.empty() || id.empty()) {
    std::cout << "Le siège et l’identifiant du passager sont obligatoires.\n";
    return true;
  }
  if (action == "5") {
    std::cout << label(trip->cancel(upper(seat_name), id)) << '\n';
    return true;
  }

  std::string name, email;
  if (!read("Nom du passager : ", name) || !read("Adresse e-mail : ", email)) {
    return false;
  }
  if (name.empty() || email.empty()) {
    std::cout << "Le nom et l’adresse e-mail sont obligatoires.\n";
    return true;
  }
  std::cout << label(trip->reserve(upper(seat_name), passenger{id, name, email}))
            << '\n';
  return true;
}
} // namespace

int main() {
  auto flights = demo_flights();
  std::cout << "Gestion des vols · Données de démonstration\n";
  std::string action;
  while (read("\n1. Lister les vols\n2. Rechercher un vol\n3. Voir les sièges\n"
              "4. Réserver un siège\n5. Annuler une réservation\n0. Quitter\n"
              "Choix : ", action)) {
    if (action == "0") {
      break;
    }
    if (action == "1") {
      for (const auto &trip : flights) {
        print_flight(trip);
      }
    } else if (action == "2") {
      if (!search(flights)) {
        break;
      }
    } else if (action == "3" || action == "4" || action == "5") {
      if (!manage_flight(flights, action)) {
        break;
      }
    } else {
      std::cout << "Choix invalide.\n";
    }
  }
}
