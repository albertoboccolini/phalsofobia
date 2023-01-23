#include "gamelib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Variabili globali mappa
static Zona_mappa *gPCaravan;   // contiene il puntatore al caravan
static Zona_mappa *gPPrimaZona; // contiene il puntatore alla prima zona della mappa
static Zona_mappa *gPMappa[7];  // array che contiene tutti i puntatori delle zone della mappa
static int gCreazioneMappa;     // impostata ad 1 se l'utente termina l'impostazione della mappa con l'apposita funzione
static int gZoneEsistenti;      // contiene il numero di zone esistenti
// Variabili globali fantasma
static int gProbabilitaApparizione; // contiene la probabilità di apparizione del fantasma, scelta in base alla difficoltà
static int gFantasma;               // viene impostata ad 1 quando il fantasma appare
// Variabili globali prove
static int gProvaEMF = 0;         // viene impostata ad 1 se un giocatore torna al caravan con la prova_EMF
static int gProvaSpiritBox = 0;   // viene impostata ad 1 se un giocatore torna al caravan con la prova_spirit_box
static int gProvaVideocamera = 0; // viene impostata ad 1 se un giocatore torna al caravan con la prova_videocamera
static int gCountProve = 0;       // conta quante delle prove che sono state trovate
// Variabili globali di gioco
static int gAvanzato = 0;                    // viene impostata ad 1 se un giocatore avanza di zona
static Giocatore *gPGiocatori[4];            // array che contiene i puntatori ad ogni giocatore
static Giocatore *gPGiocatoriPrecedenti[4];  // array che contiene i puntatori ad ogni giocatore precedente
static int gNGiocatori;                      // contiene il numero dei giocatori della partita attuale
static int gNGiocatoriPrecedenti;            // contiene il numero dei giocatori precedenti
static int gDifficolta;                      // contiene la difficoltà della partita
static int gEsito;                           // variabile che contiene l'esito: 1 = Vittoria 2 = Perdita
static int gMorte;                           // variabile che viene impostata ad 1 se durante un turno muore un giocatore
static int gSale;                            // variabile che si aggiorna quando viene utilizzato l'oggetto sale
static int gMatrice[5][4];                   // matrice che contiene tutti gli zaini dei giocatori precedenti
static char gNomiGiocatoriPrecedenti[5][50]; // matrice che contiene tutti i nomi dei giocatori precedenti
static int gTurnoIniziale[5];

static void perso()
{
  printf("\n");
  system("clear");
  printf("------------------------\n");
  printf("| La partita è persa!! |\n");
  printf("------------------------\n");
  /* imposto gEsito a 2,
  poiché corrisponde alla perdita della partita*/
  gEsito = 2;
}

static void morte(int count, Zona_mappa *zonaMappa)
{
  printf("%s, sei morto\n", gPGiocatori[count]->nome_giocatore);
  int lGiocatore;
  /* se l'ultimo giocatore muore, decremento la variabile
  gNGiocatori e chiamo la funzione perso()*/
  if (gNGiocatori == 1)
  {
    gNGiocatori = gNGiocatori - 1;
    perso();
    return;
  }
  else if (gNGiocatori != 0)
  {
    /* assegno casualmente ad un giocatore l'oggetto
    corrispondente alla prova trovata dal giocatore
    morto e ri-colloco la prova nella zona in cui era*/
    do
    {
      lGiocatore = rand() % gNGiocatori;
    } while (lGiocatore == count);
    for (int lCount = 0; lCount < 4; lCount++)
    {
      switch (gPGiocatori[count]->zaino[lCount])
      {
      case prova_EMF:
        zonaMappa->prova = prova_EMF;
        gPGiocatori[lGiocatore]->zaino[lCount] = EMF;
        break;
      case prova_spirit_box:
        zonaMappa->prova = prova_spirit_box;
        gPGiocatori[lGiocatore]->zaino[lCount] = spirit_box;
        break;
      case prova_videocamera:
        zonaMappa->prova = prova_videocamera;
        gPGiocatori[lGiocatore]->zaino[lCount] = videocamera;
        break;
      }
    }
    /* decremento la probabilità di apparizione del fantasma
    poiché un giocatore è morto e dunque la prova non è stata
    raccolta da nessuno*/
    gProbabilitaApparizione = gProbabilitaApparizione - 5;
    /* alloco nuovamente la memoria occupata dal giocatore morto
    ed imposto il suo puntatore a NULL*/
    gPGiocatori[count] = realloc(gPGiocatori[count], sizeof(Giocatore));
    gPGiocatori[count] = NULL;
    /* lPerso[gNGiocatori] serve a controllare quali puntatori
    sono uguali a NULL, poiché in una volta possono morire più
    giocatori e l'array gPGiocatori potrebbe non essere correttamente
    pulito*/
    int lPerso[gNGiocatori];
    /* sposto tutti i puntatori dei giocatori, in modo da
    non avere problemi durante il gioco*/
    do
    {
      for (int lCount = gNGiocatori - 1; lCount >= 0; lCount--)
      {
        if (gPGiocatori[lCount] == NULL)
        {
          Giocatore *temp = gPGiocatori[lCount + 1];
          gPGiocatori[lCount + 1] = gPGiocatori[lCount];
          gPGiocatori[lCount] = temp;
        }
      }
    } while (gPGiocatori[gNGiocatori - 1] != NULL);
    /* controllo quali puntatori sono uguali a NULL
    ed imposto lPerso di conseguenza: 1 se il puntatore è NULL,
    0 se non è NULL*/
    for (int lCount = gNGiocatori - 1; lCount >= 0; lCount--)
    {
      if (gPGiocatori[lCount] == NULL)
      {
        lPerso[lCount] = 1;
      }
      else
      {
        lPerso[lCount] = 0;
      }
    }
    /* se vi è almeno un puntatore non uguale a NULL, quindi di cui
    lPerso[lCount] è uguale a 0, allora viene solo decrementata la variabile
    gNGiocatori, in caso in cui vi siano tutti puntatori a NULL,
    oppure dopo il decremento gNGiocatori sia uguale a 0,
    allora viene chiamata la funzione perso()*/
    for (int lCount = gNGiocatori - 1; lCount >= 0; lCount--)
    {
      if (lPerso[lCount] == 0)
      {
        gEsito = 0;
        break;
      }
      else
      {
        gEsito = 2;
      }
    }
    if (gEsito == 2)
    {
      perso();
      return;
    }
    else
    {
      gNGiocatori = gNGiocatori - 1;
      if (gNGiocatori == 0)
      {
        perso();
      }
    }
  }
  else
  {
    perso();
  }
}

static void riduciSanita(Zona_mappa *zona)
{
  // se gSale è impostata ad 1 non decrementa la sanità di nessun'giocatore
  if (gSale != 1)
  {
    /* controllo ogni giocatore, e se si trova nella zona dove è apparso
    il fantasma, decremento la sanità mentale in base alla difficoltà*/
    for (int lCount = 0; lCount < gNGiocatori; lCount++)
    {
      if (gPGiocatori[lCount]->posizione == zona)
      {
        switch (gDifficolta)
        {
        case 0:
          gPGiocatori[lCount]->sanita_mentale = gPGiocatori[lCount]->sanita_mentale - 20;
          break;
        case 1:
          gPGiocatori[lCount]->sanita_mentale = gPGiocatori[lCount]->sanita_mentale - 40;
          break;
        case 2:
          gPGiocatori[lCount]->sanita_mentale = gPGiocatori[lCount]->sanita_mentale - 60;
          break;
        }
        /* essendo che, se la sanità di un giocatore scende sotto lo zero,
        viene comunque rappresentata da un intero, essa potrebbe anche essere un numero
        al di sopra di 100, oltre che 0, quindi in questi 2 casi chiamo la funzione di morte,
        imposto la variabile globale gAvanzato a 0 per poter permettere al giocatore successivo
        di avanzare ed interrompo il turno impostando gMorte ad 1 per comunicare al programma
        che in questo turno sono morti uno o più giocatori*/
        if (gPGiocatori[lCount]->sanita_mentale < 0 || gPGiocatori[lCount]->sanita_mentale > 100)
        {
          morte(lCount, gPGiocatori[lCount]->posizione);
          gAvanzato = 0;
          gMorte = 1;
        }
        else
        {
          printf("%s, la tua sanità mentale è scesa a: %u\n", gPGiocatori[lCount]->nome_giocatore, gPGiocatori[lCount]->sanita_mentale);
        }
      }
    }
  }
}

