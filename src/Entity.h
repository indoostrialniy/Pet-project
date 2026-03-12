#pragma once
#ifndef ENTITY_H
#define ENTITY_H


struct CollisionDescriptor;//#include "CollisionDescriptor.h"
#include "Collision.h"
#include "AnimationController.h"

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>

class Mesh;
class Shader;
class Material;

struct entityInitializationParameters
{
public:

	std::string initSysname; 								// системное имя, уникальное в пределах актора
	glm::vec3 initPos; 										// локальные координаты сущности
	glm::quat initRot;										// локальное вращение
	std::string initMeshName; 								// текстовое имя модели
	std::string initType;  									// тип сущности, пока не используется
	std::string shaderName = "forward"; 					// имя шейдера
	std::string lightmapName = "none";						// текстовое имя карты света
	CollisionDescriptor collisionType;						// описание типа коллизии, назначаемой этой сущности
	
	std::vector <std::string> initMeshMaterialsVector;		// вектор текстовых имен назначенных материалов
	
};


// этот класс - описание сущности из комбо

class Entity : public Collision, public AnimationController
{
	// чем обладает сущность
	bool		visible			= true;									// по умолчанию сущность видима
public:
	bool 		status 			= false;								// статус сущности, определяется результатом функции configureEntity()	

	// уникализирующие сущность характеристики
	std::string 				entityName 			= "default";		// имя сущности (экземпляра класса), нужно для отладки
	std::string 				entityMeshName 		= "entityMeshName"; // по умолчанию - модель куба
	
	std::vector <std::string>	entityMaterialsNamesVector;				// список названий материалов, назначенных данному объекту - из файла уровня
	std::string					entityShaderName	= "forward";
	
	// хранилище ресурсов сущности
	std::shared_ptr<Mesh>		entityMeshPtr;							// умный указатель на ассет меша. Конфигурируется в configureEntity() и более о нем думать не надо
	std::shared_ptr<Shader>		entityShaderPtr;						// умный указатель на ассет шейдера. Конфигурируется в configureEntity() и более о нем думать не надо
	std::vector < std::shared_ptr<Material> > 	entityMaterials;		// вектор умных указателей на ассеты материалов. Конфигурируется в configureEntity() и более о нем думать не надо



	//std::shared_ptr<Collision> 	entityCollision;						// указатель на экземпляр класса-обработчика коллизий

	//Collision entityCollision;// (entityPosition, entityRotation, comboPosition, comboRotation);											// экземпляр класса с описанием коллизии сущности


	// методы
	Entity(	std::string initSysname, 
			glm::vec3 initPos, glm::quat initRot, 
			glm::vec3& comboPos, glm::quat& comboRot, glm::vec3& comboDir,					// во имя инициализации ссылочных переменных comboPosition и comboRotation
			std::string initModelName, CollisionDescriptor initCollisionDescr, std::string initType, 
			std::vector <std::string> initMaterialsNames, std::string lightmapName, std::string initShaderName);
			
	Entity(	 const entityInitializationParameters& entityParameters, glm::vec3& comboPos, glm::quat& comboRot, glm::vec3& comboDir );			
	// во имя инициализации ссылочных переменных comboPosition и comboRotation
			
			
	~Entity();
	
	bool 		configureEntity();										// метод для конфигурации сущности, загрузки/настройки ресурсов и т.д. Определяет статус
	
	bool 		bRenderEntity();										// метод для отрисовки сущности

	bool 		getStatus();	// функция для неискажающего получения статуса сущности
	
	void		setVisibility( bool visibility );
	bool		getVisibility();
	
	
	
	void 		updateShader();																// вспомогательная функция для перезагрузки шейдера сущности
	glm::vec4 	v4RotatePoint9(glm::vec4 point, int bone_index, int transformMatrixIndex);	// вспомогательная функция для отладки шейдера
	glm::vec4 	v4CalculateBranchRotation11(int bone_index, glm::vec4 rotated_point);		// вспомогательная функция для отладки шейдера

/*
 * 
 * name: Entity::configureModelMatrix
 * @param На входе ссылочные аргументы матрицы модели, мировые координаты сета, которому принадлежит сущность Entity и мировая ориентация этого же сета
 * @return Не возвращает ничего, результирующая матрица записывается в ссылочный аргумент
 * 
 */
	void configureModelMatrix(glm::mat4& matrix, glm::vec3& setWorldLocation, glm::quat setWorldRotation);
		
	
	
	
	

	
	
	
		//~ std::shared_ptr<Texture>	entityLightmapPtr;							// умный указатель на ассет запеченной карты света. Конфигурируется в configureEntity() и более о нем думать не надо

	// XXX вынести в отдельный родительский класс
	//~ std::shared_ptr<Skeleton>		entitySkeletonPtr;
	
//private:
	
	
	
	
	
};

#endif
