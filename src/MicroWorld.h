#pragma once
#ifndef MicroWorld_H
#define MicroWorld_H

#include <string>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include <memory>

class Entity;
#include <vector>

struct entityInitializationParameters;

class CollisionDescriptor;

#include "global.h"
#include <functional>

struct animStateConfig;

class MicroWorld 
{
public:
	MicroWorld();
	MicroWorld(std::string n );											// конструкторы и деструкторы априори должны быть публичными
	MicroWorld(glm::vec3 worldLoc, glm::quat worldRot, std::string n);
	~MicroWorld();
	
	
	// критически важные для архитектуры методы
	void MicroWorldUpdate();											// обязательная функция, вызывается в каждом цикле из levelUpdate()

protected:
	virtual void update();												// обновление состояний вызывается из функции MicroWorld::MicroWorldUpdate(). Может быть переопределено дочерне
	virtual void logic();												// функция логики для сета вызывается из функции MicroWorld::MicroWorldUpdate(). Может быть переопределено дочерне

	bool initGL();														// вспомогательная функция для инициализации OpenGL

public:
	// переменные
	std::string 		MicroWorldName = "MicroWorld";
	
	std::map < std::string, std::shared_ptr<Entity> > components; 		// словарь указателей на компоненты-сущности
	
	glm::mat3 			m3VectorsForwardLeftUp 		= glm::mat3(1.0f);	// единичная матрица векторов вперед, влево и вверх, характеризующая оси комбо
	bool 				overview = true;								// буль, разрешающий перемещение и обзор класса камеры/игрока
	
	std::string 		mainVisualEntity = "none";						// имя визуального компонента
	std::string 		mainCollisionEntity = "none";					// имя коллизионного компонента
	
	bool 				visible = true;	// доступен ли сет для рендера. У физических сущностей всегда true, а вот у источников света, которые тоже сеты - варьируется
	
	bool 				status = false;	
	
	std::vector <std::string> tags; 					// вектор тегов. Добавление/удаление тегов извне может использоваться для скриптования
	
	glm::vec3 			position = glm::vec3(0, 0, 0);			// координаты сета в мире, к ним часто придется обращаться через метод getWorldLocation()
	glm::quat 			rotation = glm::quat(1, 0, 0, 0); 		// на основании этого кватерниона можно получить матрицу ориентации, к нему часто придется обращаться через метод getWorldRotation()
		
	glm::vec3 			startupPosition = glm::vec3(0, 0, 0);			// координаты сета в мире, к ним часто придется обращаться через метод getWorldLocation()
	glm::quat 			startupRotation = glm::quat(1, 0, 0, 0); 
	
	glm::vec3 			moveDirection = glm::vec3(0,0,-1);		// вектор текущего перемещения комбо
		
	float 				fSlideCoeff = 0.1f;
	glm::vec3 			v3Reflect = glm::vec3(0,0,0.01);
	
	int 				moveInDirectionWatchDog = 0;
	
	glm::vec3 			prevPos = glm::vec3(0,0,0);
	glm::quat 			prevRot = glm::quat(1,0,0,0);
		
	glm::mat3 			m3DefaultVectorsForwardLeftUp 	= glm::mat3(1.0f);	// единичная матрица векторов по умолчанию
	
	std::string 		previosShaderName 			= "default";	// строка хранит имя шейдера, который используется в данный момент
	
	int 				focuseTimer = 0;
	
	
	//~ //сет может быть привязан к чужому компоненту и даже к сокету чужого компонента, поэтому надо хранить ссылки на возможного родителя
	
	MicroWorld* 		parentMicroWorld = nullptr;		// std::shared_ptr <MicroWorld> parentMicroWorld;
	std::string 		parentMicroWorldComponentName = "none";
	std::string 		parentMicroWorldComponentSocketName = "none";
	glm::vec4 			ownCoordXYZ = glm::vec4(0, 0, 0, 0);			//координаты в пространстве сета для привязки к нужному сокету
	glm::vec4 			ownQuatXYZW = glm::vec4(0, 0, 0, 1);
	MicroWorld* 		childMicroWorld = nullptr;	// std::shared_ptr <MicroWorld> childMicroWorld;
	
	
	
	
	
	// методы обращения к переменным
	bool 				getStatus();													// проверить статус
	bool 				getVisibility();												// проверить видимость
	
	glm::vec3 			getPosition();											// получить координаты
	glm::quat 			getRotation();
	
	void 				setPosition(glm::vec3 worldLoc);						// установить координаты
	void 				setRotation(glm::quat worldRot);
		
	void 				addTag(std::string tag); 										// публичный метод, чтобы добавить тег
	bool 				hasTag(std::string tag);										// публичный метод, чтобы проверить наличие тега
	void 				removeTag(std::string tag);
	
	
	// методы взаимодействия с комбо
	bool addEntity( const entityInitializationParameters& newEntity );	// новая версия метода, чтобы добавить сущность в списко компонентов комбо
	
	bool addEntity(std::string initSysname, glm::vec3 initPos, glm::quat initRot, std::string initModelName, CollisionDescriptor initCollisionType,
					std::string initType, std::vector <std::string> initMaterialsNames, std::string lightmapName, std::string initShaderName);		// XXX старая - избавиться
			
	std::shared_ptr<MicroWorld> traceRay( const glm::vec3& traceDirectionWS );			// трассировка по линии, возврат ближайшего пересеченного объекта

	
	bool bCheckIntersections( std::string componentName, glm::vec3 origin, glm::vec3 direction, glm::vec3& slide, float& slideK ); //метод проверки, не пересекаем ли чью-то оболочку в направлении dir
	// метод для проверки, не пересекает ли чья-либо коллизионная оболочка КОЛЛИЗИОННУЮ ОБОЛОЧКУ указанной сущности-компонента, сдвинутую на заданный вектор
		
	void reset();
		
	bool bIsTargetInRadius( std::string targetComboName, float radius);
	
	void moveInDirection( std::string collisionEntityName, glm::vec3 directionVec, float movingSpeed );
		


public:

	void setEntityVisibility( const std::string& entityName, bool visibility);
	bool getEntityVisibility( const std::string& entityName );
	
	glm::vec3 getEntityPosition( const std::string& entityName );

	void setComponentAnimation(	const std::string& entityName, int channelIndex, animStateConfig& newAnim, animStateConfig& secondAnim 	);	

	
	
	void attachMicroWorldToParent();
	
	bool attachTo( MicroWorld* parent, std::string componentName, std::string socketName, glm::vec4 socketCoordXYZ, glm::vec4 socketQuatXYZW );
	
	bool detach();

	bool focuse(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);
	
	void focuseToSpecifiedPoint( const glm::vec3& point, int duration, std::function<void()> start_clb, std::function<void()> end_clb );
	void goAndFocuseToSpecifiedPoint( const glm::vec3& POV, const glm::vec3& pos, int duration, std::function<void()> start_clb, std::function<void()> end_clb );
	
	glm::quat getQuaternionFocusedToPoint( const glm::vec3 point );		// XXX вроде как ненужная пока функция
	
	glm::quat getHorizontalQuaternionFocusedToPoint( const glm::vec3 point );
	glm::quat getVerticalQuaternionFocusedToPoint( const glm::vec3 point );	// XXX вроде как ненужная пока функция

	
};


#endif
