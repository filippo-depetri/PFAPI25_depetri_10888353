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
#define NOT_VALID_TRAVEL -1
#define GRIGIO 7
#define BIANCO 9
#define COLLEGAMENTI 6
#define MAX_COST 65535
#define MAX_HEAP 480000
#define VISITED 1


//strutture
typedef struct heap{
    u_int16_t x_heap;
    u_int16_t y_heap;
    u_int16_t costo_h;
}heap;  //djikstra
typedef struct esagono
{
    int *x_ar;          
    int *y_ar;
    u_int8_t costo;        
    u_int8_t already_visited;
    u_int8_t dim_rotte;
}esagono_t;   

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

esagono_t *mappa;

void comando_init(FILE *output);
void comando_change_cost(int x, int y, int v, int raggio, FILE *output);
void comando_air_route(int x1, int y1, int x2, int y2, FILE *output);
void comando_travel_cost(int x1, int y1, int x2, int y2, FILE *output);
void alloca_mappa();
void libera_mappa();
float max (float n1, float n2);
void aggiorna_costo(int xloc, int yloc, int v, int raggio, int dist_esagoni);
void nodi_adiacenti(int x, int y);
int dist_esag(int startX, int startY, int arrX, int arrY);
void quicksort(heap coda[dim_mappa.dimx*dim_mappa.dimy], int head, int tail, int pos[dim_mappa.dimx*dim_mappa.dimy]);
int partition(heap coda[dim_mappa.dimx*dim_mappa.dimy], int head, int tail, int pos[dim_mappa.dimx*dim_mappa.dimy]);
void swap(heap coda[dim_mappa.dimx*dim_mappa.dimy], int i, int j, int pos[dim_mappa.dimx*dim_mappa.dimy]);

int main(){
    //PREPARATIVI
    //variabili per gestione comandi
        char comando[LUNGHEZZA_STR_COMANDO_MAX];
        int sc=0;
        FILE *f_in, *f_out;
        f_in=stdin;
        f_out=stdout;
        mappa=NULL;
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
    sc=fscanf(f_in, "%s", comando);
    while (sc==1){
        if(comando[0]==INIT){
                libera_mappa();
                sc=fscanf(f_in, "%d", &dim_mappa.dimy);
                sc=fscanf(f_in, "%d", &dim_mappa.dimx);
                alloca_mappa();
                comando_init(f_out);
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
                comando_change_cost(gen_comandi.rigx, gen_comandi.colx, gen_comandi.v, gen_comandi.raggio, f_out);
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
                        comando_air_route(gen_comandi.rigx, gen_comandi.colx, gen_comandi.rigy, gen_comandi.coly, f_out);
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
                            comando_travel_cost(gen_comandi.rigx, gen_comandi.colx, gen_comandi.rigy, gen_comandi.coly, f_out);
                            #ifdef DEBUG
                            printf("TRAVEL\n");
                            #endif
                        }
                    }
                }
            }
        sc=fscanf(f_in, "%s", comando);
    }
    fclose(f_in);
    fclose(f_out);
    libera_mappa(mappa);
    return 0;
}

//alloca mappa
void alloca_mappa(){
    mappa=malloc(dim_mappa.dimx*dim_mappa.dimy*sizeof(esagono_t));
}
void libera_mappa(){
    if (mappa==NULL)
    {
        return;
    }
    else{
        free(mappa);
    }
    mappa=NULL;
}


//comando init: costo di ogni esagono inizializzato a 1 (ottimizzabile)
void comando_init(FILE *output){
    for (int i = 0; i <dim_mappa.dimx*dim_mappa.dimy; i++)
    {
        mappa[i].costo=1;
        mappa[i].already_visited=BIANCO;
        mappa[i].dim_rotte=0;
        mappa[i].x_ar=NULL;
        mappa[i].y_ar=NULL;
    }
    fprintf(output, "%s\n", AFFERMATIVO);
    return;   
}


