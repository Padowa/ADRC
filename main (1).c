                                                                       #include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#define unvisited -1 // nÃ³ do grafo ainda nÃ£o foi visitado
#define visiting 0 // nÃ³ do grafo estÃ¡ em processo de visitado
#define visited 1 // nÃ³ do grafo jÃ¡ foi visitado por completo
#define MAX_NODES 65535 // numero maximo de nos numa rede
#define STACK_EMPTY -240 // erro caso a pilha se encontre vazia
#define Lines 246589


// criar estrutura de cada nó
struct node {
  int vertex;
  int connectionType;
  struct node* next;
};


// criar estrutura de cada nó
struct bgpNode {
  int lastVertex;
  int connectionType;
};

struct queue {
  struct node *head;
  struct node *tail; // ponteiro para o ultimo no do tipo p na queue
  struct node *r; // ponteiro para o ultimo no do tipo r na queue
  struct node *c; // ponteiro para o ultimo no do tipo c na queue
};

struct Graph {
  int numVertices;
  struct node** adjLists;
};


// criar um nó
struct node* createNode(int vertice, int tipoDeLigacao) {
  struct node* newNode = malloc(sizeof(struct node));
  newNode->vertex = vertice;
  newNode->connectionType = tipoDeLigacao;
  newNode->next = NULL;
  return newNode;
}

// criar o grafo
struct Graph* createAGraph(int numVertices) {
  struct Graph* graph = malloc(sizeof(struct Graph));

  // numero de nos no grafo
  graph->numVertices = numVertices;

  // alocacao de memoria
  graph->adjLists = malloc(numVertices * sizeof(struct node*));

  int i = 0;

  // inicializacao da lista de adjacencias a null
  for (i = 0; i < numVertices; i++)
    graph->adjLists[i] = NULL;

  return graph;
}

int searchExistingEdge(struct Graph* graph, int s, int d) {

	struct node* temp = graph->adjLists[s];
	while (temp) {
		if (temp->vertex == d)
			return 1; // aresta ja existente no grafo
		temp = temp->next;
	}	
	return 0;
}

// funcao que adiciona arestas
void addEdge(struct Graph* graph, int s, int d, int connectionType) {
  	
  if( !searchExistingEdge(graph, s, d) ) // ativado caso nao se tenha considerado antes esta aresta
  {
	  // ligacao de s (cauda) para d (cabeça) de tipo connectionType
	  struct node* newNode = createNode(d, connectionType);
	  newNode->next = graph->adjLists[s];
	  graph->adjLists[s] = newNode;

	  // ligacao contraria (de d para s)
	  if ( connectionType == 1 )
	  	newNode = createNode(s, 3);

	  else if ( connectionType == 3 )
	  	newNode = createNode(s, 1);

	  else if ( connectionType == 2 )
	  	newNode = createNode(s, 2);

	  newNode->next = graph->adjLists[d];
	  graph->adjLists[d] = newNode;
	}
}

// Print the graph
void printGraph(struct Graph* graph) {
  int v;
  for (v = 0; v < graph->numVertices; v++) {
    struct node* temp = graph->adjLists[v];
    printf("\n Vertex %d\n: ", v);
    while (temp) {
      printf("(node: %d,type: %d) -> ", temp->vertex, temp->connectionType);
      temp = temp->next;
    }
    printf("\n");
  }
}

void populateAdjencyList(struct Graph* graph, char *ficheiro){


  
  FILE* file = fopen (ficheiro, "r");
  int cauda = 0;
  int cabeca = 0; 
  int tipoDeLigacao = 0;
  int cnt = 0 ; 
  
  do{
    fscanf (file, "%d %d %d", &cauda, &cabeca, &tipoDeLigacao);
    addEdge(graph, cauda,cabeca,tipoDeLigacao);
  } while (!feof (file));
  fclose (file);        
  return;
}

void newQueue(struct queue* bgpQueue) {
  
  bgpQueue->head = NULL;
  bgpQueue->tail = NULL;
  bgpQueue->r = NULL;
  bgpQueue->c = NULL;

  return;
}

