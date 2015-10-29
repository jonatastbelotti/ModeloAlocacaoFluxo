/* Programa para determinar as rotas de fuga para uma população em áreas de risco, com várias origens e vários destinos.
*
* autores: Jônatas Trabuco Belotti e Sheila Morais de Almeida
* data: 14/10/2015
* versão: 2.0
*
* Implementa o método de alocação de fluxo em redes para evacuação de populações proposto por Belotti e Almeida em Trabalho de Conclusão de Curso apresentado a UTFPR Campus Ponta Grossa, Paraná.
* Explicações mais detalahadas sobre o método podem ser encontradas no documento impresso.
*
*/

#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <limits.h>

//Struct para a fila usada na busca em largura
struct fila{
    int val;
    struct fila *prox;
};

int numVertices, numArestas, numAreasRisco, numAbrigos, inicio, destino, removeu = 0, popAreasRisco = 0, popAbrigos = 0;
int **grafo, **redeResidual, *pai, **fluxo;

//Função responsável por inserir elementos na fila
struct fila * insereFila(struct fila *p, struct fila **fim, int v) {
    if(p == NULL){
        p = (struct fila *)malloc(sizeof(struct fila));
        if (p == NULL) {
            printf("Elemento não empilhado por falta de memória!\n");
            exit(0);
        }
        p->val = v;
        p->prox = NULL;
        *fim = p;
        return p;
    }else{
        (*fim)->prox = (struct fila *)malloc(sizeof(struct fila));
        (*fim)->prox->val = v;
        (*fim)->prox->prox = NULL;
        (*fim) = (*fim)->prox;
        return p;
    }
}

//Função responsável por remover determinado elemento da fila
struct fila * removeFila(struct fila *p){
    if(p == NULL){
        return NULL;
    }
    struct fila *aux = p;
    p = p->prox;
    free(aux);
    return p;
}

//Função responsável por remover todos os elementos da fila
struct fila * limpaFila(struct fila *fila, struct fila **fim){
    if(fila == NULL){
        *fim = NULL;
        return NULL;
    }else{
        struct fila *p = NULL;
        while(fila != NULL){
            p = fila;
            fila = fila->prox;
            free(p);
        }
        *fim = NULL;
        return fila;
    }
}

//Função responsável por inicar o grafo e a rede residual com os valores referentes a vazio
void iniciaValores(){
    for (int i = 0; i < numVertices; i++) {
        for (int j = 0; j < numVertices; j++) {
            grafo[i][j] = -1;
            redeResidual[i][j] = 0;
        }
    }
}

