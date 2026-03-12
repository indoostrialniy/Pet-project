#pragma once
#ifndef COLLISION_H
#define COLLISION_H

#include "CollisionDescriptor.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <memory>

// экземпляр данного класса является родительским для класса Entity

struct PointTriangleArgumentsPack		// структура-обертка аргументов для функции pointToTriangleRelativeCalculator
{
	// уточняющие параметры
	glm::vec3 point = glm::vec3(0,0,0); 
	glm::vec3 direction = glm::vec3(0,0,0);
	int triangle_index = -1;
	
	// рассчитываемые параметры
	glm::vec3 heightProjectionCoordinates = glm::vec3(0,0,0);
	glm::vec3 rayIntersectionCoordinates = glm::vec3(0,0,0); 
	
	int pointRelativePosition = -1;
	int rayRelativeDirection = -1;
	
	float heightProjectionLength = -1.0f;
	float rayIntersectionLength = -1.0f;
	
	bool heightInside = false;
	bool rayInside = false;
	
	float t_param_Point = 0.0f;
	float t_param_Ray = 0.0f;
};


class Collision
{
public:
	Collision( std::string eSysname, CollisionDescriptor initCollisionDescr, glm::vec3 ePos, glm::quat eRot, glm::vec3& cPos, glm::quat& cRot, glm::vec3& cDir );	
	
	~Collision();
	
	// ссылочные переменные инициализируются в конструкторе в момент создания сущности
	std::string 	entitySysname;											// системного имени сущщности, нужен для отладки
	glm::vec3		entityPosition = glm::vec3(0,0,0);						// локальных (внутрисетовых) координат сущности
	glm::quat		entityRotation = glm::quat(1,0,0,0);					// локальной ориентации сущности
	glm::vec3&		comboPosition;											// дубликат мировых координат комбо
	glm::quat&		comboRotation;											// дубликат мировой ориентации комбо
	glm::vec3&		comboDirection;											// дубликат направления движения комбо
	
//~ private:
	CollisionDescriptor collisionDescriptor;							// структура CollisionDescriptor определяет параметры коллизии сущности
	
public:	
	void 		enableCollision();
	void 		disableCollision();
	bool 		getCollision();
	float 		getCollisionRadius();
	
	glm::vec3 	getLocalPosition();										// метод получения локальной позиции сущности
	glm::quat 	getLocalRotation();
	
	void 		setLocalPosition( glm::vec3 pos );						// метод для установки локальной позиции сущности
	void 		setLocalRotation( glm::quat rot );
	
	glm::vec3 	getWorldPosition();										// метод получения мировой позиции сущности (с учетом родительских comboPosition и comboRotation )
	glm::quat 	getWorldRotation();
	
	void		setWorldPosition( glm::vec3 worldPos );					// метод устанавливает такие локальные entityPosition и entityRotation, что, с учетом comboPosition и comboRotation метод getWorldPosition() станет возвращать worldPos
	void		setWorldRotation( glm::quat worldRot );					// аналогично для ориентации
		
	
	bool bCheckAnotherAABB( std::shared_ptr<Collision>& otherAABB, glm::vec3 origin, glm::vec3 direction, glm::vec3& slideVector, float& slideCoeff);	// общий стартовый метод для обработки коллизии между 2 сущностями 
	
	bool checkIntersectionBetweenSimpleSphereAndComplexAABB( Collision& collisionWithSphere, Collision& collisionWithAABB, int leader, glm::vec3 origin, glm::vec3 direction, glm::vec3& slideVector, float& slideCoeff );


	
	glm::vec3	getPointWorldSpacePosition( glm::vec3 pointOwnedByEntity );		// метод для перевода локальной координаты точки pointOwnedByEntity, принадлежащей сущности, в мировую систему. 
	// Если явно не указать pointOwnedByEntity, то он будет по-умолчанию (0,0,0) и функция вернет мировые координаты САМОЙ СУЩНОСТИ
	// А если указать pointOwnedByEntity, то функция вернет мировые координаты точки, привязанной к сущности. 
	// А это могут быть, например, точка ААВВ-коробки или вообще точка МЕША. Это полезно!
	
	glm::vec3	getPointLocalSpacePosition( glm::vec3 otherPointInWorldSpace );	// метод для перевода мировых координат точки otherPointInWorldSpace в локальное пространство сущности с учетом расположения сета
	
	

	bool bIsPointInsideCircledVolume( glm::vec3& point );				// проверка нахождения точки внутри сложного замкнутого объема, характеризующего ЭТУ оболочку
	
	// 0 1 2 3 - перечисление возможных положений рассматриваемой точки
	// итого есть 4 возможные ситуации расположения точки point относительно треугольника: 
	// 0 - точка перед треугольником и в его границах 					(FRONT_INSIDE)	intersection = false
	// 1 - точка перед треугольником, но вне его границ 				(FRONT_OUTSIDE)	intersection = false
	// 2 - точка за треугольником и в его границах 						(BACK_INSIDE)	intersection = true (чистейшее пересечение)
	// 3 - точка за треугольником, но вне его границ 					(BACK_OUTSIDE)	intersection = false
	// сохраним их в виде перечисления:
	enum situations{ FRONT_INSIDE, FRONT_OUTSIDE, BACK_INSIDE, BACK_OUTSIDE};
	
	int getPointRelativePosition( glm::vec3& point, int triangle_index );

	
	
	
	void pointToTriangleRelativeCalculator( PointTriangleArgumentsPack& arguments );
															//~ const glm::vec3& point, const glm::vec3& direction, int triangle_index, 
														  //~ glm::vec3& heightProjectionCoordinates, float& heightProjectionLength,
														  //~ glm::vec3& rayIntersectionCoordinates,  float& rayIntersectionLength,
														  //~ int& pointRelativePosition, int& rayRelativeDirection	);
	
	float cosa(const glm::vec3& U, const glm::vec3& V); // dot
	
	
	
	

	
};



#endif