// funcao que adiciona ligacoes a uma fila prioritaria (c>r>p)
bool addToQueue(struct queue* bgpQueue, int vertex, int connectionType) {
 

  struct node* newNode = malloc(sizeof(struct node));
  struct node* temp = malloc(sizeof(struct node));
 
  // nao ha memoria para se criar novos nos
  if (newNode == NULL || temp == NULL) return false;
 
  // preencher estrutura de dados do no
  newNode->vertex = vertex;
  newNode->connectionType = connectionType;
  newNode->next = NULL;
  
  // fila prioritaria vazia, atualizar ponteiros
  if (bgpQueue->head == NULL){
    bgpQueue->head = newNode; 
    bgpQueue->tail = newNode;
    bgpQueue->c = newNode;
    bgpQueue->r = newNode;
  }

  // tail possui um no, e portanto a lista nao esta vazia
  if (bgpQueue->tail != NULL){
    switch(connectionType){

      case 1: // no do tipo p (adicionado na tail)
        (bgpQueue->tail)->next = newNode;
        (bgpQueue->tail) = newNode;
        break;

      case 2: // no do tipo r (adicionado no fim dos nos tipo r)
        temp = bgpQueue->r;

        // r aponta para primeiro tipo p, logo a fila so tem nos tipo p
        if((bgpQueue->r)->connectionType == 1){
          // adicionar node tipo r na cabeça da fila, atualizar ponteiro r e c
          newNode->next= bgpQueue->head;
          bgpQueue->head=newNode; 
          bgpQueue->r=newNode;
          bgpQueue->c=newNode;
        }
        // ponteiro aponta para o fim do conjunto dos nodes tipo r
        // se nao houver tipo r, aponta para o fim do tipo c
        else{
          // colocar novo node na posicao anterior do antigo ponteiro r,
          // atualizar ponteiro r para o novo ultimo node
          newNode->next = (bgpQueue->r)->next;
          (bgpQueue->r)->next = newNode;
          (bgpQueue->r) = newNode;
        }
        // atualizar tail para o novo node (so necessario quando ainda nao ha nos tipo p)
        if(temp == bgpQueue->tail)
          bgpQueue->tail = bgpQueue->r;
        break;

      case 3: // no do tipo c (adicionado no fim dos nos tipo c)
        temp = bgpQueue->c;
         // c aponta para primeiro tipo p ou r, logo a fila nao tem nos tipo c
        if((bgpQueue->c)->connectionType == 1 || (bgpQueue->c)->connectionType == 2 ){
          // adicionar node tipo c na cabeca da fila
          newNode->next= bgpQueue->head;
          bgpQueue->head=newNode;
          bgpQueue->c=newNode;
          // caso nao existam nos tipo r na fila (ponteiro r aponta para node tipo p)
          // necessario atualizar ponteiro r para novo ponteiro c
          if((bgpQueue->r)->connectionType == 1)
            bgpQueue->r=bgpQueue->c;
        }
        // ponteiro aponta para o fim do conjunto dos nodes tipo c
        // se nao houver tipo c, aponta para a cabeça da fila
        else{
          // colocar novo node na posicao anterior do antigo ponteiro c,
          // atualizar ponteiro c para o novo ultimo node          
          newNode->next = (bgpQueue->c)->next;
          (bgpQueue->c)->next = newNode;
          (bgpQueue->c) = newNode;


          if((bgpQueue->r)->connectionType != 2) // r aponta para antigo ultimo c, necessario atualizar com novo ultimo c
            bgpQueue->r=bgpQueue->c;
        }

        // caso so existam nos tipo c na fila, atualizar tail        
        if(temp == bgpQueue->tail)
          bgpQueue->tail = bgpQueue->c;
        break;
      }
   
  // 1 -> p
  // 2 -> r
  // 3 -> c 
  }
  printf("bgpQueue->head:%d\n", bgpQueue->head->vertex);
  return true;
}

// remover no do topo da fila, retornando 1 se houver node
int removeFromQueue(struct queue* bgpQueue, struct bgpNode* removedNode) {

  // fila vazia, nao existem nodes para remover
  if (bgpQueue->head == NULL) return 0;
 
  struct node* temp = bgpQueue->head;

  if(bgpQueue->head == bgpQueue->tail) {
    printf("entrou-se aqui!\n");
    bgpQueue->head->next = NULL;
  }

  // nova head sera o segundo no da fila
  bgpQueue->head=bgpQueue->head->next;

  // se o ponteiro c estiver a apontar para a head, atualizar posicao para a nova head
  if(bgpQueue->c == temp) {
    // mesmo para o ponteiro r
    if (bgpQueue->r==bgpQueue->c)
      bgpQueue->r=bgpQueue->head;
    bgpQueue->c=bgpQueue->head;    
    printf("Node removido!\n");
  }

  removedNode->lastVertex = temp->vertex;
  removedNode->connectionType = temp->connectionType;
  printf("antes deste if\n");
  // no caso de a fila ficar vazia, necessario tambem atualizar ponteiro da tail
  if (bgpQueue->head == NULL)
  { 
      printf("Fila ficou sem nodes!\n");
      bgpQueue->tail = NULL;
      bgpQueue->r = NULL;
      bgpQueue->c = NULL;
  }

  printf("depois deste if\n");
  free(temp);
  printf("depois deste free\n");

  return 1;
}


