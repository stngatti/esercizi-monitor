/* In un negozio lavorano C commessi e un supervisore. Il negozio è frequentato da clienti 
normali (che comprano e pagano) e clienti speciali (devono cambiare la merce). Il negozio ha
una capacità massima di CAP clienti. Per servire i clienti normali serve un commesso solo, 
per i clienti speciali serve un commesso e un supervisore. */


/// SEMANTICA MESA ///
#define NORMALE
#define SPECIALE
#define C 5
#define CAP 1000

type cliente = monitor{

    int n_commessi, clienti_in_attesa, cambi_in_attesa, clienti_in_cassa;
    variable condition speciale_servito, normale_servito, pagamento, ingresso;
    bool supervisor_busy;


    /// INIZIALIZZAZIONE ///
    supervisor_busy = false;
    n_commessi = C;

    /// METODI ///
    procedure entry enter_shop(int tipo) {
        switch(tipo) {
            case NORMALE:
                clienti_in_attesa++;
                while(n_commessi == 0 || (clienti_in_attesa + clienti_in_cassa + cambi_in_attesa > CAP)){
                    normale_servito.wait();
                }
                clienti_in_attesa--;
                n_commessi--;
            break;
            case SPECIALE:
                cambi_in_attesa++;
                while(! (n_commessi > 0 && !supervisor_busy) || (clienti_in_attesa + clienti_in_cassa + cambi_in_attesa > CAP)){
                    speciale_servito.wait();
                }
                cambi_in_attesa--;
                n_commessi--;
                supervisor_busy = true;
            break;
        }
    }

    procedure entry exit_shop(int tipo) {
        switch(tipo) {
            case NORMALE:
                clienti_in_cassa++;
                if (cambi_in_attesa > 0 && n_commessi > 0 && !supervisor_busy){
                    speciale_servito.notify();
                }
                else if (clienti_in_attesa > 0 && n_commessi > 0) {
                    normale_servito.notify();
                }
                pagamento.notify();
            break;
            case SPECIALE:
                n_commessi++;
                supervisor_busy = false;
                if (cambi_in_attesa > 0){
                    speciale_servito.notify();
                }
                else if (clienti_in_attesa > 0){
                    normale_servito.notify();
                }
            break;
        }
    }

    procedure entry pay() {
        n_commessi++;
        clienti_in_cassa++;
        while (clienti_in_cassa > 0) {
            pagamento.wait();
        }
    }
}  

/// CREAZIONE ISTANZA ///
cliente.enter_shop(NORMALE);
<cliente compra>
cliente.pay();
<pagah>
cliente.exit_shop(NORMALE);

cliente.enter_shop(SPECIALE);
<cliente compra>
cliente.exit_shop(SPECIALE);