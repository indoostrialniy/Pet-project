#pragma once
#ifndef SKELETON_ANIMATION_H
#define SKELETON_ANIMATION_H

#include <string>
#include <memory>
#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class SkeletonAnimation
{
/*
	Данный класс описывает все переменные, методы и логику загрузки и обработки анимаций скелета
	В целях уменьшения объема потребляемой памяти хранение происходит следующим образом: создаются каналы для ВСЕХ костей, и уже для каждой кости записывается необходимое количество ключевых кадров.
	Так же присутствует публичный метод для формирования позы, присланной извне. Поза представляет собой массив на максимум 70 матриц 4х4. 
	* 
	* Способ хранения данных о ключевых кадрах может быть изменен, но вместе с публичным методом getPose, и все это инкапсулировано в данном классе
*/

public:
	SkeletonAnimation();
	
	SkeletonAnimation(std::string animationName);
	
	~SkeletonAnimation();
	
/*
 * 
 * name: SkeletonAnimation::getFullPose() в присланный извне массив позы поместит QOH для всех костей по своей анимации для заданного кадра
 * @param время анимации в миллисекундах, указатель на массив для хранения позы
 * @return true если выполнено до конца, иначе false
 * 
 */
	bool getFullPose(int iSAtime, glm::mat4* pose);
	
	
	// функция возвращает true, когда отыграна полностью ((iSAtime/ (duration*1000)) * frames > frames)
	// принимает момент времени, указатель на массив позы и вектор-маску костей, для которых надо вычислить позу
	bool getPose( int iSAtime, std::weak_ptr<glm::mat4[]> pose,  float& mixCoeff, std::vector<int> mask);
	
	bool status = false;				// публичная переменная, к ней обращаемся из функции sklAddNewAnimation() класса Skeleton, когда загружаем новую, чтобы проверить успешность загрузки
	
	std::string resourceName = "none";
	
	int getDuration();
	

	int 	bonesCount 		= -1;		// число костей скелета, читается из файла, обязательно больше 0!	Минимум 1!
	int 	frames 			= -1;		// число ключевых кадров в анимации, читается из файла, обязательно больше 0! Даже больше 1!
	float 	duration 		= -1.0f;	// длительность анимации в секундах, читается из файла, обязательно больше 0! Даже больше 0.01!
	
	
	// map<int, float> - for 1 channel: key number and value
	// map<string, map<int, mat4> > - for 1 bone: channel name and map of 1 channel
	
	std::map<std::string, std::map<int, float>>* data = nullptr;
	
/*
 * 
 * name: SkeletonAnimation::calculateBonePose
 * @param Принимает индекс кости номер ключевого кадра, позу для которого надо вычислить. keyFrameIndex из миллисекунд переводится в индекс кадра в начале функции getPose()
 * @return Возвращает матрицу QOH для кости с индексом boneIndex
 * 
 */
	glm::mat4 calculateBonePose(int boneIndex, int keyFrameIndex);
	
	void defineNearestKeyframes(int& SAboneIndex, std::string SAchannelName, int& SAkey, int& SAprev, int& SAnext, float& SAcoeff);
	
	bool loadAnimation(std::string animationName);
	
	
	std::string animGetPath(std::string& filename);
};




#endif
