/*Una pinacoteca prevede precise regole di accesso per i visitatori. Al più N visitatori possono essere contemporaneamente 
presenti entro la pinacoteca. Ciascun visitatore riceve un depliant, mentre con un sovrapprezzo sul può richiedere anche una
presentazione delle opere. La presentazione viene effettuata in presenza di un numero compreso tra K1 e K2 persone (i partecipanti 
alla presentazione "contano" tra i visitatori). Al termine della presentazione dell’esperto, gli utenti del gruppo visitano 
liberamente la pinacoteca con solo il depliant, mentre l'esperto può effettuare una nuova presentazione se sussistono le condizioni 
sopracitate. La politica di ammissione degli utenti deve privilegiare l'eventuale presenza di almeno K1 visitatori in attesa della 
presentazione con esperto non impegnato. (A titolo di esempio: N=40, K1=5, K2=10). */

/// SEMANTICA MESA ///

#define TRUE 1
#define FALSE 0

#define N 40
#define K1 5
#define K2 10

type pinacoteca = monitor
{
    int coda_liberi, coda_presentazione;
    int gruppo_in_attesa;
    int guidati_dentro;
    int liberi_dentro;
    int presentazione_iniziata; //booleano
    int presentazione_in_corso; //booleano
    int presentazione_finita; //booleano
    int esperto_disponibile; //booleano
    condition esperto_disponibile, esperto_inizia_a_spiegare, esperto_rilascia_gruppo;
    condition formazione_gruppo, inizio_presentazione, fine_presentazione;

    ///INIZIALIZZAZIONE VARIABILI
    coda_liberi = 0;
    coda_presentazione = 0;
    gruppo_in_attesa = 0;
    liberi_dentro = 0;
    guidati_dentro = 0;
    presentazione_iniziata = FALSE;
    presentazione_in_corso = FALSE;
    presentazione_finita = FALSE;
    esperto_disponibile = FALSE;

    /// METODI 
    procedure entry ingresso_presentazione() {

        coda_presentazione++;
        while (gruppo_in_attesa == K2 || esperto_disponibile == FALSE) {
            formazione_gruppo.wait(); //quando il gruppo è pieno o l'esperto è impegnato, la coda cresce
        }
        coda_presentazione--;
        gruppo_in_attesa++;

        if (gruppo_in_attesa == K1 && esperto_disponibile == TRUE) {
            esperto_disponibile.signal(); //se l'esperto è disponibile, lo risveglio
        }

        while (presentazione_iniziata == FALSE) {
            inizio_presentazione.wait();
        }

        gruppo_in_attesa--;
        guidati_dentro++;
        
        if (gruppo_in_attesa == 0) {
            presentazione_iniziata = FALSE; //l'ultimo a uscire dalla visita guidata resetta e risevglia la guida.
            esperto_inizia_a_spiegare.signal();
        }

        while (presentazione_finita == FALSE) {
            fine_presentazione.wait();
        }

        guidati_dentro--;
        liberi_dentro++;

        if (guidati_dentro == 0) {
            presentazione_finita = FALSE; //l'ultimo a uscire dalla visita guidata resetta e risveglia la guida.
            esperto_rilascia_gruppo.signal();
        }
    }

    procedure entry inizia_nuova_presentazione() {
        esperto_disponibile = TRUE;

        for (int i = 0; i < min(K2, coda_presentazione); i++) {
            formazione_gruppo.signal(); //risveglio i visitatori in coda
        }

        while (gruppo_in_attesa < K1 || gruppo_in_attesa + liberi_dentro > N) {
            esperto_disponibile.wait(); //l'esperto aspetta che il gruppo raggiunga la soglia minima
        }

        esperto_disponibile = FALSE; //l'esperto smette di essere disponibile per chi entra
        presentazione_iniziata = TRUE; //l'esperto avvia la presentazione

        for(int i = gruppo_in_attesa; i >= 0; i--) {
            inizio_presentazione.signal(); //risveglia tutto il gruppo di passeggeri.
        }

        while (gruppo_in_attesa > 0) {
            esperto_inizia_a_spiegare.wait(); //l'esperto aspetta che il gruppo finisca di entrare
        }

    }

    procedure entry finisci_presentazione() {
        presentazione_finita = TRUE; //l'esperto finisce la presentazione
        
        for(int i = guidati_dentro; i >= 0; i--) {
            fine_presentazione.signal(); //risveglia tutto il gruppo di passeggeri.
        }

        while(guidati_dentro > 0) {
            esperto_rilascia_gruppo.wait(); //l'esperto aspetta che il gruppo finisca di uscire
        }
    }

    procedure entry ingresso_libero() {
        while (liberi_dentro + guidati_dentro + gruppo_in_attesa > N ) {
            coda_liberi++;
            ingresso_liberi.wait();
            coda_liberi--;
        }
        liberi_dentro++;
    }

    procedure entry uscita_visitatore() {
        liberi_dentro--; //decrementa il numero di visitatori e risveglia i visitatori in coda
        if (esperto_disponibile == TRUE && gruppo_in_attesa >= K1 && liberi_dentro + guida_dentro < N) {
            ingresso_liberi.signal();
        } else if (coda_liberi > 0) {
            ingresso_liberi.signal();
        }
    }
}

///CREAZIONE ISTANZE
pinacoteca esperto;

while(TRUE) {
    esperto.inizia_nuova_presentazione();
    esperto.finisci_presentazione();
}

pinacoteca libero;
pinacoteca guidato;

libero.ingresso_libero();
//fa la sua visita
libero.uscita_visitatore();

guidato.ingresso_presentazione();
//entra quando il gruppo è formato e quando l'esperto è disponibile
guidato.uscita_visitatore();