/*
 * Mesh.h
 */
#pragma once
#ifndef VIEWER_MESH_H
#define VIEWER_MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <utility>
#include <vector>
#include "Shader.h"

typedef unsigned int uint;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

struct Texture {
    uint id;
    std::string type;
    std::string path;
};

class Mesh{
public:
    std::vector<Vertex> vertices;
    std::vector<uint> indices;
    std::vector<Texture> textures;
    // -------------------------------------
    Mesh(std::vector<Vertex> &ver, std::vector<uint> &ind, std::vector<Texture> &text):
        vertices(ver),
        indices(ind),
        textures(text)
    {}

    void Draw(Shader &shader){
        SetUp();
        uint diffuseNr = 1;
        uint specularNr = 1;
        uint normalNr   = 1;
        uint heightNr   = 1;
        for(uint i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // 在绑定之前激活相应的纹理单元
            // 获取纹理序号（diffuse_textureN 中的 N）
            std::string number;
            std::string name = textures[i].type;
            if(name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if(name == "texture_specular")
                number = std::to_string(specularNr++);
            else if(name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to stream
            else if(name == "texture_height")
                number = std::to_string(heightNr++);

            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), int(i));
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // 绘制网格

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size() , GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
    }

private:
    uint VAO, VBO, EBO;
    // --------------------------------------
    void SetUp(){
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);

        // 顶点位置
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)nullptr);
        glEnableVertexAttribArray(0);
        // 顶点法线
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);
        // 顶点纹理坐标
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        glEnableVertexAttribArray(3);
        // vertex bitangent
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        glEnableVertexAttribArray(4);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

    }

public:
    ~Mesh(){
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1,&EBO);
        glDeleteBuffers(1, &VBO);
    }
};

#endif //VIEWER_MESH_H
