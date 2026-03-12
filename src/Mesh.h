#pragma once
#ifndef MESH_H
#define MESH_H

#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


class Mesh// : public CollisionCalculator //класс оболочки для хранения и обработки геометрической модели (mesh)
{
public:
	// 2 обязтельных метода класса ресурса публичны
	Mesh(){};
	Mesh(std::string meshName);
	
	bool status = false;
	
	std::string resourceName = "none";
	
	
	void printResourceData();			// обязательная функция, но можно оставить тело пустым или попытаться скрыть
	
	// пользовательские переменные и методы

	~Mesh();

	// методы обращения к необходимым приватным данным
	GLuint&		getVAO()					{	return VAO;	}						//возможно здесь надо возвращать ссылочную переменную
	GLuint&		getVerticesCount()			{	return meshVerticesCount;	}
	bool 		isMeshSkeletal()			{ 	return meshSkeletal; }
	std::string getMeshSkeletonName()		{	return meshSkeletonName; }
	
private:
	//очищаемые параметры
	GLuint 	VBO;
	GLuint 	VAO;								// обращаемся к нему из функции render()
	GLuint 	EBO;

	//стековые параметры
	GLuint 	meshFacesCount = 0;
	GLuint	meshVerticesCount = 0;				// обращаемся к нему из функции render()
	int		meshAttributesCount = 9; 			// 9 если без костей, 23 если с костями (которые добавят 7 пар аттрибутов)!
	
	
	//характеристики конкретно типа меша
	bool			meshSkeletal = false;		// по умолчанию меш - не скелетный. Обращаемся сюда из функции Entity::configureEntity()
	std::string 	meshSkeletonName = "none";	// сюда записывается только имя скелета из файла модели. Сам скелет подгружается в момент создания сущности!	Обращаемся сюда из функции Entity::configureEntity()

	bool meshLoad(std::string name);

	bool meshLoadBinary(std::string name);

	//~ bool meshLoadBinary1(std::string name);
	
	std::string meshGetPath(std::string filename);

	std::string meshGetPathBinary(std::string filename);
	
	bool initGL();						// вспомогательная функция для инициализации OpenGL
	
};






#endif