/* Função responsável por ler todos os dados.
* Primeiramente são lidos a quantidade de vértices e arestas do grafo
* Depois são lidas as arestas do grafo
* Depois são lidas a quantidade de áreas de risco e abrigos
* Depois são lidas as áreas de risco
* Depois são lidos os abrigos
*/
void lerDados(){
    scanf("%d", &numVertices);
    scanf("%d", &numArestas);

    //Acrescentando a superorigem e o superdestino no grafo
    numVertices += 3;
    inicio = numVertices - 2;
    destino = numVertices - 1;

    //Alocando dinamicamento o grafo, a rede residual e a estrutura responsvel por guardar as rotas determinadas
    grafo = (int **)malloc(numVertices * sizeof(int *));
    fluxo = (int **)malloc(numVertices * sizeof(int *));
    redeResidual = (int **)malloc(numVertices * sizeof(int *));
    pai = (int *)malloc(numVertices * sizeof(int));

    //Verificando se as estruturas realmente foram alocadas
    if(grafo == NULL){printf("Grafo não alocado!\n"); exit(0);}
    if(fluxo == NULL){printf("Fluxo não alocado!\n"); exit(0);}
    if(redeResidual == NULL){printf("redeResidual não alocado!\n"); free(grafo); exit(0);}
    if(pai == NULL){printf("Pais não alocado!\n"); free(grafo); free(redeResidual); exit(0);}

    //Segunda parte da alocação dinâmica das matrizes
    for (int i = 0; i < numVertices; i++) {
        grafo[i] = (int *)malloc(numVertices * sizeof(int));
        fluxo[i] = (int *)malloc(numVertices * sizeof(int));
        redeResidual[i] = (int *)malloc(numVertices * sizeof(int));

        //Verificando se as estruturas realmente foram alocadas
        if(grafo[i] == NULL){printf("Grafo %d não alocado!\n", i); exit(0);}
        if(fluxo[i] == NULL){printf("Fluxo %d não alocado!\n", i); exit(0);}
        if(redeResidual[i] == NULL){printf("redeResidual %d não alocado!\n", i); exit(0);}
    }

    //Iniciando o grafo e a rede residual
    iniciaValores();

    //Lendo as arestas do grafo juntamente com suas capacidades
    int v1, v2, c;
    for (int i = 0; i < numArestas; i++){
        scanf("%d %d %d", &v1, &v2, &c);
        grafo[v1][v2] = c;
        if(grafo[v2][v1] == -1){
            grafo[v2][v1] = c;
        }
    }

    //lendo as áreas de risco juntamente com a quantidade de UTs
    int a, ca;
    scanf("%d %d", &numAreasRisco, &numAbrigos);
    for (int i = 0; i < numAreasRisco; i++) {
        scanf("%d %d", &a, &ca);

        //Criando a aresta entre a superorigem e a área de risco com capacidade igual a quantidade de UTs presentes na área de risco
        grafo[inicio][a] = ca;
        popAreasRisco += ca;
        grafo[a][a] = ca;
    }

    //lendo os abrigos juntamente com suas capacidades
    for (int i = 0; i < numAbrigos; i++) {
        scanf("%d %d", &a, &ca);

        //Criando a aresta entre o abrigo e o superdestino com capacidade igual a capacidade do abrigo
        grafo[a][destino] = ca*(-1);
        popAbrigos += ca*(-1);
        grafo[a][a] = ca;
    }
}

//Função responsável por desalocar todas as estruturas que foram alocadas dinâmicamente
void limparDados(){
    for (int i = 0; i < numVertices; i++) {
        free(grafo[i]);
        free(fluxo[i]);
        free(redeResidual[i]);
    }
    free(grafo);
    free(fluxo);
    free(redeResidual);
    free(pai);
}

//Função responsável por imprimir na tela os dados lidos para que sejam conferidos, atualmente está função não é usada
void imprimirDados(){
    printf("Vertices: %d\n", numVertices);
    printf("Arestas: %d\n", numArestas);

    for(int i = 0; i < numVertices; i++){
        printf("%d->", i);
        for (int j = 0; j < numVertices; j++) {
            if(grafo[i][j] != -1){
                printf("(%d, %d)->", j, grafo[i][j]);
            }
        }
        printf("\n");
    }

    printf("Areas de risco:\n");
    for (int i = 0; i < numVertices; i++) {
        if(grafo[inicio][i] != -1){
            printf("%d, ", i);
        }
    }
    printf("\n");

    printf("Locais seguros:\n");
    for (int i = 0; i < numVertices; i++) {
        if(grafo[i][destino] != -1){
            printf("%d, ", i);
        }
    }
    printf("\n");

    printf("Vertice inicio: %d\nVertice destino: %d\n", inicio, destino);

}

//Função responsável por realizar a busca em largura no grafo
int bfs(){
    // Cria um vetor para marcar quais vértices já foram visitados
    int *visited, n;
    visited = (int *)malloc(numVertices * sizeof(int));
    if(visited == NULL){printf("Falta de memoria!\n");exit(0);}
    for(int i = 0; i < numVertices; i++){
        visited[i] = 0;
    }

    // Cria a fila para executar a busca em largura
    struct fila *q = NULL, *fim = NULL;
    q = insereFila(q, &fim, inicio);
    visited[inicio] = 1;
    pai[inicio] = -1;

    // Inicia a busca em largura
    while (q != NULL){
        int u = q->val;
        q = removeFila(q);

        for (int v=0; (v<numVertices); v++){
            if (visited[v]==0 && redeResidual[u][v] > 0){
                q = insereFila(q, &fim, v);
                pai[v] = u;
                visited[v] = 1;
                if(v == destino){
                    n = visited[destino];
                    free(visited);
                    q = limpaFila(q, &fim);
                    return (n == 1);
                }
            }
        }
    }

    // Retorna verdadeira caso encontre um novo caminho da superorigem até o superdestino
    // Retorna falso, caso contrário
    n = visited[destino];
    free(visited);
    q = limpaFila(q, &fim);
    return (n == 1);
}

