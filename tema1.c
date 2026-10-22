#include "tema1.h"

TLista AlocCelula(void* elem) {
    TLista aux = (TLista) malloc(sizeof(TCelula)); 
    if (aux) {                                    
        aux->info = elem;
        aux->urm = NULL;
    }
    return aux;                                   
}

System* InitS()
{
    System* s = (System*) malloc(sizeof(System));
    if(s == NULL)
    {
        return NULL;
    }
    s -> units =NULL;
    // creez santinela pentru lista circulara de incidente
    s -> incidents = (Incident*) malloc(sizeof(Incident));
    if(s -> incidents == NULL)
    {
        free(s);
        return NULL;
    }
    s->incidents->id = 0;
    strcpy(s->incidents->priority, "low");

    s->incidents->description = (char*) malloc((strlen("test incident") + 1) * sizeof(char));
    if(s->incidents->description == NULL)
    {
        free(s->incidents);
        free(s);
        return NULL;
    }
    strcpy(s->incidents->description, "test incident");
    strcpy(s->incidents->status, "solved");
    // leagaturile circulare ale santinelei
    s->incidents->urm = s->incidents;
    s->incidents->pre = s->incidents;
    // creez santinela pentru interventii
    s->interventions = (Intervention*) malloc(sizeof(Intervention));
    if(s->interventions == NULL)
    {
        free(s->incidents->description);
        free(s->incidents);
        free(s);
        return NULL;
    }

    s->interventions->incident = NULL;
    s->interventions->unit = NULL;
    // legaturile pentru santinela
    s->interventions->urm = s->interventions;
    s->interventions->pre = s->interventions;

    return s;
}

void CitireEchipaje(System* s, FILE *f)
{
    int nrechip;
    fscanf(f, "%d", &nrechip);
    s -> units = (Unit*) malloc(nrechip * sizeof(Unit));
    if(s -> units == NULL)
    {
        return;
    }
    for(int i = 0; i < nrechip; i++)
    {
        fscanf(f, "%d %c", &s->units[i].id , &s->units[i].type);
        s->units[i].availability = 1;   //masina disponibila
    }
}

int Push(TStiva* vf, void* x)
{
    TStiva aux = (TStiva) malloc(sizeof(TCelulaStiva));
    if(aux == NULL)
    {
        return 0;
    }

    aux->info = x;
    aux->urm = *vf; //noul elem arata catre vf
    *vf = aux;  // noul element este vf
    
    return 1;
}

int Pop(TStiva* vf, void** x)
{
    TStiva aux;
    if(*vf == NULL)
    {
        return 0;
    }

    *x = (*vf)->info;   // extrag informatia
    aux = *vf;
    *vf = aux->urm; // varful coboara un elem
    free(aux);  //eliberez elementul
    return 1;
}

TCoada* InitCoada()
{
    TCoada* c = (TCoada*) malloc(sizeof(TCoada));
    if(c == NULL)
    {
        return 0;
    }
    c->inc = NULL;
    c->sf = NULL;
    return c;
}

int IntrQ(TCoada *c, void* x)
{ 
  TLista aux;
  aux = AlocCelula(x);
  if ( ! aux )
  {
    return 0;
  }
      
  if(c -> sf == NULL)
  {
    c->inc = aux;
    c->sf = aux;
  }
  else
  {
    c->sf->urm = aux;
    c->sf = aux;
  }
  return 1;             
}

int ExtrQ(TCoada *c, void **x)
{ 
  TLista aux;
  if(c -> inc == NULL)
  { 
    return 0;
  }
  aux = c-> inc;
  *x = aux -> info;
  c -> inc = aux -> urm;
  
  if(c -> inc == NULL)
  {
    c -> sf = NULL;
  }
  
  free(aux);

  return 1;                      
}

