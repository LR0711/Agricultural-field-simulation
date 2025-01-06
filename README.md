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

Un tutorial sul funzionamento teorico del programma è qui disponibile. La descrizione delle funzionalità e del flusso del programma è contenuta nei file `.cpp` inclusi nel progetto.

## Costruzione del campo agricolo simulato

Il campo agricolo simulato è costruito utilizzando una matrice di dimensione `length * width`, dove ogni elemento rappresenta un punto del campo. Ogni punto è rappresentato da un oggetto di tipo `Soil`, definito da una classe che include le proprietà fisiche del terreno, come:

- **Tipo di terreno**: Può essere di quattro tipi: clay, sand, loam, silt. Il tipo di terreno influenza, attraverso due apposite funzioni, la temperatura del suolo a parità di condizioni atmosferiche e il risultato delle analisi compiute (terreni diversi hanno valori ottimali e/o accettabili diversi dei vari parametri).
- **Temperatura del suolo e dell'aria**: Questi parametri possono variare in base alla posizione nel campo (ad esempio, alcune zone potrebbero essere in ombra mentre altre al sole). Solo il secodono è impostabile dall'utente, mentre il primo è direttamente calcolato dal programma.
- **Umidità del suolo e dell'aria**: Parametri che variano in base alla zona del campo.
- **Presenza di colture**: Indica se in quella zona ci sono coltivazioni (variabile booleana `hasPlants`). Tale flag indica le celle di maggior interesse per l'analisi.

L'utente può configurare le proprietà fisiche del campo utilizzando apposite funzioni di set, sia inserendo un elemento soil in specifiche aree del campo, sia modificando solo proprietà specifiche. Ad esempio, è possibile modificare il tipo di suolo, l'umidità e la temperatura del suolo in aree specifiche del campo. La temperatura del suolo viene calcolata automaticamente in base al tipo di terreno.

Inoltre, l'utente può modificare la dimensione del campo e visualizzare i dati tramite funzioni di stampa e getter. Tuttavia, l'utente non può misurare autonomamente i parametri fisici come la temperatura e l'umidità senza l'uso di sensori, poiché sono considerati misurabili solo tramite appositi strumenti.

### Funzioni principali del campo agricolo simulato

- **setSoil**: Permette di impostare il tipo di suolo in un'area specifica della matrice, comprensivo di tutti i dati
- **modifySoilProperty**: Permette di modificare una sola delle proprietà fisiche del terreno, inclusi tipo di suolo e presenza di colture.
- **changeFieldname**: Cambia il nome del campo.
- **changeDimensions**: Cambia le dimensioni del campo.
- **getSoilTypes**: Restituisce il tipo di suolo in un'area specifica del campo.
- **getPlants**: Restituisce la presenza di piante in un'area specifica del campo.
- **printSoilTypes**: Stampa i tipi di suolo presenti nel campo.
- **printPlantPresence**: Stampa la presenza di piante nel campo.

### Limiti attuali

Attualmente, il campo agricolo è in condizioni statiche, e i parametri fisici sono hardcoded. Le condizioni del campo non cambiano dinamicamente. Ciò non rende per forza la simulazione meno realistica (può simulare bene lo stato del campo in un esatto istante di tempo) ma limita analisi dinamiche.

## Interazione ambiente agricolo utente

L'utente può interagire con l'ambiente agricolo costruito mediante unità autonome, rappresentati dalla classe `Vehicle`, che simula gli appositi veicoli di misurazione dei dati sul campo. Il veicolo viene definito dall'utente in base al tipo di veicolo (nella versione attuale non c'è differenza significativa tra field e aerial), alle sue coordinate sul campo ed ai suoi parametri fisici di velocità (in celle/sec) e di batteria. La batteria diminuisce ad ogni operazione di movimento e lettura con invio dati del veicolo e, se raggiunge un valore di soglia minima, "costringe" il veicolo a tornare alla base ed a doversi ricaricare prima di riprendere l'operazione in corso.

### Funzioni principali dei veicoli

- **setPosition**: Imposta la posizione del veicolo. Può essere usata anche per spostamenti istantanei al target.
- **moveToTarget**: Sposta il veicolo in una posizione target, con un movimento diagonale di durata dipendente dalla velocità del veicolo.
- **readDataFromCurrentCell**: Legge i dati dalla cella corrente, in base ai sensori a disposizione, e li stampa immediatamente a video (si usa più per questioni di testing).
- **readAndSendData**: Legge i dati sul campo e li invia al centro di controllo per ulteriori analisi.

