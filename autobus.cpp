/*Un autobus da 10 posti porta gli studenti al campus. Gli studenti pagano un biglietto 
in base alla loro taglia (1 spazio = ragazze + ragazzi con XS, S, M; 2 spazi = XL, L; 
4 spazi = XXL). Il bus non parte con 
meno di 4 posti occupati. Gli studenti possono salire solo alla fermata della segreteria e a
quella dell'edicola alla fine. Durante una fermata l'autista risveglia tutti e aspetta che 
chi vuole scenda. Non si può salire alla fermata della segreteria*/

/// SEMANTICA MESA ///
#define MAX 10
#define MIN 4


type minibus = monitor{
    enum type taglia {XS, S, M, L, XL, XXL};
    enum type genere {MASCHIO, FEMMINA};
    enum type fermata {CASA, SEGRETERIA, EDICOLA};
    int coda_magri_casa, coda_grassi_casa, coda_obesi_casa;
    int coda_magri_uni, coda_grassi_uni, coda_obesi_uni; 
    int posti_occupati, persone_in_uscita;
    var condition fermata, partenza;
    var condition ingresso_magri_casa, ingresso_magri_uni;
    var condition ingresso_grassi_casa, ingresso_grassi_uni;
    var condition ingresso_obesi_casa, ingresso_obesi_uni;


    /// INIZIALIZZAZIONE ///
    posti_occupati = 0;
    persone_in_uscita = 0;
    coda_magri_casa = 0;
    coda_magri_uni = 0;
    coda_grassi_casa = 0;
    coda_grassi_uni = 0;
    coda_obesi_casa = 0;
    coda_obesi_uni = 0;


    /// METODI ///

    //voglio avere una funzione per calcolare gli spazi occupati da una persona
    int calcola_taglia(taglia t, genere g) {
        if (g == FEMMINA) {
            return 1; //le ragazze possono essere pure obese ma a noi piacciono così
        } else if (t == XS || t == S || t == M) {
            return 1;
        } else if (t == L || t == XL) {
            return 2;
        } else {
            return 4;
        }
    }

    procedure entry sali_sul_bus_casa(taglia t, genere g) {
        int spazi = calcola_taglia(t, g);
        while (posti_occupati + spazi > MAX) {
            switch (spazi) {
            case 1:
                coda_magri_casa++;
                ingresso_magri_casa.wait();
                posti_occupati += spazi;
                coda_magri_casa--;
                partenza.notify();
                break;
            case 2:
                coda_grassi_casa++;
                ingresso_grassi_casa.wait();
                posti_occupati += spazi;
                coda_grassi_casa--;
                partenza.notify();
                break;
            case 4:
                coda_obesi_casa++;
                ingresso_obesi_casa.wait();
                posti_occupati += spazi;
                coda_obesi_casa--;
                partenza.notify();
                break;
            }
        }
    }

    procedure entry sali_sul_bus_uni(taglia t, genere g) {
        int spazi = calcola_taglia(t, g);
        while (posti_occupati + spazi > MAX) {
            switch (spazi) {
            case 1:
                coda_magri_uni++;
                ingresso_magri_uni.wait();
                posti_occupati += spazi;
                coda_magri_uni--;
                partenza.notify();
                break;
            case 2:
                coda_grassi_uni++;
                ingresso_grassi_uni.wait();
                posti_occupati += spazi;
                coda_grassi_uni--;
                partenza.notify();
                break;
            case 4:
                coda_obesi_uni++;
                ingresso_obesi_uni.wait();
                posti_occupati += spazi;
                coda_obesi_uni--;
                partenza.notify();
                break;
            }
        }
    }

    procedure entry scendi_dal_bus(taglia t, genere g) {
        persone_in_uscita++;
        fermata.wait();
        persone_in_uscita--;
        posti_occupati -= calcola_taglia(t, g);
    }

    procedure entry fai_scendere() {
        for (int i = 0; i < persone_in_uscita; i++) {
            fermata.notify();
        }
        partenza.notify();
    }

    procedure entry fai_entrare(fermata f) {
        while (posti_occupati < MAX) {
            switch (f) {
            case CASA:
                if (coda_obesi_casa > 0 && posti_occupati + 4 < MAX) ingresso_obesi_casa.notify();
                if (coda_grassi_casa > 0 && posti_occupati + 2 < MAX) ingresso_grassi_casa.notify();
                if (coda_magri_casa > 0 && posti_occupati + 1 < MAX) ingresso_magri_casa.notify();
                break;
            case EDICOLA:
                if (coda_obesi_uni > 0 && posti_occupati + 4 < MAX) ingresso_obesi_uni.notify();
                if (coda_grassi_uni > 0 && posti_occupati + 2 < MAX) ingresso_grassi_uni.notify();
                if (coda_magri_uni > 0 && posti_occupati + 1 < MAX) ingresso_magri_uni.notify();
                break;
            case SEGRETERIA:
                partenza.notify(); //non fa nulla ma serve per svegliare il bus
                break;
            }
        }
    }

    procedure entry parti(fermata f) {
        switch (f) {
            case CASA:
                while (posti_occupati < MIN ||
                    (coda_magri_casa > 0 && posti_occupati + 1 < MAX) ||
                    (coda_grassi_casa > 0 && posti_occupati + 2 < MAX) ||
                    (coda_obesi_casa > 0 && posti_occupati + 4 < MAX) ||
                    persone_in_uscita > 0) {
                    partenza.wait();
                }
                break;
            case SEGRETERIA:
                while (persone_in_uscita > 0) {
                    partenza.wait();
                }
                break;
            case EDICOLA:
                while (posti_occupati < MIN ||
                    (coda_magri_uni > 0 && posti_occupati + 1 < MAX) ||
                    (coda_grassi_uni > 0 && posti_occupati + 2 < MAX) ||
                    (coda_obesi_uni > 0 && posti_occupati + 4 < MAX) ||
                    persone_in_uscita > 0) {
                    partenza.wait();
                }
                break;
        }
    }

}

/// CREAZIONE ISTANZA ///
passeggero.sali_sul_bus_casa(taglia, genere);
<cazzeggia sul bus come tutti gli studenti unipr>
passeggero.scendi_dal_bus(taglia, genere);
<va in uni a farsi dire dai professori che a modena sono più bravi>

minibus.fai_entrare(CASA);
<il minibus si riempie>
minibus.parti(CASA);
<il minibus parte>
minibus.fai_scendere();
<il minibus si svuota>
minibus.parti(SEGRETERIA);
<il minibus parte>
minibus.fai_scendere();
<il minibus si svuota>
minibus.parti(EDICOLA);