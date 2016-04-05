#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

typedef struct numeros{
	int val1;
	int val2;
}Num;

void *MDC(void* v){
	int resto,a,b;
	Num* valores = (Num*) v;
	a =  valores->val1;//atribuições autoexplicativas...
	b =  valores->val2;
	while(b != 0){
		resto = a % b;	//o nome da variável torna claro o funcionamento, é um while "recursivo" que substitui sempre o menor termo
		a = b;		//pelo resto da divisão e o maior termo é substituído pelo menor termo, fazendo com que esses valores
		b = resto;	//se aproximem de zero, encerrando a "recursão" (quando b chega em 0). A resposta fica em "a", que recebe 
				//o último b (MDC) antes de zero (resto = 0). 
	}
	int* ret = (int*) malloc(sizeof(int));
	*ret = a; //aloca espaço para um ponteiro e retorna ele, com o valor apontado para a resposta (a)
  	pthread_exit(ret);
}

int main(){
	int equipes, qtd_membro, qtd_pares, i, rc, j,k,w;
	Num** x;

	printf("Digite numero de equipes: ");
	scanf("%d", &equipes);

	printf("Digite o numero de membros: ");
	scanf("%d", &qtd_membro);
	
	pthread_t threads[equipes]; //criando o vetor de threads conforme a quantidade de equipes
	x =(Num**) malloc(equipes*sizeof(Num*)); //alocando o espaço da variável "x" do tipo struct "Num", que será parâmetro na criação das threads
	for(i = 0; i < equipes; i++)//esses laços aninhados nada mais são do que o preenchimento da matriz x.
	{
		x[i]= (Num*)malloc(qtd_membro*sizeof(Num));
		for(j = 0; j < qtd_membro; j++)
		{
			printf("Digite o par de numeros da pessoa %d da equipe %d: ", j+1, i+1);
			scanf("%d %d", &x[i][j].val1, &x[i][j].val2);//val1 e val2 são valores da estrutura "Num", e x é uma matriz de "Num"
		}//Fizemos assim com a struct porque fica mais fácil (não havia necessidade de uma terceira dimensão não mensurável para "x"
	}	
	

	for(j=0;j<qtd_membro;j++){
		int aux = 0;
		for(i=0;i<equipes;i++){
			rc = pthread_create(&threads[i], NULL, MDC,(void*) &x[i][j]);//criação das threads conforme a quantidade de equipes.
			if(rc){							//envio do endereço de x para MDC.
				printf("ERRO; código de retorno é %d\n",rc);
				exit(-1);//se der erro de criação da thread, fim da execução
			}
		}
		for(k=0;k<equipes;k++){
			int *res;
    			pthread_join(threads[k], (void **) &res);
			aux = aux + *((int*)res);//laço para fazer com que cada thread aguarde a próxima, fazendo com que todas
		}				 //sejam aguardadas no final.
		printf("%d\n",aux);//a quantidade de turnos é dita pela quantidade de membros, por isso o for de membros está fora
	}				//e por isso se encontra aqui o print do resultado.
	pthread_exit(NULL);
	return 0;
}
