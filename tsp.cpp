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
struct Vertex
{
    int x;
    int y;
    int parent_index;
    vector<int> children_indices;

    Vertex(int x, int y) : x(x), y(y), parent_index(-1), children_indices() { }
};

double euclideanDistance(Vertex &v1, Vertex &v2)
{
    return sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
}

struct HeapElement
{
    int vertex_index;
    double cost;

    HeapElement(int vertex_index, double cost)
        : vertex_index(vertex_index), cost(cost) { }
};

int left(int index)
{
    return 2*index + 1;
}

int right(int index)
{
    return 2*index + 2;
}

int parent(int index)
{
    return (index-1)/2;
}

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

void build_min_heap(vector<HeapElement> &heap)
{
    for (int i = parent(heap.size()-1); i >= 0; --i)
    {
        min_heapify(heap, i);
    }
}

HeapElement extract_min(vector<HeapElement> &heap)
{
    swap(heap[0], heap[heap.size()-1]);
    HeapElement min = heap.back();
    heap.pop_back();
    min_heapify(heap, 0);
    return min;
}

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

void prim(vector<Vertex> &vertices)
{
    vector<HeapElement> heap;

    double real_max = numeric_limits<double>::max();
    if (numeric_limits<double>::has_infinity)
    {
        real_max = numeric_limits<double>::infinity();
    }

    for (int i = 0; i < vertices.size(); ++i)
    {
        heap.push_back(HeapElement(i, real_max));
    }

    // selecionar primeiro vértice:
    heap[0].cost = 0.0;
    build_min_heap(heap);

    while (heap.size() > 0)
    {
        HeapElement min = extract_min(heap);
        int u_index = min.vertex_index;
        Vertex& u = vertices[u_index];
        // se o vértice retirado tem um pai, nesse passo realizamos a
        // otimização de ligar o pai ao filho
        if (u.parent_index != -1)
        {
            vertices[u.parent_index].children_indices.push_back(u_index);
        }

        for (int i = 0; i < heap.size(); ++i)
        {
            // distância entre o vértice 'u' retirado do heap e o vértice no
            // índice 'i' do heap
            double dist = euclideanDistance(u, vertices[heap[i].vertex_index]);
            if (dist < heap[i].cost)
            {
                vertices[heap[i].vertex_index].parent_index = u_index;
                decrease_key(heap, i, dist);
            }
        }
    }
}

vector<Vertex> readVertices(string inputFile)
{
    ifstream fileReader(inputFile);
    if (!fileReader)
    {
        cerr << "Arquivo não pode ser aberto" << endl;
        fflush(stdin);
        getchar();
        exit(1);
    }
    vector<Vertex> vertices;
    int size = 0;
    int x = 0, y = 0;
    fileReader >> size;
    vertices.reserve(size);

    while (fileReader >> x >> y)
    {
        vertices.push_back(Vertex(x, y));
    }
    return vertices;
}

double calculateCost(vector<Vertex> &vertices)
{
    double cost = 0;
    if (vertices.size() == 0)
    {
        cerr << "Ciclo não contém nenhum vértice" << endl;
        fflush(stdin);
        getchar();
        exit(1);
    }
    if (vertices.size() == 1)
        return 0;

    for (int i = 1; i < vertices.size(); i++)
    {
        cost += euclideanDistance(vertices[i - 1], vertices[i]);
    }
    return cost;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Utilize ./tsp input.txt" << endl;
        return 1;
    }

    time_t start = clock();

    //--------------------------------------------------------------------------------
    // Faz todos os processos aqui
    vector<Vertex> vertices = readVertices(argv[1]);
    // for (int i = 0; i < vertices.size(); i++)
    // {
    //     cout << vertices[i].x << " " << vertices[i].y << endl;
    // }
    // vertices.push_back(vertices[0]);

    prim(vertices);
    // para testar a implementação do alg. de prim
    // começa no índice 1 pq o índice 0 não tem pai
    for (int i = 1; i < vertices.size(); ++i)
    {
        Vertex& v = vertices[i];
        cout << vertices[v.parent_index].x << " " << vertices[v.parent_index].y << endl;
        cout << v.x << " " << v.y << endl << endl;
    }
    //--------------------------------------------------------------------------------

    time_t end = clock();
    double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
    // cout << std::setprecision(6) << fixed << time_taken << " " << calculateCost(vertices) << endl;

    return 0;
}
