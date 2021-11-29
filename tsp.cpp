/**
Gabriel Henrique Linke - 2126630
Thiago de Mendonça Mildemberger - 1716980
**/

#include <iostream>
#include <vector>
#include <string>
#include <cmath>

#include <fstream>

using std::cerr;
using std::cout;
using std::endl;

using std::ifstream;
using std::string;
using std::vector;

struct Vertex
{
    int x;
    int y;
};

vector<Vertex> readVertices(string inputFile)
{
    ifstream fileReader(inputFile);
    if (!fileReader)
    {
        cerr << "Arquivo não pode ser aberto" << endl;
        fflush(stdin);
        getchar();
    }
    vector<Vertex> vertices;
    int size = 0;
    Vertex vertex;
    fileReader >> size;
    vertices.reserve(size);

    while (fileReader >> vertex.x >> vertex.y)
    {
        vertices.push_back(vertex);
    }
    return vertices;
}

double getEuclideanDistance(Vertex &v1, Vertex &v2)
{
    return sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
}

int main(int argc, char *argv[])
{
    if (argc != 2)
        return 1;

    vector<Vertex> vertices = readVertices(argv[1]);
    for (int i = 0; i < vertices.size(); i++)
    {
        cout << vertices[i].x << " " << vertices[i].y << endl;
    }

    return 0;
}
