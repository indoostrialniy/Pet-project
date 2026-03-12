#pragma once
#ifndef ANIMATION_CONTROLLER_H
#define ANIMATION_CONTROLLER_H

#include <vector>
#include <string>
#include <functional>
#include <memory>

#include "global.h"

class Skeleton;
class SkeletonAnimation;

struct animStateConfig			// с помощью таких структур конфигурируются каналы
{
	animStateConfig() {}
	animStateConfig(float mix, std::vector<int> stencil, std::string name, int repeating, int timeIn) // int timeOut=0
	{
		mixCoeff		= mix;
		mask			= stencil;
		animName 		= name;
		repeatCount 	= repeating;
		blendingTimeIn	= timeIn;
		//blendingTimeOut	= timeOut;
	}
	
	//обязательные для инициализации параметры 
	float mixCoeff				= 0.0;
	std::vector<int> mask		= {0};
	std::string animName 		= "none";
	int repeatCount				= -1;		// отрицательное число означает, что анимация должна отыгрываться бесконечно
	int blendingTimeIn			= 1000;
	int blendingTimeOut			= 1000;
	
	// параметры по-умолчанию
	int timer					= 0;
	int iAnimTime_milsecs		= 0;
	SkeletonAnimation* animPtr 	= nullptr;
	
	
	// можно дополнительно прописать смещения по времени, для точной настройки моментов срабатывания
	std::function<void()> start_clb = {};
	std::function<void()> middle_clb = {};
	std::function<void()> end_clb = {};
};


class AnimationController
{	
/*
	Данный класс служит для управления анимациями конкретной сущности. Является родительским для каждого класса сущности, которые могут быть скелетными или нет. 
	Соответственно в методы класса управления анимациями встроены проверки на наличие скелета и привязанность к нему анимаций
	Должен быть доступ к скелету и списку его анимаций, а также текущей анимации
	
	Задачи:
	назначать анимацию для однократного отыгрыша и возвращения к предыдущей
	назначить анимацию для однократного отыгрыша и перехода к следующей указанной
	
	параметры перехода: прервать отыгрыш текущей или перейти к следующей только после полного отыгрыша текущей анимации
	
	Должен быть механизм плавного перехода между анимациями curr и next
	
	Система должна быть гибкой!
	
	1) Если further не назначен:
	Надо сначала скопировать current в further. Затем за назначенное пользователем время подвести замороженную curr к 0-му кадру next,
	отыграть полностью 1 раз next, а затем за аналогично указанное время подвести обратно к 0-му либо к замороженному кадру curr и продолжить отыгрыш
	curr уже с их собственными временами смешивания.
	
	2) Если further указан явно, ...


	анимации хранятся в стеке, куда заносится, например, анимация с флагом, однократен ли ее отыгрыш
	если да, то после отыгрывания она удаляется из стека (pop)
	а если анимация назначена на постоянку, то не попается
	
	но нужно смешивать с предыдущим (его хранить отдельно!)

*/
	
	#define PLAYER_LEFT_HAND 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24
	#define PLAYER_RIGHT_HAND 25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47
	
	#define HUMAN_LEGS 0,55,56,57,58,59,60,61,62,63,64
	#define HUMAN_LEFT_HANDS 7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30
	#define HUMAN_RIGHT_HAND 31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54
	
	
	
	#define ANIM_CHANNELS_COUNT 2
	
public:

	AnimationController( std::string& name) : entityName(name) {}		// проинициализируем ссылочную пе
	
	std::weak_ptr<glm::mat4[]> 		sklPose;							// слабая ссылка на канал анимации, которая пихается в шейдеры
	
	std::shared_ptr<glm::mat4[]> 	animChannels[ANIM_CHANNELS_COUNT] = {};	// массив из ANIM_CHANNELS_COUNT умных указателей на матрицы поз
	
	stackADV <animStateConfig>		animConfigs[ANIM_CHANNELS_COUNT];	// кастомный стек для хранения конфигов анимаций
	bool							canUpdate[ANIM_CHANNELS_COUNT] {true, true};	// Переменные позволяют временно заблокировать переход к следующему кадру анимации канала на время выполнения функции bringToPose()

	~AnimationController();
	
	std::shared_ptr<Skeleton>		entitySkeletonPtr = nullptr;
	std::string& 					entityName;								// Ссылочный дубликат на строчное имя сущности из дочернего класса Entity. Инициализируется в конструкторе
	
	//~ bool goBack = false;
	
	bool directionOfMixing = false;
	
	bool bSecondExist = false;
	
	//~ int temp_ms = 0;
	
	//~ std::vector <int> temp_mask;

public:	

	
	bool setAnimation(int channelIndex, animStateConfig& newAnim, animStateConfig& secondAnim  );
	bool mixAnim(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas);	//void* ptr, int var);
	void switchAnimConfig(int channelIndex);
	
	
	
	void mixByMask (int& channelIndex, animStateConfig* first, animStateConfig* second, std::vector<int>& mask, float& mix, int& frame, int& frame2);
	
	
	bool lockChannelUpdating(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas);	//void* ptr, int var);


	
	
	
	void updateState(); //данная функция вызывается при каждом кадре для каждого объекта, независимо от того, скелетный он или нет
	
	


	
public:	

	

/*
 * 
 * name: AnimationController::MicroWorldAnimationChannel
 * @param индекс канала, флаг прерывания текущей анимации, коэффициент смешивания
 * @return
 * 
 */
 
	void checkChannel(int ch);
	
	void checkChannels();

	//~ int turningTimer = 0;
	
	glm::mat4 mixPosesForBone(float mixValue, glm::mat4 poseA, glm::mat4 poseB);
	
	
	glm::mat4 mixPosesForChannels(float mixValue, glm::mat4 poseA, std::vector<int>& maskA, glm::mat4 poseB, std::vector<int>& maskB);
	
	
	
};



#endif
