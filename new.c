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
#define MAX_ALLOCATED 100


//strutture
typedef struct mappa
{
    int **rotte_aeree;          //matrice in cui ci sono coord x y e costo rotta aerea
    u_int16_t **esagoni;        //matrice in cui sono presenti costi e flag already visited

}map;

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

void comando_init(u_int16_t **esagoni, FILE *output);
void comando_change_cost(map *mappa, int x, int y, int v, int raggio, FILE *output);
void comando_air_route(map *mappa, int x1, int y1, int x2, int y2, FILE *output);
void comando_travel_cost(map *mappa, int x1, int y1, int x2, int y2, FILE *output);
void alloca_mappa();
void libera_mappa(map *mappa);
float max (float n1, float n2);
void aggiorna_costo(map *mappa, int xloc, int yloc, int v, int raggio, int dist_esagoni);
void nodi_adiacenti(int x, int y);
int dist_esag(int startX, int startY, int arrX, int arrY);
int verifica_nodi(map *mappa, int x, int y);
int verifica_nodi_air(map *mappa, int x, int y, int arrX, int arrY);

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
                comando_init(mappa.esagoni, f_out);
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
void comando_init(u_int16_t **esagoni, FILE *output){
    for (int i = 0; i <dim_mappa.dimx*dim_mappa.dimy; i++)
    {
        esagoni[i][0]=1;
        esagoni[i][1]=BIANCO;
    }
    fprintf(output, "%s\n", AFFERMATIVO);
    #ifdef DEBUG
    for (int i = 0; i <dim_mappa.dimx*dim_mappa.dimy; i++)
    {

        fprintf(output, "%d", esagoni[i][0]);
        fprintf(output, "%d\n", esagoni[i][1]);
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
    int **coda=NULL;
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
    //creazione coda di coordinate
    coda=malloc(dim_coda*sizeof(int *));
    for (i = 0; i < dim_coda; i++)
    {
        coda[i]=malloc(2*sizeof(int));
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
    for (i = 0; i < dim_coda; i++)
    {
        free(coda[i]);
    }
    free(coda);
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
    }
    if (x1>=dim_mappa.dimx || x1<0 || x2>=dim_mappa.dimx || x2<0 || y1>=dim_mappa.dimy || y1<0 || y2>=dim_mappa.dimy || y2<0)   //check se sono nei limiti mappa
    {
        fprintf(output, "%s\n", FALSO);
        return;
    }
    //cancellazione rotta aerea
    for (int i = 0; i < MAX_ALLOCATED; i++)
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
        for (int i = 0; i < MAX_ALLOCATED; i++)
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
        fprintf(output, "%s\n", AFFERMATIVO);
        #ifdef DEBUG
        fprintf(output, "%d %d %d %d", x1, y1, x2, y2);
        fprintf(output, "%d", rotte);
        #endif
        return;
    }
    
}

