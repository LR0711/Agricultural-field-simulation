# Homework Invernale 2024-2025

## Descrizione del progetto

Il progetto presentato è una realizzazione personale dell'homework invernale del corso di "Progettazione di Sistemi Embedded" del corso di Laurea Magistrale in Ingegneria Meccatronica dell'Università di Padova. Lo scopo del progetto è la progettazione e lo sviluppo di un sistema di monitoraggio agricolo simulato in C++. In particolare, il progetto si concentra sulla simulazione di un campo agricolo con diverse condizioni fisiche, veicoli di analisi, sensori e un centro di controllo per la gestione e l'analisi dei dati.

Il sistema simula un campo agricolo con vari parametri fisici (temperatura del suolo, umidità dell'aria e del suolo, tipo di terreno) e consente di monitorare le colture. I veicoli sono utilizzati per raccogliere i dati tramite sensori, mentre il centro di controllo coordina le operazioni e fornisce analisi sui dati raccolti.

Il progetto implementa anche concetti di programmazione concorrente, simulando la gestione di più thread per il coordinamento delle attività dei veicoli e del centro di controllo.

### Funzionalità principali

- **Simulazione del campo agricolo**: Creazione di un campo agricolo con dimensioni personalizzabili e diverse condizioni fisiche (temperatura, umidità, tipo di suolo).
- **Gestione delle colture**: Possibilità di aggiungere o modificare colture in specifiche aree del campo.
- **Sensori e veicoli**: Utilizzo di sensori per raccogliere dati sul campo e veicoli che eseguono letture dei parametri fisici.
- **Centro di controllo**: Un sistema che gestisce la movimentazione dei veicoli e raccoglie i dati dai sensori per fornire analisi sullo stato del campo.
- **Programmazione concorrente**: Gestione dei thread per il coordinamento delle attività di lettura/scrittura tra veicoli e centro di controllo.

### Istruzioni per l'uso

Un tutorial dettagliato sul funzionamento del programma è disponibile. La descrizione delle funzionalità e del flusso del programma è contenuta nei file `.cpp` inclusi nel progetto.

## Costruzione del campo agricolo simulato

Il campo agricolo simulato è costruito utilizzando una matrice di dimensione `length * width`, dove ogni elemento rappresenta un punto del campo. Ogni punto è rappresentato da un oggetto di tipo `Soil`, definito da una classe che include le proprietà fisiche del terreno, come:

- **Tipo di terreno**: Può essere di quattro tipi: clay, sand, loam, silt.
- **Temperatura del suolo e dell'aria**: Questi parametri possono variare in base alla posizione nel campo (ad esempio, alcune zone potrebbero essere in ombra mentre altre al sole).
- **Umidità del suolo e dell'aria**: Parametri che variano in base alla zona del campo.
- **Presenza di colture**: Indica se in quella zona ci sono coltivazioni (variabile booleana `hasPlants`).

L'utente può configurare le proprietà fisiche del campo utilizzando apposite funzioni di set, sia inserendo un elemento soil in specifiche aree del campo, sia modificando solo proprietà specifiche. Ad esempio, è possibile modificare il tipo di suolo, l'umidità e la temperatura del suolo in aree specifiche del campo. La temperatura del suolo viene calcolata automaticamente in base al tipo di terreno.

Inoltre, l'utente può modificare la dimensione del campo e visualizzare i dati tramite funzioni di stampa e getter. Tuttavia, l'utente non può misurare autonomamente i parametri fisici come la temperatura e l'umidità senza l'uso di sensori, poiché sono considerati misurabili solo tramite appositi strumenti.

### Funzioni principali del campo agricolo simulato

- **setSoil**: Permette di impostare il tipo di suolo in un'area specifica della matrice.
- **modifySoilProperty**: Permette di modificare le proprietà fisiche del terreno, inclusi tipo di suolo e presenza di colture.
- **changeFieldname**: Cambia il nome del campo.
- **changeDimensions**: Cambia le dimensioni del campo.
- **getSoilTypes**: Restituisce il tipo di suolo in un'area specifica del campo.
- **getPlants**: Restituisce la presenza di piante in un'area specifica del campo.
- **printSoilTypes**: Stampa i tipi di suolo presenti nel campo.
- **printPlantPresence**: Stampa la presenza di piante nel campo.

### Limiti attuali

Attualmente, il campo agricolo è in condizioni statiche, e i parametri fisici sono hardcoded. Le condizioni del campo non cambiano dinamicamente.

## Interazione ambiente agricolo utente

L'utente può interagire con l'ambiente agricolo costruito mediante unità autonome, rappresentati dalla classe `Vehicle`, che simula gli appositi veicoli di misurazione dei dati sul campo. Il veicolo viene definito dall'utente in base al tipo di veicolo (nella versione attuale non c'è differenza significativa tra field e aerial), alle sue coordinate sul campo ed ai suoi parametri fisici di velocità (in celle/sec) e di batteria. La batteria diminuisce ad ogni operazione di movimento e lettura con invio dati del veicolo e, se raggiunge un valore di soglia minima, "costringe" il veicolo a tornare alla base ed a doversi ricaricare prima di riprendere l'operazione in corso.

### Funzioni principali dei veicoli

