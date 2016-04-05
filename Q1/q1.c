
/*
A senha esta definida no #define SENHA, e o numero de threads esta definida no #define QTD_THREADS

Teste para achar uma senha muito alta: 9999999999

Quantidade de threads			Tempo medio(s)
1					22
2					14
4					8.5
8					4.5

esso pequeno teste mostra que o tempo reduz quase que pela metade toda vez que duplicamos a quantidade de threads
*/


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX_SENHA 10000000000

#define QTD_THREADS 10
#define SENHA 9999999999
#define INTERVAL MAX_SENHA/QTD_THREADS

int achou=0;
long long min;
long long num;

void *testar_senha(void *threadid){
	long tid = *((long *)threadid);
	long long i, fim;
	if(!achou){
	    fim = ((tid+1)*(INTERVAL)); // fim é o limite final que cada thread vai procurar
	    if(tid+1 == QTD_THREADS) fim = MAX_SENHA; // se numero maximo a ser testado dividido pelo numero de threads 
	    // de resto diferente de zero, para evitar que procure em senhas invalidas
		for(i=tid*(INTERVAL); i<fim && achou==0; i++)
		{
			if(i == num){ // se achou a senha imprime e ativa a variavel achou para que as outras threads tmbem parem
				achou = 1;
				printf("Senha %.10lld, achada na thread: %ld\n", i, tid+1);
				break;
			}
		}
	}
	pthread_exit(NULL);
}

int main(){
	int i,j;
	long *taskids[QTD_THREADS]; //  guarda os ids das threads
	pthread_t threads[QTD_THREADS]; // vetor de threads
	num=SENHA; // numero recebe a senha

	min = 0;
	for(i=0; i< QTD_THREADS; i++){ // cria as threads
		taskids[i] = (long *) malloc(sizeof(long));
		*taskids[i] = i;
		pthread_create(&threads[i], NULL, testar_senha, (void *) taskids[i]);
	}

	pthread_exit(NULL);
}
