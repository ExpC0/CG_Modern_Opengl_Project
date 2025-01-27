#ifndef cylinder_h
#define cylinder_h

#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "shader.h"

using namespace std;
using namespace glm;

#define PI 3.1416

class Cylinder
{
public:
	Cylinder(float radius = 1.0f, int sectorCount = 36, int stackCount = 18, glm::vec3 amb = glm::vec3(0.922, 0.91, 0.902), glm::vec3 diff = glm::vec3(0.922, 0.91, 0.902), glm::vec3 spec = glm::vec3(0.1f, 0.1f, 0.1f), float shiny = 8.0f) 
	{
		this->radius = radius;
		this->height = height;

		this->stackCount = stackCount;
		this->sectorCount = sectorCount;

		this->texture0 = texture0;
		this->texture1 = texture1;

		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->shininess = shininess;

		this->generateCoordinates_Normals_TextureCoordinates();
		this->generateVertices();
		this->generateIndices();

		glGenVertexArrays(1, &materialVAO);
		glBindVertexArray(materialVAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, this->getVertexArraySize(), this->getVertexArray(), GL_STATIC_DRAW);

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->getIndexArraySize(), this->getIndexArray(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, this->getStride(), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, this->getStride(), (void*)(3 * sizeof(float)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, this->getStride(), (void*)(6 * sizeof(float)));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	~Cylinder()
	{
		glDeleteVertexArrays(1, &materialVAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
	void drawWithColor()
	{

	}
	void drawWithMaterialProperties(Shader& shader, mat4 model)
	{
		shader.use();

		shader.setVec3("material.ambient", this->ambient);
		shader.setVec3("material.diffuse", this->diffuse);
		shader.setVec3("material.specular", this->specular);
		shader.setFloat("material.shininess", this->shininess);

		shader.setMat4("model", model);

		glBindVertexArray(materialVAO);
		glDrawElements(GL_TRIANGLES, this->getIndexArrayCount(), GL_UNSIGNED_INT, (void*)0);

	}
	void drawWithTexture(Shader& shader, mat4 model)
	{
		shader.use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->texture0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, this->texture1);

		shader.setInt("material.diffuseMap", 0);
		shader.setInt("material.specularMap", 1);

		shader.setMat4("model", model);

		glBindVertexArray(materialVAO);
		glDrawElements(GL_TRIANGLES, this->getIndexArrayCount(), GL_UNSIGNED_INT, (void*)0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
private:
	unsigned int getStride()
	{
		return 8 * sizeof(float);
	}
	unsigned int getVertexArraySize()
	{
		return vertices.size() * sizeof(float);
	}
	float* getVertexArray()
	{
		return vertices.data();
	}
	unsigned int getIndexArraySize()
	{
		return indices.size() * sizeof(unsigned int);
	}
	unsigned int getIndexArrayCount()
	{
		return indices.size();
	}
	unsigned int* getIndexArray()
	{
		return indices.data();
	}
	void generateCoordinates_Normals_TextureCoordinates()
	{
		for (int stackStep = 0; stackStep <= this->stackCount; ++stackStep)
		{
			for (int sectorStep = 0; sectorStep <= this->sectorCount; ++sectorStep)
			{
				float theta = 2.0f * PI * sectorStep / this->sectorCount;

				float x = this->radius * cosf(theta);
				float y = this->height / 2.0f - this->height * stackStep / this->stackCount;
				float z = this->radius * sinf(theta);

				coordinates.push_back(x);
				coordinates.push_back(y);
				coordinates.push_back(z);

				if (stackStep == 0)
				{
					topCircle.push_back(x);
					topCircle.push_back(y);
					topCircle.push_back(z);

					topCircle.push_back(0.0f);
					topCircle.push_back(1.0f);
					topCircle.push_back(0.0f);

					topCircle.push_back(cosf(theta) / 2.0f + 0.5f);
					topCircle.push_back(sinf(theta) / 2.0f + 0.5f);
				} 
				else if (stackStep == this->stackCount)
				{
					bottomCircle.push_back(x);
					bottomCircle.push_back(y);
					bottomCircle.push_back(z);

					bottomCircle.push_back(0.0f);
					bottomCircle.push_back(-1.0f);
					bottomCircle.push_back(0.0f);

					bottomCircle.push_back(cosf(theta) / 2.0f + 0.5f);
					bottomCircle.push_back(sinf(theta) / 2.0f + 0.5f);
				}

				float nx = x / this->radius;
				float ny = 0.0f;
				float nz = z / this->radius;

				normals.push_back(nx);
				normals.push_back(ny);
				normals.push_back(nz);

				float s = (float)sectorStep / sectorCount;
				float t = 1.0f - (float)stackStep / stackCount;

				textureCoordinates.push_back(s);
				textureCoordinates.push_back(t);
			}
		}
	}

	void generateVertices()
	{
		int count = (this->coordinates).size();

		for (int i = 0, j = 0; i < count; i += 3, j += 2)
		{
			vertices.push_back(coordinates[i]);
			vertices.push_back(coordinates[i + 1]);
			vertices.push_back(coordinates[i + 2]);

			vertices.push_back(normals[i]);
			vertices.push_back(normals[i + 1]);
			vertices.push_back(normals[i + 2]);

			vertices.push_back(textureCoordinates[j]);
			vertices.push_back(textureCoordinates[j + 1]);
		}

		topCircleCenter = (this->vertices).size() / 8;
		topCircleIndexStart = topCircleCenter + 1;

		vertices.push_back(0.0f);
		vertices.push_back(this->height / 2.0f);
		vertices.push_back(0.0f);

		vertices.push_back(0.0f);
		vertices.push_back(1.0f);
		vertices.push_back(0.0f);

		vertices.push_back(0.5f);
		vertices.push_back(0.5f);

		count = (this->topCircle).size();

		for (int i = 0; i < count; ++i)
		{
			vertices.push_back(topCircle[i]);
		}

		bottomCircleCenter = (this->vertices).size() / 8;
		bottomCircleIndexStart = bottomCircleCenter + 1;

		vertices.push_back(0.0f);
		vertices.push_back(-this->height / 2.0f);
		vertices.push_back(0.0f);

		vertices.push_back(0.0f);
		vertices.push_back(-1.0f);
		vertices.push_back(0.0f);

		vertices.push_back(0.5f);
		vertices.push_back(0.5f);

		count = (this->bottomCircle).size();

		for (int i = 0; i < count; ++i)
		{
			vertices.push_back(bottomCircle[i]);
		}
	}

	void generateIndices()
	{
		int k1, k2;
		for (int stackStep = 0; stackStep < this->stackCount; ++stackStep)
		{
			k1 = stackStep * (sectorCount + 1);
			k2 = k1 + (sectorCount + 1);
			for (int sectorStep = 0; sectorStep < this->sectorCount; ++sectorStep, ++k1, ++k2)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);

				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}


		for (int sectorStep = 0; sectorStep < this->sectorCount; ++sectorStep)
		{
			indices.push_back(topCircleCenter);
			indices.push_back(topCircleIndexStart + sectorStep);
			indices.push_back(topCircleIndexStart + sectorStep + 1);
		}

		for (int sectorStep = 0; sectorStep < this->sectorCount; ++sectorStep)
		{
			indices.push_back(bottomCircleCenter);
			indices.push_back(bottomCircleIndexStart + sectorStep);
			indices.push_back(bottomCircleIndexStart + sectorStep + 1);
		}
	}

	float radius;
	float height;

	int stackCount;
	int sectorCount;

	unsigned int texture0, texture1;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;

	unsigned int colorVAO, materialVAO, textureVAO;
	unsigned int VBO;
	unsigned int EBO;

	vector<float> coordinates;
	vector<float> normals;
	vector<float> textureCoordinates;

	vector<float> topCircle;
	vector<float> bottomCircle;

	int topCircleCenter;
	int bottomCircleCenter;

	int topCircleIndexStart;
	int bottomCircleIndexStart;

	vector<float> vertices;

	vector<unsigned int> indices;
};

#endif

