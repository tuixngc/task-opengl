#ifndef MESH_H
#define MESH_H
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "Shader.h"

using namespace std;

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    glm::vec4 BoneIDs;
    glm::vec4 Weights;
};

struct Texture
{
	unsigned int Id;
	string Type;
};

class Mesh {
public:
    // mesh data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
    void Draw(Shader& shader);
private:
    //  render data
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};
#endif // !MESH_H

