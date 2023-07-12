/*L’azienda di ristorazione che gestisce il servizio mensa del campus ha deciso di adottare una politica di gestione volta ad ottimizzare 
l’utilizzo dei tavoli ed evitare interferenze tra diverse tipologie dei clienti. La mensa è dotata di k tavoli con capienza convenzionale
4 persone ciascuno. Il numero totale di posti disponibili è quindi N=4k. Sono previste due tipologie di clienti: Studenti (S) e Professori (P). 
Clienti di tipo diverso non devono tuttavia condividere lo stesso tavolo, ovvero se un tavolo è parzialmente occupato da clienti di un 
tipo in esso non possono essere fatti accomodare clienti dell’altro tipo. E’ inoltre presente un cameriere di sala che provvede ad 
accogliere e fare accomodare ciascun cliente (uno per volta) ad un tavolo appropriato, se il posto è disponibile. Una volta sistemato
al tavolo il cliente, il cameriere va ad approvvigionarsi del cibo ordinato per poi fornire il pasto al cliente. Al termine di questa 
azione si rende disponibile ad accogliere un altro cliente se possibile, ovvero si pone in attesa. In assenza di posti disponibili di 
tipo appropriato i clienti devono restare in attesa. I clienti, dopo aver ricevuto il cibo consumano liberamente il pasto senza ulteriori 
interazioni con il cameriere ne con gli altri clienti dello stesso tavolo, e al termine liberano il posto. La politica proposta prevede 
che sia data priorità ai Professori e che si ricerchi il più efficiente utilizzo dei tavoli, compatibilmente con le specifiche precedenti.*/

///SEMANTICA MESA///

#define K 40
#define N (MAX)*(K)
#define MAX 4

#define STUDENTE 0
#define PROFESSORE 1

#define TRUE 1
#define FALSE 0

type ristorante = monitor 
{
    condition coda_P, coda_S;
    condition cameriere_fa_accomodare;
    condition cameriere_prende_ordine;
    condition cibo;

    int professori_fuori, studenti_fuori;
    int studenti_dentro, professori_dentro;

    int cameriere_disponibile = FALSE; //booleano
    int cameriere_aspetta_clienti = FALSE; //booleano
    int pasto_pronto = FALSE; //booleano


    //int tavoli_occupati = 0;
    int tavoli_professori = 0;
    int tavoli_studenti = 0;
    int posti_occupati = 0;

    procedure entry cliente_entra(int tipo) {
        switch (tipo) {
            case STUDENTE:
                if (cameriere_aspetta_clienti == TRUE) {
                    //il cliente che arriva si mette in coda a prescindere. Se il cameriere non sta lavorando lo sveglia.
                    cameriere_fa_accomodare.signal();
                }
                studenti_fuori++;
                while (cameriere_disponibile == FALSE || tavoli_occupati == K) {
                    coda_S.wait();
                }
                cameriere_disponibile = FALSE; //una volta che il cameriere ha risvegliato un cliente reimposta il suo stato a non disponibile per fare aspettare gli altri
                studenti_fuori--;
                studenti_dentro++;
                posti_occupati++;

                while (pasto_pronto == FALSE) {
                    cibo.wait();
                }
                break;
            case PROFESSORE:
                if (professori_fuori > 0 || posti_occupati != N || cameriere_aspetta_clienti == TRUE) {
                    cameriere_fa_accomodare.signal();
                }
                professori_fuori++;
                while (cameriere_disponibile == FALSE || tavoli_occupati == K) {
                    coda_P.wait();
                }
                professori_fuori--;
                professori_dentro++;
                posti_occupati++;

                while (pasto_pronto == FALSE) {
                    cibo.wait();
                }
        }

    }

    procedure entry fai_entrare_clienti() {
        pasto_pronto = FALSE;
        cameriere_aspetta_clienti = TRUE;

        while ((studenti_fuori == 0 && professori_fuori == 0) || posti_occupati == N) {
            cameriere_fa_accomodare.wait(); //se non c'è nessuno il cameriere aspetta
        }
        cameriere_aspetta_clienti = FALSE; 
        cameriere_disponibile = TRUE;
        if (professori_fuori > 0 && (MAX - posti_occupati) > (studenti_dentro % MAX) && posti_occupati != N) {
            coda_P.signal();
        }
        else if (studenti_fuori > 0 && (MAX - posti_occupati) > (professori_dentro % MAX) && posti_occupati != N) {
            coda_S.signal();
        }

    }

    procedure entry servi_cibo() {
        pasto_pronto = TRUE;
        cibo.signal();
    }
    
    procedure entry cliente_esce(int tipo) {
        switch (tipo) {
            case STUDENTE:
                studenti_dentro--;
                posti_occupati--;
                if (posti_occupati == N - 1) {
                    cameriere_fa_accomodare.signal();
                }
                break;
            case PROFESSORE:
                professori_dentro--;
                posti_occupati--;
                if (posti_occupati == N - 1) {
                    camerie_fa_accomodare.signal();
                }
                break;
        }
    }
}

///CREAZIONE ISTANZE

ristorante cameriere;

while(1) {
    cameriere.fai_entrare_clienti();
    cameriere.servi_cibo();
}

ristorante student;

student.cliente_entra(STUDENTE);
//mangia
student.cliente_esce(STUDENTE);

ristorante professore;

professore.cliente_entra(PROFESSORE);
//mangia
professore.cliente_esce(PROFESSORE);


