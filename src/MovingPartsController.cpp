#include "MovingPartsController.h"

#include "macros.h"
#include "Entity.h"

MovingPartsController::MovingPartsController()
{
	
}

void MovingPartsController::checkmainMovingParts()
{
	printMessage("\n\nCheck massives of lid components:")
	for(int i=0; i<MovingPartsController_MAIN_COMPONENTS; i++)
	{
		printVarComment("\nComponent index: ", i)
		printVariable(mainMovingPartsMassive[i].name)
		printVariable(mainMovingPartsMassive[i].status)
	}
}



// XXX совмещенные MMP_ROTATABLE_MOVABLE и MMP_ROTATABLE_MOVABLE_REVERSE надо обрабатывать немного по-другому, с учетом, возможно, разного времени на изменение

bool MovingPartsController::transformUniversal(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)					
{		
	if( components.count( mainMovingPartsMassive[data.var].name ) == 0 ) //    CHECK_PTR( door->components[door->entityNameForRotation] ) )
	{
		#ifdef MOVINGPARTS_DEBUG
			printVarComment("No entity assigned! Break! MovingPartsController::transformUniversal(), var: ", data.var)
			DEBUG_INFO
		#endif
		return true; 
	}
	
	float x, y;
	
	#define TYPE mainMovingPartsMassive[data.var].transformationType
	
	// 1. Как только зафиксировали превышение, жестко привяжемся к крайнему значению
	if(TYPE == MMP_ROTATABLE || TYPE == MMP_ROTATABLE_REVERSE)
	{
		if( mainMovingPartsMassive[data.var].ticK > mainMovingPartsMassive[data.var].timeForRotation)	// 
		{
			mainMovingPartsMassive[data.var].ticK = mainMovingPartsMassive[data.var].timeForRotation;
		}
		x =  (float)mainMovingPartsMassive[data.var].ticK/(float)mainMovingPartsMassive[data.var].timeForRotation ;	// абсцисса: 0 ... 1
	}
	
	if(TYPE == MMP_MOVABLE || TYPE == MMP_MOVABLE_REVERSE)
	{
		if( mainMovingPartsMassive[data.var].ticK > mainMovingPartsMassive[data.var].timeForMoving)	// 
		{
			mainMovingPartsMassive[data.var].ticK = mainMovingPartsMassive[data.var].timeForMoving;
		}
		x =  (float)mainMovingPartsMassive[data.var].ticK/(float)mainMovingPartsMassive[data.var].timeForMoving ;	// абсцисса: 0 ... 1
	}
	
	
	
	
	
	
	
	
	
	// 2. Обработка поворотов/смещений...
	if(TYPE == MMP_ROTATABLE || TYPE == MMP_MOVABLE)					// || TYPE == MMP_ROTATABLE_MOVABLE)							// для невозвратных
	{	y = mainMovingPartsMassive[data.var].status ? 1-x : x;		}	// линейный закон смешивания
	
	if(TYPE == MMP_ROTATABLE_REVERSE || TYPE == MMP_MOVABLE_REVERSE)	// || TYPE == MMP_ROTATABLE_MOVABLE_REVERSE)	// для возвратных
	{	y = 1-(x-0.5f)*(x-0.5f)/0.25f;		}							// параболический закон изменения коэффициента смешивания
	
	
	if(TYPE == MMP_ROTATABLE || TYPE == MMP_ROTATABLE_REVERSE)			// || TYPE == MMP_ROTATABLE_MOVABLE || TYPE == MMP_ROTATABLE_MOVABLE_REVERSE)
	{
		components[ mainMovingPartsMassive[data.var].name ]->setLocalRotation( glm::slerp( mainMovingPartsMassive[data.var].defaultQuat, mainMovingPartsMassive[data.var].targetQuat, y ) );	//применяем смешивание
	}
	
	if(TYPE == MMP_MOVABLE || TYPE == MMP_MOVABLE_REVERSE)				// || TYPE == MMP_ROTATABLE_MOVABLE || TYPE == MMP_ROTATABLE_MOVABLE_REVERSE)
	{
		components[ mainMovingPartsMassive[data.var].name ]->setLocalPosition( glm::mix( mainMovingPartsMassive[data.var].defaultPose, mainMovingPartsMassive[data.var].targetPose, y ) );
	}
	
	// 3. Условие выполнится только после выхода за пределы и жесткой привязки п.1
	if( mainMovingPartsMassive[data.var].ticK == mainMovingPartsMassive[data.var].timeForRotation || mainMovingPartsMassive[data.var].ticK == mainMovingPartsMassive[data.var].timeForMoving)
	{
		if(TYPE == MMP_ROTATABLE || TYPE == MMP_MOVABLE || TYPE == MMP_ROTATABLE_MOVABLE)
		{
			mainMovingPartsMassive[data.var].status = !mainMovingPartsMassive[data.var].status;
		}
		
		mainMovingPartsMassive[data.var].ticK = 0;
		return true;
	}
	
	mainMovingPartsMassive[data.var].ticK += tick;
	
	
	return false;
	
	
	
}