static void convertiOggetto(int lOggettoGenerato)
{
  switch (lOggettoGenerato)
  {
  case EMF:
    printf("EMF");
    break;
  case spirit_box:
    printf("spirit_box");
    break;
  case videocamera:
    printf("videocamera");
    break;
  case calmanti:
    printf("calmanti");
    break;
  case sale:
    printf("sale");
    break;
  case adrenalina:
    printf("adrenalina");
    break;
  case cento_dollari:
    printf("cento_dollari");
    break;
  case coltello:
    printf("coltello");
    break;
  case nessun_oggetto:
    printf("nessun_oggetto");
    break;
  case prova_EMF:
    printf("prova_EMF");
    break;
  case prova_spirit_box:
    printf("prova_spirit_box");
    break;
  case prova_videocamera:
    printf("prova_videocamera");
    break;
  case nessuna_prova:
    printf("nessuna_prova");
    break;
  case 99:
    printf("vuoto");
    break;
  }
}

static void convertiZona(int zonaMappa)
{
  switch (zonaMappa)
  {
  case caravan:
    printf("Caravan");
    break;
  case cucina:
    printf("Cucina");
    break;
  case soggiorno:
    printf("Soggiorno");
    break;
  case camera:
    printf("Camera");
    break;
  case bagno:
    printf("Bagno");
    break;
  case garage:
    printf("Garage");
    break;
  case seminterrato:
    printf("Seminterrato");
    break;
  }
}

static int *generaTurni(int controllo[gNGiocatori], int nGiocatori)
{
  /* se il numero di giocatori è uno viene
  generato solo un numero e non vengono controllati i duplicati*/
  if (nGiocatori == 1)
  {
    controllo[nGiocatori] = rand() % nGiocatori;
  }
  else
  {
    /* variabile che viene aggiornata in caso
    in cui il for trovi un duplicato*/
    int duplicati;
    /* genero tanti numeri quanti i giocatori*/
    for (int count = nGiocatori; count > 0; count--)
    {
      controllo[count] = rand() % nGiocatori;
    }
    do
    {
      duplicati = 0;
      /* for che viene eseguito finché ci sono dei duplicati
      nell'array controllo*/
      for (int count1 = nGiocatori; count1 > 0; count1--)
      {
        for (int count2 = count1 - 1; count2 > 0; count2--)
        {
          if (controllo[count1] == controllo[count2])
          {
            controllo[count2] = rand() % nGiocatori;
            duplicati = 1;
          }
        }
      }
    } while (duplicati);
  }
  return controllo;
}

static void inserisciZona()
{
  if (gZoneEsistenti < 6)
  {
    /* se non esiste nessuna zona creo la zona caravan
    e la prima zona*/
    if (gZoneEsistenti == 0)
    {
      gZoneEsistenti = 1;
      gPCaravan = (Zona_mappa *)realloc(gPCaravan, sizeof(Zona_mappa));
      gPPrimaZona = (Zona_mappa *)realloc(gPPrimaZona, sizeof(Zona_mappa));
      gPPrimaZona->zona = rand() % 6 + 1;
      gPCaravan->zona = 0;
      gPCaravan->prossima_zona = gPPrimaZona;
      gPMappa[0] = gPCaravan;
      gPMappa[1] = gPPrimaZona;
    }
    else
    {
      /* in questo caso incremento il numero di zone,
      creo una zona temporanea dove inserire i vari dati
      e la inserisco nella posizione della mappa corrispondente*/
      gZoneEsistenti++;
      Zona_mappa *pZonaCorrente = (Zona_mappa *)malloc(sizeof(Zona_mappa));
      pZonaCorrente->zona = rand() % 6 + 1;
      /* for che controlla che la zona
      non sia stata già inserita*/
      int duplicati = 0;
      do
      {
        duplicati = 0;
        // for che viene eseguito finché ci sono dei duplicati
        for (int count1 = 1; count1 <= gZoneEsistenti; count1++)
        {
          for (int count2 = 1; count2 <= count1 - 1; count2++)
          {
            if (pZonaCorrente->zona == gPMappa[count2]->zona)
            {
              pZonaCorrente->zona = rand() % 6 + 1;
              duplicati = 1;
            }
          }
        }
      } while (duplicati);
      /* il campo prossima_zona della zona precedente
      punterà a pZonaCorrente*/
      gPMappa[gZoneEsistenti - 1]->prossima_zona = pZonaCorrente;
      gPMappa[gZoneEsistenti] = pZonaCorrente;
    }
    /* genero l'oggetto della zona in base alle probabilità
    fornite nel testo del progetto*/
    enum Tipo_oggetto_zona lOggettoZona;
    int lNum = rand() % 100 + 1;
    if (lNum <= 40)
    {
      lOggettoZona = nessun_oggetto;
    }
    else if (lNum > 40 && lNum <= 60)
    {
      lOggettoZona = adrenalina;
    }
    else if (lNum > 60 && lNum <= 80)
    {
      lOggettoZona = cento_dollari;
    }
    else if (lNum > 80 && lNum <= 100)
    {
      lOggettoZona = coltello;
    }
    gPMappa[gZoneEsistenti]->oggetto_zona = lOggettoZona;
    printf("\n");
    system("clear");
    printf("-------------------------------\n");
    printf("| Zona inserita correttamente |\n");
    printf("-------------------------------\n");
    char lIndex;
    printf("\nPremi invio per tornare al menù mappa: ");
    freopen(NULL, "rb", stdin);
    scanf("%c", &lIndex);
    while (lIndex != '\n')
    {
      printf("Errore:\nCarattere errato\n");
      printf("\nPremi invio per tornare al menù mappa: ");
      freopen(NULL, "rb", stdin);
      scanf("%c", &lIndex);
    }
  }
  else
  {
    printf("\n");
    system("clear");
    printf("-----------------------------------------------\n");
    printf("| Massimo numero di zone inseribili raggiunto |\n");
    printf("-----------------------------------------------\n");
    char lIndex;
    printf("\nPremi invio per tornare al menù mappa: ");
    freopen(NULL, "rb", stdin);
    scanf("%c", &lIndex);
    while (lIndex != '\n')
    {
      printf("Errore:\nCarattere errato\n");
      printf("\nPremi invio per tornare al menù mappa: ");
      freopen(NULL, "rb", stdin);
      scanf("%c", &lIndex);
    }
  }
}

static void cancellaZone()
{
  if (gZoneEsistenti == 0)
  {
    printf("\n");
    system("clear");
    printf("----------------------------------------------\n");
    printf("| Mappa vuota, si prega di inserire una zona |\n");
    printf("----------------------------------------------\n");
    char lIndex;
    printf("\nPremi invio per tornare al menù mappa: ");
    freopen(NULL, "rb", stdin);
    scanf("%c", &lIndex);
    while (lIndex != '\n')
    {
      printf("Errore:\nCarattere errato\n");
      printf("\nPremi invio per tornare al menù mappa: ");
      freopen(NULL, "rb", stdin);
      scanf("%c", &lIndex);
    }
  }
  else
  {
    printf("\n");
    system("clear");
    printf("-----------------\n");
    printf("| Zone presenti |\n");
    printf("-----------------\n");
    printf("\n");
    // stampo le zone presenti nella mappa
    for (int count = 1; count <= gZoneEsistenti; count++)
    {
      printf("Zona %d: ", count);
      convertiZona(gPMappa[count]->zona);
      printf("\n");
    }
    // chiedo il numero della zona da eliminare all'utente
    int zonaDaEliminare;
    freopen(NULL, "rb", stdin);
    printf("\nInserisci numero zona da eliminare (0 per tornare al menù mappa): ");
    int lControllo = scanf("%d", &zonaDaEliminare);
    while (zonaDaEliminare < 0 || zonaDaEliminare > gZoneEsistenti || !lControllo)
    {
      printf("\n");
      freopen(NULL, "rb", stdin);
      printf("Errore:\nIl numero inserito non corrisponde a nessuna funzione del menù\n");
      printf("\nInserisci numero zona da eliminare (0 per tornare al menù mappa): ");
      lControllo = scanf("%d", &zonaDaEliminare);
    }
    if (zonaDaEliminare != 0)
    {
      free(gPMappa[zonaDaEliminare]);
      /* for che sposta le varie
      zone della mappa quando una viene eliminata*/
      for (int count = zonaDaEliminare; count <= gZoneEsistenti; count++)
      {
        Zona_mappa *temp;
        temp = gPMappa[count];
        gPMappa[count] = gPMappa[count + 1];
        // modifico il puntatore alla prossima zona
        gPMappa[count - 1]->prossima_zona = gPMappa[count];
        gPMappa[count + 1] = temp;
      }
      gZoneEsistenti--;
      printf("\n");
      system("clear");
      printf("--------------------------------\n");
      printf("| Zona eliminata correttamente |\n");
      printf("--------------------------------\n");
      char lIndex;
      printf("\nPremi invio per tornare al menù mappa: ");
      freopen(NULL, "rb", stdin);
      scanf("%c", &lIndex);
      while (lIndex != '\n')
      {
        printf("Errore:\nCarattere errato\n");
        printf("\nPremi invio per tornare al menù mappa: ");
        freopen(NULL, "rb", stdin);
        scanf("%c", &lIndex);
      }
    }
  }
}

