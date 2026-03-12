#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>

//31 05 2024
// дефайны для правильной адресации карт в материалах
#define COLOR_ADDRES [0][0]
#define NORMAL_ADDRES [0][1]
#define ROUGH_ADDRES [0][2]
#define METALLIC_ADDRES [0][3]
#define SPECULAR_ADDRES [1][0]
#define LIGHTMAP_ADDRES [1][1]
#define RESERVE_1_ADDRES [1][2]
#define RESERVE_2_ADDRES [1][3]

#define COLOR_SCALE [2][0]
#define NORMAL_SCALE [2][1]
#define ROUGH_SCALE [2][2]
#define METALLIC_SCALE [2][3]
#define SPECULAR_SCALE [3][0]
#define LIGHTMAP_SCALE [3][1]
#define RESERVE_1_SCALE [3][2]
#define RESERVE_2_SCALE [3][3]

#define color_ 0
#define normal 0
#define rough 0
#define metal 0
#define spec 0
#define lightmap 0
#define reserv1 0
#define reserv2 0

#define color_sc 1
#define normal_sc 1
#define rough_sc 1
#define metal_sc 1
#define spec_sc 1
#define lightmap_sc 1
#define reserv1_sc 1
#define reserv2_sc 1






class Material	//класс оболочки для хранения материала (массива текстур 2Д)
{
public:
	Material();
	
	Material(std::string materialName);
	
	bool status = false;
	
	std::string resourceName = "none";
	
	
	
	~Material();

	// пользовательские переменные и методы
	
	GLuint& getTexture2DArray();
	glm::mat4 getMatInfo();
	
private:

	GLuint 	texture2DArray;
	GLuint 	texture2DArrayMapsCount = 1;
	glm::ivec2	textureSize = glm::ivec2(128, 128);
	int		textureColorChannels = 4;
	std::vector <std::string>	textureSubMapsNames;

	//в этой матрице строго определены места для адресов текстур и их коэффициентов масштабирования
	glm::mat4 matInfo = glm::mat4(	glm::vec4(color_, 	normal, 		rough, 			metal),
									glm::vec4(spec, 		lightmap, 		reserv1, 		reserv2),
									glm::vec4(color_sc, 	normal_sc, 		rough_sc, 		metal_sc),
									glm::vec4(spec_sc, 	lightmap_sc, 	reserv1_sc, 	reserv2_sc)	);	//матрица для хранения коэффициентов масштабирования текстур и адресов нужных карт в массиве texture2Darray



	bool LoadMaterial(std::string& name);


	bool createTextureArray();

	std::string getMaterialPath(std::string& filename);
	
	std::string getTexturePath(std::string& filename);


	bool initGL();
};


#endif