Le funzioni di movimento e di spedizione dei dati includono al loro interno una logica rudimentale di scarica della batteria, che impone una quota (fissabile dall'utente ma costante per ogni operazione) di scarica della stessa, imponendo un ritorno alla base per una ricarica completa nel caso di raggiungimento di un valore di soglia minima. A tal fine, sono presenti anche due funzioni rudimentali di carica e scarica della batteria. Tali operazioni sono regolate nel funzionamento mediante specifici lock ed unlock delle operazioni al fine di prevenire deadlock che bloccavano il programma in una condizione perenne di attesa dati senza che il veicolo potesse più muoversi dopo la ricarica(emerse in fase di testing senza una apposita gestione del lock ed unlock del mutex).


## Centro di controllo

Il centro di controllo, rappresentato dalla classe `ControlCenter`, gestisce la movimentazione dei veicoli e raccoglie i dati dai sensori per fornire analisi sullo stato del campo.

### Funzioni principali del centro di controllo

- **sendMovementCommandToVehicle**: Invia un comando di movimento a un veicolo.
- **getVehiclePosition**: Ottiene la posizione di un veicolo.
- **commandDataRead**: Invia un comando di lettura dei dati a un veicolo.
- **appendData**: Aggiunge i dati al buffer del centro di controllo.
- **analyzeData**: Analizza i dati del buffer, restituendo all'utente eventuali parametri critici sul campo comprensivi di coordinate in cui si sono ottenuti.

Sono inoltre presenti varie funzioni di flag o di notifica sullo stato del buffer, introdotti in una seconda versione del progetto ed utili nell'ambito della programmazione concorrente.

## Sensori

I sensori, rappresentati dalla classe `Sensor`, sono utilizzati per raccogliere dati sul campo. Ogni sensore è di un tipo specifico e può leggere diversi parametri fisici del suolo.

### Funzioni principali dei sensori

- **readTemperature**: Legge la temperatura dal suolo.
- **readMoisture**: Legge l'umidità del suolo.
- **readHumidity**: Legge l'umidità dell'aria.

In una prima versione del progetto si è pensato ad una lettura perfetta dei dati della cella. In quest'ultima versione è stato aggiunto al valore esatto un valore randomico di rumore (a simulare la precisione del sensore), di modo da aumentare il realismo della simulazione.

## Programmazione concorrente

Il progetto utilizza la programmazione concorrente per gestire le operazioni simultanee dei veicoli e del centro di controllo. Questo è realizzato utilizzando thread e mutex per garantire la sincronizzazione e l'accesso sicuro alle risorse condivise.

### Concetti chiave

- **Thread**: Ogni veicolo e il centro di controllo operano su thread separati per eseguire le loro operazioni simultaneamente.
Ciò consente una migliore efficienza del programma.
- **Mutex**: Utilizzati per proteggere l'accesso alle risorse condivise, come la mappa delle posizioni dei veicoli e il buffer dei dati nel centro di controllo.
- **Condition Variables**: Utilizzate per sincronizzare le operazioni tra i thread. Prendendo come esempio il control center, permettono ad esempio ad esso la notifica di quando ci sono nuovi dati disponibili nel buffer.

### Esempi di utilizzo

- **Movimentazione dei veicoli**: Ogni veicolo utilizza un thread separato per muoversi nel campo. L'accesso alla mappa delle posizioni dei veicoli è protetto da un mutex per evitare condizioni di gara.
    ```cpp
    void ControlCenter::sendMovementCommandToVehicle(Vehicle& vehicle, int x, int y) {
    std::unique_lock<std::mutex> lock(vehiclepositionmutex_); // Lock per proteggere l'accesso alla mappa delle posizioni dei veicoli
    cellfreecv_.wait(lock, [this, x, y] { 
        for (const auto& pos : vehiclepositions_) {
            if (pos.second.first == x && pos.second.second == y) {
                return false;
            }
        }
        return true;
    }); 
    vehiclepositions_[vehicle.getId()] = {x, y}; 
    lock.unlock(); 
    ```

- **Lettura e analisi dei dati**: Il centro di controllo utilizza un thread per analizzare i dati raccolti dai veicoli. L'accesso al buffer dei dati è protetto da un mutex e sincronizzato con una condition variable.
    ```cpp
    void ControlCenter::analyzeData() {
    std::cout << "Debug: Buffer size before analysis: " << databuffer_.size() << std::endl;
    std::unique_lock<std::mutex> lock(bufferMutex_); // Protegge l'accesso al buffer su cui agisce sia il control center che i veicoli
    // Debug iniziale
    std::cout << "Debug (analyzeData): Entrando in wait, activevehicles_ = " 
              << activevehicles_ 
              << ", dataCollectionComplete_ = " << dataCollectionComplete_ 
              << ", databuffer_.empty() = " << databuffer_.empty() 
              << std::endl;
    // Finché ci sono dati nel buffer o la raccolta dati non è completata, il control center analizza i dati
    while (true) { 
        cvnotdata_.wait(lock, [this] {
        // Debug dentro la lambda
        std::cout << "Debug (wait lambda): activevehicles_ = " 
                  << activevehicles_
                  << ", dataCollectionComplete_ = " << dataCollectionComplete_
                  << ", databuffer_.empty() = " << databuffer_.empty() 
                  << std::endl;
             return !databuffer_.empty() || dataCollectionComplete_; }); // Attendi finché non ci sono dati nel buffer o la raccolta dati non è completata
        std::cout << "Debug (analyzeData): Uscito da wait, databuffer_.empty() = " 
              << databuffer_.empty() 
              << std::endl;
        if (databuffer_.empty() && dataCollectionComplete_) { 
            break; 
        }
        std::cout << "Debug: Analyzing data..." << std::endl;
        isanalyzing_ = true; // Tale booleano indica che il control center sta analizzando i dati

        auto dataBatch = databuffer_.front();
        databuffer_.pop();
        lock.unlock();

        // Simula il tempo di analisi
        std::this_thread::sleep_for(std::chrono::seconds(5));

        // Analisi del dato
        std::map<Sensor::SensorType, double> dataMap;
        for (const auto& data : dataBatch) {
            dataMap[data.type] += data.data;
        }

        int x{dataBatch[0].x};
        int y{dataBatch[0].y};
        std::cout << "Debug: Analyzing data for cell (" << x << ", " << y << ")" << std::endl;
        // Dalle coordinate del veicolo, si ottiene il tipo di suolo e si verifica la presenza di piante
        Soil AnalyzedSoil;
        field_.getSoil(x, y, AnalyzedSoil);
        bool hasPlants{AnalyzedSoil.getPlants()};
        std::string soilType = AnalyzedSoil.soilTypeToString(AnalyzedSoil.getSoilType());

        std::cout << "Debug: Soil has plants: " << (hasPlants ? "Yes" : "No") << std::endl;

        std::vector<std::string> analysisResults;
        // Se ci sono piante, si procede con l'analisi, altrimenti questa non è necessaria
        if (hasPlants) {
            std::cout << "Plants detected, proceeding with analysis." << std::endl;
            for (const auto& sensorData : dataMap) {
                std::string result = evaluateData(soilType, sensorData.first, sensorData.second, x, y);
                std::cout << "  " << Sensor::sensorTypeToString(sensorData.first) << ": " << sensorData.second << " (" << result << ")" << std::endl;
                analysisResults.push_back(result);
            }
        } else {
            std::cout << "No plants in this area. No need for analysis." << std::endl;
        }

        lock.lock(); // Riacquisisce il lock per scrivere
        analysisResults_.insert(analysisResults_.end(), analysisResults.begin(), analysisResults.end());
    }

    isanalyzing_ = false;
    cvnotdata_.notify_all(); // Notifica che l'analisi è completata e che il buffer è vuoto
    std::cout << "Debug: Analysis complete for current buffer." << std::endl;
}

    ```

### Vantaggi della programmazione concorrente

- **Efficienza**: Permette di eseguire operazioni simultanee, migliorando l'efficienza del sistema. Permette l'utilizzo di più veicoli sul campo per paralellizzare le operazioni di analisi. Permette in parallelo operazioni di scrittura sul buffer e di analisi dei dati precedentemente raccolti.
- **Reattività**: I veicoli possono muoversi e raccogliere dati mentre il centro di controllo analizza i dati precedentemente raccolti. 
- **Scalabilità**: Facilita l'aggiunta di nuovi veicoli e sensori senza compromettere le prestazioni del sistema.


## Conclusioni

Questo progetto rappresenta una simulazione di un sistema di monitoraggio agricolo, con l'obiettivo di fornire un ambiente di test per la raccolta e l'analisi dei dati agricoli. Il sistema utilizza concetti di programmazione concorrente per gestire le operazioni dei veicoli e del centro di controllo, offrendo un esempio pratico di come tali tecniche possano essere applicate in un contesto reale.
