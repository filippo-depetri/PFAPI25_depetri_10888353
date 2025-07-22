/*                                                                                                                 Progetto di Algoritmi e Strutture dati
Autore: Depetri Filippo
CP: 10888353
Mat: 236852
Consegna:   Movhex è una compagnia di autotrasporti che dispone di una flotta di veicoli dispersi su un’ampia area geografica. 
            Al fine di minimizzare i costi, Movhex ti ha commissionato l’implementazione di un programma che aiuti nel calcolo delle rotte ottimali per i suoi mezzi. 
                Mappa: è piastrellata in modo rettangolare, e quindi composta da un numero fissato di righe e colonne, specificate all’inizio dell’esecuzione del programma. 6 collegamenti massimi tra le piastrelle
                Piastrella: Ogni esagono è collegato a tutti quelli ad esso adiacenti sulla mappa. Durante l’esecuzione, gli esagoni possono ottenere o perdere collegamenti tramite apertura e chiusura di rotte aeree. 
                            Una rotta aerea è identificata univocamente dalla coppia ordinata dei due esagoni che essa collega monodirezionalmente, rendendoli adiacenti tra loro, inoltre ogni rotta aerea è dotata di un costo di attraversata. Da ogni esagono partono al più 5 rotte aeree.
                            Ad ogni esagono è associato un numero naturale: se positivo, rappresenta la costo di uscita via terra dall’esagono. Il valore 0 (zero) indica che l’esagono non può essere abbandonato (ma può essere visitato). Altrimenti, il costo minimo è 1, e il massimo è 100. 
                            Spostarsi da un esagono ad un altro ad esso collegato ha un costo pari al numero associato all’esagono di partenza. Il costo di uscita via terra dagli esagoni e il costo di attraversata delle rotte aeree può variare durante l’esecuzione del programma.
                Comandi:
                        init (col, rig): Inizializza (o reinizializza se già inizializzata) la mappa di ⟨n. righe⟩ × ⟨n. colonne⟩. Il programma risponde con il messaggio OK. Tutti gli esagoni hanno un costo iniziale pari a 1 e non sono presenti rotte aeree.
                        change_cost (x, y, v, raggio): Il comando change_cost modifica il costo di un qualunque esagono in posizione (xe,ye) tale per cui DistEsagoni((xe,ye),(⟨x⟩,⟨y⟩)) < ⟨raggio⟩, e di tutte le sue rotte aeree uscenti, secondo la formula indicata in specifica.
                        toggle_air_route (x1, y1, x2, y2): Aggiunge, se assente, o rimuove, se già presente, una rotta aerea tra due esagoni. Il costo della nuova connessione è la media (approssimata per difetto) dei costi di tutte le connessioni aeree uscenti nell’esagono in (⟨x1⟩,⟨y1⟩) precedentemente esistenti, e del suo costo di uscita.
                        travel_cost (x1, y1, x2, y2): Risponde con la più piccola somma dei costi delle connessioni aeree e/o i costi di uscita via terra degli esagoni da attraversare per raggiungere l’esagono di destinazione (⟨xd⟩, ⟨yd⟩) partendo da (⟨xp⟩, ⟨yp⟩). Il costo di uscita dell’esagono di destinazione è ignorato.
                N.B.:   gli input del programma saranno simili a quelli che si riscontrano nella realtà: raramente vengono eseguiti i comandi change_cost e toggle_air_route, mentre il comando travel_cost è molto utilizzato. 
                        Inoltre, la maggior parte delle sorgenti e delle destinazioni del comando travel_cost si concentra nelle stesse zone della mappa, mentre altre sono completamente ignorate. Questo offre opportunità di ottimizzazione.
                N.B.:   informazioni più dettagliate sui comandi sono disponibili nella specifica
*/

//librerie
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