/*
* 
* name: MovingPartsController::launchMainMovingPartMovement()
* @param адрес булева флага и ссылочный индекс двигающегося компонента
* @return void
* 
*/
void MovingPartsController::launchMainMovingPartMovement( 	int MMP_index,
															std::function<void()> start_callback = [](){},
															std::function<void()> end_callback = [](){} )	
															
//~ void MovingPartsController::launchMainMovingPartMovement( 	int MMP_index,
															//~ std::function<void()> start_callback = nullptr,	//[](){},	// mainMovingPartsMassive[MMP_index].start_callback,	// ,
															//~ std::function<void()> end_callback = nullptr )	//= [](){}
{
	// колбеки прописываются в еще конструкторе и данная функция по умолчанию должна брать их.
	//Но если они указаны явно, то - брать новые
	
	//~ printVarComment("launchMainMovingPartMovement ", MicroWorldName)
	
	//~ auto p = start_callback.target_type();
	//~ printVariable(p)
	
	//~ auto c = start_callback.target_type().name();
	//~ printVariable( c )
	
	//~ if(!start_callback)	{ printMessage("void default") }
	//~ else {printMessage("custom") }
	// функция launchMainMovingPartMovement вызывается однократно по конпке (см. processMMP) и должна запустить в секвенсор последовательность, которая изменит состояние компонента
	// и надо явно прописать в эту секвенсорную последовательность колбеки из структуры lidProcConfig
	
	BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;							//&completeFlag, auxVar);
	datas.var = MMP_index;
	
	std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function;	// главная изменяющая функция. Обязана принимать кастомный тип аргумента и возвращать буль
	//~ std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> start = mainMovingPartsMassive[MMP_index].startFunction;;		// колбек начала
	//~ std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> end = mainMovingPartsMassive[MMP_index].endFunction;		// колбек конца
		
	//~ auto start_callback = mainMovingPartsMassive[MMP_index].start_callback;
	//~ auto end_callback 	= mainMovingPartsMassive[MMP_index].end_callback;
		
	//~ if( mainMovingPartsMassive[MMP_index].transformationType == MMP_ROTATABLE || mainMovingPartsMassive[MMP_index].transformationType == MMP_ROTATABLE_REVERSE)
	//~ {
		function 	= std::bind( &MovingPartsController::transformUniversal, this, datas);	// главная вращающая функция
	//~ }
	
	//~ if( mainMovingPartsMassive[MMP_index].transformationType == MMP_MOVABLE || mainMovingPartsMassive[MMP_index].transformationType == MMP_MOVABLE_REVERSE)
	//~ {
		//~ function 	= std::bind( &MovingPartsController::rotateUniversal, this, datas);	// главная вращающая функция
	//~ }
	
	//~ if( mainMovingPartsMassive[MMP_index].transformationType == MMP_ROTATABLE_REVERSE)
	//~ {
		//~ function 	= std::bind( &MovingPartsController::rotateMainComponentAndBack, this, datas);	// главная вращающая функция, в т.ч. сразу обратно
	//~ }
	
	
	
	
	//~ Flow.CallSequence(MPS);
	//~ Flow.CallSequence( { std::move(function), std::move(start), std::move(end), std::move(datas) } );
	Flow.CallSequence( { std::move(function), std::move(start_callback), std::move(end_callback), std::move(datas) } );

	
}

