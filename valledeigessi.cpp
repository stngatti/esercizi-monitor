/*TRACCIA D'ESAME Accesso al parco dell'alta valle dei Gessi solo a N visitatori. I turisti possono 
scegliere tra visita libera L e guidata G. La visita L include la visita alle fonti delle 
Fonti di Poiano. La visita guidata richiede min 10 e max 20 visitatori. I visitatori devono 
aspettare la formazione del gruppo e la presenza dell'unica guida. I visitatori G entrano solo 
se ci sono abbastanza posti disponibili, però hanno la priorità sui turisti L, che vengono sospesi 
finche non viene ammesso un gruppo G*/

//SEMANTICA MESA

#define MIN 10
#define MAX 20
#define N 50

type parco = monitor{
    bool guida_impegnata, in_viaggio;
    int coda_liberi, coda_guidati, visitatori_dentro, dim_prossimo_gruppo, dim_gruppo_dentro;
    var condition libero, guidati, gruppo_pronto, visita_parco, ristoro_fonti_poiano;


    /// INIZIALIZZAZIONE ///
    guida_impegnata = false;
    in_viaggio = false;
    visitatori_dentro = 0;
    dim_prossimo_gruppo = 0;
    dim_gruppo_dentro = 0;
    coda_liberi = 0;
    coda_guidati = 0;
    ritrovo = 0;

    /// METODI ///
    procedure entry libero_vuole_entrare() {
        coda_liberi++;
        while (visitatori_dentro == N || 
        visitatori_dentro + coda_guidati > N) {
            libero.wait(); //aspetta che ci sia spazio
        }
        visitatori_dentro++;
        coda_liberi--;
    }

    procedure entry guidato_vuole_entrare() {
        coda_guidati++;
        if (coda_guidati >= MIN) {
            gruppo_pronto.notify(); //se il gruppo raggiunge la grandezza minima, risveglia la guida
        }
        while ( guida_impegnata || dim_prossimo_gruppo == MAX || coda_guidati > 0  ) {
            guidati.wait(); //attendono tutti in coda senza sorpassi
        }
        coda_guidati--;
        visitatori_dentro++;
        dim_prossimo_gruppo++;
        while (in_viaggio) { 
            visita_parco.wait(); //una volta risvegliato può fare quello che vuole
        }
        dim_gruppo_dentro++; //scendono dal pullman e cambiano gruppo
        dim_prossimo_gruppo--;
    }

    procedure entry avvia_guida() {
        guida_impegnata = false;
        while (coda_guidati < MIN || !fine_guida) {
            gruppo_pronto.wait(); //aspetta che il gruppo raggiunga la grandezza minima o che la visita di prima sia finita
        }
        in_viaggio = true;
        for(int i = 0; i <= min(MAX, coda_guidati); i++) {
            guidati.notify(); 
        }
        guida_impegnata = true; //la guida rimane impegnata per il prossimo gruppo
    }

    procedure entry libero_vuole_uscire() {
        visitatori_dentro--;
        if (visitatori_dentro + MAX < N) { 
            libero.notify(); //se ci sono abbastanza posti liberi, risveglia un visitatore libero indipendentemente dalla grandezza della coda
        }
    }

    procedure entry guidato_vuole_uscire() {
        ritrovo++; 
        while (ritrovo != dim_gruppo_dentro || !fine_visita) {
            ristoro_fonti_poiano.wait(); //finchè non si possono salutare tutti non se ne va nessuno
        }
        visitatori_dentro--;
    }

    procedure entry termina_guida() {
        fine_visita = true;
        for (int i = 0; i < dim_gruppo_dentro; i++) {
            ristoro_fonti_poiano.notify(); //saluta tutti
        }
        if (coda_guidati >= MIN) {
            gruppo_pronto.notify(); //per pianificare la prossima guida
        }
        while (persone_dentro + coda_guidati <= N && coda_liberi > 0) {
            libero.notify(); //risveglia i visitatori liberi
        }
    }
}

/// CREAZIONE ISTANZE
visitatorel.libero_vuole_entrare()
<ammira la valle dei gessi>
visitatorel.libero_vuole_uscire()
<esce>

visitatoreg.guidato_vuole_entrare()
<aspetta il pullmino e va al parco>
visitatoreg.guidato_vuole_uscire()
<aspetta di mangiare con gli altri e la guida>
<esce>

guida.avvia_guida()
<controlla il pullman e parte>
guida.termina_guida()
<si presenta al ristorante e saluta tutti>