//Função responsável por imprimir a rede residual, atualmente está função não é usada
void imprimeResidual(){
    printf("REDE RESIDUAL\n");
    for(int i = 0; i < numVertices; i++){
        for(int j = 0; j < numVertices; j++){
            if(redeResidual[i][j] > 0){
                printf("%d %d %d\n",i,j,redeResidual[i][j]);
            }
        }
    }
}

//Função responsável por imprimir o último camonho encontrado pela busca em largura, atualmente está função não é usada
void imprimeCaminho(int i){
    if(pai[i] == -1){
        printf("%d->", i);
    }else{
        imprimeCaminho(pai[i]);
        printf("%d->", i);
    }
}

/* Função que implementa o método de Ford-Fulkerson responsável por determinar o valor do fluxo máximo
 * Como as rotas são guardas na matriz fluxo é possível saber quais as rotaas usadas para atingir o fluxo máximo
*/
int fordFulkerson(){
    int u, v;

    //Iniciando redeResidual
    for (u = 0; u < numVertices; u++){
        for (v = 0; v < numVertices; v++){
            if(grafo[u][v] != -1){
                redeResidual[u][v] = grafo[u][v];
            }else{
                redeResidual[u][v] = 0;
            }

             fluxo[u][v] = 0;
         }
    }

    int max_flow = 0;  //iniciando o fluxo máximo com 0

    //Enquando existir um novo caminho entre a superorigem e o superdestino
    while(bfs()){
        // Procura a aresta de menor capacidade no caminho enconntrado, o valor
        //do fluxo deste caminho é igual a capacidade dessa aresta
        int path_flow = INT_MAX;
        for (v=destino; v!=inicio; v=pai[v]){
            u = pai[v];
            if(path_flow > redeResidual[u][v]){
                path_flow = redeResidual[u][v];
            }
        }

        // Atualiza  rede residual com o novo fluxo encontrado
        for (v=destino; v != inicio; v = pai[v]){
            u = pai[v];
            redeResidual[u][v] -= path_flow;
            //Impede que tenha retorno de fluxo
            redeResidual[v][u] = 0;

            //guarda a rota na matriz fluxo para que ela possa ser usada futuramente
            fluxo[u][v] += path_flow;
        }

        // Adiciona o fluxo encontrado no fluxo máximo
        max_flow += path_flow;
        // imprimeCaminho(destino);
    }

    // retorna o valor do fluxo máximo encontrado
    return max_flow;
}

