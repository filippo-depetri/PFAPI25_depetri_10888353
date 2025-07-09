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
#define COSTO_VIAGGIO 't'
#define AFFERMATIVO "OK"
#define FALSO "KO"
#define NOT_VALID_COST 0
#define LUNGHEZZA_STR_COMANDO_MAX 30
#define MAX_ROTTE_AR (u_int16_t)5
#define NOT_VALID (u_int16_t)-2
#define NOT_VALID_TRAVEL -1
#define GRIGIO 7
#define BIANCO 9
#define COLLEGAMENTI 6
#define MAX_COST 100000


//strutture
typedef struct rottaar
{
    int x;
    int y;
    u_int16_t costo;
}rottaar_t;

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

int coordinate[COLLEGAMENTI][2];

typedef struct esagono
{
    u_int16_t costo;
    rottaar_t rotta_ar[MAX_ROTTE_AR];
    u_int8_t already_visited;
    u_int8_t rotte_entranti;
}esagono_t;

void comando_init(esagono_t *mappa, int *cache, FILE *output);
void comando_change_cost(esagono_t *mappa, int x, int y, int v, int raggio, int *coda, FILE *output);
void comando_air_route(esagono_t *mappa, int x1, int y1, int x2, int y2, FILE *output);
void comando_travel_cost(esagono_t *mappa, int x1, int y1, int x2, int y2, int *coda, FILE *output);
esagono_t *alloca_mappa();
void libera_mappa(esagono_t *mappa);
float max (float n1, float n2);
void aggiorna_costo(esagono_t *mappa, int xloc, int yloc, int v, int raggio, int dist_esagoni);
void nodi_adiacenti(int x, int y);
int *alloca_coda();
void libera_coda(int *coda);
int dist_esag(int startX, int startY, int arrX, int arrY);
int verifica_nodi(esagono_t *mappa, int x, int y);
void migliori_coordinate_terra(esagono_t *mappa, int def[2], int x1, int y1, int x2, int y2);
void calc_dist(int distanze[2], int startX, int startY, int arrX, int arrY);
void migliori_coordinate_aria(esagono_t *mappa, int def[2], int x1, int y1, int x2, int y2);



int main(){
    //PREPARATIVI
    //variabili per gestione comandi
        char comando[LUNGHEZZA_STR_COMANDO_MAX];
        int sc;
        FILE *f_in, *f_out;
        f_in=stdin;
        f_out=stdout;
        esagono_t *mappa=NULL;
        int *cache=NULL;
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
                libera_mappa(mappa);
                sc=fscanf(f_in, "%d", &dim_mappa.dimy);
                sc=fscanf(f_in, "%d", &dim_mappa.dimx);
                mappa=alloca_mappa();
                cache=alloca_coda();
                comando_init(mappa, cache, f_out);
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
                comando_change_cost(mappa, gen_comandi.rigx, gen_comandi.colx, gen_comandi.v, gen_comandi.raggio, cache, f_out);
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
                        comando_air_route(mappa, gen_comandi.rigx, gen_comandi.colx, gen_comandi.rigy, gen_comandi.coly, f_out);
                        #ifdef DEBUG
                        printf("AIR_ROUTE\n");
                        #endif
                    }
                    else{
                        sc=fscanf(f_in, "%d", &gen_comandi.colx);
                        sc=fscanf(f_in, "%d", &gen_comandi.rigx);
                        sc=fscanf(f_in, "%d", &gen_comandi.coly);
                        sc=fscanf(f_in, "%d", &gen_comandi.rigy);
                        comando_travel_cost(mappa, gen_comandi.rigx, gen_comandi.colx, gen_comandi.rigy, gen_comandi.coly, cache, f_out);
                        #ifdef DEBUG
                        printf("TRAVEL\n");
                        #endif
                    }
                }
            }
    }while(sc!=EOF);
    fclose(f_in);
    fclose(f_out);
    libera_coda(cache);
    libera_mappa(mappa);
    return 0;
}

