# Gestion des vols

Application console en français, écrite en C++20. Le menu permet de lister les
vols, de rechercher un trajet par codes d’aéroport et par date de départ
facultative, de consulter les sièges, de réserver et d’annuler une réservation.

## Compiler et lancer

Utilisez CMake 3.16 ou supérieur et un compilateur compatible C++20.

```sh
cmake -S . -B build
cmake --build build
./build/airline_management
```

## Essayer une réservation

1. Recherchez un trajet de `CDG` vers `NCE` le `2027-01-15`.
2. Affichez les sièges du vol `AM101`, puis réservez le siège `2A` avec
   un identifiant de passager, un nom et une adresse e-mail.
3. Annulez la réservation avec le même numéro de vol, le même siège et le
   même identifiant de passager. Le siège redevient disponible.

Les horaires sont affichés en UTC. L’application charge quatre vols fictifs
au démarrage. Les réservations sont conservées en mémoire jusqu’à la fermeture.

## Interface graphique Qt

Installez les fichiers de développement de Qt Widgets (Qt 6 ou Qt 5). Sur
Ubuntu, le paquet Qt 6 s’appelle `qt6-base-dev`.

```sh
cmake -S . -B build -DAIRLINE_BUILD_QT=ON
cmake --build build
./build/airline_booking
```

Choisissez un vol pour voir ses horaires, la classe de chaque siège et le
nombre de places disponibles. Sélectionnez un siège disponible, puis saisissez
l’identifiant, le nom et l’adresse e-mail du passager avant de cliquer sur
**Réserver**. Pour annuler, sélectionnez le siège réservé et renseignez le
même identifiant.
Les deux interfaces démarrent chacune avec leur propre session de démonstration.