- **setPosition**: Imposta la posizione del veicolo. Può essere usata anche per spostamenti istantanei al target.
- **moveToTarget**: Sposta il veicolo in una posizione target, con un movimento diagonale di durata dipendente dalla velocità del veicolo.
- **readDataFromCurrentCell**: Legge i dati dalla cella corrente, in base ai sensori a disposizione, e li stampa immediatamente a video (si usa più per questioni di testing).
- **readAndSendData**: Legge i dati sul campo e li invia al centro di controllo per ulteriori analisi.

## Centro di controllo

Il centro di controllo, rappresentato dalla classe `ControlCenter`, gestisce la movimentazione dei veicoli e raccoglie i dati dai sensori per fornire analisi sullo stato del campo.

### Funzioni principali del centro di controllo

- **sendMovementCommandToVehicle**: Invia un comando di movimento a un veicolo.
- **getVehiclePosition**: Ottiene la posizione di un veicolo.
- **commandDataRead**: Invia un comando di lettura dei dati a un veicolo.
- **appendData**: Aggiunge i dati al buffer del centro di controllo.
- **analyzeData**: Analizza i dati del buffer.

## Sensori

I sensori, rappresentati dalla classe `Sensor`, sono utilizzati per raccogliere dati sul campo. Ogni sensore è di un tipo specifico e può leggere diversi parametri fisici del suolo.

### Funzioni principali dei sensori

- **readTemperature**: Legge la temperatura dal suolo.
- **readMoisture**: Legge l'umidità del suolo.
- **readHumidity**: Legge l'umidità dell'aria.

Allo stato attuale del progetto i sensori leggono il valore esatto delle celle nel campo. Un'idea per rendere il loro funzionamento più verosimile è quello di aggiungere un rumore alle misure dei sensori stessi.

## Programmazione concorrente

Il progetto utilizza la programmazione concorrente per gestire le operazioni simultanee dei veicoli e del centro di controllo. Questo è realizzato utilizzando thread e mutex per garantire la sincronizzazione e l'accesso sicuro alle risorse condivise.

### Concetti chiave

- **Thread**: Ogni veicolo e il centro di controllo operano su thread separati per eseguire le loro operazioni simultaneamente.
- **Mutex**: Utilizzati per proteggere l'accesso alle risorse condivise, come la mappa delle posizioni dei veicoli e il buffer dei dati nel centro di controllo.
- **Condition Variables**: Utilizzate per sincronizzare le operazioni tra i thread, ad esempio, per notificare al centro di controllo quando ci sono nuovi dati disponibili nel buffer.

### Esempi di utilizzo

- **Movimentazione dei veicoli**: Ogni veicolo utilizza un thread separato per muoversi nel campo. L'accesso alla mappa delle posizioni dei veicoli è protetto da un mutex per evitare condizioni di gara.
    ```cpp
    void ControlCenter::sendMovementCommandToVehicle(Vehicle& vehicle, int x, int y) {
        vehicle.moveToTarget(x, y);
        std::unique_lock<std::mutex> lock(mtx_); // Protegge l'aggiornamento della mappa
        vehiclepositions_[vehicle.getId()] = {vehicle.getX(), vehicle.getY()};
    }
    ```

- **Lettura e analisi dei dati**: Il centro di controllo utilizza un thread per analizzare i dati raccolti dai veicoli. L'accesso al buffer dei dati è protetto da un mutex e sincronizzato con una condition variable.
    ```cpp
    void ControlCenter::analyzeData() {
        std::unique_lock<std::mutex> lock(mtx_);
        while (databuffer_.empty() || isanalyzing_) {
            cvnotdata_.wait(lock);
        }
        isanalyzing_ = true;

        auto dataBatch = databuffer_.front();
        databuffer_.pop();
        lock.unlock();

        // Simula il tempo di analisi
        std::this_thread::sleep_for(std::chrono::seconds(10));

        // Analisi del dato
        std::map<Sensor::SensorType, double> dataMap;
        for (const auto& data : dataBatch) {
            dataMap[data.type] = data.data;
        }

        int x {dataBatch[0].x};
        int y {dataBatch[0].y};
        Soil AnalyzedSoil;
        field_.getSoil(x, y, AnalyzedSoil);

        bool hasPlants = AnalyzedSoil.getPlants();
        std::string soilType = AnalyzedSoil.soilTypeToString(AnalyzedSoil.getSoilType());
        std::cout << "Analyzing data for cell (" << x << ", " << y << "):" << std::endl;
        for (const auto& entry : dataMap) {
            std::cout << "  " << Sensor::sensorTypeToString(entry.first) << ": " << entry.second << std::endl;
        }
    }
    ```

### Vantaggi della programmazione concorrente

- **Efficienza**: Permette di eseguire operazioni simultanee, migliorando l'efficienza del sistema.
- **Reattività**: I veicoli possono muoversi e raccogliere dati mentre il centro di controllo analizza i dati precedentemente raccolti.
- **Scalabilità**: Facilita l'aggiunta di nuovi veicoli e sensori senza compromettere le prestazioni del sistema.


## Conclusioni

Questo progetto rappresenta una simulazione di un sistema di monitoraggio agricolo, con l'obiettivo di fornire un ambiente di test per la raccolta e l'analisi dei dati agricoli. Il sistema utilizza concetti di programmazione concorrente per gestire le operazioni dei veicoli e del centro di controllo, offrendo un esempio pratico di come tali tecniche possano essere applicate in un contesto reale.