//define
//#define DEBUGTEST
//#define DEBUG
#define INIT 'i'
#define CAMBIO_COSTO 'c'
#define ROTTA_AEREA 'o'
#define COSTO_VIAGGIO 'r'
#define AFFERMATIVO "OK"
#define FALSO "KO"
#define NOT_VALID_COST 0
#define LUNGHEZZA_STR_COMANDO_MAX 30
#define MAX_ROTTE_AR 5
#define NOT_VALID (u_int16_t)-2
#define NOT_VALID_TRAVEL -1
#define GRIGIO 7
#define BIANCO 9
#define COLLEGAMENTI 6
#define MAX_COST 65535
#define MAX_ALLOCATED 1000


//strutture
typedef struct heap{
    int x_heap;
    int y_heap;
    u_int8_t costi;
    u_int8_t visitati;
}heap;  //djikstra
typedef struct mappa
{
    int **rotte_aeree;          //matrice in cui ci sono coord x y e costo rotta aerea
    u_int8_t **esagoni;        //matrice in cui sono presenti costi e flag already visited
    u_int8_t dim_rotte;
}map;   //in memory heap

int coordinate[COLLEGAMENTI][2];

struct generale_comandi
{
    int colx;
    int rigx;
    int coly;
    int rigy;
    int v;
    int raggio;
}gen_comandi;
struct dimensioni_mappa
{
    int dimx;     //dim righe
    int dimy;     //dim colonne
}dim_mappa;

void comando_init(map *mappa, FILE *output);
void comando_change_cost(map *mappa, int x, int y, int v, int raggio, FILE *output);
void comando_air_route(map *mappa, int x1, int y1, int x2, int y2, FILE *output);
void comando_travel_cost(map *mappa, int x1, int y1, int x2, int y2, FILE *output);
void alloca_mappa();
void libera_mappa(map *mappa);
float max (float n1, float n2);
void aggiorna_costo(map *mappa, int xloc, int yloc, int v, int raggio, int dist_esagoni);
void nodi_adiacenti(int x, int y);
int dist_esag(int startX, int startY, int arrX, int arrY);
heap pop_heap(heap nodo[dim_mappa.dimx*dim_mappa.dimy], int *dim_heap);
void min_heapify(heap nodo[dim_mappa.dimx*dim_mappa.dimy], int value, int size);
void swap(heap *nodo1, heap *nodo2);
void push_heap(heap nodo[dim_mappa.dimx*dim_mappa.dimy], int *size, int x, int y);

int main(){
    //PREPARATIVI
    //variabili per gestione comandi
        char comando[LUNGHEZZA_STR_COMANDO_MAX];
        int sc;
        FILE *f_in, *f_out;
        f_in=stdin;
        f_out=stdout;
        map mappa;
        mappa.esagoni=NULL;
        mappa.rotte_aeree=NULL;
    //fine variabili gestione comandi
    #ifdef DEBUGTEST
    int ssc;
    char c;
    do{
        ssc=fscanf(f_in, "%c", &c);
        fprintf(f_out, "%c", c);
    }while(ssc!=EOF);
    #endif
    //FINE PREPARATIVI
    do{
        sc=fscanf(f_in, "%s", comando);
        if(comando[0]==INIT){
                libera_mappa(&mappa);
                sc=fscanf(f_in, "%d", &dim_mappa.dimy);
                sc=fscanf(f_in, "%d", &dim_mappa.dimx);
                alloca_mappa(&mappa);
                comando_init(&mappa, f_out);
                #ifdef DEBUG
                printf("INIT\n");
                #endif
            }
            else {
                if(comando[0]==CAMBIO_COSTO) {
                sc=fscanf(f_in, "%d", &gen_comandi.colx);
                sc=fscanf(f_in, "%d", &gen_comandi.rigx);
                sc=fscanf(f_in, "%d", &gen_comandi.v);
                sc=fscanf(f_in, "%d", &gen_comandi.raggio);
                comando_change_cost(&mappa, gen_comandi.rigx, gen_comandi.colx, gen_comandi.v, gen_comandi.raggio, f_out);
                #ifdef DEBUG
                printf("CHANGE_COST\n");
                #endif
                }
                else{
                    if(comando[1]==ROTTA_AEREA){
                        sc=fscanf(f_in, "%d", &gen_comandi.colx);
                        sc=fscanf(f_in, "%d", &gen_comandi.rigx);
                        sc=fscanf(f_in, "%d", &gen_comandi.coly);
                        sc=fscanf(f_in, "%d", &gen_comandi.rigy);
                        comando_air_route(&mappa, gen_comandi.rigx, gen_comandi.colx, gen_comandi.rigy, gen_comandi.coly, f_out);
                        #ifdef DEBUG
                        printf("AIR_ROUTE\n");
                        #endif
                    }
                    else{
                        if (comando[1]==COSTO_VIAGGIO)
                        {
                            sc=fscanf(f_in, "%d", &gen_comandi.colx);
                            sc=fscanf(f_in, "%d", &gen_comandi.rigx);
                            sc=fscanf(f_in, "%d", &gen_comandi.coly);
                            sc=fscanf(f_in, "%d", &gen_comandi.rigy);
                            comando_travel_cost(&mappa, gen_comandi.rigx, gen_comandi.colx, gen_comandi.rigy, gen_comandi.coly, f_out);
                            #ifdef DEBUG
                            printf("TRAVEL\n");
                            #endif
                        }
                    }
                }
            }
    }while(sc!=EOF);
    fclose(f_in);
    fclose(f_out);
    libera_mappa(&mappa);
    return 0;
}

