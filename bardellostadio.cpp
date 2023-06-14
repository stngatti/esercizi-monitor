/*Il bar dello stadio serve tifosi locali e ospiti. Il bar ha capacità NMAX e non 
può ospitare contemporaneamente tifosi di squadre opposte. Il barista può chiudere il locale
in qualsiasi momento per la pulizia. Durante la chiusura non può entrare nessuno ma possono uscire
quelli che sono già dentro */

/// SEMANTICA MESA ///
#define NMAX 1000
#define LOCALE 0
#define OSPITE 1


type bar = monitor {
    
    int coda_locali, coda_ospiti, persone_dentro;
    bool aperto, tifosi_locali;
    var condition locale_in_coda, ospite_in_coda, chiusura; 

    /// INIZIALIZZAZIONE ///
    tifosi_locali = true;
    persone_dentro = 0;

    /// METODI ///
    procedure entry entra(int squadra) {
        switch (squadra) {
            case LOCALE:
                while (!aperto || tifosi_locali == false || persone_dentro == NMAX) {
                    coda_locali++;
                    locale_in_coda.wait();
                    coda_locali--;
                }
                persone_dentro++;
                break;
            case OSPITE:
                while (!aperto || tifosi_locali == true || persone_dentro == NMAX) {
                    coda_ospiti++;
                    ospite_in_coda.wait();
                    coda_ospiti--;
                }
                persone_dentro++;
                break;
            }
        }
    
    procedure entry esci() {
        persone_dentro--;
        if (persone_dentro == 0) {
            chiusura.notify(); //"L'ultimo chiude la porta"
        } 
    }

    procedure entry pulisci_il_locale() {
        aperto = false;
        while (persone_dentro > 0) {
            chiusura.wait();
        }
    }

    procedure entry riapri() {
        while (aperto) {
            if (coda_locali > 0 && tifosi_locali == true ) locale_in_coda.notify();
            else if (coda_ospiti > 0 && tifosi_locali == false) ospite_in_coda.notify();
            else if (posti_dentro == 0) tifosi_locali = !tifosi_locali;
        }
    }
}

/// CREAZIONE ISTANZA ///
bar bar_stadio;
bar cliente;

/// THREADS ///
cliente.entra(LOCALE);
<bevi>
cliente.esci();

bar_stadio.riapri();
<serve da bere finche non c'è un casino>
bar_stadio.pulisci_il_locale();
<fai uscire tutti e chiudi il bar>
bar_stadio.riapri();