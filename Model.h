/*
 * Model.h
 */
#pragma once
#ifndef VIEWER_MODEL_H
#define VIEWER_MODEL_H
#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Shader.h"
#include "Mesh.h"
#include "stb_image.h"
#include <codecvt>
#include <locale>
typedef unsigned int uint;

uint TextureFromFile(const char *path, const std::string &directory, bool gamma = false);
static std::string gb2312_to_utf8(std::string const &strGb2312);

class Model{
public:
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;
    bool gammaCorrection;

    // ---------------------------------------------
    explicit Model(std::string const &path, bool gamma = false):gammaCorrection(gamma)
    {
        loadModel(path);
    }

    void Draw(Shader &shader){
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:
    // --------------------------------
    void loadModel(std::string const &path){
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
            std::cout<< "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }
        std::cout<<"success load model"<<std::endl;
        directory = path.substr(0,path.find_last_of('/'));
        processNode(scene->mRootNode, scene);

    }

    void processNode(aiNode *node, const aiScene *scene){
        for(uint i=0; i<node->mNumMeshes;i++){
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.push_back(processMesh(mesh,scene));
        }
        for(uint i=0; i< node->mNumChildren; i++){
            processNode(node->mChildren[i],scene);
        }
    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene){
        std::vector<Vertex> vertices;
        std::vector<uint> indices;
        std::vector<Texture> textures;
        for(uint i=0; i< mesh->mNumVertices; i++){
            glm::vec3 Vec3;
            Vertex vertex{};
            // 顶点数据
            Vec3.x = mesh->mVertices[i].x;
            Vec3.y = mesh->mVertices[i].y;
            Vec3.z = mesh->mVertices[i].z;
            vertex.Position = Vec3;
            // 法向量
            if(mesh->HasNormals()){
                Vec3.x = mesh->mNormals[i].x;
                Vec3.y = mesh->mNormals[i].y;
                Vec3.z = mesh->mNormals[i].z;
                vertex.Normal = Vec3;
            }
            if(mesh->mTextureCoords[0]){
                glm::vec2 Vec2;
                Vec2.x = mesh->mTextureCoords[0][i].x;
                Vec2.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = Vec2;
                // tangent
                Vec3.x = mesh->mTangents[i].x;
                Vec3.y = mesh->mTangents[i].y;
                Vec3.z = mesh->mTangents[i].z;
                vertex.Tangent = Vec3;
                // bitangent
                Vec3.x = mesh->mBitangents[i].x;
                Vec3.y = mesh->mBitangents[i].y;
                Vec3.z = mesh->mBitangents[i].z;
                vertex.Bitangent = Vec3;

            }
            else{
                vertex.TexCoords = glm::vec2(0.0f,0.0f);
                vertex.Tangent = glm::vec3(0.0f,0.0f,0.0f);
                vertex.Bitangent = glm::vec3(0.0f,0.0f,0.0f);
            }
            vertices.push_back(vertex);
        }

        for(uint i=0; i < mesh->mNumFaces; i++){
            aiFace face = mesh->mFaces[i];
            for(uint j = 0; j < face.mNumIndices; j++){
                indices.push_back(face.mIndices[j]);
            }
        }
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material,aiTextureType_DIFFUSE,"texture_diffuse");
        textures.insert(textures.end(),diffuseMaps.begin(),diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMaterialTextures(material,aiTextureType_SPECULAR,"texture_specular");
        textures.insert(textures.end(),specularMaps.begin(),specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        return Mesh(vertices, indices, textures);
    }

    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                         const std::string& typeName){
        std::vector<Texture> text;
        for(uint i=0; i< mat->GetTextureCount(type); i++){
            aiString str;
            mat->GetTexture(type,i,&str);
            bool skip = false;
            for(uint j=0; j < textures_loaded.size(); j++){
                if(std::strcmp(textures_loaded[j].path.data(),str.C_Str())==0){
                    text.push_back(textures_loaded[j]);
                    skip= true;
                    break;
                }
            }
            if(!skip){
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(),this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                text.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        return text;
    }

};


uint TextureFromFile(const char *path, const std::string &directory, bool gamma){
    std::string filename = std::string(path);
    if(std::string::npos != filename.find('\\')){
        std::string firstname = filename.substr(0, filename.find_last_of('\\'));
        std::string lastname = filename.substr(filename.find_last_of('\\')+1);
        filename = directory + '/' + firstname + '/' + lastname;
    }
    else{
        filename = directory + '/' + filename;
    }
    uint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLint format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 4)
            format = GL_RGBA;
        else
            format = GL_RGB;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
#endif //VIEWER_MODEL_H