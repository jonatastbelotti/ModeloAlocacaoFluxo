/* v3
* Programa para gerar aleatoriamente grafos valorados e orientados cujo grafo subjacente é conexo
* autores: Sheila e Jonatas
* data: 17/08/2015
* entrada: V, E, P, AR, AB e PM, inteiros, onde V é o número de vértices, E é o número de arestas, P é a capacidade máxima,
* AR é o número de áreas de risco, AB é o número de abrigos e PM é a população máxima a ser evacuada.
*/

#include<stdio.h>
#include<time.h>
#include<stdlib.h>

#define MAXPESSOASVERTICE 100;

int main(void){
  int E, V, P, PMAB, PMAR, v1, v2, c, AR, AB,  **matriz,  i, j, pessoas = 0, p, ca;

	srand( (unsigned)time(NULL));

	scanf("%d%d%d", &V,&E, &P);
	scanf("%d%d%d%d", &AR, &AB, &PMAR, &PMAB);

	matriz = (int **) malloc(V*sizeof(int *));

	for(i=0;i<V;i++){
		matriz[i] = (int *) malloc(V*sizeof(int));
		for(j=0;j<V;j++)
			matriz[i][j]= 0;
	}
	//printf("matriz alocada!\n");
	for(i=1; i < V; i++){
	        v1 = i;
		v2 = rand() % i;
		c = (rand() % P)+1;
		matriz[v1][v2]=c;
	}

	for(i=V-1; i < E; i++){
	  do{
	        v1 = rand() % V;
		v2 = rand() % V;
	  }while(matriz[v1][v2]);
	  c = (rand() % P)+1;
	  matriz[v1][v2]=c;
	}

	printf("%d %d\n", V, E);
	for(i = 0; i < V; i++){
		for (j = 0; j < V; j++) {
			if(matriz[i][j] > 0){
				printf("%d %d %d\n", i, j, matriz[i][j]);
			}
		}
	}

    // AR = rand()%AR;
    // AB = rand()%AB;
	printf("%d %d\n", AR, AB);



	i=0;
	while(i<AR){
		v1 = rand() % V;
		//se v1 já é área de risco, v1+1 será área de risco
		while(matriz[v1][v1])
		{
			v1 = (v1+1)%V;
		}
		matriz[v1][v1]=1;
        p = rand() % (PMAR+1);
		printf("%d %d\n", v1, p);//Imprimindo áreas de risco
        pessoas += p;
		i++;
	}
	//printf("\n");
	i=0;
	while(i<AB)
	{
		v1 = rand() % V;
		while(matriz[v1][v1])
		{
			v1 = (v1+1)%V;
		}
		matriz[v1][v1]=2;
        ca = rand() % (PMAB+1);
		printf("%d %d\n", v1, -1*ca);//Imprimindo abrigos
		i++;
	}
	//printf("\n");


	for(i=0;i<V;i++)
		free(matriz[i]);

	free(matriz);
	return 0;
}
