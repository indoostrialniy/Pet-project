#pragma once
#ifndef SKELETON_H
#define SKELETON_H

#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <map>
#include <set>
#include <tuple>


class SkeletonAnimation;	//~ #include "SkeletonAnimation.h"

class Skeleton // экземпляр скелета может назначаться некольким сущностям, но у каждой  сущности должна быть своя поза
{
public:
	Skeleton(){};
	Skeleton(std::string skeletonName);
	
	bool status = false;
	std::string resourceName = "none";
	

	bool getStatus();

	// пользовательские переменные и методы

	~Skeleton();
	
	bool sklCheckMatrices();
	
	std::unique_ptr<glm::mat4[]> sklIerarchyMatrices;
	std::unique_ptr<glm::mat4[]> sklInputSpaceMatrices;
	std::unique_ptr<glm::mat4[]> sklOutputSpaceMatrices;
	std::unique_ptr<glm::mat4[]> sklLocalSpaceMatrices;
	std::unique_ptr<glm::mat4[]> sklTransformMatrices;
	
	//~ glm::mat4* 	sklIerarchyMatrices = nullptr;
	//~ glm::mat4* 	sklInputSpaceMatrices = nullptr; // матрицу input возможно можно заменить на вектор или даже на int, сэкономив т.о. около 4 кБ
	//~ glm::mat4* 	sklOutputSpaceMatrices = nullptr;
	//~ glm::mat4* 	sklLocalSpaceMatrices = nullptr;
	//~ glm::mat4* 	sklTransformMatrices = nullptr;
	
	
	std::map <std::string, int> boneIndexesMap;		// для облегчения дальнейшего скриптования обеспечим заполнение словаря имен и индексов костей
	std::map <int, std::string> boneNamesMap;
	std::map<std::string, SkeletonAnimation*> sklAnimations;		//словарь анимаций, назначенных данному скелету

	int 		sklBonesCount = 1;	// раз это скелет, значит минимум 1 кость



	glm::vec4 rotate_point10(glm::vec4 point, int bone_index, int transformMatrixIndex, std::weak_ptr<glm::mat4[]>& pose);//glm::mat4* pose);
	
	glm::vec4 calculateBranchRotation11(std::string bone_name, glm::vec4 rotated_point, std::weak_ptr<glm::mat4[]>& pose);//glm::mat4* pose);
	
	glm::vec4 update_normal_quat(glm::vec4 quaternion, int bone_index, int transformMatrixIndex, std::weak_ptr<glm::mat4[]>& pose);//glm::mat4* pose);
	
	glm::vec4 calculateBranchRotationNormal(std::string bone_name, glm::vec4 quaternion, std::weak_ptr<glm::mat4[]>& pose);//glm::mat4* pose);

private:

	

	std::string sklGetPath(std::string filename);

	std::set < std::tuple<int, int> > unique_transfers;
	
	std::string getValueOrNone(std::string strSource, int num); //возврат слова с порядковым номером num из строки strSource. используется для загрузки скелета
	
	
	// XXX пусто
	void sklAddNewAnimation(std::string animationName);
	
	int getUniqueTransferIndex( std::tuple<int, int> pair );
	
	bool loadSkeleton(std::string skeletonName);
};




#endif
