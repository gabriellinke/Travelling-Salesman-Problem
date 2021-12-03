/**
Gabriel Henrique Linke - 2126630
Thiago de Mendonça Mildemberger - 1716980
**/

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <fstream>

using namespace std;


/**
 * O vértice é a estrutura principal do programa, que carrega dentro de si as
 * suas coordenadas, além dos índices dos vértices filhos, índice do vértice
 * pai, e a cor (utilizada na busca em profundidade).
 */
struct Vertex
{
    int x;
    int y;
    int parent_index;
    vector<int> children_indices;
    int color;

    Vertex(int x, int y) : x(x), y(y), parent_index(-1), children_indices(), color(0) {}
};

/**
 * Função auxiliar para calcular a distância euclideana entre dois vértices.
 * Recebe dois vértices por referência e retorna um double com a distância.
 */
double euclidean_distance(Vertex &v1, Vertex &v2)
{
    return sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
}

/**
 * Estrutura auxiliar utilizada no heap mínimo, que usa o campo 'cost' como
 * chave.
 */
struct HeapElement
{
    int vertex_index;
    double cost;

    HeapElement(int vertex_index, double cost)
        : vertex_index(vertex_index), cost(cost) {}
};


/**
 * Função auxiliar que lê os vértices do arquivo especificado como argumento
 * para o programa.
 */
vector<Vertex> read_vertices(string input_file)
{
    ifstream file_reader(input_file);
    if (!file_reader)
    {
        cerr << "Arquivo não pode ser aberto" << endl;
        fflush(stdin);
        getchar();
        exit(1);
    }
    vector<Vertex> vertices;
    int size = 0;
    int x = 0, y = 0;
    file_reader >> size;
    vertices.reserve(size);

    while (file_reader >> x >> y)
    {
        vertices.push_back(Vertex(x, y));
    }
    return vertices;
}



/**Nesta seção do programa estão as funções auxiliares que permitem o
 * funcionamento do heap mínimo implementado pela equipe, que é utilizado no
 * algoritmo de Prim.
 */

/**
 * Funções auxiliares para calcular o índice do pai, do filho da esquerda e do
 * filho da direita no heap, dado um índice qualquer
 */
int left(int index)
{
    return 2 * index + 1;
}

int right(int index)
{
    return 2 * index + 2;
}

int parent(int index)
{
    return (index - 1) / 2;
}
/*****************************************************************************/


/**
 * A função min_heapify recebe o heap e o índice de um elemento. Ela supõe que
 * o "sub-heap" filho à esquerda e o "sub-heap" filho à direita do elemento
 * estão em um estado válido, mas o elemento de índice 'index' pode ter ficado
 * maior que algum dos filhos. Se esse for o caso, ele é trocado com o filho
 * menor e a função é chamada recursivamente com o índice que foi trocado.
 * Ao final, se a função foi chamada com as premissas verdadeiras, o heap todo
 * estará em um estado válido.
 */
void min_heapify(vector<HeapElement> &heap, int index)
{
    int l = left(index);
    int r = right(index);
    int smallest = index;
    if (l < heap.size() && heap[l].cost < heap[smallest].cost)
    {
        smallest = l;
    }
    if (r < heap.size() && heap[r].cost < heap[smallest].cost)
    {
        smallest = r;
    }
    if (smallest != index)
    {
        swap(heap[index], heap[smallest]);
        min_heapify(heap, smallest);
    }
}

/**
 * Essa função organiza o arranjo recebido para que ao final da execução seja
 * um heap mínimo válido. Para isto, a função min_heapify é chamada para todos
 * os elementos que possuem algum filho, começando pelos nós mais distantes da
 * raiz. Dessa forma, a premissa necessária para o funcionamento da min_heapify
 * é sempre atendida.
 */
void build_min_heap(vector<HeapElement> &heap)
{
    for (int i = parent(heap.size() - 1); i >= 0; --i)
    {
        min_heapify(heap, i);
    }
}

/**
 * A função extract_min retorna o elemento de menor chave no heap, que é o
 * armazemado na raiz. Esse elemento é retirado da estrutura, e o heap é
 * reorganizado, colocando o último elemento do arranjo no lugar da raiz e
 * chamando a função min_heapify na raiz.
 */
HeapElement extract_min(vector<HeapElement> &heap)
{
    swap(heap[0], heap[heap.size() - 1]);
    HeapElement min = heap.back();
    heap.pop_back();
    min_heapify(heap, 0);
    return min;
}

/**
 * Essa função diminui a chave do elemento do heap especificado. Não funciona
 * caso se tente aumentar a chave do elemento. Ao ter a sua chave diminuída, o
 * elemento pode acabar menor que o seu pai, e se esse for o caso, a função
 * realiza a troca entre esses nós, até que se encontre a raiz ou até que o
 * elemento acabe em uma posição tal que o seu nó pai tenha uma chave menor que
 * a sua.
 */
void decrease_key(vector<HeapElement> &heap, int index, double key)
{
    if (key > heap[index].cost)
    {
        throw "chamada a 'decrease_key' com uma chave maior que a original";
    }
    heap[index].cost = key;
    while (index > 0 && heap[parent(index)].cost > heap[index].cost)
    {
        swap(heap[parent(index)], heap[index]);
        index = parent(index);
    }
}
/*****************************************************************************/


