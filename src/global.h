#pragma once
#ifndef GLOBAL_H
#define GLOBAL_H

#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <biCycle_13.h>

class Level;

//
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

// структура данных для секвенсора
struct fhNovFlowData
{
	fhNovFlowData( bool* bReportPtr = nullptr, int value = 0 ) : report(bReportPtr), var(value) {}
	
	bool* 	report 	= nullptr; 	//указатель на булев флаг завершенности действия (для отчета, не обязателен)
	int 	var 	= 0;		// вспомогательная переменная
		
	glm::vec3 currentPoint = glm::vec3(0,0,0);
	glm::quat currentRot = glm::quat(1,0,0,0);
	
	glm::vec3 focusedPoint = glm::vec3(0,0,0);
	glm::quat focusedRot = glm::quat(1,0,0,0);
	
	// glm::quat focusedRotH = glm::quat(1,0,0,0);
	// glm::quat focusedRotV = glm::quat(1,0,0,0);

	
	int focuseDuration = 1000;
};

#define BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE fhNovFlowData
//



extern std::shared_ptr<Level> level;	//Level* level;

extern GLFWwindow* engineWindow;

extern int tick;


#include <resourceManager.h>
#include <stack_adv.h>
class Mesh;
class Shader;
class Skeleton;
class Material;
class SoundAsset;
class CollisionShell;

extern resManAsset <Mesh> 			Meshes;
extern resManAsset <Shader> 		Shaders;
extern resManAsset <Skeleton>		Skeletons;
extern resManAsset <Material> 		Materials;
extern resManAsset <SoundAsset>		SoundAssets;
extern resManAsset <CollisionShell> CollisionShells;

extern biCycleSequencer<BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE> Flow;

extern glm::mat4 lights[20];

extern glm::mat3* CameraAxisPtr;
extern glm::vec3* CameraPositionPtr;
extern glm::vec3* CameraPrevPosPtr;
extern glm::quat* CameraQuaternion;



#endif