static void stampaMappa()
{
  if (gZoneEsistenti == 0)
  {
    printf("\n");
    system("clear");
    printf("----------------------------------------------\n");
    printf("| Mappa vuota, si prega di inserire una zona |\n");
    printf("----------------------------------------------\n");
    char lIndex;
    printf("\nPremi invio per tornare al menù mappa: ");
    freopen(NULL, "rb", stdin);
    scanf("%c", &lIndex);
    while (lIndex != '\n')
    {
      printf("Errore:\nCarattere errato\n");
      printf("\nPremi invio per tornare al menù mappa: ");
      freopen(NULL, "rb", stdin);
      scanf("%c", &lIndex);
    }
  }
  else
  {
    printf("\n");
    system("clear");
    printf("------------------\n");
    printf("| Mappa di gioco |\n");
    printf("------------------\n");
    printf("\n");
    // stampo le zone presenti nella mappa
    for (int count = 1; count <= gZoneEsistenti; count++)
    {
      printf("Zona %d: ", count);
      convertiZona(gPMappa[count]->zona);
      printf("\n");
    }
    char lIndex;
    printf("\nPremi invio per tornare al menù mappa: ");
    freopen(NULL, "rb", stdin);
    scanf("%c", &lIndex);
    while (lIndex != '\n')
    {
      printf("Errore:\nCarattere errato\n");
      printf("\nPremi invio per tornare al menù mappa: ");
      freopen(NULL, "rb", stdin);
      scanf("%c", &lIndex);
    }
  }
}

static void chiudiMappa()
{
  if (gZoneEsistenti < 2)
  {
    printf("\n");
    system("clear");
    printf("--------------------------------------------------------------\n");
    printf("| Impossibile iniziare una partita: zone minime necessarie 2 |\n");
    printf("--------------------------------------------------------------\n");
    char lIndex;
    printf("\nPremi invio per tornare al menù mappa: ");
    freopen(NULL, "rb", stdin);
    scanf("%c", &lIndex);
    while (lIndex != '\n')
    {
      printf("Errore:\nCarattere errato\n");
      printf("\nPremi invio per tornare al menù mappa: ");
      freopen(NULL, "rb", stdin);
      scanf("%c", &lIndex);
    }
  }
  else
  {
    /* imposto gCreazioneMappa ad 1 in modo da comunicare
    al programma che l'utente ha terminato l'impostazione della mappa*/
    gCreazioneMappa = 1;
    // cambio il puntatore dell'ultima zona con quello della prima zona
    gPMappa[gZoneEsistenti]->prossima_zona = gPPrimaZona;
  }
}

static void menuMappa()
{
  printf("\n");
  system("clear");
  printf("-------------------------\n");
  printf("| Creazione della mappa |\n");
  printf("-------------------------\n");
  printf("\n");
  int lIndex;
  freopen(NULL, "rb", stdin);
  printf("1 - Inserisci zona\n");
  printf("2 - Cancella zona\n");
  printf("3 - Stampa mappa\n");
  printf("4 - Chiudi mappa\n");
  printf("\nInserisci: ");
  int lControlloInt = scanf("%d", &lIndex);
  while (lIndex < 1 || lIndex > 4 || !lControlloInt)
  {
    printf("\n");
    freopen(NULL, "rb", stdin);
    printf("Errore:\nIl numero inserito non corrisponde a nessuna funzione del menù\n");
    printf("\nInserisci: ");
    lControlloInt = scanf("%d", &lIndex);
  }
  switch (lIndex)
  {
  case 1:
    inserisciZona();
    break;
  case 2:
    cancellaZone();
    break;
  case 3:
    stampaMappa();
    break;
  case 4:
    chiudiMappa();
    break;
  }
}

static void menuFinale()
{
  int lIndex;
  printf("\n");
  system("clear");
  printf("--------------------------------\n");
  printf("| Impostazione gioco terminata |\n");
  printf("--------------------------------\n");
  printf("\n");
  freopen(NULL, "rb", stdin);
  printf("1 - Gioca\n");
  printf("2 - Torna al menù\n");
  printf("\nInserisci: ");
  int lControlloInt = scanf("%d", &lIndex);
  while (lIndex < 1 || lIndex > 2 || !lControlloInt)
  {
    printf("\n");
    freopen(NULL, "rb", stdin);
    printf("Errore:\nIl numero inserito non corrisponde a nessuna funzione del menù\n");
    printf("\nInserisci: ");
    lControlloInt = scanf("%d", &lIndex);
  }
  switch (lIndex)
  {
  case 1:
    gioca();
    break;
  case 2:
    /* gEsito impostato ad 1, oltre che per rappresentare la vittoria,
    viene utilizzato anche come variabile di uscita per tornare al menù principale,
    senza però chiamare la funzione vittoria()*/
    gEsito = 1;
    break;
  }
}