/*
* Функция при нажатии указанной клавиши для указанного объекта перенаправляет на соответствующую функцию для движения компонента, основываясь на типе (mainMovingPartsMassive[MMP_index].moveOrRotate)
* name: MovingPartsController::processMMP
* @param индекс наживаемой клавиши, ссылочная копия флага для однократного нажатия, ссылочный индекс двигающегося объекта, ссылочный реззультат дополнительного условия
* @return void
* 
*/
//~ void MovingPartsController::processMMP(const int& GLFW_KEY, bool& flag_, const int& MMP_index, const bool& adv_condition)
void MovingPartsController::processMMP(const int& MMP_index, const bool& adv_condition)
{
	if( mainMovingPartsMassive[MMP_index].name == "none" )				// XXX придумать более быструю проверку конфигурации двигающегося компонента
	{
		return;
	}
		
	// если указано, что мышь - проверим мышь, иначе (по умолчанию) - проверим клавиатуру
	bool inputSource = mainMovingPartsMassive[MMP_index].mouse ? glfwGetMouseButton(engineWindow, mainMovingPartsMassive[MMP_index].keyCode) : glfwGetKey( engineWindow, mainMovingPartsMassive[MMP_index].keyCode );
	
	if( inputSource && !mainMovingPartsMassive[MMP_index].keyPressed && adv_condition )
	{				
		mainMovingPartsMassive[MMP_index].keyPressed = true;			// защитимся от повторного запуска
		
		
		//многоразовая функция
		launchMainMovingPartMovement( MMP_index, mainMovingPartsMassive[MMP_index].start_callback, mainMovingPartsMassive[MMP_index].end_callback);	//запустим двигающийся компонент
		
		//launchMainMovingPartMovement(nullptr, MMP_index);	//запустим двигающийся компонент

		
		//~ mainMovingPartsMassive[MMP_index].waitingForEnd = true;			// разрешаем ждать конца движения/трансформации
		
		//~ if(mainMovingPartsMassive[MMP_index].startFunction) 		
		//~ {	mainMovingPartsMassive[MMP_index].startFunction();	}
		
	}	
	
	// 														по выполнении секвенсора keyPressed вернется в false
	//~ if( mainMovingPartsMassive[MMP_index].waitingForEnd && !mainMovingPartsMassive[MMP_index].keyPressed )
	//~ {
		//~ if(mainMovingPartsMassive[MMP_index].endFunction) mainMovingPartsMassive[MMP_index].endFunction();
		
		//~ mainMovingPartsMassive[MMP_index].waitingForEnd = false;		// все выполнили, ничего не ждем до следующего запуска
	//~ }	
	if( !glfwGetKey( engineWindow, mainMovingPartsMassive[MMP_index].keyCode ) )		{	mainMovingPartsMassive[MMP_index].keyPressed = false;	}
}


void MovingPartsController::logic()
{
	// функция logic(), если не перезаписывается в дочерних классах, вызывается каждый кадр и просто циклом запускает обработку всех двигающихся компонентов, если игрок (камера) в радиусе treshold
	// Этот функционал легко скопировать и замещать/модифицировать условие в дочерних классах
	
	// printMessage("MovingPartsController logic")
		
	// лямбда функция: если указатель на коордиинаты камеры валиден, то смотреть разницу расстояния, иначе - тру
	auto chk = [&]()	{ 	if( CHECK_PTR(CameraPositionPtr) ) 		{	float length = glm::length(*CameraPositionPtr - position);		return  length < treshold ? true : false;		} 
							else 			{	printMessage("Camera ptr is invalid")	DEBUG_INFO		return true;	} 		};
	
	// циклом запустим функцию обработки всех двигающихся компонентов ( MovingPartsController_MAIN_COMPONENTS шт )
	//printVarComment("\n\nCombo moving part name: ", MicroWorldName )
	for(int i=0; i<MovingPartsController_MAIN_COMPONENTS; i++)
	{
		//std::cout << "Part " << i << ", name: " << mainMovingPartsMassive[i].name << std::endl;
		processMMP( i, chk() );	
	}
	
	
	
	
	
	// вызов обработки нажатия указанной клавишы (GLFW_KEY_E), что повлечет запуск последовательности для указанного объекта (MMP_ZERO)

	//обработаем нулевой компонент
	//~ processMMP( MMP_ZERO, chk() );	// processMMP(mainMovingPartsMassive[MMP_ZERO].keyCode, mainMovingPartsMassive[MMP_ZERO].keyPressed, MMP_ZERO, chk() );					//processMMP(GLFW_KEY_E, flag_E, MMP_ZERO, chk() );
	
	//~ processMMP( MMP_FIRST, chk() );
	
	//~ processMMP( MMP_SECOND, chk() );
	
	//~ processMMP( MMP_THIRD, chk() );
	
	//~ processMMP(GLFW_KEY_R, flag_2, MMP_FIRST, chk() );
	
	//~ processMMP(GLFW_KEY_T, flag_3, MMP_SECOND, chk() );
	
	//~ processMMP(GLFW_KEY_Y, flag_4, MMP_THIRD, chk() );
	
}