//comando change cost
void comando_change_cost(int x, int y, int v, int raggio, FILE *output){
    int dist_esagoni=0;
    int xloc;
    int yloc;
    int dim_coda=0;
    int indice_coda=0;
    int j=0;
    int i=0;
    int coda[dim_mappa.dimx*dim_mappa.dimy][2];
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
    aggiorna_costo(x, y, v, raggio, dist_esagoni);
    #ifdef DEBUG
    fprintf(output, "%d\n %d\n", mappa[x*dim_mappa.dimy +y].costo, dist_esagoni);
    #endif
    mappa[x*dim_mappa.dimy+y].already_visited=GRIGIO;
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
                if (mappa[coordinate[i][0]*dim_mappa.dimy + coordinate[i][1]].already_visited==BIANCO)
                {
                    coda[indice_coda][0]=coordinate[i][0];
                    coda[indice_coda][1]=coordinate[i][1];
                    mappa[coordinate[i][0]*dim_mappa.dimy + coordinate[i][1]].already_visited=GRIGIO;
                    #ifdef DEBUG
                    fprintf(output, "%d %d %d\n", coordinate[i][0], coordinate[i][1], indice_coda);
                    #endif
                    indice_coda++;
                }
            }
        }
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
        aggiorna_costo(coda[i][0], coda[i][1], v, raggio, dist_esagoni);
        i++;
    }
    fprintf(output, "%s\n", AFFERMATIVO);
    #ifdef DEBUG
    for (i = 0; i < indice_coda; i++)
    {
        fprintf(output, "%d %d %d\n", coda[i][0], coda[i][1], mappa[coda[i][0]*dim_mappa.dimy+coda[i][1]].costo);
    }
    #endif
    //reset dell'already visited
    for(i = 0; i<indice_coda; i++){
        mappa[coda[i][0]*dim_mappa.dimy+coda[i][1]].already_visited=BIANCO;
    }
    mappa[x*dim_mappa.dimy + y].already_visited=BIANCO;
    return;
}

//comando air_route
void comando_air_route(int x1, int y1, int x2, int y2, FILE *output){
    if (mappa==NULL)        //check se mappa è stata creata
    {
        fprintf(output, "%s\n", FALSO);
        return;
    }
    if (x1>=dim_mappa.dimx || x1<0 || x2>=dim_mappa.dimx || x2<0 || y1>=dim_mappa.dimy || y1<0 || y2>=dim_mappa.dimy || y2<0)   //check se sono nei limiti mappa
    {
        fprintf(output, "%s\n", FALSO);
        return;
    }
    if (mappa[x1*dim_mappa.dimy+y1].x_ar==NULL)
    {
        mappa[x1*dim_mappa.dimy+y1].x_ar=malloc(sizeof(int));
        mappa[x1*dim_mappa.dimy+y1].y_ar=malloc(sizeof(int));
        mappa[x1*dim_mappa.dimy+y1].x_ar[mappa[x1*dim_mappa.dimy+y1].dim_rotte]=x2;
        mappa[x1*dim_mappa.dimy+y1].y_ar[mappa[x1*dim_mappa.dimy+y1].dim_rotte]=y2;
        mappa[x1*dim_mappa.dimy+y1].dim_rotte++;
        fprintf(output, "%s\n", AFFERMATIVO);
        #ifdef DEBUG
        fprintf(output, "%d %d %d %d", x1, y1, x2, y2);
        fprintf(output, "%d", mappa[x1*dim_mappa.dimy+y1].dim_rotte-1);
        #endif
        return;
    }
    //cancellazione rotta aerea
    if (mappa[x1*dim_mappa.dimy+y1].dim_rotte==1)
    {
        if (mappa[x1*dim_mappa.dimy+y1].x_ar[0]==x2 && mappa[x1*dim_mappa.dimy+y1].y_ar[0]==y2)
        {
            #ifdef DEBUG
            fprintf(output, "%d %d %d %d", x1, y1, x2, y2);
            #endif
            mappa[x1*dim_mappa.dimy+y1].dim_rotte--;
            free(mappa[x1*dim_mappa.dimy+y1].x_ar);       //cancello rotta aerea
            free(mappa[x1*dim_mappa.dimy+y1].y_ar);
            mappa[x1*dim_mappa.dimy+y1].x_ar=NULL;
            mappa[x1*dim_mappa.dimy+y1].y_ar=NULL;
            fprintf(output, "%s\n", AFFERMATIVO);
            return;
        }
    }
    else
    {
        //cancello rotta
        for (int i = 0; i < mappa[x1*dim_mappa.dimy+y1].dim_rotte; i++)
        {
            if (mappa[x1*dim_mappa.dimy+y1].x_ar[i]==x2 && mappa[x1*dim_mappa.dimy+y1].y_ar[i]==y2)
            {
                #ifdef DEBUG
                fprintf(output, "%d %d %d %d", x1, y1, x2, y2);
                #endif
                for(int j=i; j<mappa[x1*dim_mappa.dimy+y1].dim_rotte-1; j++)
                {
                    mappa[x1*dim_mappa.dimy+y1].x_ar[j]=mappa[x1*dim_mappa.dimy+y1].x_ar[j+1];
                    mappa[x1*dim_mappa.dimy+y1].y_ar[j]=mappa[x1*dim_mappa.dimy+y1].y_ar[j+1];
                }
                mappa[x1*dim_mappa.dimy+y1].dim_rotte--;
                mappa[x1*dim_mappa.dimy+y1].x_ar=realloc(mappa[x1*dim_mappa.dimy+y1].x_ar, mappa[x1*dim_mappa.dimy+y1].dim_rotte*sizeof(int));
                mappa[x1*dim_mappa.dimy+y1].y_ar=realloc(mappa[x1*dim_mappa.dimy+y1].y_ar, mappa[x1*dim_mappa.dimy+y1].dim_rotte*sizeof(int));  
                fprintf(output, "%s\n", AFFERMATIVO);
                return;
            }
        }
        if (mappa[x1*dim_mappa.dimy+y1].dim_rotte>=MAX_ROTTE_AR)
        {
            fprintf(output, "%s\n", FALSO);
            return;
        }

        #ifdef DEBUG
        fprintf(output, "%d", mediapercosto);
        #endif
        mappa[x1*dim_mappa.dimy+y1].dim_rotte++;
        mappa[x1*dim_mappa.dimy + y1].x_ar=realloc(mappa[x1*dim_mappa.dimy + y1].x_ar, mappa[x1*dim_mappa.dimy+y1].dim_rotte*sizeof(int));
        mappa[x1*dim_mappa.dimy + y1].y_ar=realloc(mappa[x1*dim_mappa.dimy + y1].y_ar, mappa[x1*dim_mappa.dimy+y1].dim_rotte*sizeof(int));
        mappa[x1*dim_mappa.dimy + y1].x_ar[mappa[x1*dim_mappa.dimy+y1].dim_rotte-1]=x2;
        mappa[x1*dim_mappa.dimy + y1].y_ar[mappa[x1*dim_mappa.dimy+y1].dim_rotte-1]=y2;
        fprintf(output, "%s\n", AFFERMATIVO);
        #ifdef DEBUG
        fprintf(output, "%d %d %d %d", x1, y1, x2, y2);
        fprintf(output, "%d", mappa[x1*dim_mappa.dimy+y1].dim_rotte-1);
        #endif
        return;
    }
}

