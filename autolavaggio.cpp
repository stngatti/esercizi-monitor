/*In un autolavaggio vengono lavati auto e camper. L'autolavaggio può lavare più auto contemporaneamente. 
I camper possono essere lavati solo se non ci sono auto o camper ad essere lavati. Le auto danno 
la precedenza ad eventuali camper in attesa*/

/// SEMANTICA MESA ///

type autolavaggio = monitor {
    enum type veicolo {AUTO, CAMPER};
    int coda_camper, coda_auto, auto_in_lavaggio;
    bool camper_in_lavaggio;
    var condition lava_auto, lava_camper;

    /// INIZIALIZZAZIONE ///
    camper_in_lavaggio = false;
    auto_in_lavaggio = 0;

    /// METODI ///
    procedure entry lava_auto() {
        while (camper_in_lavaggio ) {
            coda_auto++;
            lava_auto.wait();
            coda_auto--;
        }
        auto_in_lavaggio++;
    }

    procedure entry lava_camper() {
        while (camper_in_lavaggio || auto_in_lavaggio > 0) {
            coda_camper++;
            lava_camper.wait();
            coda_camper--;
        }
        camper_in_lavaggio = true;
    }

    procedure entry fine_lavaggio(veicolo v) {
        if (v == AUTO) auto_in_lavaggio--;
        else camper_in_lavaggio = false;

        while (auto_in_lavaggio == 0) {
            if (coda_camper > 0) lava_camper.notify();
            else lava_auto.notify();
        }
    }

    procedure entry fine_lavaggio_camper() {
        camper_in_lavaggio = false;
        if (coda_camper > 0) lava_camper.notify();
        else lava_camper.notify();
    }
}

/// CREAZIONE ISTANZA ///
autolavaggio.lava_auto();
<usa lo spazzolone>
autolavaggio.fine_lavaggio(AUTO);

autolavaggio.lava_camper();
<usa lo spazzolone ma più grande>
autolavaggio.fine_lavaggio_camper();