#pragma once

#include "flight.hpp"

#include <string_view>

inline std::string_view label(Flightstatus status) {
  switch (status) {
  case Flightstatus::ON_TIME: return "À l'heure";
  case Flightstatus::DELAYED: return "Retardé";
  case Flightstatus::CANCELLED: return "Annulé";
  }
  return {};
}

inline std::string_view label(type_seat type) {
  switch (type) {
  case type_seat::ECONOMY: return "Économie";
  case type_seat::pre_economy: return "Économie premium";
  case type_seat::BUSINESS: return "Affaires";
  case type_seat::CLASS_first: return "Première";
  }
  return {};
}

inline std::string_view label(status_seat status) {
  switch (status) {
  case status_seat::AVAILABLE: return "Disponible";
  case status_seat::RESERVED: return "Réservé";
  case status_seat::OCCUPEED: return "Occupé";
  }
  return {};
}

inline std::string_view label(ReservationResult result) {
  switch (result) {
  case ReservationResult::SUCCESS: return "Opération confirmée.";
  case ReservationResult::FLIGHT_CANCELLED: return "Ce vol est annulé.";
  case ReservationResult::UNKNOWN_SEAT: return "Siège introuvable.";
  case ReservationResult::SEAT_UNAVAILABLE: return "Ce siège est indisponible.";
  case ReservationResult::PASSENGER_ALREADY_BOOKED:
    return "Ce passager a déjà une réservation sur ce vol.";
  case ReservationResult::NO_RESERVATION:
    return "Aucune réservation pour ce passager sur ce siège.";
  }
  return {};
}