//alloca mappa
void alloca_mappa(map* mappa){
    mappa->esagoni=malloc(dim_mappa.dimx*dim_mappa.dimy*sizeof(u_int16_t *));
    for (int i = 0; i < dim_mappa.dimx*dim_mappa.dimy; i++)
    {
        mappa->esagoni[i]=malloc(2*sizeof(u_int16_t));
    }
}
void libera_mappa(map* mappa){
    if (mappa->esagoni==NULL)
    {
        return;
    }
    else{
        for (int i = 0; i < dim_mappa.dimx*dim_mappa.dimy; i++)
        {
            free(mappa->esagoni[i]);
        }
        free(mappa->esagoni);
    }
    mappa->esagoni=NULL;
    if (mappa->rotte_aeree==NULL)
    {
        return;
    }
    else{
        for (int i = 0; i < MAX_ALLOCATED; i++)
        {
            free(mappa->rotte_aeree[i]);
        }
        free(mappa->rotte_aeree);
    }
    mappa->rotte_aeree=NULL;
}


//comando init: costo di ogni esagono inizializzato a 1 (ottimizzabile)
void comando_init(map *mappa, FILE *output){
    for (int i = 0; i <dim_mappa.dimx*dim_mappa.dimy; i++)
    {
        mappa->esagoni[i][0]=1;
        mappa->esagoni[i][1]=BIANCO;
    }
    fprintf(output, "%s\n", AFFERMATIVO);
    #ifdef DEBUG
    for (int i = 0; i <dim_mappa.dimx*dim_mappa.dimy; i++)
    {

        fprintf(output, "%d", mappa->esagoni[i][0]);
        fprintf(output, "%d\n", mappa->esagoni[i][1]);
    }
    #endif
    return;   
}


