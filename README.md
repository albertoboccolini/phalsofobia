# PhalsoPhobia: By Alberto Boccolini

## Getting Started

1. Clone this repository.

2. Navigate to this folder with terminal

```bash
   $ cd phalsofobia
```

3. Compile main.c and gamelib.c with this command

```bash
   $ gcc -o phalsofobia main.c gamelib.c -std=c11 -Wall
```

4. Start the game and have fun

```bash
   $ ./phalsofobia
```

## Play in no-UNIX systems with Docker

Use the **[docker image](https://hub.docker.com/r/albertoboccolini/phalsofobia)** to run Phalsofobia

```bash
   $ docker run -it quellodeibeat/phalsofobia:latest
```

## Pull your personal Phalsofobia version

1. Clone this repository.

2. Navigate to this folder with terminal.

```bash
   $ cd phalsofobia
```

3. Build the image (replace x.y.z with your release name).

```bash
   $ docker build -t albertoboccolini/phalsofobia:x.y.z .
```

Push the docker image just built to docker hub (replace x.y.z with your release name).

```bash
   $ docker push albertoboccolini/phalsofobia:x.y.z
```


## Design choices

1. Add the `impostazioneGioco()` function to separate the actual settings section from the `imposta_gioco()` function, which, in case of a repeat game, displays the menu by communicating to the user that the game had already been set the appropriate functions: `impostazioneGioco()` for modifying the settings, `gioca()` for starting the game.
2. Add the `convertiOggetto()` function that takes an integer assigned to objects and challenges and prints the corresponding string, this to address the problem of enum types, for which it is not possible to print objects or challenges, replacing `%d` with `%s` or `%c`.
3. Added the `convertiZona()` function that takes an integer assigned to zones and prints the corresponding string, this to address the problem of enum types, for which it is not possible to print zones, replacing `%d` with `%s` or `%c`.
4. Add the `generaTurni()` function that takes an array of integers with as many positions as there are players and the number of them. The function generates integers based on the number of players and checks for duplicates, then inserts them in random order into the array that will be returned after the function call. This ensures that at the beginning of each turn, a new sequence of players is generated that are not repeated and prevent one of them from being called to play more than once in that turn.
5. Add the `giocaTurno()` function that simply serves as a turn menu for each player. It initially calls the `generaTurni()` function and based on the generated order makes each player play their turn, where they can choose from various options and call the corresponding functions. This can only be left in case of victory or loss of the game and every time all players complete their turn, it re-sets the global variable `gSale`, as in the case where in that turn it is set to 1 using the salt object, in case of the ghost's appearance the players will not receive the decrement of mental health and this variable is updated at the end of each turn.
6. Add the `riduciSanita()` function that first checks the global variable gSale, if it is set to 1, it does not decrement the health of any of the players, as it indicates that in that turn a player used the salt object, but otherwise it decrements the health of all players in the zone where the ghost appeared. In addition, the function performs a check, as if after the decrement, the player in question reaches 0 mental health, the `morte()` function will be called which will handle the loss of the player in question.
7. Add the `morte()` function which, in case the dead player had a challenge, relocates it in the zone where the player died. Then the function gives the corresponding object to the challenge collected by the dead player to another player randomly, in order to avoid that it is no longer possible to complete the game. Finally, the function re-allocates the memory used by the dead player, sets the pointer to NULL, decrements the gNGiocatori variable and interrupts the whole turn, this to ensure that a new turn is generated with only the remaining players and avoid the turn going to the dead player. If decrementing the gNGiocatori variable makes it 0, then the `morte()` function calls the `perso()` function.
8. Add the `perso()` function that prints a simple message to communicate to the user that: "the game is lost!!" and sets the global variable gEsito to 2, which represents the loss of the game. Pressing enter will return to the initial menu, called through `the menu()` function from the `main()` function.
9. Add the `vittoria()` function that prints a simple message: "You won!!" and sets the global variable gEsito to 1, which represents the victory of the game. Pressing enter will return to the initial menu, called through the `menu()` function from the `main()` function.
10. When the user uses the `tornaCaravan()` function, the function, in addition to checking the challenges as completed and giving another object for the challenge research, performs a check. In case the user who goes to the caravan has an object for which the challenge has already been found, the function replaces that object with one for which the challenge is missing. This check serves to speed up the game in case 2 out of 3 challenges have already been found, and to avoid players with an initial object different from the one necessary to obtain the last one, continue to go around uselessly without being able to collect the remaining challenge.
11. `lControlloInt`, used in various parts of the program, serves to check that the user in the various menus, at the request of an integer input, does not enter a character.

created by albertoboccolini.
