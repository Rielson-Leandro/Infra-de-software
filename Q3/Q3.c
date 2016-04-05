#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
int* contador;//variável global ponteiro contador, para facilitar o manuseio (ele é usado tanto na função Contagem como na main)
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

void* Contagem(void* arqid){
	int input, fid = *((int *)arqid);
	FILE* f;
	char FileName[10];//cria um arquivo f e uma auxiliar FileName. 

	sprintf(FileName,"%d.in",fid);//FileName recebe o valor recebido por parametro, que vai indicar o "x" de "x.in"
	f = fopen(FileName,"rt"); //abre o arquivo em "f" conforme seu FileName
	
	do{
	input = -1;//input é iniciado com -1 para que, caso o arquivo esteja vazio, simplesmente pula para fechar o arquivo.
	fscanf(f," %d",&input);//"faça: leia o arquivo enquanto input != -1
		if(input != -1){// quando input é != -1 (já que P só assume valores positivos, a partir de 0)
			pthread_mutex_lock(&mymutex);// trava (porque irá mexer na zona crítica)
			contador[input]++; //mexe na zona crítica
			pthread_mutex_unlock(&mymutex);//libera
		}
	}while(input != -1); //input sempre vai deixar de ser -1 quando fscanf tiver valores diferentes de -1, por isso funciona.
	fclose(f);           
	pthread_exit(NULL);
}

int main(){
	int i,j,k,soma=0,rc,NumArq,NumThread,NumProd;
	printf("Num de Arquivos: ");
	scanf(" %d",&NumArq);
	printf("Num de Threads: ");
	scanf(" %d",&NumThread);
	printf("Num de Produtos: ");
	scanf(" %d",&NumProd);

	pthread_t threads[NumThread];

	int* id[NumArq];
	contador = (int*) malloc(NumProd*sizeof(int));//alocar a zona crítica conforme quantidade de produtos.

	for(i=0;i<NumProd;i++)//zerar todos os endereços do contador
		contador[i] = 0;


	for(i=0;i<NumArq;i++){
		id[i] = (int*) malloc(sizeof(int));
		*id[i] = i+1;
		rc = pthread_create(&threads[i%NumThread],NULL,Contagem,(void*) id[i]);//criação das threads (de 0 até numero de threads)
		if (rc){         //esse [i%NumThread] trabalha como um mod, para manter a variação com o tamanho de "NumArq"
      			printf("ERRO; código de retorno é %d\n", rc);         
      			exit(-1);//caso ocorra algum erro na criação da thread, fim da execução.      
    		}   
	}
	
	for(k=0;k<NumArq;k++){
    		pthread_join(threads[k%NumThread], NULL);//join das threads, uma com a próxima, no intervalo igual ao da criação
	}

	float percent[NumProd]; //variável para impressão dos valores
	for(j=0;j<NumProd;j++){
		soma += contador[j];//total de produtos
	}
	printf("\nTotal de produtos lidos: %d\n",soma);
	for(j=0;j<NumProd;j++){
		percent[j]=((float)(100*contador[j]))/soma;//cast de float para a divisão ser float e armazenada no vetor percent
		printf("Percentual do produto %d: %7.3f%%\n",j,percent[j]);
	}
	
	pthread_exit(NULL);
	return 0;
}