int emptyQueue (struct queue* bgpQueue) {

  if (bgpQueue->head != NULL)
    return 0;

  else
    return 1;
}

// verifica se o caminho descoberto atualmente e melhor do que o descoberto anteriormente
int connectionPower(struct bgpNode* bgpVector, struct node* temp) { 

  // primeiro caminho decoberto para o node visitado
  if ( bgpVector[temp->vertex].connectionType == -1 )
    return 1;


  /////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////// WARNING //////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  // se o tipo de ligacao usado para se aceder ao node temp for mais favoravel do que o melhor caminho descoberto para esse
  // vertice, entao deve-se guardar o novo caminho descoberto
  if ( temp->connectionType < bgpVector[temp->vertex].connectionType  )
    return 1;
 
  else
    return 0;
}

// verifica se a aresta decoberta eh comercialmente valida
int pathIsValid(struct bgpNode* bgpVector, struct node* temp, struct bgpNode* removedNode) {

  // comportamentos diferentes mediante tipos de conexao em consideracao
  switch (bgpVector[removedNode->lastVertex].connectionType) {
    case 1: // caminho do tipo c para o no seguinte na bgp
      if (temp->connectionType == 1) // descoberto pela bgp um caminho do tipo c
        return 1;
      if (temp->connectionType == 2) // descoberto pela bgp um caminho do tipo r
        return 1;
      if (temp->connectionType == 3) // descoberto pela bgp um caminho do tipo p
        return 1;
      break;
    case 2: // caminho do tipo r para o no seguinte na bgp
      if (temp->connectionType == 1) // descoberto pela bgp um caminho do tipo c
        return 0;
      if (temp->connectionType == 2) // descoberto pela bgp um caminho do tipo r
        return 0;
      if (temp->connectionType == 3) // descoberto pela bgp um caminho do tipo p
        return 1;
      break;
    case 3:
      if (temp->connectionType == 1) // descoberto pela bgp um caminho do tipo c
        return 0;
      if (temp->connectionType == 2) // descoberto pela bgp um caminho do tipo r
        return 0;
      if (temp->connectionType == 3) // descoberto pela bgp um caminho do tipo p
        return 1;      
      break;
    default:
      return 0;
      break;
  }


}


struct bgpNode* bgpPath(struct Graph* graph, int destination, int source) {
  // alocar memoria para...
  // vetor de que guarda o salto escolhido pela BGP em cada node
  struct bgpNode* bgpVector = malloc(MAX_NODES*sizeof(struct bgpNode));
  // estrutura auxiliar para guardar nodes que saiam da fila
  struct bgpNode* removedNode = malloc(sizeof(struct node*));
  // estrutura da fila que guarda os nos alcancados que faltam ser vistos
  struct queue* bgpQueue = malloc(sizeof(struct queue));

  


  int i = 0;

  // inicializacao do vetor que irá guardar o caminho bgp
  for (i=0; i<MAX_NODES; i++) {
      (bgpVector[i]).lastVertex = -1;
      (bgpVector[i]).connectionType = -1;
  }

  // criacao da nova fila
  newQueue(bgpQueue);

  (bgpVector[destination]).lastVertex = destination;
  (bgpVector[destination]).connectionType = 0; // inicio do caminho bgp (destino)
  
  

  struct node* temp = graph->adjLists[destination];

  // adicionar arestas adjacentes ao nó destino, de onde se vai correr o bgp
  while (temp) {
    // adicionar aresta na fila
    printf("Primeiras arestas a entrar na queue\n");
    printf("node %d, connectionType %d\n", temp->vertex, temp->connectionType);
    addToQueue(bgpQueue, temp->vertex, temp->connectionType);
    bgpVector[temp->vertex].lastVertex = destination; 
    bgpVector[temp->vertex].connectionType = temp->connectionType;
    temp = temp->next;
  }


  // percorrer o grafo com uma bfs partindo do no 'destination' do algoritmo ate se chegar com o melhor caminho possivel ao no 'source'
  while (!emptyQueue(bgpQueue)) { // enquanto existirem arestas na fila
    printf("Antes de remove from queue\n");
    if(removeFromQueue(bgpQueue, removedNode)) { // remover uma aresta da cabeca da fila 
      printf("Depois de remover\n");
      printf("RemovedNode:%d\n", removedNode->lastVertex);
      printf("QueueHead:%d\n", bgpQueue->head);
      temp = graph->adjLists[removedNode->lastVertex];
      printf("Segundo while\n");

      // percorrer todas as arestas do node retirado da fila (para onde nos leva a aresta retirada)
      while (temp) {  
        // verifica se caminho encontrado eh melhor do que o guardado
        if (connectionPower(bgpVector,temp)) {
          printf("Connection power\n");
          if (bgpVector[temp->vertex].lastVertex != removedNode->lastVertex)
            if(pathIsValid(bgpVector,temp,removedNode)) {
               printf("node %d, connectionType %d\n", temp->vertex, temp->connectionType);
              // adicionar aresta encontrada na fila, confirmar se esta de acordo com o que se pretende na fila
              addToQueue(bgpQueue, temp->vertex, temp->connectionType);
              // atualizar caminho bgp no vetor, confirmar se esta de acordo com o que se pretende no vetor
              bgpVector[temp->vertex].lastVertex = removedNode->lastVertex;
              bgpVector[temp->vertex].connectionType = temp->connectionType;
            }
        }
        temp = temp->next;
      }
    }
  }
  printf("Saiu-se do bgp\n");
  //scanf("%d", &i);
 // if( removeFromQueue(bgpQueue, removedNode) ) {
    for (i=0;i<5;i++)
      printf("(%d,%d)->\n",bgpVector[i].lastVertex, bgpVector[i].connectionType);