void impostazioneGioco()
{
  int nGiocatori; // variabile locale che contiene il numero dei giocatori
  // re-imposto a 0 tutte le varibili globali
  gCreazioneMappa = 0;
  gZoneEsistenti = 0;
  gProbabilitaApparizione = 0;
  gFantasma = 0;
  gEsito = 0;
  gAvanzato = 0;
  gDifficolta = 0;
  gNGiocatori = 0;
  gNGiocatoriPrecedenti = 0;
  gProvaEMF = 0;
  gProvaSpiritBox = 0;
  gProvaVideocamera = 0;
  gCountProve = 0;
  gMorte = 0;
  gSale = 0;
  time_t t;
  srand((unsigned)time(&t));
  printf("\n");
  freopen(NULL, "rb", stdin);
  printf("Numero giocatori, da 1 a 4: ");
  int lControlloInt = scanf("%d", &nGiocatori);
  while (nGiocatori > 4 || nGiocatori < 1 || !lControlloInt)
  {
    printf("\n");
    freopen(NULL, "rb", stdin);
    printf("Errore:\nCarattere non valido, impossibile impostare una partita\n");
    printf("Numero giocatori, da 1 a 4: ");
    lControlloInt = scanf("%d", &nGiocatori);
  }
  printf("Giocatori: %d\n", nGiocatori);
  printf("\n");
  for (int count = 0; count < nGiocatori; count++)
  {
    /* alloco in memoria dinamica ogni struttura giocatore
    e posiziono i puntatori nell'array*/
    gPGiocatori[count] = realloc(gPGiocatori[count], sizeof(Giocatore));
    gPGiocatoriPrecedenti[count] = realloc(gPGiocatoriPrecedenti[count], sizeof(Giocatore));
    freopen(NULL, "rb", stdin);
    char nomeGiocatore[50];
    printf("Nome Giocatore %d: ", count + 1);
    fgets(nomeGiocatore, 50, stdin);
    // controllo che il primo carattere non sia vuoto o che l'utente abbia premuto invio
    while (nomeGiocatore[0] == '\n' || nomeGiocatore[0] == ' ')
    {
      printf("Errore:\nIl nome del giocatore non può essere vuoto\n");
      printf("Nome Giocatore %d: ", count + 1);
      freopen(NULL, "rb", stdin);
      fgets(nomeGiocatore, 50, stdin);
    }
    nomeGiocatore[strlen(nomeGiocatore) - 1] = '\0'; // serve a pulire la stringa
    strcpy(gPGiocatori[count]->nome_giocatore, nomeGiocatore);
    strcpy(gNomiGiocatoriPrecedenti[count], nomeGiocatore); // copio il nome del giocatore nella matrice contenente i nomi dei giocatori precedenti
    gPGiocatori[count]->sanita_mentale = 100;
  }
  /* porzione di codice che stampa il benvenuto in base al numero di giocatori
  con eventuali controlli su dove inserire , ed e prima dell'ultimo nome*/
  if (nGiocatori == 1)
  {
    printf("\nBenvenuto %s\n", gPGiocatori[0]->nome_giocatore);
  }
  else
  {
    printf("\nBenvenuti");
    for (int lCount = 0; lCount < nGiocatori; lCount++)
    {
      if (lCount == nGiocatori - 2)
      {
        printf(" %s, e", gPGiocatori[lCount]->nome_giocatore);
      }
      else
      {
        if (lCount == nGiocatori - 1)
        {
          printf(" %s", gPGiocatori[lCount]->nome_giocatore);
        }
        else
        {
          printf(" %s,", gPGiocatori[lCount]->nome_giocatore);
        }
      }
    }
  }
  // for che imposta a NULL i puntatori dei gPGiocatori non giocanti
  for (int count = nGiocatori; count < 4; count++)
  {
    gPGiocatori[count + 1] = NULL;
    gPGiocatoriPrecedenti[count + 1] = NULL;
  }
  printf("\n");
  freopen(NULL, "rb", stdin);
  printf("Seleziona la difficoltà (0 = dilettante, 1 = intermedio, 2 = incubo): ");
  lControlloInt = scanf("%d", &gDifficolta);
  while (gDifficolta < 0 || gDifficolta > 2 || !lControlloInt)
  {
    printf("\n");
    freopen(NULL, "rb", stdin);
    printf("Carattere inserito non valido (0 = dilettante, 1 = intermedio, 2 = incubo): ");
    lControlloInt = scanf("%d", &gDifficolta);
  }
  printf("\n");
  // aggiorno la probabilità di apparizione in base alla difficoltà
  switch (gDifficolta)
  {
  case 0:
    printf("Difficoltà impostata a dilettante\n");
    gProbabilitaApparizione = 35;
    break;
  case 1:
    printf("Difficoltà impostata ad intermedio\n");
    gProbabilitaApparizione = 55;
    break;
  case 2:
    printf("Difficoltà impostata ad incubo\n");
    gProbabilitaApparizione = 75;
    break;
  }
  sleep(1);
  // array di interi che salva i giocatori già estratti
  int controllo[5];
  generaTurni(controllo, nGiocatori);

  for (int count1 = nGiocatori; count1 > 0; count1--)
  {
    gTurnoIniziale[count1] = controllo[count1];
    gNGiocatori = nGiocatori; // copio il numero di giocatori nella variabile globale corrispondente
    /* copio la variabile globale gNGiocatori in gNGiocatoriPrecedenti,
    poiché nel corso della partita gNGiocatori potrebbe essere decrementata*/
    gNGiocatoriPrecedenti = gNGiocatori;
    system("clear");
    printf("-------------------------------------------------\n");
    printf("| Seleziona gli oggetti da inserire nello zaino |\n");
    printf("-------------------------------------------------\n");
    for (int count2 = 0; count2 < 4; count2++)
    {
      enum Tipo_oggetto_inziale lOggettoGenerato;
      if (count2 == 0)
      {
        enum Tipo_oggetto_inziale lOggettoIniziale = rand() % 3; // genera un numero casuale tra 0 e 2
        gPGiocatori[controllo[count1]]->zaino[count2] = lOggettoIniziale;
        printf("\n");
        convertiOggetto(lOggettoIniziale);
        if (lOggettoIniziale == videocamera)
        {
          printf(" inserita nello zaino, sarà il tuo oggetto iniziale\n");
        }
        else
        {
          printf(" inserito nello zaino, sarà il tuo oggetto iniziale\n");
        }
      }
      else
      {
        lOggettoGenerato = rand() % 2 + 3; // genera un numero casuale tra 3 e 4
        printf("\n%s vuoi inserire ", gPGiocatori[controllo[count1]]->nome_giocatore);
        convertiOggetto(lOggettoGenerato);
        printf(" nello zaino?\n");
        int scelta;
        printf("0 = Sì, 1 = No\n");
        printf("Inserisci: ");
        freopen(NULL, "rb", stdin);
        lControlloInt = scanf("%d", &scelta);
        printf("\n");
        while (scelta < 0 || scelta > 1 || !lControlloInt)
        {
          printf("Carattere non valido\n");
          printf("0 = Sì, 1 = No\n");
          printf("Inserisci: ");
          freopen(NULL, "rb", stdin);
          lControlloInt = scanf("%d", &scelta);
          printf("\n");
        }
        if (scelta == 0)
        {
          gPGiocatori[controllo[count1]]->zaino[count2] = lOggettoGenerato;
          printf("Oggetto ");
          convertiOggetto(lOggettoGenerato);
          printf(" inserito nello zaino\n");
        }
        else
        {
          printf("Oggetto scartato\n");
          /* se viene scartato inserisco un numero scelto da me,
          in questo caso 99 rappresenta la casella vuota*/
          gPGiocatori[controllo[count1]]->zaino[count2] = 99;
        }
      }
    }
    gPGiocatoriPrecedenti[count1] = gPGiocatori[count1];
    printf("\n");
  }
  for (int lCount1 = 0; lCount1 < nGiocatori; lCount1++)
  {
    for (int count2 = 0; count2 < 4; count2++)
    {
      gMatrice[lCount1][count2] = gPGiocatori[lCount1]->zaino[count2]; // colloco gli oggetti dello zaino nella matrice
    }
  }
  while (gCreazioneMappa == 0)
  {
    menuMappa();
  }
  menuFinale();
}

void imposta_gioco()
{
  /* se la mappa è già stata impostata, vuol'dire che
  l'utente ha già impostato il gioco*/
  if (gCreazioneMappa == 1)
  {
    gEsito = 0;
    int lIndex;
    system("clear");
    printf("----------------------------------\n");
    printf("| Il gioco è già stato impostato |\n");
    printf("----------------------------------\n");
    printf("\n");
    freopen(NULL, "rb", stdin);
    printf("Cosa vuoi fare?\n");
    printf("1 - Imposta nuovamente il gioco\n");
    printf("2 - Gioca\n");
    printf("\nInserisci: ");
    int lControlloInt = scanf("%d", &lIndex);
    /* in caso in cui l'utente digiti un carattere
    non contenuto nel menù, il programma restituisce un errore*/
    while (lIndex < 1 || lIndex > 2 || !lControlloInt)
    {
      printf("\n");
      freopen(NULL, "rb", stdin);
      printf("Errore:\nIl numero inserito non corrisponde a nessuna funzione del menù\n\n");
      printf("\nInserisci: ");
      lControlloInt = scanf("%d", &lIndex);
    }
    switch (lIndex)
    {
    case 1:
      impostazioneGioco();
      break;
    case 2:
      gNGiocatori = gNGiocatoriPrecedenti;
      for (int count1 = gNGiocatori - 1; count1 > 0; count1--)
      {
        gPGiocatori[count1] = realloc(gPGiocatori[count1], sizeof(Giocatore));         // re-alloco tutte le strutture giocatore
        gPGiocatori[count1] = gPGiocatoriPrecedenti[count1];                           // copio i campi dei giocatori precedenti nei giocatori attuali
        strcpy(gPGiocatori[count1]->nome_giocatore, gNomiGiocatoriPrecedenti[count1]); // copio il nome del giocatore precedente in quello attuale
        if (gPGiocatori[count1] == gPGiocatori[gTurnoIniziale[count1]])
          for (int count2 = 0; count2 < 4; count2++)
          {
            gPGiocatori[count1]->zaino[count2] = gMatrice[count1][count2]; // copio gli oggetti di ogni zaino contenuto nella matrice nello zaino di ogni giocatore
          }
      }
      gioca();
      break;
    }
  }
  else
  {
    impostazioneGioco();
  }
}