//comando change cost
void comando_change_cost(map *mappa, int x, int y, int v, int raggio, FILE *output){
    int dist_esagoni=0;
    int xloc;
    int yloc;
    int dim_coda=0;
    int indice_coda=0;
    int j=0;
    int i=0;
    int coda[dim_mappa.dimx*dim_mappa.dimy][2];
    x=dim_mappa.dimx-x-1;
    if (mappa==NULL)        //check se mappa è stata creata
    {
        fprintf(output, "%s\n", FALSO);
        return;
    }
    if (x>=dim_mappa.dimx || x<0 || y>=dim_mappa.dimy || y<0 || raggio==0 || v<-10 || v>10)   //check se sono nei limiti mappa o se raggio=0 o se v non è compreso tra -10 e 10
    {
        fprintf(output, "%s\n", FALSO);
        return;
    }
    //dimensioni della coda di coordinate al massimo
    for (i = 1; i <= raggio; i++)
    {
        dim_coda+=COLLEGAMENTI*i;
    }
    #ifdef DEBUG
    fprintf(output, "coda allocata\n");
    #endif
    //aggiornamento nodo sorgente
    aggiorna_costo(mappa, x, y, v, raggio, dist_esagoni);
    #ifdef DEBUG
    fprintf(output, "%u\n %d\n", mappa->esagoni[x*dim_mappa.dimy +y][0], dist_esagoni);
    #endif
    mappa->esagoni[x*dim_mappa.dimy+y][1]=GRIGIO;
    xloc=x;
    yloc=y;
    //incodamento nodi adiacenti
    while (indice_coda<dim_coda)
    {
        nodi_adiacenti(xloc, yloc);
        for (i = 0; i < COLLEGAMENTI; i++)
        {
            if (coordinate[i][0]>=0 && coordinate[i][0]<dim_mappa.dimx && coordinate[i][1]>=0 && coordinate[i][1]<dim_mappa.dimy && indice_coda<dim_coda)
            {
                if (mappa->esagoni[coordinate[i][0]*dim_mappa.dimy + coordinate[i][1]][1]==BIANCO)
                {
                    coda[indice_coda][0]=coordinate[i][0];
                    coda[indice_coda][1]=coordinate[i][1];
                    mappa->esagoni[coordinate[i][0]*dim_mappa.dimy + coordinate[i][1]][1]=GRIGIO;
                    #ifdef DEBUG
                    fprintf(output, "%d %d %d\n", coordinate[i][0], coordinate[i][1], indice_coda);
                    #endif
                    indice_coda++;
                }
            }
        }
        #ifdef DEBUG
        fprintf(output, "%d\n", j);
        #endif
        if (j==indice_coda){
            break;
        }
        xloc=coda[j][0];
        yloc=coda[j][1];
        j++;
    }
    #ifdef DEBUG
    fprintf(output, "%s\n", "fine incodamento");
    #endif
    //aggiornamento costo
    i=0;
    while (i<indice_coda)
    {
        dist_esagoni=dist_esag(x, y, coda[i][0], coda[i][1]);
        aggiorna_costo(mappa, coda[i][0], coda[i][1], v, raggio, dist_esagoni);
        i++;
    }
    fprintf(output, "%s\n", AFFERMATIVO);
    #ifdef DEBUG
    for (i = 0; i < indice_coda; i++)
    {
        fprintf(output, "%u\n", mappa->esagoni[coda[i][0]*dim_mappa.dimy+coda[i][1]][0]);
    }
    #endif
    //reset dell'already visited
    for(i = 0; i<indice_coda; i++){
        mappa->esagoni[coda[i][0]*dim_mappa.dimy+coda[i][1]][1]=BIANCO;
    }
    mappa->esagoni[x*dim_mappa.dimy + y][1]=BIANCO;
    return;
}