//alloca mappa
esagono_t *alloca_mappa(){
        esagono_t *mappa;
        mappa=malloc((dim_mappa.dimx*dim_mappa.dimy)*sizeof(esagono_t));
        return(mappa);
}
void libera_mappa(esagono_t *mappa){
    if (mappa==NULL)
    {
        return;
    }
    else{
        free(mappa);
    }
    
}


//comando init: costo di ogni esagono inizializzato a 1 (ottimizzabile)
void comando_init(esagono_t *mappa, int *cache, FILE *output){
    for (int i = 0; i<dim_mappa.dimx; i++)
    {
        for (int j = 0; j < dim_mappa.dimy; j++)
        {
            mappa[i*dim_mappa.dimy+j].costo=1;
            mappa[i*dim_mappa.dimy+j].already_visited=BIANCO;
            for (int k = 0; k < MAX_ROTTE_AR; k++)
            {
                mappa[i*dim_mappa.dimy+j].rotta_ar[k].costo=NOT_VALID;       //inizializzo vettori rotte aeree
                mappa[i*dim_mappa.dimy+j].rotta_ar[k].x=NOT_VALID;
                mappa[i*dim_mappa.dimy+j].rotta_ar[k].y=NOT_VALID;
                mappa[i*dim_mappa.dimy+j].rotte_entranti=NOT_VALID_COST;
                #ifdef DEBUG
                fprintf(output, "%d", mappa[i*dim_mappa.dimy+j].rotta_ar[k].costo);
                #endif
            }
        }
    }
    for (int i = 0; i < dim_mappa.dimx*dim_mappa.dimy; i++)
    {
        cache[i*2]=NOT_VALID;
        cache[i*2+1]=NOT_VALID;  
    }
    fprintf(output, "%s\n", AFFERMATIVO);
    #ifdef DEBUG
    for (int i = 0; i < dim_mappa.dimx; i++)
    {
        for (int j = 0; j < dim_mappa.dimy; j++)
        {
            fprintf(output, "%d", mappa[i*dim_mappa.dimy+j].costo);
        }
        
    }
    #endif
    return;   
}