static void vittoria()
{
  if (gNGiocatori == 1)
  {
    printf("\n");
    system("clear");
    printf("   _   _      _         _       _          \n");
    printf("  | | | |__ _(_) __   _(_)_ ___| |_ ____   \n");
    printf("  | |_| | _` | | | | | | | '_  | __| _  |  \n");
    printf("  |  _  |(_| | |  | | || | | | | | |(_) |  \n");
    printf("  |_| |_|__,_|_|   |_| |_|_| |_|___|____|  \n");
    printf("-------------------------------------------\n");
    printf("| hai trovato tutte e 3 le prove, bravo!! |\n");
    printf("-------------------------------------------\n");
  }
  else
  {
    printf("\n");
    system("clear");
    printf("  _____             _                _       _         \n");
    printf(" |  _  |__   ______| |_ ____  __   _(_)_ ___| |_ ____  \n");
    printf(" | |_| || | | | _  | __| _  | | | | | | '_  | __| _  | \n");
    printf(" | __  | | | || ___| | |  __|  | | || | | | | | |(_) | \n");
    printf(" |_| |_|  |_| |____|___|____|   |_| |_|_| |_|___|____| \n");
    printf("--------------------------------------------------------\n");
    printf("|   avete trovato tutte le prove necessarie, bravi!!   |\n");
    printf("--------------------------------------------------------\n");
  }
  char lIndex;
  printf("\nPremi invio per tornare al menù principale: ");
  freopen(NULL, "rb", stdin);
  scanf("%c", &lIndex);
  while (lIndex != '\n')
  {
    printf("Errore:\nCarattere errato\n");
    printf("\nPremi invio per tornare al menù principale: ");
    freopen(NULL, "rb", stdin);
    scanf("%c", &lIndex);
  }
  gEsito = 1;
}

static void tornaCaravan(Giocatore *giocatore, Zona_mappa *zonaMappa)
{
  if (gFantasma == 0)
  {
    giocatore->posizione = gPCaravan;
    system("clear");
    printf("-----------\n");
    printf("| Caravan |\n");
    printf("-----------\n");
    printf("\n");
    /* sostituisce la prova ottenuta con un'oggetto
    generato in base alle varie prove trovate*/
    for (int count = 0; count < 4; count++)
    {
      switch (giocatore->zaino[count])
      {
      case prova_EMF:
        gProvaEMF = 1;
        if (gProvaSpiritBox == 1)
        {
          giocatore->zaino[count] = videocamera;
        }
        else if (gProvaVideocamera == 1)
        {
          giocatore->zaino[count] = spirit_box;
        }
        else
        {
          giocatore->zaino[count] = rand() % 2 + 1; // genera un numero casuale tra 1 e 2
        }
        break;
      case prova_spirit_box:
        gProvaSpiritBox = 1;
        if (gProvaEMF == 1)
        {
          giocatore->zaino[count] = videocamera;
        }
        else if (gProvaVideocamera == 1)
        {
          giocatore->zaino[count] = EMF;
        }
        else
        {
          do
          {
            giocatore->zaino[count] = rand() % 3; // genera un numero casuale tra 0 e 2
          } while (giocatore->zaino[count] == spirit_box);
        }
        break;
      case prova_videocamera:
        gProvaVideocamera = 1;
        if (gProvaEMF == 1)
        {
          giocatore->zaino[count] = spirit_box;
        }
        else if (gProvaSpiritBox == 1)
        {
          giocatore->zaino[count] = EMF;
        }
        else
        {
          giocatore->zaino[count] = rand() % 2; // genera un numero casuale tra 0 e 1
        }
        break;
      /* nel caso in cui un giocatore torni al caravan senza prove,
      ma abbia un'oggetto di cui la prova è già stata trovata,
      lo sostituisco con un'oggetto generato in base alle varie prove trovate*/
      case EMF:
        if (gProvaEMF == 1)
        {

          if (gProvaSpiritBox == 1)
          {
            giocatore->zaino[count] = videocamera;
          }
          else if (gProvaVideocamera == 1)
          {
            giocatore->zaino[count] = spirit_box;
          }
          else
          {
            giocatore->zaino[count] = rand() % 2 + 1; // genera un numero casuale tra 1 e 2
          }
          break;
        }
        else
        {
          break;
        }
      case spirit_box:
        if (gProvaSpiritBox == 1)
        {
          if (gProvaEMF == 1)
          {
            giocatore->zaino[count] = videocamera;
          }
          else if (gProvaVideocamera == 1)
          {
            giocatore->zaino[count] = EMF;
          }
          else
          {
            do
            {
              giocatore->zaino[count] = rand() % 3; // genera un numero casuale tra 0 e 2
            } while (giocatore->zaino[count] == spirit_box);
          }
          break;
        }
        else
        {
          break;
        }
      case videocamera:
        if (gProvaVideocamera == 1)
        {
          if (gProvaEMF == 1)
          {
            giocatore->zaino[count] = spirit_box;
          }
          else if (gProvaSpiritBox == 1)
          {
            giocatore->zaino[count] = EMF;
          }
          else
          {
            giocatore->zaino[count] = rand() % 2; // genera un numero casuale tra 0 e 1
          }
          break;
        }
        else
        {
          break;
        }
      }
    }
    // stampo le prove
    printf("Prova EMF: ");
    if (gProvaEMF == 1)
    {
      printf("trovata\n");
    }
    else
    {
      printf("non trovata\n");
    }
    printf("Prova spirit_box: ");
    if (gProvaSpiritBox == 1)
    {
      printf("trovata\n");
    }
    else
    {
      printf("non trovata\n");
    }
    printf("Prova videocamera: ");
    if (gProvaVideocamera == 1)
    {
      printf("trovata\n");
    }
    else
    {
      printf("non trovata\n");
    }
    char lIndex;
    printf("\nPremi invio per tornare alla zona di partenza: ");
    freopen(NULL, "rb", stdin);
    scanf("%c", &lIndex);
    while (lIndex != '\n')
    {
      printf("Errore:\nCarattere errato\n");
      printf("\nPremi invio per tornare alla zona di partenza: ");
      freopen(NULL, "rb", stdin);
      scanf("%c", &lIndex);
    }
    giocatore->posizione = gPPrimaZona; // posiziono il giocatore sulla prima zona della mappa
    /* se tutte e 3 le prove sono state trovate,
    chiamo la funzione vittoria()*/
    if (gProvaEMF == 1 && gProvaSpiritBox == 1 && gProvaVideocamera == 1)
    {
      vittoria();
    }
  }
  else
  {
    /* se in questo turno è comparso il fantasma,
    i giocatori non possono tornare al caravan*/
    printf("\n");
    system("clear");
    printf("-------------------------------------------------------\n");
    printf("| La porta è bloccata, impossibile tornare al caravan |\n");
    printf("-------------------------------------------------------\n");
    char lIndex;
    printf("\nPremi invio per tornare al menù: ");
    freopen(NULL, "rb", stdin);
    scanf("%c", &lIndex);
    while (lIndex != '\n')
    {
      printf("Errore:\nCarattere errato\n");
      printf("\nPremi invio per tornare al menù: ");
      freopen(NULL, "rb", stdin);
      scanf("%c", &lIndex);
    }
  }
}

static void stampaGiocatore(Giocatore *giocatore)
{
  system("clear");
  printf("----------------------\n");
  printf("| Dettagli giocatore |\n");
  printf("----------------------\n");
  printf("\nNome: %s\n", giocatore->nome_giocatore);
  printf("Sanità mentale: %u\n", giocatore->sanita_mentale);
  printf("Posizione: ");
  convertiZona(giocatore->posizione->zona);
  printf("\n");
  /* stampo gli oggetti contenuti nello zaino,
  assieme a qualche abbellimento grafico*/
  printf("Zaino:");
  for (int count = 0; count < 4; count++)
  {
    printf(" |");
    convertiOggetto(giocatore->zaino[count]);
    printf("|");
  }
  printf("\n");
  char lIndex;
  printf("\nPremi invio per tornare al menù: ");
  freopen(NULL, "rb", stdin);
  scanf("%c", &lIndex);
  while (lIndex != '\n')
  {
    printf("Errore:\nCarattere errato\n");
    printf("\nPremi invio per tornare al menù: ");
    freopen(NULL, "rb", stdin);
    scanf("%c", &lIndex);
  }
}