//comando air_route
void comando_air_route(map* mappa, int x1, int y1, int x2, int y2, FILE *output){
    u_int8_t cancellazione=0;
    float mediapercosto=0;
    u_int8_t rotte=0;
    int indice_coda=0;
    x1=dim_mappa.dimx-x1-1;
    x2=dim_mappa.dimx-x2-1;
    if (mappa->esagoni==NULL)        //check se mappa è stata creata
    {
        fprintf(output, "%s\n", FALSO);
        return;
    }
    if (mappa->rotte_aeree==NULL)
    {
        mappa->rotte_aeree=malloc(MAX_ALLOCATED*sizeof(int *));
        for (int i = 0; i < MAX_ALLOCATED; i++)
        {
            mappa->rotte_aeree[i]=malloc(MAX_ROTTE_AR*sizeof(int));
        }
        for (int i = 0; i < MAX_ALLOCATED; i++)
        {
            for (int j = 0; j < MAX_ROTTE_AR; j++)
            {
                mappa->rotte_aeree[i][j]=NOT_VALID;
            }
        }
        mappa->dim_rotte=0;
    }
    if (x1>=dim_mappa.dimx || x1<0 || x2>=dim_mappa.dimx || x2<0 || y1>=dim_mappa.dimy || y1<0 || y2>=dim_mappa.dimy || y2<0)   //check se sono nei limiti mappa
    {
        fprintf(output, "%s\n", FALSO);
        return;
    }
    //cancellazione rotta aerea
    for (int i = 0; i < mappa->dim_rotte; i++)
    {
        if (mappa->rotte_aeree[i][0]==x1 && mappa->rotte_aeree[i][1]==y1 && mappa->rotte_aeree[i][2]==x2 && mappa->rotte_aeree[i][3]==y2)
        {
            #ifdef DEBUG
            fprintf(output, "%d %d %d %d", x1, y1, x2, y2);
            #endif
            mappa->rotte_aeree[i][0]=mappa->rotte_aeree[i][1]=mappa->rotte_aeree[i][2]=mappa->rotte_aeree[i][3]=mappa->rotte_aeree[i][4]=NOT_VALID;       //cancello rotta aerea
            cancellazione++;
            fprintf(output, "%s\n", AFFERMATIVO);
            return;
        }
    }
    if (cancellazione==0)
    {
        for (int i = 0; i < mappa->dim_rotte; i++)
        {
            if (mappa->rotte_aeree[i][0]==x1 && mappa->rotte_aeree[i][1]==y1)
            {
                rotte++;
                mediapercosto+=mappa->rotte_aeree[i][4];
            }
            if (mappa->rotte_aeree[i][0]!=NOT_VALID)
            {
                indice_coda++;
            }
        }
        if (rotte>=MAX_ROTTE_AR)
        {
            fprintf(output, "%s\n", FALSO);
            return;
        }
        mediapercosto+=mappa->esagoni[x1*dim_mappa.dimy + y1][0];
        mediapercosto=floor(mediapercosto/(float)(rotte+1));

        #ifdef DEBUG
        fprintf(output, "%f", mediapercosto);
        #endif
        
        mappa->rotte_aeree[indice_coda][0]=x1;       //creo rotta aerea
        mappa->rotte_aeree[indice_coda][1]=y1;
        mappa->rotte_aeree[indice_coda][2]=x2;
        mappa->rotte_aeree[indice_coda][3]=y2;
        mappa->rotte_aeree[indice_coda][4]=mediapercosto;
        mappa->dim_rotte++;
        fprintf(output, "%s\n", AFFERMATIVO);
        #ifdef DEBUG
        fprintf(output, "%d %d %d %d", x1, y1, x2, y2);
        fprintf(output, "%d", rotte);
        #endif
        return;
    }
    
}

