#include "gamelib.h"
#include <stdio.h>
#include <stdlib.h>

int gIndex;

void menu()
{
  //system("clear");
  printf("-----------------------------------------------------------\n");
  printf("| Benvenuto in Phalsofobia, un gioco di Alberto Boccolini |\n");
  printf("-----------------------------------------------------------\n");
  printf("\n");
  freopen(NULL, "rb", stdin);
  printf("Menù di selezione\n");
  printf("1 - Imposta gioco\n");
  printf("2 - Gioca\n");
  printf("3 - Termina gioco\n");
  printf("\nInserisci: ");
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