// XXX deprecated!
bool MovingPartsController::changeMicroWorldVisibility(void* ptr, int var)							// метод для обработки 1 компонента: моментальная активация и удержание до истечения заданного времени
{
	IF_DYNAMIC_CAST( MovingPartsController*, lid, ((MovingPartsController*)ptr ))		// принудительно воспринимаем void* ptr как указатель на Door и динамически кастуем 			
	{
		
		//~ if( lid->components.count( lid->mainMovingPartsMassive[var].name ) == 0 ) //    CHECK_PTR( door->components[door->entityNameForRotation] ) )
		//~ {
			//~ printVarComment("No entity for activating assigned! Break! MovingPartsController::activateMainComponent(), var: ", var)
			//~ return true; 
		//~ }
		
		//static int t = 0;
		lid->mainMovingPartsMassive[var].ticK += tick;
		
		float mix = (float)lid->mainMovingPartsMassive[var].ticK/(float)lid->mainMovingPartsMassive[var].timeForChangeState;	// определим текущий коэффициет времени
					
		if(mix>1.01f)
		{
			lid->mainMovingPartsMassive[var].ticK=0;		
			lid->mainMovingPartsMassive[var].status = !lid->mainMovingPartsMassive[var].status;	//инвертируем статус компонента
			return true;
		}
		else
		{
			// вот тут вся разница между направлениями
			if(!lid->mainMovingPartsMassive[var].status) // если закрыто, то открыть
			{
				lid->visible = true;
			}
			else // иначе-наоборот-закрыть
			{
				lid->visible = false;
			}
			return false;
		}
	}
	else
	{
		printVarComment("Error! Invalid pointer, detach sequence! MovingPartsController::activateMainComponent() var: ", var)
		return true; //неудача, неправильный указатель, отвязываем
	}
}

bool MovingPartsController::changeMicroWorldVisibilityAndBack(void* ptr, int var)					// метод для обработки 1 компонента: смещение из положения default в положение target и сразу обратно в default
{
	//здесь описывается последовательность анимации 1 компонента для смещения ТУДА и ОБРАТНО за заданное время, поэтому mix удваивается, а при смещении обратно из mix вычитается единица,
	// чтобы нормализовать коэффициент смешивания
	IF_DYNAMIC_CAST( MovingPartsController*, lid, ((MovingPartsController*)ptr ))		// принудительно воспринимаем void* ptr как указатель на Door и динамически кастуем 			
	{
		
		//~ if( lid->components.count( lid->mainMovingPartsMassive[var].name ) == 0 ) //    CHECK_PTR( door->components[door->entityNameForRotation] ) )
		//~ {
			//~ printVarComment("No entity for moving assigned! Break! LidProcessor::offMicroWorldMainComponent(), var: ", var)
			//~ return true; 
		//~ }
		
		//static int t = 0;
		lid->mainMovingPartsMassive[var].ticK += tick;
		
		float mix = (float)lid->mainMovingPartsMassive[var].ticK/ (float)lid->mainMovingPartsMassive[var].timeForChangeState;	// определим текущий коэффициет поворота
		//~ printVariable(mix)
				
		lid->mainMovingPartsMassive[var].status = mix > 1 ? true : false;
				
					
		if(mix>2.01f)
		{
			lid->mainMovingPartsMassive[var].ticK=0;		
			return true;
		}
		else
		{
			// вот тут вся разница между направлениями
			if(!lid->mainMovingPartsMassive[var].status) // если закрыто, то открыть
			{
				lid->visible = true;
				//lid->components[ lid->mainMovingPartsMassive[var].name ]->entityPosition = glm::mix( lid->mainMovingPartsMassive[var].defaultPose, lid->mainMovingPartsMassive[var].targetPose, mix );
			}
			else // иначе-наоборот-закрыть
			{
				lid->visible = false;
				//lid->components[ lid->mainMovingPartsMassive[var].name ]->entityPosition = glm::mix( lid->mainMovingPartsMassive[var].targetPose, lid->mainMovingPartsMassive[var].defaultPose , mix-0.99f );	
			}
			return false;
		}
	}
	else
	{
		printVarComment("Error! Invalid pointer, detach sequence! MovingPartsController::offMicroWorldMainComponent() var: ", var)
		return true; //неудача, неправильный указатель, отвязываем
	}
}