void AdaugaIncident(System *s, TCoada *q_high, TCoada *q_med, TCoada *q_low,
                    int id, char *prioritate, char *descriere) 
{
    // 1. Alocare incident nou
    Incident *nou = (Incident*)malloc(sizeof(Incident));
    if (nou == NULL) return;

    // 2. Populare date de baza
    nou->id = id;
    strcpy(nou->priority, prioritate);
    strcpy(nou->status, "queued");

    // Alocare descriere
    int len = strlen(descriere) + 1;
    nou->description = (char*)malloc(len * sizeof(char));
    if (nou->description != NULL) {
        strcpy(nou->description, descriere);
    }

    // 3. Legare la finalul listei dublu inlantuite
    Incident *sant = s->incidents;
    Incident *ultim = sant->pre;

    nou->urm = sant;
    nou->pre = ultim;
    ultim->urm = nou;
    sant->pre = nou;

    // 4. Introducere in coada corespunzatoare
    if (strcmp(prioritate, "high") == 0) {
        IntrQ(q_high, nou);
    } else if (strcmp(prioritate, "medium") == 0) {
        IntrQ(q_med, nou);
    } else if (strcmp(prioritate, "low") == 0) {
        IntrQ(q_low, nou);
    }
}

void AfisareIncident(System *s, int id, FILE *out) {
    Incident *curent = s->incidents->urm;   //primul elemnt

    while (curent != s->incidents) {
        if (curent->id == id) {
            fprintf(out, "Incident %d has %s priority, the following description: \"%s\" and is %s\n", 
                    curent->id, curent->priority, curent->description, curent->status);
            
            return;
        }
        curent = curent->urm;
    }
    fprintf(out, "INVALID OPERATION! ERROR 404\n");
}

void AfisareEchipaj(System* s, int id_cautat, int nr_echipaje, FILE* out)
{
    for(int i = 0; i < nr_echipaje; i++)
    {
        if(s->units[i].id == id_cautat)
        {
            char status[20];
            if(s->units[i].availability == 1)
            {
                strcpy(status, "available");
            }
            else
            {
                strcpy(status, "unavailable");
            }
            fprintf(out, "Unit %d is type %c and is %s\n", id_cautat, s->units[i].type, status);
            return;
        }
    }
    fprintf(out, "INVALID OPERATION! ERROR 404\n");
}

void VerificareDisponibilitate(TCoada* queue_available, FILE* out)
{
    int disponibile = 0;
    TLista p = queue_available->inc;
    while(p != NULL)
    {
        disponibile++;
        p = p->urm;
    }
    fprintf(out, "Number of available units: %d\n", disponibile);
}

void Dispatch(System* s, TCoada* queue_available, TCoada* q_h,
        TCoada* q_m, TCoada* q_l, TStiva* stack_interventions, FILE* out)
        {
            // verific daca am ceva disponibil de intervenit 
            if(queue_available->inc != NULL && (q_h->inc != NULL || q_m->inc != NULL || q_l->inc != NULL))
            {
                Unit* echipaj = NULL;
                Incident* incident = NULL;
                // extarg urgenta cu cea mai mare prioritate
                ExtrQ(queue_available, (void**)&echipaj);
                if(q_h->inc != NULL)
                {
                    ExtrQ(q_h, (void**)&incident);
                }
                else if(q_m->inc != NULL)
                {
                    ExtrQ(q_m, (void**)&incident);
                }
                else
                {
                    ExtrQ(q_l, (void**)&incident);
                }
                // salvarea unui istoric al interventiilor
                echipaj->availability = 0;  // ocup masina
                strcpy(incident->status, "intervened");
                // creez un dosar de iterventie care leaga masina de incident
                Intervention* interventie_noua = (Intervention*) malloc(sizeof(Intervention));
    
                interventie_noua->incident = incident;
                interventie_noua->unit = echipaj;
                // inserez in lista circulara 
                Intervention* sant = s->interventions;
                Intervention* ultim = sant->pre;
                
                interventie_noua->urm = sant;
                interventie_noua->pre = ultim;
                ultim->urm = interventie_noua;
                sant->pre = interventie_noua;
                // pun interventia pe stiva pt a putea face undo
                Push(stack_interventions, (void*)interventie_noua);
            } 
            else
            {
                fprintf(out, "INVALID OPERATION! ERROR 404\n");
            }
        }

