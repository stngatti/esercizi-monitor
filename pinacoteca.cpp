/*Una pinacoteca prevede precise regole di accesso per i visitatori. Al più N visitatori possono essere contemporaneamente 
presenti entro la pinacoteca. Ciascun visitatore riceve un depliant, mentre con un sovrapprezzo sul può richiedere anche una
presentazione delle opere. La presentazione viene effettuata in presenza di un numero compreso tra K1 e K2 persone (i partecipanti 
alla presentazione "contano" tra i visitatori). Al termine della presentazione dell’esperto, gli utenti del gruppo visitano 
liberamente la pinacoteca con solo il depliant, mentre l'esperto può effettuare una nuova presentazione se sussistono le condizioni 
sopracitate. La politica di ammissione degli utenti deve privilegiare l'eventuale presenza di almeno K1 visitatori in attesa della 
presentazione con esperto non impegnato. (A titolo di esempio: N=40, K1=5, K2=10). */

/// SEMANTICA MESA ///

#define N 40 //numero massimo di visitatori
#define K1 5
#define K2 10

type pinacoteca = monitor {
    int n_depliant, dim_gruppo, coda_presentazione, coda_ingresso;
    bool esperto_impegnato, ascoltando_presentazione;
    var condition presentazione, fine_presentazione;
    var condition ingresso;

    /// INIZIALIZZAZIONE ///
    n_depliant = 0;
    dim_gruppo = 0;
    coda_presentazione = 0;
    coda_ingresso = 0;
    esperto_impegnato = false;
    ascoltando_presentazione = false;

    /// METODI ///

    procedure entry entra() {
        coda_ingresso++;
        while (n_depliant == N) {
            ingresso.wait();
        }
        coda_ingresso--;
        n_depliant++;
    }

    procedure entry ascolta_presentazione() {
        coda_presentazione++;
        while (esperto_impegnato || coda_presentazione < K1 || dim_gruppo == K2) {
           presentazione.wait();
           dim_gruppo++;
        }
        while (!esperto_impegnato) {
            fine_presentazione.wait();
        }
    }

    procedure entry avvia_presentazione() {
        esperto_impegnato = true;
        coda_presentazione -= dim_gruppo;
        for (int i = 0; i < dim_gruppo; i++) {
            presentazione.notify();
        }
    }

    procedure entry concludi_presentazione() {
        esperto_impegnato = false; //condizione 1 per entrare nel gruppo
        int n_richiami = dim_gruppo;
        dim_gruppo = 0; //condizione 3 per entrare nel gruppo
        for (int i = 0; i < n_richiami; i++) {
            fine_presentazione.notify();
        }
        while (dim_gruppo < K2 ) {
            ingresso.notify();
        }

    }

    procedure entry esci() {
        n_depliant--;
        if (n_depliant < N) {
            ingresso.notify();
        }
    }
}

///CREA ISTANZE///
visitatore.entra();
<prende il depliant>
visitatore.ascolta_presentazione();
<ascolta la presentazione>
visitatore.esci();
<va a vedere la Pilotta>

vittorio_sgarbi.avvia_presentazione();
<inizia la presentazione>
vittorio_sgarbi.concludi_presentazione();