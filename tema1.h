#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct celula {
    void *info;
    struct celula* urm;
} TCelula, *TLista;

typedef struct coada {
    TLista inc;
    TLista sf;
} TCoada;

typedef struct celst {
    void* info;
    struct celst* urm;
} TCelulaStiva, *TStiva;

typedef struct unit {
    int id;
    char type;
    int availability;
} Unit;

typedef struct incident {
    int id;
    char priority[7];
    char *description;
    char status[11];
    struct incident* urm;
    struct incident* pre;
} Incident;

typedef struct intervention {
    Incident *incident;
    Unit *unit;
    struct intervention* urm;
    struct intervention* pre;
} Intervention;

typedef struct system {
    Unit *units;
    Incident *incidents;
    Intervention *interventions;
} System;

System* InitS();
void CitireEchipaje(System *s, FILE *f);
int Push(TStiva* s, void* x);
TCoada* InitCoada();
int IntrQ(TCoada *c, void *x);
int ExtrQ(TCoada *c, void **x);
void AdaugaIncident(System *s, TCoada *q_high, TCoada *q_med, TCoada *q_low,
     int id, char *prioritate, char *descriere);
void AfisareIncident(System *s, int id, FILE *out);
void AfisareEchipaj(System* s, int id_cautat, int nr_echipaje, FILE* out);
void VerificareDisponibilitate(TCoada* queue_available, FILE* out);
void Dispatch(System* s, TCoada* queue_available, TCoada* q_h,
        TCoada* q_m, TCoada* q_l, TStiva* stack_interventions, FILE* out);
void Undo_Dispatch(System *s, TCoada* queue_available, TCoada* q_h,
        TCoada* q_m, TCoada* q_l, TStiva* stack_interventions, FILE* out);
void AfisareInterventii(System* s, FILE* out);
void IncidentRezolvat(System* s, int id, TCoada* queue_available_units, FILE* out);
void DistrugeCoada(TCoada* c);
void DistrugeStiva(TStiva* stiva);
void DistrugeSistem(System* s);