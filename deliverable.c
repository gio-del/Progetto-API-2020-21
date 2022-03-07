#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct {
    unsigned int cost; // e' il peso del grafo
    unsigned int id; //id grafo, numero progressivo da 0 a k_nodes-1
}graph;

typedef enum{false=0,true=1
}bool;

typedef struct { //struct per nodi dijkstra
    unsigned int dist; //la distanza dal nodo sorgente
    bool visited;
}dijkstra_node;

//Variabili globali
unsigned int n_nodes; //numero di nodi
unsigned int k_top; //k=numero di grafi "migliori" da stampare
unsigned int ** adj_mat; // e' la matrice di buffer usata per fare i conti sui cammini minimi dei grafi
unsigned int n_graph=0; //numero grafi inseriti fino a quel momento
graph * heap; //è lo heap che conterrà la classifica dei topK
//FILE * fin;

//Prototipi funzioni
void input_parse();
void crea_matrice();
void aggiungi_grafo();
void topk();
dijkstra_node * dijkstra_nodes_alloc(unsigned int len);
unsigned int sum_dijkstra();
unsigned int node_mindist(dijkstra_node * vect); //ritorna quale nodo ha distanza minima nel vettore dei nodi
void heap_allocate();
void max_heapify(unsigned int n);
void swap(graph * n1, graph * n2);
void costruisci_max_heap();
void inserimento_contestuale(graph node); //inserisce l'elemento sostituendo quello con costo maggiore
unsigned int atoi_veloce( char * str );


int main(){
//    fin=fopen("input.txt","r");
    input_parse();
    return 0;
}

void input_parse(){
    char buffer[20];
    char * token = NULL;
    //prende i primi due numeri: numero di nodi e k
    if(fgets(buffer,sizeof(buffer),stdin)) {
        token = strtok(buffer, " ");
    }
    n_nodes=atoi_veloce(token);
    token=strtok(NULL," ");
    k_top=atoi_veloce(token);
    crea_matrice(); //crea la matrice di adiacenza di supporto su cui verrà eseguito dijkstra
    heap_allocate();
    //parsing del resto dell'input
    while(fgets(buffer,sizeof(buffer),stdin)!=NULL){
        if(buffer[0]=='A'){
            aggiungi_grafo();
            n_graph++;
        }
        else if(buffer[0]=='T'){
            topk();
        }
    }
}

void crea_matrice(){
    unsigned int i;
    adj_mat=(unsigned int **)malloc(sizeof(unsigned int *)*n_nodes);
    for(i=0;i<n_nodes;i++){
        *(adj_mat+i)=(unsigned int *)malloc(sizeof(unsigned int)*n_nodes);
    }
}

void heap_allocate(){
    heap=(graph *)malloc(sizeof(graph)*k_top);
}

unsigned int atoi_veloce( char * str )
{
    unsigned int n = 0;
    while( *str!='\0' && *str!='\n' && *str!='\r') {
        n = n*10 + *str - '0'; // *str-'0' converte il numero *str da char a unsigned int
        str++;
    }
    return n;
}

void aggiungi_grafo(){
    unsigned int i=0,j;
    int idx_line,idx_buff;
    char linebuffer[4196];
    char buffer_int[15];
    graph grafo_da_inserire;
    while(i<n_nodes){
        j=0;
        idx_line=0;
        idx_buff=0;
        if(fgets(linebuffer,sizeof(linebuffer),stdin)){
            while(linebuffer[idx_line]!='\n'){
                if(linebuffer[idx_line]==','){
                    buffer_int[idx_buff]='\0';
                    adj_mat[i][j]=atoi_veloce(buffer_int);
                    j++;
                    idx_buff=0;
                    idx_line++;
                }
                else{
                    buffer_int[idx_buff]=linebuffer[idx_line];
                    idx_buff++;
                    idx_line++;
                }
            }
            buffer_int[idx_buff]='\0';
            adj_mat[i][j]=atoi_veloce(buffer_int);
            i++;
        }
    }
    grafo_da_inserire.cost = sum_dijkstra();
    grafo_da_inserire.id = n_graph;
    //printf("Il grafo numero %d ha peso %d\n",n_graph,grafo_da_inserire.cost);
    if(n_graph<k_top){
        heap[n_graph] = grafo_da_inserire;
        if (n_graph == k_top - 1) {
            costruisci_max_heap();
        }
    }
    else{
        inserimento_contestuale(grafo_da_inserire);
    }
}

