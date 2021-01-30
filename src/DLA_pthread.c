#include "dla.h"
#include <pthread.h>
int X, M, P, NUM_THREAD; //dichiarazione valori dimensione matrice, mosse, particelle e numero thread
int semeX,semeY; //coordinate seme
struct Particella *parti; //dichiarazione struttura che contiente le informazioni delle particelle
pthread_mutex_t lock;  //lock 
int **cristallo;	//dichiarazione matrice 	
int cristalli=0; //contatore particelle trasformate ini cristallo
int **campo;     // DEBUGGING	
int zp; //variabile di appoggio per la suddivisione del carico tra thread

void stampa(){ //funzione stampa
	for(int y=0;y<X;y++){ //iterazioni su dim Matrice
		for(int x=0;x<X;x++) //iterazioni su dim Matrice
			if(x==semeX && y==semeY) printf("\x1b[31m" "██" "\x1b[0m"); //se seme: quadrato rosso
			else if(cristallo[y][x]>=20) printf("██");//se cristallo: quadrato bianco
			else if(cristallo[y][x]>0) printf("||");//se area: || (DEBUGGING)
			else if(campo[y][x]!=0) printf("\x1b[%dm" "* " "\x1b[0m",campo[y][x]+31);  //DEBUGGING      
			else printf("  "); 
		printf("|\n");
	}
}/*stampa*/
int min(int a, int b){ //funzione che controlla che le coordinate rientrino nel range della matrice
	return a<=b?a<0?0:a:b<0?0:b;
	//restituisce a se minore di b e maggiore di 0
	//restituisce b se minore di a e maggiore di 0
	//resistuisce 0 altrimenti 
}
void *popolaParticelle(void *rank){ //funzione che inizializza i parametri delle particelle
	long m_rank=(long)rank; //numero del thread
	int local_n; //dichiarazione del numero di particelle per ogni thread
	if (P%NUM_THREAD==0){ //se le particelle sono divisibili per i thread
		local_n = P/NUM_THREAD; //ogni thread avrà numero uguale di particelle
	}else{ //se non divisibile
		int ap = P%NUM_THREAD; //ap contiene il resto delle divisione
		local_n = P/NUM_THREAD; //assegno a local_n con stesso numero di particelle per thread
		for(int i=P-(P-ap);i!=0;i--){ //per le restanti particelle 
			if (m_rank==i){ //se il rank è uguale all'indice
				local_n++; //incremento il suo local_n
			}
		} 
	}

	int p; //indice del for
	int local_start = P-zp; //indice delle particella iniziale del thread
	int local_finsih = local_start+(local_n-1); //indice della particella finale del thread
	pthread_mutex_lock(&lock);
	zp=zp-local_n; //aggiorno la variabile di appoggio
	pthread_mutex_unlock(&lock);
	for(p=local_start;p<=local_finsih;p++){ //per ogni particella
		int randx = rand()%X;  //prendo coordinata X casuale
		int randy = rand()%X;  //prendo coordinata Y casuale
		parti[p].x = randx;    //assegno coordinata
		parti[p].y = randy;    //assegno coordinata
		parti[p].cristallo = false;  //assegno il valore di NON cristallo
		campo[randy][randx]=1; //DEBUGGING
	}	
	return NULL;
}/*popolaParticelle*/

