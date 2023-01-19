void imposta_gioco();
void gioca();
void termina_gioco();

enum Tipo_oggetto_inziale
{
  EMF = 0,
  spirit_box = 1,
  videocamera = 2,
  calmanti = 3,
  sale = 4
};

enum Tipo_oggetto_zona
{
  adrenalina = 5,
  cento_dollari = 6,
  coltello = 7,
  nessun_oggetto = 8
};

enum Tipo_zona
{
  caravan,
  cucina,
  soggiorno,
  camera,
  bagno,
  garage,
  seminterrato
};

enum Tipo_prova
{
  prova_EMF = 9,
  prova_spirit_box = 10,
  prova_videocamera = 11,
  nessuna_prova = 12
};

struct Giocatore
{
  char nome_giocatore[50];
  unsigned char sanita_mentale;
  struct Zona_mappa *posizione;
  unsigned char zaino[4];
};

struct Zona_mappa
{
  enum Tipo_zona zona;
  enum Tipo_prova prova;
  enum Tipo_oggetto_zona oggetto_zona;
  struct Zona_mappa *prossima_zona;
};

typedef struct Zona_mappa Zona_mappa;
typedef struct Giocatore Giocatore;