void comando_travel_cost(map *mappa, int x1, int y1, int x2, int y2, FILE *output){
    heap nodi[dim_mappa.dimx*dim_mappa.dimy];
    int costo;
    int i;
    int partenza;
    int dim_heap=0;
    heap nodo_corrente;
    int successivo;
    int costo_corrente;
    int index;
    x1=dim_mappa.dimx-x1-1;
    x2=dim_mappa.dimx-x2-1;
    if (mappa->esagoni==NULL)
    {
        fprintf(output, "%d\n", NOT_VALID_TRAVEL);
    }
    if (x1>=dim_mappa.dimx || x1<0 || x2>=dim_mappa.dimx || x2<0 || y1>=dim_mappa.dimy || y1<0 || y2>=dim_mappa.dimy || y2<0 || mappa->esagoni[x1*dim_mappa.dimy+y1][0]==0)   //check se sono nei limiti mappa
    {
        fprintf(output, "%d\n", NOT_VALID_TRAVEL);
        return;
    }
    //check se ho le coordinate =
    if(x1==x2 && y1==y2)
    {
        fprintf(output, "%d\n", NOT_VALID_COST);
        return;
    }
    for (i = 0; i < dim_mappa.dimx*dim_mappa.dimy; i++)
    {
        nodi[i].costi=MAX_COST;
        nodi[i].visitati=0;
    }
    
    //algoritmo di djikstra
    partenza=x1*dim_mappa.dimy+y1;
    nodi[partenza].costi=0;
    push_heap(nodi, &dim_heap, x1, y1);
    while (dim_heap>0)
    {
        nodo_corrente=pop_heap(nodi, &dim_heap);
        index=nodo_corrente.x_heap*dim_mappa.dimy+nodo_corrente.y_heap;
        nodi[index].visitati=1;
        //se sono arrivato al nodo destinazione
        if (nodo_corrente.x_heap==x2 && nodo_corrente.y_heap==y2)
        {
            break;
        }
        //cerco via aria
        if (mappa->rotte_aeree!=NULL)
        {
            for (i = 0; i < mappa->dim_rotte; i++)
            {
                if (mappa->rotte_aeree[i][0]==nodo_corrente.x_heap && mappa->rotte_aeree[i][1]==nodo_corrente.y_heap)
                {
                    successivo=mappa->rotte_aeree[i][2]*dim_mappa.dimy+mappa->rotte_aeree[i][3];
                    costo_corrente=mappa->rotte_aeree[i][4];
                    if (costo_corrente>0 || (costo_corrente==0 && mappa->rotte_aeree[i][2]==x2 && mappa->rotte_aeree[i][3]==y2))
                    {
                        if (nodi[successivo].visitati==0 && nodo_corrente.costi + costo_corrente<nodi[successivo].costi)
                        {
                            nodi[successivo].costi=nodo_corrente.costi + costo_corrente;
                            push_heap(nodi, &dim_heap, mappa->rotte_aeree[i][2], mappa->rotte_aeree[i][3]);
                        }
                    }
                }
                    
            }
        }
        //cerco via terra
        nodi_adiacenti(nodo_corrente.x_heap, nodo_corrente.y_heap);
        for (i = 0; i < COLLEGAMENTI; i++)
        {
            if (coordinate[i][0]>=0 && coordinate[i][0]<dim_mappa.dimx && coordinate[i][1]>=0 && coordinate[i][1]<dim_mappa.dimy)
            {
                successivo=coordinate[i][0]*dim_mappa.dimy+coordinate[i][1];
                costo_corrente=mappa->esagoni[index][0];
                if (costo_corrente>0 || (costo_corrente==0 && coordinate[i][0]==x2 && coordinate[i][1]==y2))
                {
                    if (nodi[successivo].visitati==0 && nodo_corrente.costi + costo_corrente<nodi[successivo].costi)
                    {
                        nodi[successivo].costi=nodo_corrente.costi + costo_corrente;
                        push_heap(nodi, &dim_heap, coordinate[i][0], coordinate[i][1]);
                    }
                }
            }
            
        }
            
    }
        costo=nodi[x2*dim_mappa.dimy+y2].costi;
        if (costo>=MAX_COST)
        {
            fprintf(output, "%d\n", NOT_VALID_TRAVEL);
        }
        else{
            fprintf(output, "%d\n", costo);
        }
        for (i = 0; i < dim_mappa.dimx*dim_mappa.dimy; i++)
        {
            nodi[i].costi=MAX_COST;
            nodi[i].visitati=NOT_VALID_COST;
        }
        return;
}
heap pop_heap(heap nodo[dim_mappa.dimx*dim_mappa.dimy], int *dim_heap){
    heap min=nodo[0];
    nodo[0]=nodo[*dim_heap-1];
    *dim_heap=*dim_heap-1;
    min_heapify(nodo, 0, *dim_heap);
    return min;
}
void min_heapify(heap nodo[dim_mappa.dimx*dim_mappa.dimy], int value, int size){
    int l=2*value+1;
    int r=2*value+2;
    int min;
    if (l<size && nodo[l].costi<nodo[value].costi)
    {
        min=l;
    }
    else min=value;
    if (r<size && nodo[r].costi<nodo[min].costi)
    {
        min=r;
    }
    if (min!=value)
    {
        swap(&nodo[value], &nodo[min]);
        min_heapify(nodo, min, size);
    }
}
void swap(heap *nodo1, heap *nodo2){
    heap temp=*nodo1;
    *nodo1=*nodo2;
    *nodo2=temp;
}
void push_heap(heap nodo[dim_mappa.dimx*dim_mappa.dimy], int *size, int x, int y){
    int pos=*size;
    *size=*size+1;
    nodo[pos].x_heap=x;
    nodo[pos].y_heap=y;
    while (pos>0 && nodo[(pos-1)/2].costi>nodo[pos].costi)
    {
        swap(&nodo[pos], &nodo[(pos-1)/2]);
        pos=(pos-1)/2;
    }
    #ifdef DEBUG
    for (int i = 0; i < *size; i++)
    {
        printf("%d, %d\n", dim_mappa.dimx-nodo[i].x_heap-1, nodo[i].y_heap);
    }
    #endif
    
}