void Undo_Dispatch(System *s, TCoada* queue_available, TCoada* q_h,
        TCoada* q_m, TCoada* q_l, TStiva* stack_interventions, FILE* out)
        {
            if(*stack_interventions == NULL)
            {
                fprintf(out, "INVALID OPERATION! ERROR 404\n");
                return;   
            }
            // ma uit la vf stivei, iar daca s-a terminat intervenita nu o putem anula
            Intervention* interventie_varf = (Intervention*)((*stack_interventions)->info);
    
            if(strcmp(interventie_varf->incident->status, "solved") == 0)
            {
                fprintf(out, "INVALID OPERATION! ERROR 404\n");
                return; 
            }
            // extrag actiunea din stiva
            Intervention* interventie_amanata = NULL;
            Pop(stack_interventions, (void**)&interventie_amanata);
            // eliberez masina si o pun in coada
            interventie_amanata->unit->availability = 1;
            IntrQ(queue_available, interventie_amanata->unit);
            // resetez incidentul si il pun inapoi in coada lui
            strcpy(interventie_amanata->incident->status, "queued");

            if(strcmp(interventie_amanata->incident->priority, "high") == 0)
            {
                IntrQ(q_h, interventie_amanata->incident);
            } else if(strcmp(interventie_amanata->incident->priority, "medium") == 0)
            {
                IntrQ(q_m, interventie_amanata->incident);
            } else 
            {
                IntrQ(q_l, interventie_amanata->incident);
            }
            // tai legaturile nodului din lista circulara
            interventie_amanata->pre->urm = interventie_amanata->urm;
            interventie_amanata->urm->pre = interventie_amanata->pre;
            // sterg
            free(interventie_amanata);
        }

void AfisareInterventii(System* s, FILE* out)
{
    if(s->interventions->urm == s->interventions)
    {
        fprintf(out, "No intervention has been initiated\n");
        return;
    }

    Intervention* p = s->interventions->urm;

    while(p != s->interventions)
    {
        fprintf(out, "Incident %d was assigned to unit %d, and has the following status: \"%s\"\n",
                p->incident->id, p->unit->id, p->incident->status);
        
        p = p->urm;
    }
}

void IncidentRezolvat(System* s, int id, TCoada* queue_available_units, FILE* out)
{
    if(s->interventions-> urm == s->interventions)
    {
        fprintf(out, "INVALID OPERATION! ERROR 404\n");
        return;
    }

    Intervention* p = s->interventions->urm;

    while(p != s->interventions)
    {
        if(p->incident->id == id)
        {
            if (strcmp(p->incident->status, "solved") == 0) {
                fprintf(out, "INVALID OPERATION! ERROR 404\n");
                return;
            }
            //actualizez statusul si mut in coada
            strcpy(p->incident->status, "solved");
            p->unit->availability = 1;
            IntrQ(queue_available_units, p->unit);
            return;
        }
        p = p->urm;
    }
    fprintf(out, "INVALID OPERATION! ERROR 404\n");
}

void DistrugeCoada(TCoada* c) 
{
    if (c == NULL) return;
    void* temp;
    // ExtrQ eliberează automat celulele cozii, noi doar o golim
    while (ExtrQ(c, &temp)) {} 
    free(c);
}

void DistrugeStiva(TStiva* stiva) 
{
    void* temp;
    // Pop eliberează automat celulele stivei
    while (Pop(stiva, &temp)) {} 
}

void DistrugeSistem(System* s)
{
    if (s == NULL) return;

    // 1. Eliberăm array-ul de echipaje
    if (s->units != NULL) {
        free(s->units);
    }

    // 2. Eliberăm lista circulară de intervenții
    if (s->interventions != NULL) {
        Intervention* p = s->interventions->urm;
        while (p != s->interventions) {
            Intervention* aux = p;
            p = p->urm;
            free(aux);
        }
        free(s->interventions); // Eliberăm santinela
    }

    // 3. Eliberăm lista circulară de incidente și descrierile lor
    if (s->incidents != NULL) {
        Incident* p = s->incidents->urm;
        while (p != s->incidents) {
            Incident* aux = p;
            p = p->urm;
            if (aux->description != NULL) {
                free(aux->description);
            }
            free(aux);
        }
        // Santinela de la incidente are și ea o descriere alocată dinamic ("test incident")
        if (s->incidents->description != NULL) {
            free(s->incidents->description);
        }
        free(s->incidents); // Eliberăm santinela
    }

    // 4. La final, eliberăm sistemul în sine
    free(s);
}