/* Função responsável por verificar se as rotas atendem as restrições estabelecidas
 * Verifica em todos os vértices e retira a aresta de menor fluxo
*/
int verificaFluxo(){
    int r = 1;
    //Impede que arestas incidentes na superorigem ou superdestino sejam retiradas
    int limite = numVertices - 2;

    //Verifica para todos os vértices quais não atendem as restrições das rotas
    for (int i = 0; i < limite; i++) {
        int quantSai = 0, menSai = i, quatEnt = 0, menEnt = i, menor;
        fluxo[i][i] = INT_MAX;

        //As rotas não são verificadas para as áres de risco ou abrigos
        // if (grafo[i][i] != -10){
            // Calcula quantoas fluxos entram no vértice e quantos fluxos saem do vértice
            for(int j = 0; j < limite; j++) {
                // As restrições apenas são verificadas para vértices que não são abrigos ou áres de risco
                // if(grafo[j][j] != -10){
                    if((i != j) && (fluxo[i][j] > 0)){
                        quantSai++;
                        // Guarda qual aresta com menor fluxo que sai do vértice
                        if(fluxo[i][j] < fluxo[i][menSai]){
                            menSai = j;
                        }
                    }
                    if((i != j) && (fluxo[j][i] > 0)){
                        quatEnt++;
                        // Guarda qual aresta com menor fluxo que entra no vértice
                        if(fluxo[j][i] < fluxo[menEnt][i]){
                            menEnt = j;
                        }
                    }
                // }
            }

            // Verifica se mais de um fluxo entra no vértice e mais de um fluxo sai do vértice
            if(quatEnt > 1 && quantSai > 1){
                removeu++;
                r = 0;

                // Remove aresta com menor fluxo do grafo
                if(fluxo[i][menSai] < fluxo[menEnt][i]){
                    // printf("Removeu %d->%d\n", i,menSai);
                    grafo[i][menSai] = -1;
                    grafo[menSai][i] = -1;
                    fluxo[i][menSai] = 0;
                    fluxo[menSai][i] = 0;
                }else{
                    // printf("Removeu %d->%d\n", menEnt, i);
                    grafo[menEnt][i] = -1;
                    grafo[i][menEnt] = -1;
                    fluxo[menEnt][i] = 0;
                    fluxo[i][menEnt] = 0;
                }
            }
        // }
        fluxo[i][i] = 0;
    }

    //Retorna se as rotas estavam de acordo com as restrições
    // 1- Sim
    // 0 -Não
    return r;
}

// Função responsável por escoar o fluxo e atualizar os valores das áreas de risco e abrigos
void escoaFluxo(){
    int limite = numVertices-2;

    for(int i = 0; i < limite; i++){
        //Verificando áreas de risco
        if(fluxo[inicio][i] > 0){
            grafo[i][i] -= fluxo[inicio][i];
            grafo[inicio][i] = grafo[i][i];
        }

        // Verificando abrigos
        if(fluxo[i][destino] > 0){
            grafo[i][i] += fluxo[i][destino];
            grafo[i][destino] = grafo[i][i]*-1;
        }
    }
}

//Função responsável por imprimir as rotas de fuga
void imprimeFluxo(){
  int limite = numVertices - 2;
    for(int i = 0; i < limite; i++){
        for(int j = 0; j < limite; j++){
            if(fluxo[i][j] > 0){
                printf("%d -> %d (%d)\n",i,j,fluxo[i][j]);
            }
        }
    }
}

// Função principal
int main(){
    int f = 1, t = 0, i =1, pe = 0;
    float p = 100.0;
    setlocale(LC_ALL, "Portuguese");

    // Lê os dados de entrada
    lerDados();

    //Verificando se será possivel evacuar toda população, caso não apresenta qual a porcentagem da população será evacuada
    if(popAreasRisco > popAbrigos){
        p = ((double)100/(double)popAreasRisco)*(double)popAbrigos;
        printf("Atenção!\nOs abrigos não tem capacidade para atender a todos, apenas %d das %d UTs (%.2f%%) serão salvas!\n\n", popAbrigos, popAreasRisco, p);
    }

    // Enquanto existir fluxo das áreas de risco para os abrigos
    while(f > 0){
        do{
            f = fordFulkerson();
        }while(!verificaFluxo());

        if(f > 0){
            //Escoando fluxo e atualizando valores
            escoaFluxo();

            // Imprimindo as rotas da leva
            printf("--Rotas do %dº turno-----------------------------\n",i);
            imprimeFluxo();

            //Imprimindo o fluxo total da leva
            printf("Fluxo: %d\nArestas removidas: %d\n\n", f, removeu);

            pe += f;
            t++;
            i++;
        }
    }

    // Verifica se alguma população foi evacuada
    if(pe > 0){
        //Imprimendo o tempo total da evacuação
        printf("População evacuada: %d de %d UTs\n", pe, popAreasRisco);
        printf("O tempo total para evacuar %.2f%% da população é: %d unidades de tempo.\n\n",p,t);
    }else{
        printf("Não é posspivel evacuar ninguém!\n");
    }


    // Desalocando as estruturas dinâmicas
    limparDados();
    return 0;
}