/**
 * Uma das funções principais do programa, recebe como entrada o vetor de
 * vértices lidos do arquivo, e calcula a AGM do grafo completo cujas arestas
 * tem peso dado pela distância entre os dois vértices ligados pela aresta.
 * Para economizar muita memória, o peso de cada aresta não é armazenado em uma
 * estrutura de dados, mas sim calculado sob demanda (se aproveitando do fato
 * de que todos os vértices estão ligados a todos os outros).
 */
void prim(vector<Vertex> &vertices)
{
    // Cada vértice é unicamente identificado pela sua posição no vetor
    // original 'vertices', e essa posição não é alterada ao longo do programa.
    //
    // É criado um outro vetor, que em breve será organizado para funcionar
    // como um heap mínimo, e nesse vetor sim é que os vértices podem ser
    // movidos e removidos, sendo identificados pelo índice no vetor original.
    vector<HeapElement> heap;

    // Todos os vértices que não são o inicial começam com uma chave possuindo
    // o maior valor 'double' possível, normalmente o "infinito" positivo.
    double real_max = numeric_limits<double>::max();
    if (numeric_limits<double>::has_infinity)
    {
        real_max = numeric_limits<double>::infinity();
    }

    for (int i = 0; i < vertices.size(); ++i)
    {
        heap.push_back(HeapElement(i, real_max));
    }

    // Podemos selecionar qualquer vértice como o inicial para o algoritmo. Nós
    // escolhemos o primeiro vértice do vetor, que recebe o custo 0.0, e
    // portanto será o primeiro a ser retirado do heap.
    heap[0].cost = 0.0;
    build_min_heap(heap);

    while (heap.size() > 0)
    {
        // A chave de cada vértice no heap representa o custo da menor aresta
        // do corte atual (definido pelos vértices já retidados do heap) que
        // liga o vértice em questão a um dos vértices retirados.
        //
        // Assim, extrair o vértice de menor chave significa escolher a aresta
        // mais leve para ser adicionada à AGM em construção. O vértice
        // escolhido tem a informação de qual vértice vem essa aresta mais
        // leve, pois quando esse menor custo foi calculado, o campo
        // 'parent_index' desse vértice recebeu o índice do vértice no qual a 
        // aresta se origina.
        HeapElement min = extract_min(heap);
        int u_index = min.vertex_index;
        Vertex &u = vertices[u_index];

        // se o vértice retirado tem um pai (que é o caso para todos que não
        // são o vértice inicial), nesse passo realizamos a
        // otimização de ligar o pai ao filho (será útil para a DFS)
        if (u.parent_index != -1)
        {
            vertices[u.parent_index].children_indices.push_back(u_index);
        }

        // Todos os vértices restantes no heap são verificados, já que o grafo
        // em questão é completo e todos os vértices se conectam a todos os
        // outros.
        for (int i = 0; i < heap.size(); ++i)
        {
            double dist = euclidean_distance(u, vertices[heap[i].vertex_index]);
            if (dist < heap[i].cost)
            {
                // (Para facilitar a escrita, o vértice chamado no programa de
                // "vertices[heap[i].vertex_index]" vai ser nomado de 'v')
                //
                // Como a aresta (u, v) tem um custo menor do que as outras
                // antes encontradas até 'v', o novo pai de 'v' é setado como
                // sendo 'u', e a chave de 'v' no heap é atualizada.
                vertices[heap[i].vertex_index].parent_index = u_index;
                decrease_key(heap, i, dist);
            }
        }
    }
}

/**
 * Função auxiliar para escrever as arestas da AGM para o arquivo especificado
 */
void write_edges_to_file(vector<Vertex> &vertices)
{
    // As arestas são escritas no arquivo 'tree.txt' no formato especificado.

    ofstream edges_file("tree.txt");

    // (Chamando de 'n' o número de vértices)
    //
    // Cada vértice a partir do segundo tem exatamente um pai. Ao percorrer
    // todos os vértices a partir do segundo imprimindo a aresta do vértice
    // para o pai, todas as (n - 1) arestas da AGM são impressas.
    for (int i = 1; i < vertices.size(); ++i)
    {
        Vertex& v = vertices[i];
        edges_file << vertices[v.parent_index].x << " "
                   << vertices[v.parent_index].y << endl;
        edges_file << v.x << " " << v.y << endl;
    }
}

/**
 * Parte recursiva da busca em profundidade (explicação abaixo)
 */
void depth_first_search_visit(vector<Vertex> &vertices, vector<Vertex> &computed_cycle, int index)
{
    // Vértice é marcado como visitado e é adicionado ao ciclo
    vertices[index].color = 1;
    computed_cycle.push_back(vertices[index]);

    // Para cada vértice filho do vértice analisado (arestas)
    // é verificado se já foi visitado, se não foi faz nova chamada à dfs
    for (int i = 0; i < vertices[index].children_indices.size(); ++i)
    {
        int children_index = vertices[index].children_indices[i];
        if (vertices[children_index].color == 0)
            depth_first_search_visit(vertices, computed_cycle, children_index);
    }

    // O vértice atual é marcado como finalizado
    vertices[index].color = 2;
}