void comando_travel_cost(int x1, int y1, int x2, int y2, FILE *output){
    heap nodi[MAX_HEAP];
    u_int16_t costi[dim_mappa.dimx*dim_mappa.dimy];
    u_int8_t visitati[dim_mappa.dimx*dim_mappa.dimy];
    u_int8_t aggiornato;
    int pos[dim_mappa.dimx*dim_mappa.dimy];
    int i;
    int head=0;
    int dim_heap=0;
    heap nodo_corrente;
    int successivo;
    int costo_corrente;
    int index;
    if (mappa==NULL)
    {
        fprintf(output, "%d\n", NOT_VALID_TRAVEL);
        return;
    }
    if (x1>=dim_mappa.dimx || x1<0 || x2>=dim_mappa.dimx || x2<0 || y1>=dim_mappa.dimy || y1<0 || y2>=dim_mappa.dimy || y2<0 || mappa[x1*dim_mappa.dimy+y1].costo==0)   //check se sono nei limiti mappa
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
        costi[i]=MAX_COST;
        visitati[i]=NOT_VALID_COST;
        pos[i]=NOT_VALID_TRAVEL;
    }
    
    //algoritmo di djikstra
    costi[x1*dim_mappa.dimy+y1]=NOT_VALID_COST;
    nodi[head].x_heap=x1;
    nodi[head].y_heap=y1;
    nodi[head].costo_h=NOT_VALID_COST;
    dim_heap++;
    while (head<dim_heap)
    {
        nodo_corrente=nodi[head];
        index=nodo_corrente.x_heap*dim_mappa.dimy+nodo_corrente.y_heap;
        visitati[index]=VISITED;
        pos[index]=NOT_VALID_TRAVEL;
        head++;
        aggiornato=0;
        //se sono arrivato al nodo destinazione
        if (nodo_corrente.x_heap==x2 && nodo_corrente.y_heap==y2)
        {
            break;
        }
        //cerco via aria
        if (mappa[index].x_ar!=NULL)
        {
            for (i = 0; i < mappa[index].dim_rotte; i++)
            {
                successivo=mappa[index].x_ar[i]*dim_mappa.dimy+mappa[index].y_ar[i];
                costo_corrente=mappa[index].costo;
                if (visitati[successivo]==0 && costi[index] + costo_corrente<costi[successivo] && (mappa[successivo].costo!=NOT_VALID_COST || (mappa[successivo].costo==NOT_VALID_COST && successivo==x2*dim_mappa.dimy+y2)))
                {
                    costi[successivo]=costi[index] + costo_corrente;
                    if (pos[successivo]==-1)
                    {
                        nodi[dim_heap].x_heap=mappa[index].x_ar[i];
                        nodi[dim_heap].y_heap=mappa[index].y_ar[i];
                        nodi[dim_heap].costo_h=costi[successivo];
                        pos[successivo]=dim_heap;
                        dim_heap++;
                        if (nodo_corrente.costo_h>costi[successivo])
                        {
                            aggiornato=1;
                        }
                    }
                    else
                    {
                        nodi[pos[successivo]].costo_h=costi[successivo];
                        quicksort(nodi, head, dim_heap-1, pos);
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
                costo_corrente=mappa[index].costo;
                if (visitati[successivo]==0 && costi[index] + costo_corrente<costi[successivo] && (mappa[successivo].costo!=NOT_VALID_COST || (mappa[successivo].costo==NOT_VALID_COST && successivo==x2*dim_mappa.dimy+y2)))
                {
                    costi[successivo]=costi[index] + costo_corrente;
                    if (pos[coordinate[i][0]*dim_mappa.dimy+coordinate[i][1]]==-1)
                    {
                        nodi[dim_heap].x_heap=coordinate[i][0];
                        nodi[dim_heap].y_heap=coordinate[i][1];
                        nodi[dim_heap].costo_h=costi[successivo];
                        pos[successivo]=dim_heap;
                        dim_heap++;
                        if (nodo_corrente.costo_h>costi[successivo])
                        {
                            aggiornato=1;
                        }
                    }
                    else
                    {
                        nodi[pos[successivo]].costo_h=costi[successivo];
                        quicksort(nodi, head, dim_heap-1, pos);
                    }
                }
            }
        }
        if (aggiornato==1)
        {
            quicksort(nodi, head, dim_heap-1, pos);
        }
        #ifdef DEBUG
        for (i = 0; i < dim_heap; i++)
        {
            printf("%d, %d, %d\n", nodi[i].x_heap, nodi[i].y_heap, nodi[i].costo_h);
        }
        printf("\n");
        #endif
            
    }
    if (costi[x2*dim_mappa.dimy+y2]>=MAX_COST)
    {
        fprintf(output, "%d\n", NOT_VALID_TRAVEL);
    }
    else{
        fprintf(output, "%d\n", costi[x2*dim_mappa.dimy+y2]);
    }
    return;
}
void quicksort(heap coda[dim_mappa.dimx*dim_mappa.dimy], int head, int tail, int pos[dim_mappa.dimx*dim_mappa.dimy]){
    int q;
    if (head<tail)
    {
        q=partition(coda, head, tail, pos);
        quicksort(coda, head, q-1, pos);
        quicksort(coda, q+1, tail, pos);
    }
}
int partition(heap coda[dim_mappa.dimx*dim_mappa.dimy], int head, int tail, int pos[dim_mappa.dimx*dim_mappa.dimy]){
    int costo=coda[tail].costo_h;
    int i=head-1;
    for (int j = head; j < tail-1; j++)
    {
        if (coda[j].costo_h<=costo)
        {
            i++;
            swap(coda, i, j, pos);
        }
    }
    swap(coda, i+1, tail, pos);
    return i+1;
}

void swap(heap coda[dim_mappa.dimx*dim_mappa.dimy], int i, int j, int pos[dim_mappa.dimx*dim_mappa.dimy]){
    heap temp;
    int pos_temp;
    pos_temp=pos[coda[i].x_heap*dim_mappa.dimy+coda[i].y_heap];
    pos[coda[i].x_heap*dim_mappa.dimy+coda[i].y_heap]=pos[coda[j].x_heap*dim_mappa.dimy+coda[j].y_heap];
    pos[coda[j].x_heap*dim_mappa.dimy+coda[j].y_heap]=pos_temp;
    temp=coda[i];
    coda[i]=coda[j];
    coda[j]=temp;
    return;
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
void aggiorna_costo(int xloc, int yloc, int v, int raggio, int dist_esagoni){
    int prog=0;
    prog=mappa[xloc*dim_mappa.dimy+yloc].costo+floorf((float)v*max(0.0f,(raggio-dist_esagoni)/(float)raggio));
    if (prog<0)
    {
        prog=0;
    }
    if (prog>100)
    {
        prog=100;
    }
    mappa[xloc*dim_mappa.dimy+yloc].costo=prog;
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