float max(float n1, float n2){
    if (n1>n2)
    {
        return n1;
    }
    else{
        return n2;
    }
}
void aggiorna_costo(map *mappa, int xloc, int yloc, int v, int raggio, int dist_esagoni){
    int prog=0;
    prog=mappa->esagoni[xloc*dim_mappa.dimy+yloc][0]+floor((float)v*max(0.0f,(raggio-dist_esagoni)/(float)raggio));
    if (prog<0)
    {
        prog=0;
    }
    if (prog>100)
    {
        prog=100;
    }
    mappa->esagoni[xloc*dim_mappa.dimy+yloc][0]=prog;
    if (mappa->rotte_aeree!=NULL)
    {
        for (int i = 0; i < mappa->dim_rotte; i++)
        {
            if (mappa->rotte_aeree[i][0]==xloc && mappa->rotte_aeree[i][1]==yloc)
            {
                mappa->rotte_aeree[i][4]=prog;
            }
        }
    }
    #ifdef DEBUG
    printf("costo modificato\n");
    #endif
}
void nodi_adiacenti(int x, int y){
    if (x%2==0)
    {
        coordinate[0][0]=x+1;
        coordinate[0][1]=y;
        coordinate[1][0]=x;
        coordinate[1][1]=y+1;
        coordinate[2][0]=x-1;
        coordinate[2][1]=y;
        coordinate[3][0]=x-1;
        coordinate[3][1]=y-1;
        coordinate[4][0]=x;
        coordinate[4][1]=y-1;
        coordinate[5][0]=x+1;
        coordinate[5][1]=y-1;
    }
    else{
        coordinate[0][0]=x+1;
        coordinate[0][1]=y;
        coordinate[1][0]=x+1;
        coordinate[1][1]=y+1;
        coordinate[2][0]=x;
        coordinate[2][1]=y+1;
        coordinate[3][0]=x-1;
        coordinate[3][1]=y+1;
        coordinate[4][0]=x-1;
        coordinate[4][1]=y;
        coordinate[5][0]=x;
        coordinate[5][1]=y-1;
    }
}

int dist_esag(int startX, int startY, int arrX, int arrY){
    int dist=0;
    while (startX!=arrX || startY!=arrY)
    {
        if (startX==arrX)
        {
            dist+=abs(arrY-startY);
            break;
        }
        if(startY==arrY)
        {
            dist+=abs(arrX-startX);
            break;
        }
        if (startX%2==0)
        {
            if (startX>arrX)
            {
                if (startY>arrY)
                {
                    startX=startX-1;
                    startY=startY-1;
                }
                else{
                    if (startY<arrY)
                    {
                        startX=startX-1;
                    }
                    
                }
                
            }
            else{
                if (startX<arrX)
                {
                    if (startY>arrY)
                    {
                        startX=startX+1;
                        startY=startY-1;
                    }
                    else{
                        if(startY<arrY)
                        {
                            startX=startX+1;
                        }
                    
                    }
                }
                
            }
            
        }
        else{
            if (startX>arrX)
            {
                if (startY>arrY)
                {
                    startX=startX-1;
                }
                else{
                    if (startY<arrY)
                    {
                        startX=startX-1;
                        startY=startY+1;
                    }
                    
                }
                
            }
            else{
                if (startX<arrX)
                {
                    if (startY>arrY)
                    {
                        startX=startX+1;
                    }
                    else{
                        if(startY<arrY)
                        {
                            startX=startX+1;
                            startY=startY+1;
                        }
                    
                    }
                }
                
            }
        }
        if (startX<0)
        {
            startX=0;
        }
        if (startX>=dim_mappa.dimx)
        {
            startX=dim_mappa.dimx-1;
        }
        if (startY<0)
        {
            startY=0;
        }
        if (startY>=dim_mappa.dimy)
        {
            startY=dim_mappa.dimy-1;
        }
        dist++;
    }
    return dist;
}