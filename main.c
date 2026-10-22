#include "tema1.h"

int main()
{
    FILE* f = fopen("tema1.in", "r");
    FILE* out = fopen("tema1.out", "w");

    if(f == NULL || out == NULL)
    {
        printf("Eroare la deschiderea fisierelor!\n");
        return -1;
    }

    System* s = InitS();
    if(s == NULL)
    {
        printf("Eroare la alocarea memoriei pentru sistem!\n");
        if(f != NULL) fclose(f);
        if(out != NULL) fclose(out);
        return -1;
    }

    // 1. Inițializăm cozile și stiva
    TCoada* queue_high = InitCoada();
    TCoada* queue_medium = InitCoada();
    TCoada* queue_low = InitCoada();
    TCoada* queue_available_units = InitCoada();
    
    // Stiva o inițializăm cu NULL (fiind o simplă listă)
    TStiva stack_interventions = NULL;

    // 2. Citim echipajele și le punem direct în coada de disponibilitate
    int nrechip;
    fscanf(f, "%d", &nrechip);
    
    s->units = (Unit*) malloc(nrechip * sizeof(Unit));
    if(s->units != NULL)
    {
        for(int i = 0; i < nrechip; i++)
        {
            fscanf(f, "%d %c", &s->units[i].id, &s->units[i].type);
            s->units[i].availability = 1; // 1 = available
            
            // Adăugăm adresa echipajului în coadă
            IntrQ(queue_available_units, &s->units[i]);
        }
    }

    // 3. Citim numărul de operații N
    int N;
    fscanf(f, "%d", &N);

    // 4. Procesăm cele N operații
    char comanda[50];
    for(int i = 0; i < N; i++)
    {
        fscanf(f, "%s", comanda); // Citim primul cuvânt al comenzii
     
        if(strcmp(comanda, "ADD_INCIDENT") == 0) {
            int id_inc;
            char prioritate[20];
            char restul_liniei[100]; // Un buffer mare în care aruncăm tot restul rândului

            // 1. Citim ID-ul și prioritatea
            fscanf(f, "%d %s", &id_inc, prioritate);
            
            // 2. Citim ABSOLUT TOT ce a mai rămas pe acel rând, până la Enter (\n)
            // Acum, restul_liniei va conține ceva de genul: ' "fire in building"\n'
            fgets(restul_liniei, 100, f);
            
            // 3. Ne folosim de <string.h> ca să decupăm elegant textul
            char *start = strchr(restul_liniei, '"'); // Caută prima ghilimeea
            char *end = strrchr(restul_liniei, '"');  // Caută ultima ghilimeea (de la coadă la cap)
            
            if (start != NULL && end != NULL && start != end) {
                *end = '\0'; // "Tăiem" șirul transformând a doua ghilimeea în terminator de șir
                start++;     // Mutăm indicatorul de start cu o literă mai la dreapta (să ignorăm prima ghilimeea)
                
                // Acum, pointerul 'start' arată EXACT către "fire in building", curat!
                AdaugaIncident(s, queue_high, queue_medium, queue_low, id_inc, prioritate, start);
            }
        } else if (strcmp(comanda, "SHOW_INCIDENT") == 0) {
            int id_cautat;
            fscanf(f, "%d", &id_cautat);
            
            // Apelăm funcția care va căuta și afișa incidentul în fișierul 'out'
            AfisareIncident(s, id_cautat, out);
        } else if (strcmp(comanda, "SHOW_UNIT") == 0) {
            int id_cautat;
            fscanf(f, "%d", &id_cautat);

            AfisareEchipaj(s, id_cautat, nrechip, out);
        } else if(strcmp(comanda, "CHECK_UNITS_AVAILABILITY") == 0) {
            VerificareDisponibilitate(queue_available_units, out);
        } else if(strcmp(comanda, "DISPATCH") == 0) {
            Dispatch(s, queue_available_units, queue_high, queue_medium, queue_low, &stack_interventions, out);
        } else if(strcmp(comanda, "UNDO_LAST_DISPATCH") == 0) {
            Undo_Dispatch(s, queue_available_units, queue_high, queue_medium, queue_low, &stack_interventions, out);
        } else if(strcmp(comanda, "SHOW_INTERVENTIONS") == 0){
            AfisareInterventii(s, out);
        } else if(strcmp(comanda, "SOLVED_INCIDENT") == 0) {
            int id;
            fscanf(f, "%d", &id);
            IncidentRezolvat(s, id, queue_available_units, out);
        }
    }

    DistrugeCoada(queue_high);
    DistrugeCoada(queue_medium);
    DistrugeCoada(queue_low);
    DistrugeCoada(queue_available_units);
    
    DistrugeStiva(&stack_interventions);
    
    DistrugeSistem(s);
    // La final închidem fișierele
    fclose(f);
    fclose(out);

    return 0;
}