bool MovingPartsController::rotateAndOffMicroWorldMainComponent(void* ptr, int var)
{
	//здесь описывается последовательность анимации 1 компонента
	IF_DYNAMIC_CAST( MovingPartsController*, lid, ((MovingPartsController*)ptr ))		// принудительно воспринимаем void* ptr как указатель на Door и динамически кастуем 			
	{
		
		if( lid->components.count( lid->mainMovingPartsMassive[var].name ) == 0 ) //    CHECK_PTR( door->components[door->entityNameForRotation] ) )
		{
			printVarComment("No entity for rotation assigned! Break! LidProcessor::rotateAndOffMicroWorldMainComponent(), var: ", var)
			return true; 
		}
		
		//static int t = 0;
		lid->mainMovingPartsMassive[var].ticK += tick;
		
		float mix = 0.5;
		if( lid->mainMovingPartsMassive[var].powValue != 1.0)
		{
			mix = pow( (float)lid->mainMovingPartsMassive[var].ticK/(float)lid->mainMovingPartsMassive[var].timeForRotation, lid->mainMovingPartsMassive[var].powValue);	// определим текущий коэффициет поворота. Возведение в степень позволяет добавить кривизны
		}
		else
		{
			mix =  (float)lid->mainMovingPartsMassive[var].ticK/(float)lid->mainMovingPartsMassive[var].timeForRotation ;	// определим текущий коэффициет поворота
		}
					
		if(mix>1.01f)
		{
			lid->mainMovingPartsMassive[var].ticK=0;		
			lid->mainMovingPartsMassive[var].status = !lid->mainMovingPartsMassive[var].status;	//инвертируем статус компонента
			return true;
		}
		else
		{
			// вот тут вся разница между направлениями
			if(!lid->mainMovingPartsMassive[var].status) // если закрыто, то открыть
			{
				lid->components[ lid->mainMovingPartsMassive[var].name ]->setLocalRotation( glm::slerp( lid->mainMovingPartsMassive[var].defaultQuat, lid->mainMovingPartsMassive[var].targetQuat, mix ) );
				lid->components[ lid->mainMovingPartsMassive[var].name ]->setLocalPosition( glm::mix( lid->mainMovingPartsMassive[var].defaultPose, lid->mainMovingPartsMassive[var].targetPose, mix ) );
			}
			else // иначе-наоборот-закрыть
			{
				lid->components[ lid->mainMovingPartsMassive[var].name ]->setLocalRotation( glm::slerp( lid->mainMovingPartsMassive[var].targetQuat, lid->mainMovingPartsMassive[var].defaultQuat , mix ) );
				lid->components[ lid->mainMovingPartsMassive[var].name ]->setLocalPosition( glm::mix( lid->mainMovingPartsMassive[var].targetPose, lid->mainMovingPartsMassive[var].defaultPose , mix ) );		
			}
			return false;
		}
	}
	else
	{
		printVarComment("Error! Invalid pointer, detach sequence! MovingPartsController::rotateAndOffMicroWorldMainComponent() var: ", var)
		return true; //неудача, неправильный указатель, отвязываем
	}
}