void comando_travel_cost(map *mappa, int x1, int y1, int x2, int y2, FILE *output){
    int costo=0;
    int i;
    u_int8_t check1=0;
    u_int8_t check2=0;
    u_int16_t loc_cost;
    int arrivi_ar[2];
    int loc_dist=0;
    int mincost_terra;
    int mindist_terra;
    int mincost_air;
    int mindist_air;
    int coord_terra[2];
    int coord_aria[2];
    x1=dim_mappa.dimx-x1-1;
    x2=dim_mappa.dimx-x2-1;
    if (mappa==NULL)        //check se mappa è stata creata
    {
        fprintf(output, "%d\n", NOT_VALID_TRAVEL);
        return;
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
    //check se posso partire e arrivare in qualche modo
    check1=verifica_nodi(mappa, x1, y1);
    check2=verifica_nodi(mappa, x2, y2);
    for (i = 0; i < MAX_ALLOCATED; i++)
    {
        if ((check1==1 || (mappa->rotte_aeree[i][0]==x1 && mappa->rotte_aeree[i][1]==y1)) && (check2==1 || (mappa->rotte_aeree[i][2]==x2 && mappa->rotte_aeree[i][3]==y2)))
        {
            break;
        }
        else{
            fprintf(output, "%d\n", NOT_VALID_TRAVEL);
            return; 
        }
    }
    costo+=mappa->esagoni[x1*dim_mappa.dimy+y1][0];
    //calcolo percorso
    while (x1!=x2 || y1!=y2)
    {
        //inizializzazione ciclo
        mincost_terra=MAX_COST;
        mindist_terra=dim_mappa.dimx*dim_mappa.dimy;
        mincost_air=MAX_COST;
        mindist_air=dim_mappa.dimx*dim_mappa.dimy;
        //terra
        nodi_adiacenti(x1, y1);
        for (i = 0; i < COLLEGAMENTI; i++)
        {
            if (coordinate[i][0]>=0 && coordinate[i][0]<dim_mappa.dimx && coordinate[i][1]>=0 && coordinate[i][1]<dim_mappa.dimy)
            {
                loc_cost=mappa->esagoni[coordinate[i][0]*dim_mappa.dimy+coordinate[i][1]][0];
                if (loc_cost!=0 || (loc_cost==0 && coordinate[i][0]==x2 && coordinate[i][1]==y2))
                {
                    loc_dist=dist_esag(coordinate[i][0], coordinate[i][1], x2, y2);
                    if (loc_cost<mincost_terra && loc_dist<mindist_terra)
                    {
                        mincost_terra=loc_cost;
                        mindist_terra=loc_dist;
                        coord_terra[0]=coordinate[i][0];
                        coord_terra[1]=coordinate[i][1];
                    }
                }
            }
        }
        //aria
        if (mappa->rotte_aeree!=NULL)
        {
            for (i = 0; i < MAX_ALLOCATED; i++)
            {
                if (mappa->rotte_aeree[i][0]==x1 && mappa->rotte_aeree[i][1]==y1)
                {
                    arrivi_ar[0]=mappa->rotte_aeree[i][2];
                    arrivi_ar[1]=mappa->rotte_aeree[i][3];
                    loc_cost=mappa->esagoni[arrivi_ar[0]*dim_mappa.dimy+arrivi_ar[1]][0];
                    if (loc_cost!=0 && (arrivi_ar[0]!=x2 || arrivi_ar[1]!=y2))
                    {
                        if (verifica_nodi_air(mappa, arrivi_ar[0], arrivi_ar[1], x2, y2)==1)
                        {
                            loc_dist=dist_esag(arrivi_ar[0], arrivi_ar[1], x2, y2);
                            if (loc_cost<mincost_air && loc_dist<mindist_air)
                            {
                            mincost_air=loc_cost;
                            mindist_air=loc_dist;
                            coord_aria[0]=arrivi_ar[0];
                            coord_aria[1]=arrivi_ar[1];
                            }
                        }
                    }
                    if (loc_cost==0 && arrivi_ar[0]==x2 && arrivi_ar[1]==y2)
                    {
                        loc_dist=dist_esag(arrivi_ar[0], arrivi_ar[1], x2, y2);
                        if (loc_cost<mincost_air && loc_dist<mindist_air)
                        {
                        mincost_air=loc_cost;
                        mindist_air=loc_dist;
                        coord_aria[0]=arrivi_ar[0];
                        coord_aria[1]=arrivi_ar[1];
                        }
                    }
                }
            }
        }
        if (mindist_air<mindist_terra)
        {
            costo+=mincost_air;
            x1=coord_aria[0];
            y1=coord_aria[1];
        }
        else{
            costo+=mincost_terra;
            x1=coord_terra[0];
            y1=coord_terra[1];
        }
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
        for (int i = 0; i < MAX_ALLOCATED; i++)
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
int verifica_nodi(map *mappa, int x, int y){
    nodi_adiacenti(x, y);
    int i=0;
    while(i<COLLEGAMENTI)
    {
        if (coordinate[i][0]>=0 && coordinate[i][0]<dim_mappa.dimx && coordinate[i][1]>=0 && coordinate[i][1]<dim_mappa.dimy)
        {
            if (mappa->esagoni[coordinate[i][0]*dim_mappa.dimy + coordinate[i][1]][0]!=0)
            {
                return 1;
            }
        }
        i++;
    }
    return 0;
}
int verifica_nodi_air(map *mappa, int x, int y, int arrX, int arrY){
    nodi_adiacenti(x, y);
    int i=0;
    while(i<COLLEGAMENTI)
    {
        if (coordinate[i][0]>=0 && coordinate[i][0]<dim_mappa.dimx && coordinate[i][1]>=0 && coordinate[i][1]<dim_mappa.dimy)
        {
            if (mappa->esagoni[coordinate[i][0]*dim_mappa.dimy + coordinate[i][1]][0]!=0 || (mappa->esagoni[coordinate[i][0]*dim_mappa.dimy + coordinate[i][1]][0]==0 && coordinate[i][0]==arrX && coordinate[i][1]==arrY))
            {
                return 1;
            }
        }
        i++;
    }
    return 0;
}