static void stampaZona(Zona_mappa *zonaMappa)
{
  system("clear");
  printf("-----------------\n");
  printf("| Dettagli zona |\n");
  printf("-----------------\n");
  printf("\nNome zona: ");
  convertiZona(zonaMappa->zona);
  printf("\n");
  printf("Oggetto zona: ");
  convertiOggetto(zonaMappa->oggetto_zona);
  printf("\n");
  printf("Prova: ");
  convertiOggetto(zonaMappa->prova);
  printf("\n");
  printf("Prossima zona: ");
  convertiZona(zonaMappa->prossima_zona->zona);
  printf("\n");
  char lIndex;
  printf("\nPremi invio per tornare al menù: ");
  freopen(NULL, "rb", stdin);
  scanf("%c", &lIndex);
  while (lIndex != '\n')
  {
    printf("Errore:\nCarattere errato\n");
    printf("\nPremi invio per tornare al menù: ");
    freopen(NULL, "rb", stdin);
    scanf("%c", &lIndex);
  }
}

static void avanza(Giocatore *giocatore, Zona_mappa *zonaMappa)
{
  if (gAvanzato == 0) // se il giocatore non è ancora avanzato
  {
    giocatore->posizione = zonaMappa->prossima_zona; // sposto il giocatore nella prossima zona della mappa
    printf("\nSei avanzato verso: ");
    convertiZona(giocatore->posizione->zona);
    /* ad ogni avanzamento di un giocatore,
    genero una nuova prova con le probabilità fornite
    dal testo del progetto*/
    enum Tipo_prova lProvaZona;
    int lNum = rand() % 100 + 1;
    if (lNum <= 40)
    {
      lProvaZona = nessuna_prova;
    }
    else if (lNum > 40 && lNum <= 60)
    {
      lProvaZona = prova_EMF;
    }
    else if (lNum > 60 && lNum <= 80)
    {
      lProvaZona = prova_spirit_box;
    }
    else if (lNum > 80 && lNum <= 100)
    {
      lProvaZona = prova_videocamera;
    }
    giocatore->posizione->prova = lProvaZona; // colloco la prova generata nella zona
    printf("\n");
    char lIndex;
    printf("\nPremi invio per tornare al menù: ");
    freopen(NULL, "rb", stdin);
    scanf("%c", &lIndex);
    while (lIndex != '\n')
    {
      printf("Errore:\nCarattere errato\n");
      printf("\nPremi invio per tornare al menù: ");
      freopen(NULL, "rb", stdin);
      scanf("%c", &lIndex);
    }
    gAvanzato++;
  }
  else
  {
    printf("\n");
    printf("Errore:\nSei già avanzato in questo turno\n");
    char lIndex;
    printf("\nPremi invio per tornare al menù: ");
    freopen(NULL, "rb", stdin);
    scanf("%c", &lIndex);
    while (lIndex != '\n')
    {
      printf("Errore:\nCarattere errato\n");
      printf("\nPremi invio per tornare al menù: ");
      freopen(NULL, "rb", stdin);
      scanf("%c", &lIndex);
    }
  }
}

static void raccogliProva(Giocatore *giocatore, Zona_mappa *zonaMappa)
{
  if (gFantasma == 0) // se il fantasma non sta cacciando
  {
    int lTrovato = 0;                      // rappresenta i diversi messaggi: 0 = non puoi raccogliere, 1 = hai raccolto, 2 = prova già raccolta
    if (zonaMappa->prova == nessuna_prova) // se non è presente una prova in quella zona
    {
      printf("\n");
      system("clear");
      printf("---------------------------------------\n");
      printf("| Nessuna prova presente nella stanza |\n");
      printf("---------------------------------------\n");
      char lIndex;
      printf("\nPremi invio per continuare: ");
      freopen(NULL, "rb", stdin);
      scanf("%c", &lIndex);
      while (lIndex != '\n')
      {
        printf("Errore:\nCarattere errato\n");
        printf("\nPremi invio per continuare: ");
        freopen(NULL, "rb", stdin);
        scanf("%c", &lIndex);
      }
    }
    else
    {
      for (int lCount = 0; lCount < 4; lCount++)
      {
        /* se possiedo l'oggetto corrispondente alla prova
        e quella prova non è già stata trovata, allora posso raccoglierla*/
        if (giocatore->zaino[lCount] == EMF && zonaMappa->prova == prova_EMF)
        {
          if (gProvaEMF == 1)
          {
            lTrovato = 2;
            break;
          }
          else
          {
            lTrovato = 1;
            giocatore->zaino[lCount] = zonaMappa->prova;
            break;
          }
        }
        else if (giocatore->zaino[lCount] == spirit_box && zonaMappa->prova == prova_spirit_box)
        {
          if (gProvaSpiritBox == 1)
          {
            lTrovato = 2;
            break;
          }
          else
          {
            lTrovato = 1;
            giocatore->zaino[lCount] = zonaMappa->prova;
            break;
          }
        }
        else if (giocatore->zaino[lCount] == videocamera && zonaMappa->prova == prova_videocamera)
        {
          if (gProvaVideocamera == 1)
          {
            lTrovato = 2;
            break;
          }
          else
          {
            lTrovato = 1;
            giocatore->zaino[lCount] = zonaMappa->prova;
            break;
          }
        }
      }
      if (lTrovato == 0)
      {
        printf("\n");
        printf("Non puoi raccogliere: ");
        convertiOggetto(zonaMappa->prova);
        printf("\n");
        printf("Non possiedi l'oggetto corrispondente\n");
        char lIndex;
        printf("\nPremi invio per continuare: ");
        freopen(NULL, "rb", stdin);
        scanf("%c", &lIndex);
        while (lIndex != '\n')
        {
          printf("Errore:\nCarattere errato\n");
          printf("\nPremi invio per continuare: ");
          freopen(NULL, "rb", stdin);
          scanf("%c", &lIndex);
        }
      }
      else if (lTrovato == 1)
      {
        printf("\nHai raccolto: ");
        convertiOggetto(zonaMappa->prova);
        zonaMappa->prova = nessuna_prova;
        printf("\n");
        /* effettuo l'estrazione per l'apparizione del fantasma
        utilizzando la probabilità di apparizione che dipende dalla difficoltà*/
        int num = rand() % 100 + 1; // genera un numero casuale tra 1 e 100
        if (num <= gProbabilitaApparizione)
        {
          printf("Il fantasma è apparso!\n");
          sleep(1);
          gFantasma = 1;
          riduciSanita(zonaMappa); // riduco la sanità dei giocatori in quella zona
        }
        else
        {
          printf("Il fantasma non è apparso.\n");
        }
        char lIndex;
        printf("\nPremi invio per continuare: ");
        freopen(NULL, "rb", stdin);
        scanf("%c", &lIndex);
        while (lIndex != '\n')
        {
          printf("Errore:\nCarattere errato\n");
          printf("\nPremi invio per continuare: ");
          freopen(NULL, "rb", stdin);
          scanf("%c", &lIndex);
        }
      }
      else if (lTrovato == 2)
      {
        printf("\n");
        system("clear");
        printf("-------------------------------------------------------\n");
        printf("| La prova è già stata raccolta da un altro giocatore |\n");
        printf("-------------------------------------------------------\n");
        char lIndex;
        printf("\nPremi invio per continuare: ");
        freopen(NULL, "rb", stdin);
        scanf("%c", &lIndex);
        while (lIndex != '\n')
        {
          printf("Errore:\nCarattere errato\n");
          printf("\nPremi invio per continuare: ");
          freopen(NULL, "rb", stdin);
          scanf("%c", &lIndex);
        }
      }
      gProbabilitaApparizione = gProbabilitaApparizione + 5; // incremento di 5 la probabilità di apparizione
    }
  }
  else
  {
    printf("\n");
    system("clear");
    printf("------------------------------------------------------------------\n");
    printf("| Il fantasma sta cacciando, non puoi raccogliere un'altra prova |\n");
    printf("------------------------------------------------------------------\n");
    char lIndex;
    printf("\nPremi invio per tornare al menù: ");
    freopen(NULL, "rb", stdin);
    scanf("%c", &lIndex);
    while (lIndex != '\n')
    {
      printf("Errore:\nCarattere errato\n");
      printf("\nPremi invio per tornare al menù: ");
      freopen(NULL, "rb", stdin);
      scanf("%c", &lIndex);
    }
  }
}