bool MovingPartsController::rotateAndOffMicroWorldMainComponentAndBack(void* ptr, int var)
{
	IF_DYNAMIC_CAST( MovingPartsController*, lid, ((MovingPartsController*)ptr ))		// принудительно воспринимаем void* ptr как указатель на Door и динамически кастуем 
	{
		if( lid->components.count( lid->mainMovingPartsMassive[var].name ) == 0 ) //    CHECK_PTR( door->components[door->entityNameForRotation] ) )
		{
			printVarComment("No entity for moving assigned! Break! LidProcessor::rotateAndOffMicroWorldMainComponentAndBack(), var: ", var)
			return true; 
		}
		
		lid->mainMovingPartsMassive[var].ticK += tick;
		
		float mix = 0.5;
		if( lid->mainMovingPartsMassive[var].powValue != 1.0)
		{
			mix = pow( (float)lid->mainMovingPartsMassive[var].ticK/(float)lid->mainMovingPartsMassive[var].timeForRotation, lid->mainMovingPartsMassive[var].powValue);	// определим текущий коэффициет поворота. Возведение в степень позволяет добавить кривизны
		}
		else
		{
			mix =  (float)lid->mainMovingPartsMassive[var].ticK/(float)lid->mainMovingPartsMassive[var].timeForRotation ;	// определим текущий коэффициет поворота
		}
							
		lid->mainMovingPartsMassive[var].status = mix > 1 ? true : false;
				
					
		if(mix>2.01f)
		{
			lid->mainMovingPartsMassive[var].ticK=0;		
			return true;
		}
		else
		{
			// вот тут вся разница между направлениями
			if(!lid->mainMovingPartsMassive[var].status) // если закрыто, то открыть
			{
				lid->components[ lid->mainMovingPartsMassive[var].name ]->setLocalRotation( glm::slerp( lid->mainMovingPartsMassive[var].defaultQuat, lid->mainMovingPartsMassive[var].targetQuat, mix ) );
				lid->components[ lid->mainMovingPartsMassive[var].name ]->setLocalPosition( glm::mix( lid->mainMovingPartsMassive[var].defaultPose, lid->mainMovingPartsMassive[var].targetPose, mix ) );

			}
			else // иначе-наоборот-закрыть
			{
				lid->components[ lid->mainMovingPartsMassive[var].name ]->setLocalRotation( glm::slerp( lid->mainMovingPartsMassive[var].targetQuat, lid->mainMovingPartsMassive[var].defaultQuat , mix-0.99f) );	
				lid->components[ lid->mainMovingPartsMassive[var].name ]->setLocalPosition( glm::mix( lid->mainMovingPartsMassive[var].targetPose, lid->mainMovingPartsMassive[var].defaultPose , mix-0.99f ) );	

			}
			return false;
		}
	}
	else
	{
		printMessage("Error! Invalid pointer, detach sequence! MovingPartsController::rotateAndOffMicroWorldMainComponentAndBack()")
		return true; //неудача, неправильный указатель, отвязываем
	}
}



//~ bool MovingPartsController::rotateMainComponent(void* ptr, int var)							// метод для обработки 1 компонента: вращение из ориентации default в ориентацию target 
//~ {
	//~ //здесь описывается последовательность анимации 1 компонента
	//~ IF_DYNAMIC_CAST( MovingPartsController*, lid, ((MovingPartsController*)ptr ))		// принудительно воспринимаем void* ptr как указатель на Door и динамически кастуем 			
	//~ {
		
		//~ if( lid->components.count( lid->mainMovingPartsMassive[var].name ) == 0 ) //    CHECK_PTR( door->components[door->entityNameForRotation] ) )
		//~ {
			//~ printVarComment("No entity for rotation assigned! Break! LidProcessor::rotateMainComponent(), var: ", var)
			//~ return true; 
		//~ }
		
		//~ lid->mainMovingPartsMassive[var].ticK += tick;
		
		//~ float mix = 0.5;
		//~ if( lid->mainMovingPartsMassive[var].powValue != 1.0)
		//~ {
			//~ mix = pow( (float)lid->mainMovingPartsMassive[var].ticK/(float)lid->mainMovingPartsMassive[var].timeForRotation, lid->mainMovingPartsMassive[var].powValue);	// определим текущий коэффициет поворота. Возведение в степень позволяет добавить кривизны
		//~ }
		//~ else
		//~ {
			//~ mix =  (float)lid->mainMovingPartsMassive[var].ticK/(float)lid->mainMovingPartsMassive[var].timeForRotation ;	// определим текущий коэффициет поворота
		//~ }
		
					
		//~ if(mix>1.01f)
		//~ {
			//~ lid->mainMovingPartsMassive[var].ticK=0;		
			//~ lid->mainMovingPartsMassive[var].status = !lid->mainMovingPartsMassive[var].status;	//инвертируем статус компонента
			//~ return true;
		//~ }
		//~ else
		//~ {
			//~ // вот тут вся разница между направлениями
			//~ if(!lid->mainMovingPartsMassive[var].status) // если закрыто, то открыть
			//~ {
				//~ lid->components[ lid->mainMovingPartsMassive[var].name ]->setLocalRotation( glm::slerp( lid->mainMovingPartsMassive[var].defaultQuat, lid->mainMovingPartsMassive[var].targetQuat, mix ) );
			//~ }
			//~ else // иначе-наоборот-закрыть
			//~ {
				//~ lid->components[ lid->mainMovingPartsMassive[var].name ]->setLocalRotation ( glm::slerp( lid->mainMovingPartsMassive[var].targetQuat, lid->mainMovingPartsMassive[var].defaultQuat , mix ) );	
			//~ }
			//~ return false;
		//~ }
	//~ }
	//~ else
	//~ {
		//~ printVarComment("Error! Invalid pointer, detach sequence! MovingPartsController::rotateMainComponent() var: ", var)
		//~ return true; //неудача, неправильный указатель, отвязываем
	//~ }
