#pragma once
#ifndef MOVECONTROLLER_H
#define MOVECONTROLLER_H

#include "MicroWorld.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <functional>
#include "global.h"
#include <memory>

class SoundAsset;


// переименовать partStateController
class MovingPartsController : public MicroWorld
{
/*
	Данный класс является обобщающим описанием для возможных вращающихся (двери, крышки, тумблеры и т.п) и смещающихся (двери, затворные рамы оружия и т.п.) деталей
	Он содержит:
	- векторы в исходном/целевом положениях и кватернионы в закрытом/открытом состоянии (могут уточняться из дочерних классов), а также 
	- временные интервалы для полного смещения или открытия/закрытия (также уточняется из дочерних).
	- имена компонентов-сущностей, для которых надо выполнять смещение или открытие/закрытие.
	- функцию logic() override, переопределяющую аналогичную функцию из родительского класса MicroWorld и хранящую типовую логику для интерактива с компонентами
	 
	Типовой состав:
	1 - основная сущность
	1.1 - сущность, которая привязана к другой
	
	
	Каждую сущность-компонент можно настроить с помощью конфигураторов - экземпляров структуры lidProcConfig, где перечисляются все зависимости
	
*/

#define MovingPartsController_MAIN_COMPONENTS 30


public:
	MovingPartsController();

	void checkmainMovingParts();

	
	struct lidProcConfig 												// структура описывает один конкретный корневой двигающийся/вращающийся элемент 
	{
		std::string name = "none";
		
		glm::vec3	defaultPose = glm::vec3(0,0,0);
		glm::vec3	targetPose = glm::vec3(0,0,0);
		int 		timeForMoving = 1000;
		
		glm::quat	defaultQuat = glm::quat(1,0,0,0);
		glm::quat	targetQuat = glm::quat(1,0,0,0);
		int			timeForRotation = 1000;
		
		bool		defaultState	= false;
		bool		targetState		= true;
		int			timeForChangeState = 1000;
		
		int 		transformationType = 0; 							// см. MAIN_MOVING_PARTS_TYPE
		
		float		powValue = 1;										// по умолчанию степень коэффициента смешивания = 1, что соответствует линейному закону
		
		
		
		int			keyCode = GLFW_KEY_E;								// код назначенной клавиши (GLFW_KEY_A etc)
		bool 		mouse = false;										// флаг переключения внимания на мышь
		bool 		keyPressed = false;									// флаг нажатия назначенной клавиши
		
		bool 		status = false;
		int			ticK = 0;
		
		
		std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> startFunction;							// обертка-указатель на метод класса
		std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> endFunction;								// обертка-указатель на метод класса
		
		std::function<void()> start_callback;
		std::function<void()> end_callback;
		
	};


	lidProcConfig mainMovingPartsMassive [MovingPartsController_MAIN_COMPONENTS] {};		// массив структур данных, описывающих движение компонентов
	
	// перечисление главных родительских двигающихся объектов
	enum MAIN_MOVING_PARTS_ENUMERATION {MMP_ZERO, MMP_FIRST, MMP_SECOND, MMP_THIRD}; // 0 1 2 3
	
	// добавить одноразовые изменения
	enum MAIN_MOVING_PARTS_TYPE { MMP_ROTATABLE, MMP_ROTATABLE_REVERSE, MMP_MOVABLE, MMP_MOVABLE_REVERSE, MMP_ROTATABLE_MOVABLE, MMP_ROTATABLE_MOVABLE_REVERSE, MMP_ACTIVATE, MMP_ACTIVATE_REVERSE }; // MMP_ROTATABLE=0 - вращаемый объект, MMP_ROTATABLE_REVERSE=1 - вращаемый туда и обратно, MMP_MOVABLE=1 - сдвигаемый,  MMP_MOVABLE_REVERSE - сдвигаемый туда и обратно

	
	float treshold = 2.5;										// порог для сравнения расстояния до камеры
	

	std::shared_ptr<SoundAsset> movePartSoundPtr;
		
	
	// biCycle 1.3
	bool transformUniversal(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data);					// универсальный метод для компонентов, изменяемых по 1 параметру: либо сместить, либо повернуть!
	
	
	
	// XXX deprecated!
	static bool rotateAndOffMicroWorldMainComponent(void* ptr, int var);
	static bool rotateAndOffMicroWorldMainComponentAndBack(void* ptr, int var);
	static bool changeMicroWorldVisibility(void* ptr, int var);							// метод для обработки 1 компонента: моментальная активация и удержание до истечения заданного времени
	static bool changeMicroWorldVisibilityAndBack(void* ptr, int var);					// метод для обработки 1 компонента: смещение из положения default в положение target и сразу обратно в default
	
	
	//~ void launchMainMovingPartMovement( const int& MMP_index); // bool* flag_,
	void launchMainMovingPartMovement( 	int MMP_index, std::function<void()> start_callback,	std::function<void()> end_callback );	
	
 /*
 * Функция в каждом цикле проверяет нажатие указанной клавиши для указанного объекта и перенаправляет на соответствующую функцию для движения компонента, основываясь на типе (mainMovingPartsMassive[MMP_index].transformationType)
 * name: MovingPartsController::processMMP
 * @param индекс наживаемой клавиши, ссылочная копия флага для однократного нажатия, ссылочный индекс двигающегося объекта, ссылочный реззультат дополнительного условия
 * @return void
 * 
 */
	void processMMP( const int& MMP_index, const bool& adv_condition );	//const int& GLFW_KEY, bool& flag_, const int& MMP_index, const bool& adv_condition);
	
	
	void logic() override;
	
	
	
	
};

//~ lidProcConfig() {}
		
		//~ // конструктор для смены состояния
		//~ lidProcConfig(std::string namE, bool stat, int type, bool defState, bool targState, int stateTime)
		//~ {
			//~ name = namE;
			
			//~ status = stat;
			
			//~ transformationType = type;
			
			//~ defaultState = defState;
			//~ targetState = targState;
			//~ timeForChangeState = stateTime;
		//~ }
		
		//~ // конструктор конфига для смещения
		//~ lidProcConfig(std::string namE, bool stat, int type, glm::vec3 defPos, glm::vec3 targPos, int offTime, float pow=1.0f )
		//~ {
			//~ name = namE;
			
			//~ status = stat;
			
			//~ transformationType = type;
			
			//~ defaultPose = defPos;
			//~ targetPose = targPos;
			//~ timeForMoving = offTime;
			
			//~ powValue = pow;
		//~ }
		
		//~ // конструктор конфига для вращения
		//~ lidProcConfig(std::string namE, bool stat, int type, glm::quat defQ, glm::quat targQ, int rotTime, float pow=1.0f )
		//~ {
			//~ name = namE;
			
			//~ status = stat;
			
			//~ transformationType = type;
			
			//~ defaultQuat = defQ;
			//~ targetQuat = targQ;
			//~ timeForRotation = rotTime;
			
			//~ powValue = pow;
		//~ }
	
		//~ // конструктор конфига для смещения и вращения
		//~ lidProcConfig(std::string namE, bool stat, int type,  glm::vec3 defPos, glm::vec3 targPos, int offTime, glm::quat defQ, glm::quat targQ, int rotTime, float pow=1.0f)
		//~ {
			//~ name = namE;
			
			//~ status = stat;
			
			//~ transformationType = type;
			
			//~ defaultPose = defPos;
			//~ targetPose = targPos;
			//~ timeForMoving = offTime;
			
			//~ defaultQuat = defQ;
			//~ targetQuat = targQ;
			//~ timeForRotation = rotTime;
			
			//~ powValue = pow;
		//~ }
		

#endif
