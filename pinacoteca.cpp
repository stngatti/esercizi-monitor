/*Una pinacoteca prevede precise regole di accesso per i visitatori. Al più N visitatori 
possono essere contemporaneamente presenti entro la pinacoteca. Ciascun visitatore riceve un 
depliant, mentre con un sovrapprezzo sul può richiedere anche una presentazione delle opere. 
La presentazione viene effettuata in presenza di un numero compreso tra K1 e K2 persone (i 
partecipanti alla presentazione "contano" tra i visitatori). Al termine della presentazione 
dell’esperto, gli utenti del gruppo visitano liberamente la pinacoteca con solo il depliant, 
mentre l'esperto può effettuare una nuova presentazione se sussistono le condizioni sopracitate. 
La politica di ammissione degli utenti deve privilegiare l'eventuale presenza di almeno K1 
visitatori in attesa della presentazione con esperto non impegnato. 
(A titolo di esempio: N=40, K1=5, K2=10). */

/// SEMANTICA MESA ///

#define N 40
#define K1 5
#define K2 10

type pinacoteca = monitor {
    int n_visitatori, n_presentazioni, coda_presentazione, coda_ingresso;
    bool esperto_impegnato, ascoltando_presentazione;
    var condition presentazione, fine_presentazione;
    var condition ingresso;

    /// INIZIALIZZAZIONE ///
    n_visitatori = 0;
    n_presentazioni = 0;
    coda_presentazione = 0;
    coda_ingresso = 0;
    esperto_impegnato = false;
    ascoltando_presentazione = false;

    /// METODI ///

    procedure entry entra() {
        while (n_visitatori == N) {
            coda_ingresso++;
            ingresso.wait();
            coda_ingresso--;
        }
        n_visitatori++;
    }

    procedure entry ascolta_presentazione() {
        coda_presentazione++;
        while (esperto_impegnato || coda_presentazione < K1 || n_presentazioni > K2) {
           n_presentazioni++;
           presentazione.wait();
        }
        coda_presentazione--;
        while (ascoltando_presentazione) {
            fine_presentazione.wait();
        }
    }

    procedure entry avvia_presentazione() {
        for (int i = 0; i < min(coda_presentazione, K2); i++) {
            presentazione.notify();
        }
        esperto_impegnato = true;
        ascoltando_presentazione = true;
    }

    procedure entry concludi_presentazione() {
        esperto_impegnato = false;
        for (int i = 0; i < n_presentazioni; i++) {
            fine_presentazione.notify();
        }
        n_presentazioni = 0;
        if (coda_presentazione > K1) {
            presentazione.notify();
        }
        else if (coda_ingresso > 0) {
            ingresso.notify();
        }
    }




}