/**
 * Outra função principal, recebe o vetor com os vértices já tendo as relações
 * de pai/filho já calculadas pelo algoritmo de Prim, e realiza nessa AGM a
 * busca em profundidade, adicionando cada vértice ao vetor 'computed_cycle' no
 * momento em que é visitado pelo algoritmo de busca (na parte recursiva acima)
 */
vector<Vertex> depth_first_search(vector<Vertex> &vertices)
{
    vector<Vertex> computed_cycle;
    computed_cycle.reserve(vertices.size() + 1);

    for (int i = 0; i < vertices.size(); ++i)
        if (vertices[i].color == 0)
            depth_first_search_visit(vertices, computed_cycle, i);

    // Repete o último vértice para fechar o ciclo
    if (computed_cycle.size() > 0)
        computed_cycle.push_back(computed_cycle[0]);

    return computed_cycle;
}

/**
 * Função auxiliar que escreve os vértices do ciclo para o arquivo especificado
 */
void write_cycle_to_file(vector<Vertex> &cycle_vertices)
{
    // Os vértices são escritos no arquivo 'cycle.txt' no formato especificado.

    ofstream cycle_file("cycle.txt");

    // As coordenadas de cada vértice do ciclo são escritas, na ordem em que
    // cada vértice aparece no ciclo.
    for (int i = 0; i < cycle_vertices.size(); ++i)
    {
        cycle_file << cycle_vertices[i].x << " " << cycle_vertices[i].y << endl;
    }
}

/**
 * Função que calcula o custo total acumulado ao percorrer o ciclo calculado,
 * necessário para a saída do programa.
 */
double calculate_cycle_cost(vector<Vertex> &cycle_vertices)
{
    double cost = 0;
    if (cycle_vertices.size() == 0)
    {
        cerr << "Ciclo não contém nenhum vértice" << endl;
        fflush(stdin);
        getchar();
        exit(1);
    }
    if (cycle_vertices.size() == 1)
        return 0.0;

    for (int i = 1; i < cycle_vertices.size(); ++i)
    {
        cost += euclidean_distance(cycle_vertices[i - 1], cycle_vertices[i]);
    }
    return cost;
}


/**
 * DESCRIÇÃO GERAL:
 *
 * O programa começa com a leitura dos vértices do arquivo para o vetor
 * 'vertices'. O tipo de dados "Vertex" utilizado armazena não só as
 * coordenadas lidas do arquivo, mas também os índices dos vértices pai e
 * filhos e também a cor utilizada na busca em profundidade para marcar nós
 * visitados e finalizados. Inicialmente ao índice do pai é atribuído -1, para
 * indicar que não há, e o vetor com os índices dos filhos começa vazio.
 *
 * Cada vértice é identificado unicamente pela posição em que foi originalmente
 * colocado no vetor 'vertices', posição essa que não é mudada ao longo do
 * programa.
 *
 * Para calcular a AGM no algoritmo de Prim, é utilizado um vector<HeapElement>
 * que é gerenciado como um heap mínimo utilizando as funções auxiliares
 * definidas.
 * Os HeapElement's que compõem esse vetor são structs que possuem um campo
 * para o índice do vértice a que o HeapElement se refere, e um campo para a
 * chave desse elemento no heap, que representa o custo da aresta mais leve
 * que liga o vértice referido a um dos vértices já retirados do heap.
 *
 * Conforme o algoritmo é executado, e os HeapElement's são retirados e movidos
 * dentro do heap, a ordem dos vértices no vetor 'vertices' não é alterada, mas
 * a AGM vai sendo construída por meio de alterações nas variáveis
 * 'parent_index' e 'children_indices' dos vértices do vetor 'vertices'.
 *
 * Após a AGM ter sido calculada, a busca em profundidade é executada nela,
 * utilizando o campo 'children_indices' para percorrer a árvore, e os vértices
 * vão sendo copiados para um vetor chamado 'cycle_vertices' na ordem em que
 * são visitados na busca, que será a mesma ordem em que aparecem no ciclo.
 *
 * Ao final da busca, o ciclo foi definido. É mostrado o tempo gasto no
 * processo todo, juntamente com o custo total do ciclo, e então o programa é
 * encerrado.
 */
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Utilize ./tsp input.txt" << endl;
        return 1;
    }

    time_t start = clock();

    // 1o passo: ler os vértices
    vector<Vertex> vertices = read_vertices(argv[1]);

    // 2o passo: calcular a AGM
    prim(vertices);
    write_edges_to_file(vertices);

    // 3o passo: calcular o ciclo usando a DFS
    vector<Vertex> cycle_vertices = depth_first_search(vertices);
    write_cycle_to_file(cycle_vertices);

    time_t end = clock();
    double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
    cout << std::setprecision(6) << fixed << time_taken << " "
         << calculate_cycle_cost(cycle_vertices) << endl;

    return 0;
}