static void raccogliOggetto(Giocatore *giocatore, Zona_mappa *zonaMappa)
{
  int lTrovato = 0;                              // rappresenta i diversi messaggi: 0 = non puoi raccogliere, 1 = hai raccolto
  if (zonaMappa->oggetto_zona == nessun_oggetto) // se non vi è un'oggetto nella stanza
  {
    printf("\n");
    system("clear");
    printf("----------------------------------------\n");
    printf("| Nessun'oggetto presente nella stanza |\n");
    printf("----------------------------------------\n");
    char lIndex;
    printf("\nPremi invio per continuare: ");
    freopen(NULL, "rb", stdin);
    scanf("%c", &lIndex);
    while (lIndex != '\n')
    {
      printf("Errore:\nCarattere errato\n");
      printf("\nPremi invio per continuare: ");
      freopen(NULL, "rb", stdin);
      scanf("%c", &lIndex);
    }
  }
  else
  {

    for (int lCount = 0; lCount < 4; lCount++)
    {
      if (giocatore->zaino[lCount] == 99)
      {
        lTrovato = 1;
        giocatore->zaino[lCount] = zonaMappa->oggetto_zona;
        printf("\n");
        printf("Hai raccolto: ");
        convertiOggetto(zonaMappa->oggetto_zona);
        zonaMappa->oggetto_zona = nessun_oggetto;
        printf("\n");
        char lIndex;
        printf("\nPremi invio per continuare: ");
        freopen(NULL, "rb", stdin);
        scanf("%c", &lIndex);
        while (lIndex != '\n')
        {
          printf("Errore:\nCarattere errato\n");
          printf("\nPremi invio per continuare: ");
          freopen(NULL, "rb", stdin);
          scanf("%c", &lIndex);
        }
        break;
      }
    }
    if (lTrovato == 0)
    {
      printf("\n");
      printf("Non puoi raccogliere: ");
      convertiOggetto(zonaMappa->oggetto_zona);
      printf("\n");
      printf("Zaino pieno\n");
      char lIndex;
      printf("\nPremi invio per continuare: ");
      freopen(NULL, "rb", stdin);
      scanf("%c", &lIndex);
      while (lIndex != '\n')
      {
        printf("Errore:\nCarattere errato\n");
        printf("\nPremi invio per continuare: ");
        freopen(NULL, "rb", stdin);
        scanf("%c", &lIndex);
      }
    }
  }
}

static void usaOggetto(Giocatore *giocatore, Zona_mappa *zonaMappa)
{
  // stampo lo zaino e chiedo all'utente quale oggetto vuole usare
  printf("\n");
  printf("Zaino:");
  for (int count = 0; count < 4; count++)
  {
    printf(" |");
    convertiOggetto(giocatore->zaino[count]);
    printf("|");
  }
  printf("\n");
  int lScelta;
  printf("Quale oggetto vuoi usare? (1 - 4): ");
  int lControlloInt = scanf("%d", &lScelta);
  while (lScelta < 1 || lScelta > 4 || !lControlloInt)
  {
    printf("Carattere non valido\n");
    printf("Inserisci: ");
    freopen(NULL, "rb", stdin);
    lControlloInt = scanf("%d", &lScelta);
    printf("\n");
  }
  /* sottraggo uno alla scelta dell'utente poiché gli ho permesso
  di scegliere un oggetto da 1 a 4, ma nell'array zaino gli oggetti
  sono rappresentati da 0 a 3*/
  lScelta = lScelta - 1;
  int lControllo = 0; /* rappresenta i vari messaggi:
  0 = hai utilizzato,
  1 = cento dollari,
  2 = prova non utilizzabile,
  3 = chiamata funzione raccogliProva(),
  4 = impossibile utilizzare,
  99 = slot vuoto*/
  switch (giocatore->zaino[lScelta])
  {
  case EMF: // se utilizzo un'oggetto usato per raccogliere le prove, chiamo la funzione raccogliProva()
    raccogliProva(giocatore, zonaMappa);
    lControllo = 3;
    break;
  case spirit_box: // se utilizzo un'oggetto usato per raccogliere le prove, chiamo la funzione raccogliProva()
    raccogliProva(giocatore, zonaMappa);
    lControllo = 3;
    break;
  case videocamera: // se utilizzo un'oggetto usato per raccogliere le prove, chiamo la funzione raccogliProva()
    raccogliProva(giocatore, zonaMappa);
    lControllo = 3;
    break;
  case calmanti:
    /* nel caso in cui il giocatore abbia la sanità mentale al massimo,
    non permette di utilizzare l'oggetto, in caso contrario aumenta
    la sanità mentale del giocatore di 40, se dopo l'incremento
    essa supera 100 la imposta a 100, ovvero il massimo*/
    if (giocatore->sanita_mentale == 100)
    {
      printf("\n");
      printf("Salute al massimo, impossibile usare i calmanti\n");
      lControllo = 4;
    }
    else
    {
      giocatore->sanita_mentale = giocatore->sanita_mentale + 40;
      if (giocatore->sanita_mentale > 100)
      {
        giocatore->sanita_mentale = 100;
      }
    }
    break;
  case sale:
    if (gSale == 1) // se gSale == 1 vuol dire che un giocatore ha già usato il sale
    {
      printf("\n");
      printf("Un giocatore ha già usato il sale\n");
      lControllo = 4;
    }
    else
    {
      gSale = 1; // in caso contrario imposto gSale ad 1
    }
    break;
  case adrenalina:
    if (gAvanzato == 0) // se gAvanzato == 0 allora il giocatore non è ancora avanzato
    {
      printf("\n");
      printf("Il giocatore non è ancora avanzato, impossibile usare adrelina\n");
      lControllo = 4;
    }
    else
    {
      gAvanzato = 0; // in caso contrario imposto gAvanzato a 0, per permettere al giocatore di avanzare nuovamente
    }
    break;
  case cento_dollari:
    lControllo = 1;
    giocatore->zaino[lScelta] = rand() % 2 + 3; // genero un numero tra 3 e 4
    break;
  case coltello:
    if (giocatore->sanita_mentale > 30) // se la sanità mentale è superiore a 30 il giocatore non può usare il coltello
    {
      printf("\n");
      printf("Sanità mentale superiore a 30, impossibile usare coltello\n");
      lControllo = 4;
    }
    else
    {
      /* nel caso in cui la sanità mentale del giocatore sia al di sotto di 30,
      il giocatore uccide tutti i giocatori nella sua stessa zona, fatta eccezione per lui ovviamente*/
      for (int count1 = 0; count1 < gNGiocatori; count1++)
      {
        if (gPGiocatori[count1] != giocatore)
        {
          morte(count1, gPGiocatori[count1]->posizione);
          gMorte = 1; // imposto gMorte ad 1 per comunicare al programma che un giocatore è morto ed interrompere il turno
        }
      }
    }
    break;
  case prova_EMF: // le prove non sono oggetti utilizzabili, quindi imposto lControllo a 2
    lControllo = 2;
    break;
  case prova_spirit_box: // le prove non sono oggetti utilizzabili, quindi imposto lControllo a 2
    lControllo = 2;
    break;
  case prova_videocamera: // le prove non sono oggetti utilizzabili, quindi imposto lControllo a 2
    lControllo = 2;
    break;
  case 99: // lo slot è vuoto
    lControllo = 99;
    printf("\n");
    printf("Slot zaino vuoto");
    break;
  }
  if (lControllo == 0)
  {
    printf("\n");
    printf("Hai utilizzato: ");
    convertiOggetto(giocatore->zaino[lScelta]);
    giocatore->zaino[lScelta] = 99;
  }
  else if (lControllo == 1)
  {
    printf("\n");
    printf("Hai utilizzato: ");
    convertiOggetto(cento_dollari);
  }
  else if (lControllo == 2)
  {
    printf("\n");
    printf("Non puoi utilizzare una prova");
  }
  if (lControllo != 3)
  {
    char lIndex;
    printf("\nPremi invio per tornare al menù principale: ");
    freopen(NULL, "rb", stdin);
    scanf("%c", &lIndex);
    while (lIndex != '\n')
    {
      printf("Errore:\nCarattere errato\n");
      printf("\nPremi invio per tornare al menù principale: ");
      freopen(NULL, "rb", stdin);
      scanf("%c", &lIndex);
    }
  }
}