void creaArea(int y,int x){  //funzione che crea l'area del cristallo
	if(y>0 && x>0)cristallo[(y-1)][(x-1)]+=1; //alto sinistra
   	if(y>0)cristallo[(y-1)][x]+=1;  //alto
   	if(y>0 && x<X-1)cristallo[(y-1)][(x+1)]+=1; //alto destra
	if(x>0)cristallo[y][(x-1)]+=1; //sinistra
	if(x<X-1)cristallo[y][(x+1)]+=1; //destra
	if(y<X-1 && x>0)cristallo[(y+1)][(x-1)]+=1; //basso sinistra
	if(y<X-1)cristallo[(y+1)][x]+=1; //giu
	if(y<X-1 && x<X-1)cristallo[(y+1)][(x+1)]+=1; //basso destra
}/*creaArea*/
void *movimento(void *rank){  //funzione che muove le particelle
	long m_rank=(long)rank; //numero del thread
	int local_n; //dichiarazione del numero di particelle per ogni thread
	if (P%NUM_THREAD==0){ //se le particelle sono divisibili per i thread
		local_n = P/NUM_THREAD; //ogni thread avrà numero uguale di particelle
	}else{ //se non divisibile
		int ap = P%NUM_THREAD; //ap contiene il resto delle divisione
		local_n = P/NUM_THREAD; //assegno a local_n con stesso numero di particelle per thread
		for(int i=P-(P-ap);i!=0;i--){ //per le restanti particelle 
			if (m_rank==i){ //se il rank è uguale all'indice
				local_n++; //incremento il suo local_n
			}
		} 
	}
	int p; //indice del for
	int local_start = P-zp; //indice delle particella iniziale del thread
	int local_finsih = local_start+(local_n-1); //indice della particella finale del thread
	pthread_mutex_lock(&lock);
	zp=zp-local_n; //aggiorno la variabile di appoggio
	pthread_mutex_unlock(&lock);
	for(int mosse=0;mosse<M;mosse++){ //per ogni mossa
		//printf("\n\n\n"); //DEBUGGING
		//stampa(); //DEBUGGING
		//usleep(100000);  //DEBUGGING
		for(p=local_start;p<=local_finsih;p++){ //per ogni particella
			if(parti[p].cristallo==false){ ///se non sono un cristallo
				int x = parti[p].x; //prendo coordinata X della particella
				int y = parti[p].y; //prendo coordinata Y della particella
				if(cristallo[y][x]>0){ //se nella matrice alla pos Y,X c'è o area o cristallo	
					parti[p].cristallo=true; //trasformo particella in cristallo
					pthread_mutex_lock(&lock); //lock della sezione critica
					cristallo[y][x]=20;	//assegno alla matrice il valore di cristallo
					cristalli++;
					campo[y][x]=0; //DEBUGGING
					creaArea(y,x); //crea area
					pthread_mutex_unlock(&lock); //unlock della sezione critica
				}else{ //non mi trasformo quindi mi muovo
					int rx = ((p + x + mosse)%3)-1; //generazione numero pseudo casuale
					int ry = ((p + y + mosse)%3)-1; //generazione numero pseudo casuale
					parti[p].x=min(x+rx,X-1);	//validazione coordinate 
					parti[p].y=min(y+ry,X-1);	//validazione coordinate
					pthread_mutex_lock(&lock); //DEBUGGING
					campo[y][x]=0; //DEBUGGING
					campo[parti[p].y][parti[p].x]=m_rank+1;  //DEBUGGING
					pthread_mutex_unlock(&lock); //DEBUGGING

				}
			}
		}	
	}
}/*creaArea*/
int main(int argc, char *argv[]){
	X=strtol(argv[1],NULL,10); //prendo in input dimensione della matrice
	P=strtol(argv[2],NULL,10); //prendo in input il numero di particelle
	M=strtol(argv[3],NULL,10); //prendo in input il numero di mosse
	NUM_THREAD=strtol(argv[4],NULL,10); //prendo il numero di thread
	parti = malloc(sizeof(struct Particella)*P); //dichiaro dimensione array particelle
	cristallo =(int **)malloc(sizeof(int*)*X); //dichiaro dimensione matrice 
	campo =(int **)malloc(sizeof(int*)*X); //DEBUGGING

	srand(time(NULL));       //inizializzazione rand generator
	semeX = rand()%X; //coordinata X seme iniziale 
    semeY = rand()%X; //coordianta Y seme iniziale
    for(int i=0;i<X;i++){ 
    	cristallo[i]=(int*)malloc(sizeof(int)*X); //dichiaro dimensione riga della matrice
    	campo[i]=(int*)malloc(sizeof(int)*X); //DEBUGGING
		for(int j=0;j<X;j++){
			cristallo[i][j]=0; //inizializzo a zero
			campo[i][j]=0; //DEBUGGING
		}
	} 
	cristallo[semeY][semeX]=20;	 //posizionamento seme nelle matrice
	creaArea(semeY,semeX); //creo area del seme

	pthread_t* t_h; 
	t_h= malloc(NUM_THREAD*sizeof(pthread_t));
	zp=P;
	for(long thread=0; thread<NUM_THREAD;thread++){//funzione popolaParticelle
		pthread_create(&t_h[thread],NULL, popolaParticelle,(void*)thread);
	}
	for(long t=0;t<NUM_THREAD;t++){
		pthread_join(t_h[t],NULL);
	}

	zp=P;
	for(long thread=0; thread<NUM_THREAD;thread++){//funzione movimento
		pthread_create(&t_h[thread],NULL, movimento, (void*)thread);
	}	
	for(long t=0;t<NUM_THREAD;t++){
		pthread_join(t_h[t],NULL);
	}
	
	free(t_h);
	//stampa();
	printf("Matrice %dx%d	%d/%d	mosse:%d \n",X,X,cristalli,P,M); //stampa informazioni
	free(campo);
	free(cristallo);
	free(parti);	
	return 0;
}/*main*/