//~ }


//~ bool MovingPartsController::rotateMainComponentAndBack(void* ptr, int var)					// метод для обработки 1 компонента: позволяет дернуть компонент, чтобы "вывести его из равновесия" и вернуть обратно за 1 взаимодействие
//~ {		
	//~ IF_DYNAMIC_CAST( MovingPartsController*, lid, ((MovingPartsController*)ptr ))		// принудительно воспринимаем void* ptr как указатель на Door и динамически кастуем 
	//~ {
		//~ if( lid->components.count( lid->mainMovingPartsMassive[var].name ) == 0 ) //    CHECK_PTR( door->components[door->entityNameForRotation] ) )
		//~ {
			//~ printVarComment("No entity for moving assigned! Break! LidProcessor::rotateMainComponentAndBack(), var: ", var)
			//~ return true; 
		//~ }
		
		//~ lid->mainMovingPartsMassive[var].ticK += tick;
		
		//~ float mix = 0.5;
		//~ if( lid->mainMovingPartsMassive[var].powValue != 1.0)
		//~ {
			//~ mix = pow( (float)lid->mainMovingPartsMassive[var].ticK/(float)lid->mainMovingPartsMassive[var].timeForRotation, lid->mainMovingPartsMassive[var].powValue);	// определим текущий коэффициет поворота. Возведение в степень позволяет добавить кривизны
		//~ }
		//~ else
		//~ {
			//~ mix =  (float)lid->mainMovingPartsMassive[var].ticK/(float)lid->mainMovingPartsMassive[var].timeForRotation ;	// определим текущий коэффициет поворота
		//~ }
		
		//~ lid->mainMovingPartsMassive[var].status = mix > 1 ? true : false;
				
					
		//~ if(mix>2.01f)
		//~ {
			//~ lid->mainMovingPartsMassive[var].ticK=0;		
			//~ return true;
		//~ }
		//~ else
		//~ {
			//~ // вот тут вся разница между направлениями
			//~ if(!lid->mainMovingPartsMassive[var].status) // если закрыто, то открыть
			//~ {
				//~ lid->components[ lid->mainMovingPartsMassive[var].name ]->setLocalRotation( glm::slerp( lid->mainMovingPartsMassive[var].defaultQuat, lid->mainMovingPartsMassive[var].targetQuat, mix ) );
			//~ }
			//~ else // иначе-наоборот-закрыть
			//~ {
				//~ lid->components[ lid->mainMovingPartsMassive[var].name ]->setLocalRotation( glm::slerp( lid->mainMovingPartsMassive[var].targetQuat, lid->mainMovingPartsMassive[var].defaultQuat , mix-0.99f) );	
			//~ }
			//~ return false;
		//~ }
	//~ }
	//~ else
	//~ {
		//~ printMessage("Error! Invalid pointer, detach sequence! MovingPartsController::pullComponent()")
		//~ return true; //неудача, неправильный указатель, отвязываем
	//~ }
//~ }




//~ #include "MicroWorld.cpp"
//~ #include "SoundAsset.cpp"
//~ #include "auxiliary.cpp"
//~ #include "Entity.cpp"
//~ #include "Material.cpp"
//~ #include "Shader.cpp"
//~ #include "Mesh.cpp"
//~ #include "Skeleton.cpp"
//~ #include "CollisionCalculator.cpp"

//~ int main()
//~ {
	//~ return 0;
//~ }
