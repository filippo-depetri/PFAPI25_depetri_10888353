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
#include <string.h>
#include <stdint.h>

//define
//#define DEBUGTEST
#define DEBUG
#define INIT 'i'
#define CAMBIO_COSTO 'c'
#define ROTTA_AEREA 'o'
#define COSTO_VIAGGIO 't'
#define AFFERMATIVO "OK"
#define FALSO "KO"
#define NOT_VALID_COST -1
#define LUNGHEZZA_STR_COMANDO_MAX 16


//strutture
typedef struct rottaar
{
    u_int16_t x;
    u_int16_t y;
}rottaar_t;

struct generale_comandi
{
    u_int16_t colx;
    u_int16_t rigx;
    u_int16_t coly;
    u_int16_t rigy;
    u_int16_t v;
    u_int16_t raggio;
}gen_comandi;
struct dimensioni_mappa
{
    u_int16_t dimx;     //dim righe
    u_int16_t dimy;     //dim colonne
}dim_mappa;


typedef struct esagono
{
    int costo;
    rottaar_t rotta_ar[5];
    u_int8_t already_visited;
}esagono_t;

void comando_init(esagono_t mappa[dim_mappa.dimx][dim_mappa.dimy], int col, int rig);
void comando_change_cost(esagono_t mappa[dim_mappa.dimx][dim_mappa.dimy], int x, int y, int v, int raggio);
void comando_air_route(esagono_t mappa[dim_mappa.dimx][dim_mappa.dimy], int x1, int y1, int x2, int y2);
void comando_travel_cost(esagono_t mappa[dim_mappa.dimx][dim_mappa.dimy], int x1, int y1, int x2, int y2);

int main(){
    //PREPARATIVI
    //variabili per gestione comandi
        char comando[LUNGHEZZA_STR_COMANDO_MAX];
        int sc;
        FILE *f_in, *f_out;
        f_in=stdin;
        f_out=stdout;
    //fine variabili gestione comandi
    #ifdef DEBUGTEST
    int ssc;
    char c;
    do{
        ssc=fscanf(f_in, "%c", &c);
        fprintf(f_out, "%c", c);
    }while(ssc!=EOF);
    #endif
    //allocazione
        sc=fscanf(f_in, "%s", &comando);
        if (sc==EOF || comando[0]!=INIT)
        {
            return -1;
        }
        sc=fscanf(f_in, "%hd", &dim_mappa.dimx);
        sc=fscanf(f_in, "%hd", &dim_mappa.dimy);
        esagono_t mappa[dim_mappa.dimx][dim_mappa.dimy];
        comando_init(mappa, dim_mappa.dimx, dim_mappa.dimy);
        #ifdef DEBUG
        printf("INIT0\n");
        #endif
    //fine allocazione
    //FINE PREPARATIVI
    //COMANDI DOPO LA PRIMA INIZIALIZZAZIONE
    do{
        sc=fscanf(f_in, "%s", &comando);
       if(comando[0]==INIT){
            sc=fscanf(f_in, "%hd", &dim_mappa.dimx);
            sc=fscanf(f_in, "%hd", &dim_mappa.dimy);
            esagono_t mappa[dim_mappa.dimx][dim_mappa.dimy];
            comando_init(mappa, dim_mappa.dimx, dim_mappa.dimy);
            #ifdef DEBUG
            printf("INIT\n");
            #endif
        }
        else {
            if(comando[0]==CAMBIO_COSTO) {
            sc=fscanf(f_in, "%hd", &gen_comandi.colx);
            sc=fscanf(f_in, "%hd", &gen_comandi.rigx);
            sc=fscanf(f_in, "%hd", &gen_comandi.v);
            sc=fscanf(f_in, "%hd", &gen_comandi.raggio);
            comando_change_cost(mappa, gen_comandi.colx, gen_comandi.rigx, gen_comandi.v, gen_comandi.raggio);
            #ifdef DEBUG
            printf("CHANGE_COST\n");
            #endif
            }
            else{
                if(comando[1]==ROTTA_AEREA){
                    sc=fscanf(f_in, "%hd", &gen_comandi.colx);
                    sc=fscanf(f_in, "%hd", &gen_comandi.rigx);
                    sc=fscanf(f_in, "%hd", &gen_comandi.coly);
                    sc=fscanf(f_in, "%hd", &gen_comandi.rigy);
                    comando_air_route(mappa, gen_comandi.colx, gen_comandi.rigx,gen_comandi.coly, gen_comandi.rigy);
                    #ifdef DEBUG
                    printf("AIR_ROUTE\n");
                    #endif
                }
                else{
                    sc=fscanf(f_in, "%hd", &gen_comandi.colx);
                    sc=fscanf(f_in, "%hd", &gen_comandi.rigx);
                    sc=fscanf(f_in, "%hd", &gen_comandi.coly);
                    sc=fscanf(f_in, "%hd", &gen_comandi.rigy);
                    comando_travel_cost(mappa, gen_comandi.colx, gen_comandi.rigx,gen_comandi.coly, gen_comandi.rigy);
                    #ifdef DEBUG
                    printf("TRAVEL\n");
                    #endif
                }
            }
        }

    }while(sc!=EOF);
    fclose(f_in);
    fclose(f_out);
    return 0;
}