static void passa()
{
  gAvanzato = 0; // passando il turno gAvanzato torna a 0 per dare la possibilità al giocatore successivo di avanzare
}

static void giocaTurno()
{
  // array di interi che salva i turni dei giocatori
  int lTurni[gNGiocatori];
  generaTurni(lTurni, gNGiocatori);
  for (int count1 = gNGiocatori; count1 > 0; count1--)
  {
    if (gPGiocatori[lTurni[count1]] != NULL)
    {
      int lScelta;
      do
      {
        system("clear");
        freopen(NULL, "rb", stdin);
        printf("---------------------\n");
        printf("| Menù di selezione |\n");
        printf("---------------------\n");
        printf("\n");
        printf("1 - Torna caravan\n");
        printf("2 - Visualizza dettagli giocatore\n");
        printf("3 - Visualizza dettagli zona\n");
        printf("4 - Avanza\n");
        printf("5 - Raccogli prova\n");
        printf("6 - Raccogli oggetto\n");
        printf("7 - Usa oggetto\n");
        printf("8 - Passa il turno\n");
        printf("\n%s cosa vuoi fare?: ", gPGiocatori[lTurni[count1]]->nome_giocatore);
        int lControlloInt = scanf("%d", &lScelta);
        while (lScelta < 1 || lScelta > 8 || !lControlloInt)
        {
          lScelta = 0;
          printf("Carattere non valido\n");
          printf("Inserisci: ");
          freopen(NULL, "rb", stdin);
          lControlloInt = scanf("%d", &lScelta);
          printf("\n");
        }
        switch (lScelta)
        {
        case 1:
          tornaCaravan(gPGiocatori[lTurni[count1]], gPGiocatori[lTurni[count1]]->posizione);
          break;
        case 2:
          stampaGiocatore(gPGiocatori[lTurni[count1]]);
          break;
        case 3:
          stampaZona(gPGiocatori[lTurni[count1]]->posizione);
          break;
        case 4:
          avanza(gPGiocatori[lTurni[count1]], gPGiocatori[lTurni[count1]]->posizione);
          break;
        case 5:
          raccogliProva(gPGiocatori[lTurni[count1]], gPGiocatori[lTurni[count1]]->posizione);
          break;
        case 6:
          raccogliOggetto(gPGiocatori[lTurni[count1]], gPGiocatori[lTurni[count1]]->posizione);
          break;
        case 7:
          usaOggetto(gPGiocatori[lTurni[count1]], gPGiocatori[lTurni[count1]]->posizione);
          break;
        case 8:
          passa();
          break;
        }
        if (gEsito == 1 || gEsito == 2 || gMorte == 1) // in uno di questi 3 casi interrompo il turno
        {
          gMorte = 0; // cambiando il turno gMorte torna a 0
          return;
        }
      } while (lScelta != 8); // finché non passo il turno mostro il menù
    }
  }
  if (gFantasma == 1) // se il fantasma stava cacciando
  {
    gFantasma = 0; // ripristino la variabile gFantasma a 0
    sleep(1);
    /* comunico all'utente che il fantasma è scomparso e che può
    nuovamente tornare al caravan poiché la porta si è apertaF*/
    printf("\n");
    system("clear");
    printf("-------------------------------------------------\n");
    printf("| Il fantasma è scomparso, la porta si è aperta |\n");
    printf("-------------------------------------------------\n");
    char lIndex;
    printf("\nPremi invio per continuare: ");
    freopen(NULL, "rb", stdin);
    scanf("%c", &lIndex);
    while (lIndex != '\n')
    {
      printf("Errore:\nCarattere errato\n");
      printf("\nPremi invio per continuare: ");
      freopen(NULL, "rb", stdin);
      scanf("%c", &lIndex);
    }
  }
  else
  {
    /* se il fantasma non è apparso durante il turno
    comunico semplicemente la fine di esso*/
    sleep(1);
    printf("\n");
    system("clear");
    printf("--------------------\n");
    printf("| Turno completato |\n");
    printf("--------------------\n");
    char lIndex;
    printf("\nPremi invio per continuare: ");
    freopen(NULL, "rb", stdin);
    scanf("%c", &lIndex);
    while (lIndex != '\n')
    {
      printf("Errore:\nCarattere errato\n");
      printf("\nPremi invio per continuare: ");
      freopen(NULL, "rb", stdin);
      scanf("%c", &lIndex);
    }
  }
  gSale = 0; // finito il turno, ripristino gSale a 0
}

void gioca()
{
  /* in questi due casi chiamo imposta_gioco,
  perché vuol dire che il gioco era già stato impostato in precedenza*/
  if (gEsito == 1 || gEsito == 2)
  {
    imposta_gioco();
  }
  else
  {
    if (gCreazioneMappa == 0) // l'utente non ha ancora impostato il gioco, ovvero gCreazioneMappa == 0
    {
      printf("\n");
      system("clear");
      printf("---------------------------------------------\n");
      printf("| Per giocare devi prima impostare il gioco |\n");
      printf("---------------------------------------------\n");
      char lIndex;
      printf("\nPremi invio per tornare al menù principale: ");
      freopen(NULL, "rb", stdin);
      scanf("%c", &lIndex);
      while (lIndex != '\n')
      {
        printf("Errore:\nCarattere errato\n");
        printf("\nPremi invio per tornare al menù principale: ");
        freopen(NULL, "rb", stdin);
        scanf("%c", &lIndex);
      }
      return;
    }
    else
    {
      printf("\n");
      system("clear");
      printf("------------------\n");
      printf("| Buona fortuna! |\n");
      printf("------------------\n");
      char lIndex;
      printf("\nPremi invio per continuare: ");
      freopen(NULL, "rb", stdin);
      scanf("%c", &lIndex);
      while (lIndex != '\n')
      {
        printf("Errore:\nCarattere errato\n");
        printf("\nPremi invio per continuare: ");
        freopen(NULL, "rb", stdin);
        scanf("%c", &lIndex);
      }
      /* ripristino tutte le variabili di gioco*/
      gAvanzato = 0;
      gCountProve = 0;
      gFantasma = 0;
      gEsito = 0;
      gProvaEMF = 0;
      gProvaSpiritBox = 0;
      gProvaVideocamera = 0;
      /* genero una prova con le probabilità fornite
      dal testo del progetto*/
      enum Tipo_prova lProvaZona;
      int lNum = rand() % 100 + 1;
      if (lNum <= 40)
      {
        lProvaZona = nessuna_prova;
      }
      else if (lNum > 40 && lNum <= 60)
      {
        lProvaZona = prova_EMF;
      }
      else if (lNum > 60 && lNum <= 80)
      {
        lProvaZona = prova_spirit_box;
      }
      else if (lNum > 80 && lNum <= 100)
      {
        lProvaZona = prova_videocamera;
      }
      for (int lCount = 1; lCount <= gZoneEsistenti; lCount++)
      {
        gPMappa[lCount]->prova = nessuna_prova;
      }
      gPPrimaZona->prova = lProvaZona;
      /* colloco tutti i giocatori nella prima zona della mappa
      ed imposto la sanità mentale di essi al massimo*/
      for (int count = 0; count < gNGiocatori; count++)
      {
        gPGiocatori[count]->posizione = gPPrimaZona;
        gPGiocatori[count]->sanita_mentale = 100;
      }
      /* imposto la probabilità di apparizione
      del fantasma in base alla difficoltà*/
      switch (gDifficolta)
      {
      case 0:
        gProbabilitaApparizione = 35;
        break;
      case 1:
        gProbabilitaApparizione = 55;
        break;
      case 2:
        gProbabilitaApparizione = 75;
        break;
      }
    }
    while (gEsito == 0) // finché gEsito == 0 la partita si stà svolgendo
    {
      giocaTurno();
    }
    return; // torno al menù principale, completando l'esecuzione della funzione
  }
}

void termina_gioco()
{
  for (int count = 0; count < gZoneEsistenti; count++)
  {
    free(gPMappa[count]); // libero ogni zona della mappa
  }
  for (int count = 0; count < gNGiocatori; count++)
  {
    free(gPGiocatori[count]); // libero ogni giocatore
  }
  printf("Alla prossima!!\n");
}
