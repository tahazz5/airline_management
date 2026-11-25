#include "booking_labels.hpp"
#include "demo_flights.hpp"

#include <QApplication>
#include <QComboBox>
#include <QDateTime>
#include <QFont>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSignalBlocker>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include <string_view>
#include <vector>

namespace {
QString text(std::string_view value) {
  return QString::fromUtf8(value.data(), static_cast<int>(value.size()));
}

class BookingWindow : public QWidget {
  std::vector<flight> flights = demo_flights();
  QComboBox flight_choice;
  QListWidget seat_choice;
  QLabel schedule;
  QLabel availability;
  QLineEdit passenger_id;
  QLineEdit passenger_name;
  QLineEdit passenger_email;
  QPushButton reserve_button{QStringLiteral("Réserver")};
  QPushButton cancel_button{QStringLiteral("Annuler la réservation")};
  QLabel feedback;

public:
  BookingWindow() {
    setWindowTitle(QStringLiteral("Réservation de vol"));
    resize(760, 420);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(16);

    auto *title = new QLabel(QStringLiteral("Réserver un vol"));
    QFont title_font = title->font();
    title_font.setPointSize(20);
    title_font.setBold(true);
    title->setFont(title_font);
    layout->addWidget(title);

    auto *flight_form = new QFormLayout;
    flight_form->addRow(QStringLiteral("&Vol"), &flight_choice);
    flight_form->addRow(QStringLiteral("Horaires"), &schedule);
    layout->addLayout(flight_form);

    auto *panels = new QHBoxLayout;
    panels->setSpacing(16);
    layout->addLayout(panels, 1);

    auto *seat_group = new QGroupBox(QStringLiteral("Sièges"));
    auto *seat_layout = new QVBoxLayout(seat_group);
    seat_choice.setAccessibleName(QStringLiteral("Sièges du vol"));
    seat_choice.setAlternatingRowColors(true);
    seat_layout->addWidget(&seat_choice);
    seat_layout->addWidget(&availability);
    panels->addWidget(seat_group, 1);

    auto *passenger_group = new QGroupBox(QStringLiteral("Passager"));
    auto *passenger_layout = new QVBoxLayout(passenger_group);
    auto *form = new QFormLayout;
    form->addRow(QStringLiteral("&Identifiant du passager"), &passenger_id);
    form->addRow(QStringLiteral("&Nom"), &passenger_name);
    form->addRow(QStringLiteral("&E-mail"), &passenger_email);
    passenger_layout->addLayout(form);
    passenger_layout->addStretch();
    panels->addWidget(passenger_group, 1);

    auto *actions = new QHBoxLayout;
    actions->addStretch();
    actions->addWidget(&cancel_button);
    actions->addWidget(&reserve_button);
    passenger_layout->addLayout(actions);
    feedback.setWordWrap(true);
    feedback.setMinimumHeight(feedback.fontMetrics().lineSpacing() * 2);
    feedback.setTextFormat(Qt::PlainText);
    layout->addWidget(&feedback);
    layout->addWidget(new QLabel(
        QStringLiteral("Vols fictifs · Réservations conservées pendant la session"),
        this));

    for (const auto &trip : flights) {
      flight_choice.addItem(
          QStringLiteral("%1 · %2 → %3 · %4")
              .arg(text(trip.get_flight_number()), text(trip.get_source()),
                   text(trip.get_dest()), text(label(trip.get_status()))));
    }

    connect(&flight_choice, qOverload<int>(&QComboBox::currentIndexChanged), this,
            [this] { load_flight(); });
    connect(&seat_choice, &QListWidget::currentRowChanged, this,
            [this] { update_actions(); });
    connect(&reserve_button, &QPushButton::clicked, this,
            [this] { apply_reservation(false); });
    connect(&cancel_button, &QPushButton::clicked, this,
            [this] { apply_reservation(true); });
    load_flight();
  }

private:
  flight &selected_flight() { return flights.at(flight_choice.currentIndex()); }

  const seat &selected_seat() {
    return selected_flight().get_seats().at(seat_choice.currentRow());
  }

  void load_flight(int seat_index = 0) {
    const auto departure = QDateTime::fromSecsSinceEpoch(
        selected_flight().get_date_depart().time_since_epoch().count(), Qt::UTC);
    const auto arrival = QDateTime::fromSecsSinceEpoch(
        selected_flight().get_date_arrivee().time_since_epoch().count(), Qt::UTC);
    const auto format = QStringLiteral("dd/MM/yyyy HH:mm");
    schedule.setText(QStringLiteral("%1 → %2 UTC")
                         .arg(departure.toString(format), arrival.toString(format)));

    const QSignalBlocker block_seat_signals(&seat_choice);
    seat_choice.clear();
    for (const auto &place : selected_flight().get_seats()) {
      seat_choice.addItem(QStringLiteral("%1 · %2 · %3")
                              .arg(text(place.get_seat_name()),
                                   text(label(place.get_type())),
                                   text(label(place.get_status()))));
    }
    const auto &trip = selected_flight();
    availability.setText(
        trip.get_status() == Flightstatus::CANCELLED
            ? QStringLiteral("Réservations fermées")
            : QStringLiteral("%1 / %2 sièges disponibles")
                  .arg(static_cast<qlonglong>(trip.available_seats()))
                  .arg(trip.get_aircraft().get_total_seat()));
    seat_choice.setCurrentRow(seat_index);
    update_actions();
  }

  void update_actions() {
    const bool has_seat = seat_choice.currentRow() >= 0;
    const bool can_reserve =
        has_seat && selected_flight().get_status() != Flightstatus::CANCELLED &&
        selected_seat().get_status() == status_seat::AVAILABLE;
    reserve_button.setEnabled(can_reserve);
    const bool can_cancel =
        has_seat && selected_seat().get_status() == status_seat::RESERVED;
    cancel_button.setEnabled(can_cancel);
    passenger_id.setEnabled(can_reserve || can_cancel);
    passenger_name.setEnabled(can_reserve);
    passenger_email.setEnabled(can_reserve);
    feedback.setText(
        can_cancel ? QStringLiteral("Pour annuler, utilisez l’identifiant du passager "
                                     "associé à ce siège.")
                   : QString{});
  }

  void apply_reservation(bool cancelling) {
    const QString id = passenger_id.text().trimmed();
    if (id.isEmpty()) {
      feedback.setText(QStringLiteral("Saisissez l’identifiant du passager."));
      passenger_id.setFocus();
      return;
    }

    auto &trip = selected_flight();
    const auto seat_name = selected_seat().get_seat_name();
    ReservationResult result;
    if (cancelling) {
      result = trip.cancel(seat_name, id.toStdString());
    } else {
      const QString name = passenger_name.text().trimmed();
      const QString email = passenger_email.text().trimmed();
      if (name.isEmpty() || email.isEmpty()) {
        feedback.setText(QStringLiteral("Saisissez le nom et l’adresse e-mail."));
        (name.isEmpty() ? passenger_name : passenger_email).setFocus();
        return;
      }
      result = trip.reserve(seat_name, passenger{id.toStdString(), name.toStdString(),
                                                 email.toStdString()});
    }

    if (result == ReservationResult::SUCCESS) {
      load_flight(seat_choice.currentRow());
      feedback.setText(
          (cancelling ? QStringLiteral("Réservation annulée pour %1, siège %2.")
                      : QStringLiteral("Réservation confirmée pour %1, siège %2."))
              .arg(text(trip.get_flight_number()), text(seat_name)));
    } else {
      feedback.setText(text(label(result)));
    }
  }
};
} // namespace

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  BookingWindow window;
  window.show();
  return app.exec();
}
