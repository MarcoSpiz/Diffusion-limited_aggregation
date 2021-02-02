#include "dla.h"
#include <omp.h>

int min(int a, int b){ //funzione che controlla che le coordinate rientrino nel range della matrice
	return a<=b?a<0?0:a:b<0?0:b;
	//restituisce a se minore di b e maggiore di 0
	//restituisce b se minore di a e maggiore di 0
	//resistuisce 0 altrimenti 
}/*min*/

void stampa(int *cristallo, int *campo, int semeX, int semeY, int X){ //funzione stampa
	for(int y=0;y<X;y++){ //iterazioni su dim Matrice
		for(int x=0;x<X;x++) //iterazioni su dim Matrice
			if(x==semeX && y==semeY) printf("\x1b[31m" "██" "\x1b[0m"); //se seme: quadrato rosso
			else if(cristallo[X*y+x]>=20) printf("██");//se cristallo: quadrato bianco
			else if(cristallo[X*y+x]>0) printf("||");//se area: || (DEBUGGING)
		//	else if(campo[X*y+x]!=0) printf("\x1b[%dm" "* " "\x1b[0m",campo[X*y+x]+31);  //DEBUGGING      
			else printf("  ");
		printf("|\n");
	}
}/*stampa*/

void creaArea(int y, int x, int *cristallo,int X){  //funzione che crea l'area del cristallo
	if(y>0 && x>0)cristallo[X*(y-1)+(x-1)]+=1; //alto sinistra
   	if(y>0)cristallo[X*(y-1)+x]+=1;  //alto
   	if(y>0 && x<X-1)cristallo[X*(y-1)+(x+1)]+=1; //alto destra
	if(x>0)cristallo[X*y+(x-1)]+=1; //sinistra
	if(x<X-1)cristallo[X*y+(x+1)]+=1; //destra
	if(y<X-1 && x>0)cristallo[X*(y+1)+(x-1)]+=1; //basso sinistra
	if(y<X-1)cristallo[X*(y+1)+x]+=1; //giu
	if(y<X-1 && x<X-1)cristallo[X*(y+1)+(x+1)]+=1; //basso destra
}/*creaArea*/

void main(int argc, char *argv[])
{
	srand(time(NULL));        //inizializzazione rand generator 
    	
    int X=strtol(argv[1],NULL,10); //input dimensione matrice
	int P=strtol(argv[2],NULL,10); //input numero particelle
	int M=strtol(argv[3],NULL,10); //input numero mosse
	int NUM_THREAD=strtol(argv[4],NULL,10); //numero thread preso a tempo di esecuzione
	int cristallo[X][X]; //dichiarazione matrice
	int campo[X][X]; //DEBUGGING
	omp_set_num_threads(NUM_THREAD); //set numero thread
	#pragma omp parallel for collapse(2) 
	for(int i=0;i<X;i++){ //inzializzazione a 0 della matrice
		for(int j=0;j<X;j++){
			cristallo[i][j]=0;
			//campo[i][j]=0;//DEBUGGING
		}
	}

    	
    int semeX = rand()%X; //coordinata X seme iniziale 
    int semeY = rand()%X; //coordianta Y seme iniziale
    cristallo[semeY][semeX]=20;	 //posizionamento seme nelle matrice
    //generazione area circostante al seme
	creaArea(semeY,semeX,*cristallo,X);
	int cristalli=0; //contatore particelle trasformate in cristallo
	
	struct Particella parti[P]; //struttura particelle
	
	
	#pragma omp parallel
	{
	#pragma omp for
	for(int p=0;p<P;p++){ //generazione delle particelle
		int randx = rand()%X;  //coordinata X
		int randy = rand()%X;  //coordinata Y
		parti[p].x = randx;    //assegnazione coordinata
		parti[p].y = randy;    //assegnazione coordinata
		parti[p].cristallo = false; //assegnazione a NON cristallo
		//campo[randy][randx]=1; //DEBUGGING
	}	
	
	for(int mosse=0;mosse<M;mosse++){ //iterazione delle mosse
		//#pragma omp master
		//{
		//	printf("\n\n\n");
		//	stampa(*cristallo,*campo, semeX, semeY,X); 
		//	usleep(100000);  //DEBUGGING
		//}
		#pragma omp for reduction(+:cristalli)
		for(int p=0;p<P;p++){         //iterazione delle particelle
			if(parti[p].cristallo==false){ ///se non sono un cristallo
				int x = parti[p].x; //prendo coordinata X della particella
				int y = parti[p].y; //prendo coordinata Y della particella
				if(cristallo[y][x]>0){ //se nella matrice alla pos Y,X c'è o area o cristallo	
					parti[p].cristallo=true; //trasformo particella in cristallo
					cristalli++; //aumento contatore cristalli
					#pragma omp critical
					{
					cristallo[y][x]=20;	//assegno alla matrice il valore di cristallo
					//genero area
					//campo[y][x]=0;  //DEBUGGING
					creaArea(y,x,*cristallo,X);
					}					    
				}else{ //non mi trasformo quindi mi muovo
					int rx = ((p + mosse + x )%3)-1; //generazione numero pseudo casuale
					int ry = ((p + mosse + y )%3)-1; //generazione numero pseudo casuale
					parti[p].x=min(x+rx,X-1);	//validazione coordinate
					parti[p].y=min(y+ry,X-1);	//validazione coordinate
					int nt = omp_get_thread_num(); //DEBUGGING
					//#pragma omp critical //DEBUGGING
					//{ //DEBUGGING
					//campo[y][x]=0; //DEBUGGING
					//campo[parti[p].y][parti[p].x]=nt+1; //DEBUGGING
					//} //DEBUGGING
				}
			}
		}		
	}
	}//pragma

	//stampa(*cristallo,*campo, semeX, semeY,X); //stampa cristallo finale
	printf("Matrice %dx%d    %d/%d    mosse:%d \n",X,X,cristalli,P,M); //stampa informazioni
}/*main*/