/* Viaggio tra Ouarzazate e Zagora in taxi, che hanno 7 posti passeggeri. Parte da Ouarzazate con almeno 5 passeggeri. In assenza
di passeggeri il tassista attende. Quando il taxi è pieno, parte per Zagora. Quando arriva a Zagora, scarica i passeggeri e, se ci
sono almeno 5 visitatori in attesa, carica i turisti e li riporta a Ouarzazate. A Zagora non si può dormire e quindi bisogna riportare
tutti indietro prima di concludere.*/

///SEMANTICA MESA///
#define MIN 5
#define MAX 7


type taxi = monitor {
    
    ///VARIABILI///
    bool taxi_a_zagora, arrivati;
    int coda_ouarzazate, coda_zagora, persone_a_zagora;
    int n_passeggeri;
    var condition attesa_andata, attesa_ritorno; //condizioni per la gestione delle code
    var condition attesa_passeggeri_andata; //variabili per il servitore
    var condition visita_zagora, ritorno_a_casa; //dorme durante il viaggio

    ///INIZIALIZZAZIONE
    taxi_a_zagora = false;
    coda_ouarzazate = 0;
    coda_zagora = 0;
    persone_a_zagora = 0;


    ///METODI///
    procedure entry passeggero_vuole_partire_da_Ouarzazate() {
        coda_ouarzazate++;
        while (taxi_a_zagora || n_passeggeri >= MAX) {
            attesa_andata.wait(); //aspetta che il taxi sia disponibile e che ci siano abbastanza passeggeri
        }
        coda_ouarzazate--;
        n_passeggeri++; //incrementa il numero di passeggeri nel taxi
        if (n_passeggeri >= MIN) {
            attesa_passeggeri_andata.signal();
        }
        while (!arrivati) {
            visita_zagora.wait(); //si addormenta durante il viaggio
        }
        n_passeggeri--; //decrementa il numero di passeggeri nel taxi e aumenta il numero di persone a zagora
        persone_a_zagora++;
    }

    procedure entry taxi_parte_per_Zagora() {
        taxi_a_zagora = true;
        arrivati = false;

        if (n_passeggeri <= MIN && coda_ouarzazate > 0) {
            attesa_andata.signal(); //risveglio i passeggeri se sono abbastanza
        }
        while (n_passeggeri <= MIN) {
            attesa_passeggeri_andata.wait();
        }
    }

    procedure entry taxi_arriva_a_Zagora() {
        arrivati = true;
        while(n_passeggeri != 0) {
            visita_zagora.signal(); //risveglio i passeggeri finche non sono tutti scesi dal taxi
        }
    }

    procedure entry passeggero_vuole_partire_da_Zagora() {
        coda_zagora++;
        while (!ritorno ) {
            attesa_ritorno.wait(); //aspetta che il taxi sia disponibile
        }
        coda_zagora--; //regolo le code una volta chiamati dal taxi
        persone_a_zagora--;
        n_passeggeri++;
        while (!arrivati) {
            ritorno_a_casa.wait();
        }
        n_passeggeri--;
    }

    procedure entry taxi_parte_per_Ouarzazate() {
        taxi_a_zagora = false;
        ritorno = true;
        n_passeggeri = 0;

        while (coda_zagora >= MIN || coda_zagora + n_passeggeri <= MAX ) {
            attesa_ritorno.signal(); //risveglio i passeggeri se sono abbastanza
        }
    }

    procedure entry taxi_arriva_a_Ouarzazate() {
        arrivati = true;
        taxi_a_zagora = false;
        while(n_passeggeri != 0) {
            ritorno_a_casa.signal(); //risveglio i passeggeri finche non sono tutti scesi dal taxi
        }
        if (coda_ouarzazate == 0 && persone_a_zagora >= MIN) {
            taxi_a_zagora = true;
            attesa_ritorno.signal(); //risveglio i passeggeri se sono abbastanza
        }
    }

}

///CREO ISTANZE///
turista.passeggero_vuole_partire_da_Ouarzazate();
<aspetta il suo turno e parte>
turista.passeggero_vuole_partire_da_Zagora();
<torna a casa>

tassista.taxi_parte_per_Zagora();
<viaggia>
tassista.taxi_arriva_a_Zagora();
<scarica i passeggeri>
tassista.taxi_parte_per_Ouarzazate();
<viaggia>
tassista.taxi_arriva_a_Ouarzazate();
<scarica i passeggeri>