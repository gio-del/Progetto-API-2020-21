#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct {
    unsigned int cost;
    unsigned int id;
}graph;

typedef enum{false=0,true=1
}bool;

typedef struct {
    unsigned int dist;
    bool visited;
}dijkstra_node;

unsigned int n_nodes;
unsigned int k_top;
unsigned int ** adj_mat;
unsigned int n_graph=0;
graph * heap;

void input_parse();
void create_matrix();
void add_graph();
void topK();
dijkstra_node * dijkstra_nodes_alloc(unsigned int len);
unsigned int sum_dijkstra();
unsigned int node_min_dist(dijkstra_node * vector);
void heap_allocate();
void max_heapify(unsigned int n);
void swap(graph * n1, graph * n2);
void create_max_heap();
void insert_on_top(graph node);
unsigned int fast_atoi(char * str );


int main(){
    input_parse();
    return 0;
}

void input_parse(){
    char buffer[20];
    char * token = NULL;

    if(fgets(buffer,sizeof(buffer),stdin)) {
        token = strtok(buffer, " ");
    }
    n_nodes=fast_atoi(token);
    token=strtok(NULL," ");
    k_top=fast_atoi(token);
    create_matrix();
    heap_allocate();

    while(fgets(buffer,sizeof(buffer),stdin)!=NULL){
        if(buffer[0]=='A'){
            add_graph();
            n_graph++;
        }
        else if(buffer[0]=='T'){
            topK();
        }
    }
}

void create_matrix(){
    unsigned int i;
    adj_mat=(unsigned int **)malloc(sizeof(unsigned int *)*n_nodes);
    for(i=0;i<n_nodes;i++){
        *(adj_mat+i)=(unsigned int *)malloc(sizeof(unsigned int)*n_nodes);
    }
}

void heap_allocate(){
    heap=(graph *)malloc(sizeof(graph)*k_top);
}

unsigned int fast_atoi(char * str )
{
    unsigned int n = 0;
    while( *str!='\0' && *str!='\n' && *str!='\r') {
        n = n*10 + *str - '0';
        str++;
    }
    return n;
}

void add_graph(){
    unsigned int i=0,j;
    int idx_line,idx_buff;
    char line_buffer[4196];
    char buffer_int[15];
    graph graph_to_add;
    while(i<n_nodes){
        j=0;
        idx_line=0;
        idx_buff=0;
        if(fgets(line_buffer,sizeof(line_buffer),stdin)){
            while(line_buffer[idx_line]!='\n'){
                if(line_buffer[idx_line]==','){
                    buffer_int[idx_buff]='\0';
                    adj_mat[i][j]=fast_atoi(buffer_int);
                    j++;
                    idx_buff=0;
                    idx_line++;
                }
                else{
                    buffer_int[idx_buff]=line_buffer[idx_line];
                    idx_buff++;
                    idx_line++;
                }
            }
            buffer_int[idx_buff]='\0';
            adj_mat[i][j]=fast_atoi(buffer_int);
            i++;
        }
    }
    graph_to_add.cost = sum_dijkstra();
    graph_to_add.id = n_graph;
    if(n_graph<k_top){
        heap[n_graph] = graph_to_add;
        if (n_graph == k_top - 1) {
            create_max_heap();
        }
    }
    else{
        insert_on_top(graph_to_add);
    }
}

dijkstra_node * dijkstra_nodes_alloc(unsigned int len){
    dijkstra_node * temp;
    temp = malloc(sizeof(dijkstra_node)*len);
    return temp;
}

unsigned int node_min_dist(dijkstra_node * vector){
    unsigned int min=UINT_MAX , min_idx=0, index;
    for(index=0;index<n_nodes;index++){
        if(vector[index].visited==false && vector[index].dist<=min){
            min=vector[index].dist;
            min_idx=index;
        }
    }
    return min_idx;
}

unsigned int sum_dijkstra(){
    unsigned int idx,idx_minimum_distance, count=n_nodes,sum=0;
    dijkstra_node * nodes = dijkstra_nodes_alloc(n_nodes);
    nodes[0].dist=0;
    nodes[0].visited=false;
    for(idx=1;idx<n_nodes;idx++){
        nodes[idx].dist=UINT_MAX;
        nodes[idx].visited=false;
    }
    while(count>0){
        idx_minimum_distance = node_min_dist(nodes);
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

void insert_on_top(graph node){
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

void create_max_heap(){
    unsigned int i=k_top/2;
    for(;i>0;i--){
        max_heapify(i);
    }
    if(i==0){
        max_heapify(i);
    }
}

void max_heapify(unsigned int n){
    unsigned int l=2*n+1, r=2*n+2, pos_max;
    if((l<k_top && heap[l].cost>heap[n].cost) || (l<k_top && heap[l].cost==heap[n].cost && heap[l].id>heap[n].id )){
        pos_max=l;
    }
    else{
        pos_max=n;
    }
    if((r<k_top && heap[r].cost>heap[pos_max].cost) || (r<k_top && heap[r].cost==heap[pos_max].cost && heap[r].id>heap[pos_max].id )){
        pos_max=r;
    }
    if(pos_max!=n){
        swap(&heap[n],&heap[pos_max]);
        max_heapify(pos_max);
    }
}

void topK(){
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