dijkstra_node * dijkstra_nodes_alloc(unsigned int len){
    dijkstra_node * temp;
    temp = malloc(sizeof(dijkstra_node)*len);
    return temp;
}

unsigned int node_mindist(dijkstra_node * vect){
    unsigned int min=UINT_MAX , minidx=0, index;
    for(index=0;index<n_nodes;index++){
        if(vect[index].visited==false && vect[index].dist<=min){
            min=vect[index].dist;
            minidx=index;
        }
    }
    return minidx;
}

unsigned int sum_dijkstra(){
    unsigned int idx,idx_minimum_distance, count=n_nodes,sum=0;
    dijkstra_node * nodes = dijkstra_nodes_alloc(n_nodes);
    nodes[0].dist=0; //la sorgente è 0 e la sua distanza dal nodo 0 è 0
    nodes[0].visited=false;
    for(idx=1;idx<n_nodes;idx++){
        nodes[idx].dist=UINT_MAX;
        nodes[idx].visited=false;
    }
    while(count>0){
        idx_minimum_distance = node_mindist(nodes);
        nodes[idx_minimum_distance].visited = true;
        if(nodes[idx_minimum_distance].dist<UINT_MAX) {
            for (idx = 0; idx < n_nodes; idx++) {
                if (idx != idx_minimum_distance && adj_mat[idx_minimum_distance][idx] != 0 &&
                    (adj_mat[idx_minimum_distance][idx] + nodes[idx_minimum_distance].dist) < nodes[idx].dist) {
                    nodes[idx].dist = (adj_mat[idx_minimum_distance][idx] + nodes[idx_minimum_distance].dist);
                }
            }
        }
        count--;
    }
    for(idx=0;idx<n_nodes;idx++){
        if(nodes[idx].dist==UINT_MAX){
            nodes[idx].dist=0;
        }
        sum+=nodes[idx].dist;
    }
    free(nodes);
    return sum;
}

void inserimento_contestuale(graph node){
    if(heap[0].cost>node.cost || (heap[0].cost==node.cost && heap[0].id>node.id)){
        heap[0]=node;
        max_heapify(0);
    }
}

void swap(graph * n1, graph * n2){
    graph temp = *n1;
    *n1=*n2;
    *n2=temp;
}

void costruisci_max_heap(){
    unsigned int i=k_top/2;
    for(;i>0;i--){
        max_heapify(i);
    }
    if(i==0){
        max_heapify(i);
    }
}

void max_heapify(unsigned int n){
    unsigned int l=2*n+1, r=2*n+2, posmax;
    if((l<k_top && heap[l].cost>heap[n].cost) || (l<k_top && heap[l].cost==heap[n].cost && heap[l].id>heap[n].id )){
        posmax=l;
    }
    else{
        posmax=n;
    }
    if((r<k_top && heap[r].cost>heap[posmax].cost) || (r<k_top && heap[r].cost==heap[posmax].cost && heap[r].id>heap[posmax].id )){
        posmax=r;
    }
    if(posmax!=n){
        swap(&heap[n],&heap[posmax]);
        max_heapify(posmax);
    }
}

void topk(){
    unsigned int i;
    if(n_graph==0){
        printf("\n");
    }
    else if(k_top>n_graph){
        for(i=0;i<n_graph;i++){
            if(i!=n_graph-1){
                printf("%u ",i);
            }
            else{
                printf("%u\n",i);
            }
        }
    }
    else{
        for(i=0;i<k_top;i++){
            if(i!=k_top){
                printf("%u ",heap[i].id);
            }
            else{
                printf("%u\n",heap[i].id);
            }
        }
    }
}
