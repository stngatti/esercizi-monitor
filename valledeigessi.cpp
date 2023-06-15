/*TRACCIA D'ESAME Accesso al parco dell'alta valle dei Gessi solo a N visitatori. I turisti possono 
scegliere tra visita libera L e guidata G. La visita L include la visita alle fonti delle 
Fonti di Poiano. La visita guidata richiede min 10 e max 20 visitatori. I visitatori devono 
aspettare la formazione del gruppo e la presenza dell'unica guida. I visitatori G entrano solo 
se ci sono abbastanza posti disponibili, però hanno la priorità sui turisti L, che vengono sospesi 
finche non viene ammesso un gruppo G*/

/// SEMANTICA MESA ///

#define N 100
#define MIN 10
#define MAX 20



type parco = monitor{
    bool guida_impegnata, in_viaggio;
    int coda_liberi, coda_guidati, visitatori_dentro, dim_prossimo_gruppo, dim_gruppo_guida;
    var condition libero, guidato, coda_gruppo, visita_parco, ristoro_fonti_poiano;


    /// INIZIALIZZAZIONE ///
    guida_impegnata = false;
    visitatori_dentro = 0;
    dim_prossimo_gruppo = 0;
    coda_liberi = 0;
    coda_guidati = 0;
    ritrovo = 0;

    /// METODI ///
    procedure entry libero_vuole_entrare() {
        coda_liberi++;
        while (visitatori_dentro == N || 
        visitatori_dentro + dim_prossimo_gruppo > N) {
            libero.wait();
        }
        visitatori_dentro++;
    }

    procedure entry guidato_vuole_entrare() {
        coda_guidati++;
        dim_prossimo_gruppo = min(MAX, dim_prossimo_gruppo + 1);
        while (visitatori_dentro + dim_prossimo_gruppo > N || 
            guida_impegnata ||
            dim_prossimo_gruppo < MIN) {
            guidati.wait();
        }
        coda_guidati--;
        visitatori_dentro++;
        while (in_viaggio) { visita_parco.wait(); } //una volta risvegliato può fare quello che vuole
    }

    procedure entry avvia_guida() {
        dim_gruppo_guida = min(dim_prossimo_viaggio, MAX); //accetta il primo gruppo
        for(int i = 0; i < dim_gruppo_guida; i++) {
            guidati.notify(); 
        }
        guida_impegnata = true; //la guida rimane impegnata per il prossimo gruppo
        in_viaggio = true;
        dim_prossimo_viaggio = 0;
    }

    procedure entry libero_vuole_uscire() {
        visitatori_dentro--;
        if (visitatori_dentro + dim_prossimo_gruppo < N) { libero.notify(); }
    }

    procedure entry guidato_vuole_uscire() {
        ritrovo++;
        while (ritrovo != dim_gruppo_guida || guida_impegnata = true) {
            ristoro_fonti_poiano.wait();
        }
        n_visitatori--;
    }

    procedure entry termina_guida() {
        guida_impegnata = false;
        for (int i = 0; i < dim_gruppo_guida; i++) {
            ristoro_fonti_poiano.notify();
        }
        if (coda_guidati > 0) {
            guidati.notify();
        }
        else if (coda_liberi > 0) {
            libero.notify();
        }
    }
}

/// CREAZIONE ISTANZE
visitatorel.libero_vuole_entrare()
<ammira la valle dei gessi>
visitatorel.libero_vuole_uscire()
<se ne torna a Pavia>

visitatoreg.guidato_vuole_entrare()
<aspetta il pullmino e va al parco>
visitatoreg.guidato_vuole_uscire()
<aspetta di mangiare con gli altri e la guida>
<esce>

guida.avvia_guida()
<controlla il pullman e parte>
guida.termina_guida()
<si presenta al ristorante>