  //}

//  bgpPath(removedNode);


  free(removedNode);
  // while( fila nao vazia )


  // 1 -> p
  // 2 -> r
  // 3 -> c
  return bgpVector;
}


void printPath(int source, int destination, struct bgpNode* bgpVector) {

  int i = source;
  int t;
  printf("destination --connectionType-->\n");
  printf("%d --%d--> ", source, bgpVector[i].connectionType);
  while (bgpVector[i].lastVertex!=destination)
  {
    t = bgpVector[i].lastVertex;
    if(bgpVector[t].connectionType==1)
      printf(" %d --3--> ",bgpVector[i].lastVertex);
    else if(bgpVector[t].connectionType==3)
      printf(" %d --1--> ",bgpVector[i].lastVertex);
    else
      printf(" %d --%d--> ",bgpVector[i].lastVertex,bgpVector[t].connectionType);
    i = t;
  }
  printf(" %d\nFim do caminho.\n", destination);

  return;
}

void algoritmo1(struct Graph* graph, int source, int destination){

  struct bgpNode* bgpVector;
  int i = 0;
  bgpVector = bgpPath(graph, destination, source);
  printf("Nao passou!\n");
    for (i=0;i<13;i++)
      printf("(%d,%d)->\n",bgpVector[i].lastVertex, bgpVector[i].connectionType);
  printf("Passou\n");
  printPath(source,destination,bgpVector); // nó -> valor do nó anterior da bgp e tipo de ligacao, depois seria só recuar no caminho por esta ordem
  // source<-no1<-no2<-no3<-no4<-no5<-destination (BGP)
  // source->no1->no2->no3->no4->no5->destination (printType)

  //resetGraph(graph);

}


int main(int argc, char *argv[]) 
{ 
  int tipoDeAlgoritmo = 0;
  int noDePartida = 0;
  int noDeDestino = 0;
	if (argc>2)
	{
		printf("Numero de argumentos invalido");
		return 0; 
	}
  printf("A carregar ficheiro...\n");
  struct Graph* graph = createAGraph(13);
  populateAdjencyList(graph, argv[1]);
  printf("Ficheiro %s carregado\n", argv[1]);
  printf("Menu:\n1- Algoritmo 1\n2- Algoritmo 2\n3- Algoritmo 3\n4- Sair do Programa\n");

  while(true) {

    printf("Selecione o algoritmo pretendido: ");
    scanf("%d",&tipoDeAlgoritmo);
    if(tipoDeAlgoritmo==1||tipoDeAlgoritmo==2||tipoDeAlgoritmo==3) {
      printf("Selecione o no de partida: ");
      scanf("%d",&noDePartida);
      printf("Selecione o no de destino: ");
      scanf("%d",&noDeDestino);
    }
    
    if ( (noDePartida>MAX_NODES) || (noDePartida<0) || (noDeDestino>MAX_NODES) || (noDeDestino<0) )
      printf("No introduzido nao existente.\n");

    else {
      switch (tipoDeAlgoritmo){
        case 1: 
          printf("Algoritmo 1 introduzido.\n");
          algoritmo1(graph, noDePartida, noDeDestino);
          break;
        case 2: 
          printf("Algoritmo 2 introduzido.\n");
          break;
        case 3:
          printf("Algoritmo 3 introduzido.\n");
          break;
        case 4:
          printf("Saida do programa.\n");
          return 0;
          break;
        default:
          printf("Valor invalido, tente novamente.\n");
          break;
      }
    }

  }


  //addEdge(graph, 1000, 1, 1);
  //addEdge(graph, 1000, 2, 2);
  //addEdge(graph, 1000, 3, 1);
  //addEdge(graph, 1, 2, 3);
  //addEdge(graph, 1, 1000, 3);

  //printGraph(graph);

  return 0; 
}






