# Diffusion-limited aggregation

Nella cartella "src" sono presenti i tre codici sorgenti nominati DLA_ (seriale, openmp e pthread) rispettivamente per la versione single-thread, la versione implementata con openMP e la versione implementata con PThread.

Inoltre nella cartella "src" è presente un file nominato "dla.h" che contiente tutti i file header per il funzionamento del codice e una struttura in comune per tutte le implementazioni.

Il file nominato "test.sh" contiene un semplice script bash che, preso in input la dimensione della matrice, compila i codici sorgente di ogni implementazione e li esegue con diverso numero di particelle e mosse per ogni versione.

# Esecuzione del codice

Ogni programma può essere singolarmente eseguito passando in input la dimensione della matrice, il numero di particelle, il numero di mosse e il numero di thread per le implementazione parallele.

Esempio:
  ./dla_pthread *X* *Y* *Z* *W* 
Il valore *X* è la dimensione del lato della matrice, il valore *Y* il numero delle particelle, il valore *Z* è il numero di mossse e *W* il numero di thread.
 
 Si possono eseguire contemporanemente usando lo script test.sh passando in input la dimensione del lato della matrice
 
 Esempio:
  ./test.sh *val*
  
 Lo script testerà i casi con lato matrice *val*, con particelle uguale a *val*x10, *val*x20, val*x30, val*x40, val*x50 e mosse uguale a *val*, val*x5 e val*x10
 
 # Utilizzo delle funzioni stampa
 
In tutti sorgenti sono presenti delle linee di codice commentate riconoscibili con il commento "//DEBUGGING", esse se oppurtunamente decomentate forniscono una stampa progressiva della formazione del cristallo oppure semplicemente del risultato finale, nella stampa progressiva ogni particella ancora non trasformata è rappresentata da una stellina colorta (un colore diverso a seconda del thread, per la versione single-thread avranno tutte colore uguale)
