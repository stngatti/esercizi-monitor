/* Un campo da golf ha a disposizione P palline. Il campo è frequentato da giocatori esperti e principianti. Gli esperti
noleggiano 2 palline e hanno la priorità sui principianti; i principianti noleggiano un numero maggiore di palline,
compreso tra 3 e N (N < P). I giocatori, una volta terminato di giocare, devono restituire il numero esatto di palline
noleggiate all’inizio del gioco. Si implementi una soluzione usando il costrutto monitor per modellare il campo da golf e 
i processi per modellare i giocatori e si descriva la sincronizzazione tra i processi. Nel rispettare i vincoli richiesti, 
si cerchi di massimizzare l'utilizzo delle risorse. Si discuta se la soluzione proposta può presentare starvation e in caso 
positivo per quali processi, e si propongano modifiche e/o aggiunte per evitare starvation. */

/// SEMANTICA MESA ///

#define P 100
#define N 10
#define MIN_P 3

type golf = monitor{
    int n_palline, coda_esperti, coda_principianti;
    enum type tipo_giocatore {PRINCIPIANTE, ESPERTO};
    var condition esperti, principianti;


    /// INIZIALIZZAZIONE ///
    n_palline = P;
    coda_esperti = 0;
    coda_principianti = 0;
    int i = 0

    /// METODI ///
    procedure entry esperto_vuole_giocare() {
        coda_esperti++;
        while (n_palline < 2) {
            esperti.wait();
        }
        n_palline -= 2;
        coda_esperti--;
    }

    procedure entry principiante_vuole_giocare(int palline_da_noleggiare) {
        coda_principianti++;
        if (palline_da_noleggiare < MIN_P) { palline_da_noleggiare = MIN_P; } 
        else if (palline_da_noleggiare > N) { palline_da_noleggiare = N; }

        while (n_palline < palline_da_noleggiare || coda_esperti > 0) {
            principianti.wait();
        }
        n_palline -= palline_da_noleggiare;
        coda_principianti--;
    }

    procedure entry esperto_restituisce_palline() {
        n_palline += 2;
        if (coda_esperti > 0) { esperti.notify(); }
        else if (coda_principianti > 0) { principianti.notify(); }
    }

    procedure entry principiante_restituisce_palline(int palline_da_restituire) {
        if (palline_da_noleggiare < MIN_P) { palline_da_noleggiare = MIN_P; } 
        else if (palline_da_noleggiare > N) { palline_da_noleggiare = N; }

        n_palline += palline_da_restituire;
        if (coda_principianti > 0) { principianti.notify(); }
        else if (coda_esperti > 0) { esperti.notify(); }
    }
    
}

///ISTANZE///
tiger_woods.esperto_vuole_giocare();
<gioca>
tiger_woods.restituisce_palline();
<se ne va a casa>

geronimo_stilton.principiante_vuole_giocare(5);
<gioca>
geronimo_stilton.restituisce_palline(5);
<se ne va a casa>