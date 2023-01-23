#include "gamelib.h"
#include <stdio.h>
#include <stdlib.h>

int gIndex;

void menu()
{
  system("clear");
  printf("\033[1;39m");
  printf("  _____ _          _          __      _     _       \n");
  printf(" |  _  | |___ __ _| |___ ____| _|____| |___(_)__ _ \n");
  printf(" | |_) | '_  | _` | | __| _  ||_| _  | '_  | | _` |\n");
  printf(" |  ___| | | |(_| | |__ |(_) | _|(_) | |_) | |(_| |\n");
  printf(" |__|  |_| |_|__,_|_|___|____|_||____|_.___|_|__,_|\n");
  printf("----------------------------------------------------\n");
  printf("|     \033[5;39mun gioco sviluppato da Alberto Boccolini \033[0m\033[1;39m    |\n");
  printf("----------------------------------------------------\n");
  printf("\n");
  freopen(NULL, "rb", stdin);
  printf("Menù di selezione\n");
  printf("\033[3;33m1 - Imposta gioco\n");
  printf("\033[3;32m2 - Gioca\n");
  printf("\033[3;36m3 - Termina gioco\n");
  printf("\033[1;39m");
  printf("\n\033[0m\033[1;39mInserisci: ");
  int lControlloInt = scanf("%i", &gIndex);
  /* in caso in cui l'utente digiti un carattere
  non contenuto nel menù, il programma restituisce un errore*/
  while (gIndex < 1 || gIndex > 3 || !lControlloInt)
  {
    printf("\n");
    freopen(NULL, "rb", stdin);
    printf("Errore:\nIl numero inserito non corrisponde a nessuna funzione del menù\n\n");
    printf("Inserisci: ");
    lControlloInt = scanf("%i", &gIndex);
  }
  switch (gIndex)
  {
  case 1:
    imposta_gioco();
    menu();
    break;
  case 2:
    gioca();
    menu();
    break;
  case 3:
    termina_gioco();
    break;
  }
}

int main()
{
  do
  {
    menu();
  } while (gIndex != 3);
  return 0;
}
