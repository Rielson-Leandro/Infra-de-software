#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#define P_THREADS 2	//Quantidade de threads produtoras
#define C_THREADS 3	//Quantidade de threads consumidoras
#define TAM_BUFFER 10	//Tamanho do Buffer
#define TAM 100	//Quantidade de operações que cada thread deve fazer

typedef struct elem{
	int value;
	struct elem *prox;
}Elem; 

typedef struct blockingQueue{
	unsigned sizeBuffer, statusBuffer;
	Elem *head,*last;
}BlockingQueue;

typedef struct argm{		//Estrutura necessária para passagem de argumentos na função pthread_create
	BlockingQueue* queue;
	int id;
}Argm;

pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;		//Inicialização do mutex e variáveis de condição
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;



BlockingQueue* newBlockingQueue(unsigned inSizeBuffer){		//Criando uma fila vazia
	BlockingQueue* novo =(BlockingQueue*)malloc(sizeof(BlockingQueue));

	novo->sizeBuffer = inSizeBuffer;
	novo->statusBuffer = 0;
	novo->head = NULL;
	novo->last = NULL;
	
	return novo;
}

void *producer();

void *consumer();

int main(){
	pthread_t produtor[P_THREADS];		//Declarando as Threads produtoras e consumidoras
	pthread_t consumidor[C_THREADS];
	int i,j;
	Argm *p[P_THREADS], *c[C_THREADS];	//Declarando os parametros para a pthread_create
	BlockingQueue* fila;

	fila = newBlockingQueue(TAM_BUFFER);	//Criando uma fila

	for(i=0; i < P_THREADS; i++){		//Criando as threads produtoras
		p[i] =(Argm*)malloc(sizeof(Argm));
		p[i]->queue = fila;
		p[i]->id = i+1;
		pthread_create(&produtor[i],NULL,producer,(void*)p[i]);
			
	}
	for(j=0; j < C_THREADS; j++){		//Criando as threads consumidoras
		c[j] =(Argm*)malloc(sizeof(Argm));
		c[j]->queue = fila;
		c[j]->id = j+1;
		pthread_create(&consumidor[j],NULL,consumer,(void*)c[j]);
	}

	pthread_exit(NULL);
}

void putBlockingQueue(BlockingQueue* Q, int newValue){	//Função para inserir na fila
	Elem* aux;
	
	pthread_mutex_lock(&mymutex);				//Travando o mutex
  	while(Q->statusBuffer == Q->sizeBuffer) {		//Caso o limite máximo seja alcançado
		printf("Fila Cheia. Produtor dormiu.\n");
    		pthread_cond_wait(&empty, &mymutex);		//A thread para de produzir e aguarda liberar espaço
	}
	aux = Q->last;						//Inserindo na fila
	Elem* novo = (Elem*)malloc(sizeof(Elem));
	novo->value = newValue;
	novo->prox = NULL;
	if(Q->head == NULL)
		Q->head = novo;
	else
		aux->prox = novo;
	Q->last = novo;
	Q->statusBuffer++;					//Incrementando contador do buffer
	if(Q->statusBuffer == 1){	
		pthread_cond_broadcast(&fill);			//Aviso para as threads consumidoras acordarem
	}
	pthread_mutex_unlock(&mymutex); 			//Destrava mutex
}

void *producer(void *parametros){
	int i,total;
	Argm* aux = (Argm*)parametros;
	total = TAM*C_THREADS;				//Tamanho total de operações = Tamanho máx * quantidade de threads consumidoras
	printf("Produtor %d iniciou\n",aux->id);
	for(i=0; i < total; i++){
		putBlockingQueue(aux->queue,i);		//Inserindo valor na fila
		printf("Produtor %d produziu %d\n",aux->id,i);
	}
	printf("Produtor %d encerrou\n",aux->id);
	pthread_exit(NULL);
}

int takeBlockingQueue(BlockingQueue* Q){
	Elem* aux;
	int retorno;

	pthread_mutex_lock(&mymutex);				//Travando o mutex
	while(Q->statusBuffer == 0){				//Caso a fila esteja vazia
		printf("Fila Vazia. Consumidor dormiu.\n");
		pthread_cond_wait(&fill, &mymutex);		//A thread para de consumir e aguarda a inserção de elemento
	}
	aux = Q->head;				//Removendo item da fila
	if(Q->head == Q->last)
		Q->last = aux->prox;
	Q->head = aux->prox;
	retorno = aux->value;
	free(aux);
	Q->statusBuffer--;					//Decrementando o contador do buffer
	if(Q->statusBuffer == Q->sizeBuffer-1){
		pthread_cond_broadcast(&empty);			//Aviso para as threads produtoras acordarem
	}
	pthread_mutex_unlock(&mymutex);				//Destrava o mutex
	return retorno;						//Retorna o valor que foi removido
}

void *consumer(void *parametros){
	int i,v,total;
	Argm* aux = (Argm*)parametros;
	total = TAM*P_THREADS;				//Tamanho total de operações = Tamanho máx * quantidade de threads produtoras
	printf("Consumidor %d iniciou\n",aux->id);
	for(i=0; i < total; i++){
		v = takeBlockingQueue(aux->queue);	//Removendo valor da fila
		printf("Consumidor %d consumiu %d\n", aux->id,v);
	}
	printf("Consumidor %d encerrou\n",aux->id);
	pthread_exit(NULL);
}
