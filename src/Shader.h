#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


class Shader
{
public:
	Shader(){};
	
	Shader(std::string shaderName);
	
	~Shader();
	
	
	bool status = false;					// статус класса шейдера
	
	std::string resourceName = "none";		// имя ресурса - шейдера
	
	void printResourceData();				// обязательная функция, но можно оставить тело пустым или попытаться скрыть
		
	// пользовательские переменные и методы
	
	void updateShader();
	
	GLuint& getShaderProgram() { return shaderProgramID; }	//возможно здесь надо возвращать ссылочную переменную
	
	GLuint& getMatricesUBO()	{return PVMBlock;}
	GLuint& getSkeletonUBO()	{return skeletonUniformBlock;}
	GLuint& getLightingUBO() 	{return lightingUniformBlock;}
	
private:
	std::string vertexShaderFile = "forward.vs";
	
	std::string fragmentShaderFile = "forward.fs";

	GLuint shaderProgramID;


	GLuint PVMBlock;			//юниформ-блок для матриц PV и M

	GLuint skeletonUniformBlock; //юниформ блок для хранения всей необходимой информации для реализации скелетной анимации
	GLuint lightingUniformBlock; //юниформ блок для хранения всей необходимой информации для источников света


	// адреса юниформов ...
	GLuint PV_matrix_adress;
	GLuint M_matrix_adress;
	GLuint Skeletal_adress;

	GLuint Highlight_adress;
	GLuint SetHighlighting_adress;
	// ...


	void configureUBO();

	void configureUniformAdresses();	// находим и запоминаем адреса юниформов в шейдере


	bool compileNewShader();			// string VertexShader, string FragmentShader)
	
	
	std::string getVShaderPath(std::string filename);
	
	std::string getFShaderPath(std::string filename);
	
	bool initGL();						// вспомогательная функция для инициализации OpenGL
	
};



#endif
