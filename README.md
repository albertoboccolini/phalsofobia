# PhalsoPhobia: Un gioco in C

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

## Design choices
1) Aggiunta funzione impostazioneGioco() per dividere la sezione effettiva delle impostazioni dalla funzione imposta_gioco() che in caso di ripetizione della partita, mostra il menù comunicando all'utente che la partita era già stata impostata in precedenza e chiama le opportune funzioni: impostazioneGioco() per la modifica delle impostazioni, gioca() per l'avvio della partita.
2) Aggiunta funzione convertiOggetto() che semplicemente prende in input l'intero assegnato agli oggetti e alle prove e ne stampa la stringa corrispettiva, questo per far fronte al problema dei tipi enum, per la quale non è possibile stampare gli oggetti o le prove, sostituendo %d con %s o %c.
3) Aggiunta funzione convertiZona() che semplicemente prende in input l'intero assegnato alle zone ne stampa la stringa corrispettiva, questo per far fronte al problema dei tipi enum, per la quale non è possibile stampare le zone, sostituendo %d con %s o %c.
4) Aggiunta funzione generaTurni() che prende in input un array di interi con tante posizioni quanti sono i giocatori ed il numero di essi. La funzione genera degli interi in base al numero di giocatori e controlla che non vi siano duplicati, per poi inserirli in ordine casuale nell'array che verrà restituito dopo la chiamata di funzione. Essa fa sì che ad ogni inizio del turno, venga generata una nuova successione di giocatori che non siano ripetuti ed evitare che in quel turno uno di essi venga chiamato a giocare per più di una volta.
5) Aggiunta funzione giocaTurno() che funge semplicemente da menù turno per ogni giocatore. Essa inizialmente chiama la funzione generaTurni() ed in base all'ordine generato fa svolgere ad ogni giocatore il suo turno, dove esso potrà scegliere tra le varie opzioni e chiamare le corrispettive funzioni. Questa potrà essere lasciata solo in caso di vittoria o di perdita della partita ed ogni volta che tutti i giocatori completano il loro turno, re-imposta la variabile globale gSale, poiché nel caso in cui in quel turno sia stata impostata ad 1 utilizzando l'oggetto sale, nel caso di comparsa del fantasma i giocatori non riceveranno il decremento di sanità mentale e questa variabile viene aggiornata ad ogni fine turno.
6) Aggiunta funzione riduciSanita() che innanzitutto controlla la variabile globale gSale, se essa è impostata ad 1, non decrementa la sanità di nessuno dei giocatori, poiché essa indica che in quel turno un giocatore ha utilizzato l'oggetto sale, ma in caso contrario decrementa la sanità di tutti i giocatori nella zona dove il fantasma è apparso. Inoltre la funzione effettua un controllo, poiché nel caso in cui dopo il decremento, il giocatore in questione raggiunga 0 di sanità mentale, verrà chiamata la funzione morte(), che si occuperà della morte di esso.
7) Aggiunta funzione morte() che in caso in cui il giocatore morto avesse una prova, la ricolloca nella zona dove è morto il giocatore. Di seguito la funzione dà l'oggetto corrispondente alla prova raccolta dal giocatore morto ad un altro giocatore in maniera casuale, in modo da evitare che non sia più possibile completare la partita. Infine la funzione re-alloca la memoria utilizzata dal giocatore morto, imposta il puntatore a NULL, decrementa la variabile gNGiocatori ed interrompe tutto il turno, questo per far sì che venga generato un nuovo turno con solo i giocatori rimanenti ed evitare che il turno vada al giocatore morto. Se decrementando la variabile gNGiocatori essa diventa 0, allora la funzione morte() chiama la funzione perso().
8) Aggiunta funzione perso() che stampa una semplice scritta per comunicare all'utente che: "la partita è persa!!" ed imposta la variabile globale gEsito a 2, che rappresenta appunto la perdita della partita. Premendo invio l'utente tornerà al menù iniziale, chiamato attraverso la funzione menu() dalla funzione main().
9) Aggiunta funzione vittoria() che stampa una semplice scritta in base al numero di giocatori ancora in vita: = 1: "Hai vinto!!", > 1: "Avete vinto!!" ed imposta la variabile globale gEsito ad 1, che rappresenta appunto la vittoria della partita. Premendo invio l'utente tornerà al menù iniziale, chiamato attraverso la funzione menu() dalla funzione main().
10) Quando l'utente utilizza la funzione tornaCaravan(), la funzione, oltre che spuntare le prove come completate e dare un altro oggetto per la ricerca delle prove, effettua un controllo. Nel caso in cui l'utente che si reca nel caravan, abbia con se un oggetto di cui la prova è già stata trovata, la funzione sostituisce quell'oggetto con uno di cui la prova è mancante. Questo controllo serve a velocizzare la partita nel caso in cui 2 delle 3 prove siano già state trovate, ed evitare che i giocatori aventi un'oggetto iniziale differente da quello necessario per ottere l'ultima, continuino ad andare in giro inutilmente senza poter raccogliere la prova rimanente.
11) lControlloInt, utilizzato in diverse parti del programma, serve a controllare che l'utente nei vari menù, alla richiesta di un input intero, non inserisca un carattere.