//comando change cost
void comando_change_cost(esagono_t *mappa, int x, int y, int v, int raggio, int *coda, FILE *output){
    int dist_esagoni=0;
    int xloc;
    int yloc;
    int indice_coda=0;
    int j=0;
    int i=0;
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
    //aggiornamento nodo sorgente
    aggiorna_costo(mappa, x, y, v, raggio, dist_esagoni);
    #ifdef DEBUG
    fprintf(output, "%d\n %d\n", mappa[x*dim_mappa.dimy+y].costo, dist_esagoni);
    #endif
    mappa[x*dim_mappa.dimy+y].already_visited=GRIGIO;
    xloc=x;
    yloc=y;
    //incodamento nodi adiacenti
    while (indice_coda<dim_mappa.dimx*dim_mappa.dimy)
    {
        nodi_adiacenti(xloc, yloc);
        for (i = 0; i < COLLEGAMENTI; i++)
        {
            if (coordinate[i][0]>=0 && coordinate[i][0]<dim_mappa.dimx && coordinate[i][1]>=0 && coordinate[i][1]<dim_mappa.dimy)
            {
                if (mappa[coordinate[i][0]*dim_mappa.dimy + coordinate[i][1]].already_visited==BIANCO)
                {
                    coda[indice_coda*2]=coordinate[i][0];
                    coda[indice_coda*2+1]=coordinate[i][1];
                    mappa[coordinate[i][0]*dim_mappa.dimy + coordinate[i][1]].already_visited=GRIGIO;
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
        xloc=coda[j*2];
        yloc=coda[j*2+1];
        j++;
    }
    #ifdef DEBUG
    fprintf(output, "%s\n", "fine incodamento");
    #endif
    //aggiornamento costo
    i=0;
    while (i<indice_coda)
    {
        dist_esagoni=dist_esag(x, y, coda[i*2], coda[i*2+1]);
        aggiorna_costo(mappa, coda[i*2], coda[i*2+1], v, raggio, dist_esagoni);
        i++;
    }
    fprintf(output, "%s\n", AFFERMATIVO);
    #ifdef DEBUG
    for (i = 0; i < indice_coda; i++)
    {
        fprintf(output, "%d\n", mappa[coda[i*2]*dim_mappa.dimy+ coda[i*2+1]].costo);
    }
    #endif
    //reset dell'already visited e cache
    for(i = 0; i<indice_coda; i++){
        mappa[coda[i*2]*dim_mappa.dimy + coda[i*2+1]].already_visited=BIANCO;
        coda[i*2]=NOT_VALID;
        coda[i*2+1]=NOT_VALID;
    }
    mappa[x*dim_mappa.dimy+y].already_visited=BIANCO;
    return;
}

//comando air_route
void comando_air_route(esagono_t *mappa, int x1, int y1, int x2, int y2, FILE *output){
    u_int8_t cancellazione=0;
    float mediapercosto=0;
    u_int8_t count=1;
    u_int8_t rotte=0;
    x1=dim_mappa.dimx-x1-1;
    x2=dim_mappa.dimx-x2-1;
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
    for(int i=0; i <MAX_ROTTE_AR; i++){
        if (mappa[x1*dim_mappa.dimy + y1].rotta_ar[i].x==x2 && mappa[x1*dim_mappa.dimy + y1].rotta_ar[i].y==y2) 
        {
            #ifdef DEBUG
            fprintf(output, "%d %d %d %d", x1, y1, x2, y2);
            #endif

            mappa[x1*dim_mappa.dimy + y1].rotta_ar[i].costo=NOT_VALID;       //cancello rotta aerea
            mappa[x1*dim_mappa.dimy + y1].rotta_ar[i].x=NOT_VALID;
            mappa[x1*dim_mappa.dimy + y1].rotta_ar[i].y=NOT_VALID;
            mappa[x2*dim_mappa.dimy + y2].rotte_entranti--;
            cancellazione++;
            fprintf(output, "%s\n", AFFERMATIVO);
            return;
        }
    }
    if (cancellazione==0)
    {
        while (mappa[x1*dim_mappa.dimy + y1].rotta_ar[rotte].costo!=NOT_VALID && rotte < MAX_ROTTE_AR)
        {
        rotte++;
        }
        if (rotte>=MAX_ROTTE_AR)
        {
            fprintf(output, "%s", FALSO);
            return;
        }
        for (int j = 0; j < MAX_ROTTE_AR; j++)          //calcolo costo rotta aerea
        {
            if (mappa[x1*dim_mappa.dimy + y1].rotta_ar[j].costo!=NOT_VALID)
            {
                count++;
                mediapercosto+=mappa[x1*dim_mappa.dimy + y1].rotta_ar[j].costo;
            }
    
        }
        mediapercosto+=mappa[x1*dim_mappa.dimy + y1].costo;
        mediapercosto=floor(mediapercosto/(float)count);

        #ifdef DEBUG
        fprintf(output, "%f", mediapercosto);
        #endif

        mappa[x1*dim_mappa.dimy + y1].rotta_ar[rotte].costo=mediapercosto;       //creo rotta aerea
        mappa[x1*dim_mappa.dimy + y1].rotta_ar[rotte].x=x2;
        mappa[x1*dim_mappa.dimy + y1].rotta_ar[rotte].y=y2;
        mappa[x2*dim_mappa.dimy + y2].rotte_entranti++;
        fprintf(output, "%s\n", AFFERMATIVO);
        #ifdef DEBUG
        fprintf(output, "%d %d %d %d", x1, y1, x2, y2);
        fprintf(output, "%d", rotte);
        #endif
        return;
    }
}

void comando_travel_cost(esagono_t *mappa, int x1, int y1, int x2, int y2, int *coda, FILE *output){
    int costo=0;
    int indice_coda=0;
    int coord_terra[2];
    int coord_air[2];
    int i;
    int diffterra[2]={MAX_COST, MAX_COST};
    int diffair[2]={MAX_COST, MAX_COST};
    x1=dim_mappa.dimx-x1-1;
    x2=dim_mappa.dimx-x2-1;
    if (mappa==NULL)        //check se mappa è stata creata
    {
        fprintf(output, "%d\n", NOT_VALID_TRAVEL);
        return;
    }
    if (x1>=dim_mappa.dimx || x1<0 || x2>=dim_mappa.dimx || x2<0 || y1>=dim_mappa.dimy || y1<0 || y2>=dim_mappa.dimy || y2<0 || mappa[x1*dim_mappa.dimy + y1].costo==0)   //check se sono nei limiti mappa
    {
        fprintf(output, "%d\n", NOT_VALID_TRAVEL);
        return;
    }
    if(x1==x2 && y1==y2)
    {
        fprintf(output, "%d\n", NOT_VALID_COST);
        return;
    }
    //check se irraggiungibile sia via terra sia via aria
    if (verifica_nodi(mappa, x2, y2)==0 && mappa[x2*dim_mappa.dimy + y2].rotte_entranti==0)
    {
        fprintf(output, "%d\n", NOT_VALID_TRAVEL);
        return;
    }
    costo+=mappa[x1*dim_mappa.dimy + y1].costo;
    while (x1!=x2 || y1!=y2)
    {
        migliori_coordinate_terra(mappa, coord_terra, x1, y1, x2, y2);
        migliori_coordinate_aria(mappa, coord_air, x1, y1, x2, y2);
        if (coord_air[0]==NOT_VALID && coord_terra[0]==NOT_VALID)
        {
            fprintf(output, "%d\n", NOT_VALID_TRAVEL);
            for (i = 0; i < indice_coda; i++)     //incoda anche l'ultimo che non deve essere contato per cui indice_coda-1
            {
                coda[i*2]=NOT_VALID;
                coda[i*2+1]=NOT_VALID;
            }
            return;
        }
        
        
        diffterra[0]=abs(coord_terra[0]-x2);
        diffterra[1]=abs(coord_terra[1]-y2);
        diffair[0]=abs(coord_air[0]-x2);
        diffair[1]=abs(coord_air[1]-y2);

        if (diffair[0]<diffterra[0] && diffair[1]<diffterra[1])
        {
            coda[indice_coda*2]=coord_air[0];
            coda[indice_coda*2+1]=coord_air[1];
            x1=coda[indice_coda*2];
            y1=coda[indice_coda*2+1];
            indice_coda++;
        }
        else
        {
            coda[indice_coda*2]=coord_terra[0];
            coda[indice_coda*2+1]=coord_terra[1];
            x1=coda[indice_coda*2];
            y1=coda[indice_coda*2+1];
            indice_coda++;
        }
    }
    for (i = 0; i < indice_coda-1; i++)     //incoda anche l'ultimo che non deve essere contato per cui indice_coda-1
    {
        costo+=mappa[coda[i*2]*dim_mappa.dimy + coda[i*2+1]].costo;
    }
    //reset cache
    for (i = 0; i < indice_coda; i++)     //incoda anche l'ultimo che non deve essere contato per cui indice_coda-1
    {
        coda[i*2]=NOT_VALID;
        coda[i*2+1]=NOT_VALID;
    }
    fprintf(output, "%d\n", costo);
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
void aggiorna_costo(esagono_t *mappa, int xloc, int yloc, int v, int raggio, int dist_esagoni){
    int prog=0;
    prog=mappa[xloc*dim_mappa.dimy+yloc].costo+floor((float)v*max(0.0f,(raggio-dist_esagoni)/(float)raggio));
    if (prog<0)
    {
        prog=0;
    }
    if (prog>100)
    {
        prog=100;
    }
    mappa[xloc*dim_mappa.dimy+yloc].costo=prog;
    for (int i = 0; i < MAX_ROTTE_AR; i++)
    {
        if (mappa[xloc*dim_mappa.dimy+yloc].rotta_ar[i].costo!=NOT_VALID)
        {
            mappa[xloc*dim_mappa.dimy+yloc].rotta_ar[i].costo=prog;
        }
    }
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
int *alloca_coda(){
        int *coda;
        coda=malloc((2*dim_mappa.dimx*dim_mappa.dimy)*sizeof(int));
        return(coda);
}
void libera_coda(int *coda){
    if (coda==NULL)
    {
        return;
    }
    else{
        free(coda);
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
int verifica_nodi(esagono_t *mappa, int x, int y){
    nodi_adiacenti(x, y);
    int i=0;
    while(i<COLLEGAMENTI)
    {
        if (coordinate[i][0]>=0 && coordinate[i][0]<dim_mappa.dimx && coordinate[i][1]>=0 && coordinate[i][1]<dim_mappa.dimy)
        {
            if (mappa[coordinate[i][0]*dim_mappa.dimy + coordinate[i][1]].costo!=0)
            {
                return 1;
            }
        }
        i++;
    }
    return 0;
}
void migliori_coordinate_terra(esagono_t *mappa, int def[2], int x1, int y1, int x2, int y2){
    def[0]=NOT_VALID;
    def[1]=NOT_VALID;
    int i=0;
    int minor_costo=MAX_COST;
    int diffX=abs(x2-x1);
    int diffY=abs(y2-y1);
    int distanze[2];
    nodi_adiacenti(x1, y1);
    while (i<COLLEGAMENTI)
    {
        if (coordinate[i][0]>=0 && coordinate[i][0]<dim_mappa.dimx && coordinate[i][1]>=0 && coordinate[i][1]<dim_mappa.dimy)
        {
            if (mappa[coordinate[i][0]*dim_mappa.dimy + coordinate[i][1]].costo!=0)
            {
                calc_dist(distanze, coordinate[i][0], coordinate[i][1], x2, y2);
                if (mappa[coordinate[i][0]*dim_mappa.dimy + coordinate[i][1]].costo<minor_costo && (distanze[0]<diffX || distanze[1]<diffY))
                {
                    minor_costo=mappa[coordinate[i][0]*dim_mappa.dimy + coordinate[i][1]].costo;
                    diffX=distanze[0];
                    diffY=distanze[1];
                    def[0]=coordinate[i][0];
                    def[1]=coordinate[i][1];
                }
            }
        }
        i++;
    }
}
void calc_dist(int distanze[2], int startX, int startY, int arrX, int arrY){
    if (startX==arrX)
        {
            distanze[0]=abs(arrX-startX);
            distanze[1]=abs(arrY-startY);
            return;
        }
        if(startY==arrY)
        {
            distanze[0]=abs(arrX-startX);
            distanze[1]=abs(arrY-startY);
            return;
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
    distanze[0]=abs(arrX-startX);
    distanze[1]=abs(arrY-startY);
}
void migliori_coordinate_aria(esagono_t *mappa, int def[2], int x1, int y1, int x2, int y2){
    def[0]=NOT_VALID;
    def[1]=NOT_VALID;
    int minor_costo=MAX_COST;
    int diffX=abs(x2-x1);
    int diffY=abs(y2-y1);
    int distanze[2];
    for (int i = 0; i < MAX_ROTTE_AR; i++)
    {
        if (mappa[x1*dim_mappa.dimy + y1].rotta_ar[i].costo!=NOT_VALID)
        {
            if (mappa[x1*dim_mappa.dimy + y1].rotta_ar[i].costo!=0)
            {
                if (verifica_nodi(mappa, mappa[x1*dim_mappa.dimy + y1].rotta_ar[i].x, mappa[x1*dim_mappa.dimy + y1].rotta_ar[i].y)!=0)
                {
                    calc_dist(distanze, mappa[x1*dim_mappa.dimy + y1].rotta_ar[i].x, mappa[x1*dim_mappa.dimy + y1].rotta_ar[i].y, x2, y2);
                    if (mappa[x1*dim_mappa.dimy + y1].rotta_ar[i].costo<minor_costo && (distanze[0]<diffX || distanze[1]<diffY))
                    {
                        minor_costo=mappa[x1*dim_mappa.dimy + y1].rotta_ar[i].costo;
                        diffX=distanze[0];
                        diffY=distanze[1];
                        def[0]=mappa[x1*dim_mappa.dimy + y1].rotta_ar[i].x;
                        def[1]=mappa[x1*dim_mappa.dimy + y1].rotta_ar[i].y;
                    }
                }
                
            }
        }
        
    }
    
}