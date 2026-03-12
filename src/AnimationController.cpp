#include "AnimationController.h"
#include <iostream>
#include <nameof.hpp>

#include "Skeleton.h"
#include "SkeletonAnimation.h"
#include "macros.h"

AnimationController::~AnimationController() 
{ 
	#ifdef ANIMATION_CONTROLLER_DEBUG
		std::cout << "Destructor of anim controller [" << NAMEOF_TYPE(decltype(this)) << "] [" <<  entityName << "]" << std::endl;
	#endif
	
	//~ delete sklPose; // XXX потенциально вызов двухкратного удаления, т.к. иногда sklPose копирует указатель с animChannels
	
	//printMessage("flag 1")
	
	for(int i=0; i< ANIM_CHANNELS_COUNT; i++) 
	{	
		//printVariable(i)
		
		if( animChannels[i] != nullptr)
		{
			//printMessage("delete 1")
			//printMessage(animChannels[i])
			//~ delete animChannels[i]; 
			//printMessage("delete 2")
		}
		 
	}
	//printMessage("flag 2")
}

bool AnimationController::lockChannelUpdating(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas)
{
	canUpdate[datas.var] = !canUpdate[datas.var];	
	
	//~ #ifdef ANIMATION_CONTROLLER_DEBUG
		//~ if( !canUpdate[datas.var] ) printVarComment("lock channel: ", datas.var)
		//~ else printVarComment("unlock channel: ", datas.var)
	//~ #endif
	
	
	
	//~ printMessage("lock 2")
	return true;
}


bool AnimationController::setAnimation(int channelIndex, animStateConfig& newAnim, animStateConfig& secondAnim  )
{
	// принимает индекс канала и сам конфиг анимации 
	// еще он примет дополнительный конфиг
	
	// вывод аргументов в консоль
	//~ std::cout << "\n New config: " << newAnim.animName << ", mix: " << newAnim.mixCoeff << ", ms: " << newAnim.iAnimTime_milsecs << std::endl;
	//~ if( !newAnim.mask.empty()){for(auto a : newAnim.mask) {std::cout << a << " " ;} std::cout << std::endl;}
	
	//~ std::cout << "\n Second config: " << secondAnim.animName << ", mix: " << secondAnim.mixCoeff << ", ms: " << secondAnim.iAnimTime_milsecs << std::endl;
	//~ if( !secondAnim.mask.empty()){for(auto a : secondAnim.mask) {std::cout << a << " " ;} std::cout << std::endl;}
	
	//~ DEBUG_INFO
	bSecondExist = secondAnim.animName == "none" ? false : true; // во вспомогательной переменной определяем, указана ли следующая анимация
	
	//~ DEBUG_INFO
	
	if( entitySkeletonPtr != nullptr )// CHECK_ASSET(entitySkeletonPtr) )				// если есть скелет
	{	
		//~ DEBUG_INFO
		if( channelIndex >= 0 && channelIndex < 4 ) // если индекс канала в допустимом диапазоне
		{	
			//~ DEBUG_INFO
			if( entitySkeletonPtr->sklAnimations.count( newAnim.animName ) != 0 ) // если требуемая анимация найдена
			{
				//~ DEBUG_INFO
				/// МОЖНО ТВОРИТЬ СПОКОЙНО
				///ПРОДУМАТЬ МЕХАНИЗМ ОЧИСТКИ СТЕКА, НО ОСТАВЛЕНИЯ 3 ВЕРХНИХ ЭЛЕМЕНТОВ!!!
				/// вариантов 2: 1) указана только новая (а следующая либо не указана либо не найдена) и 2) указана новая и следующая
				
				if( entitySkeletonPtr->sklAnimations.count( newAnim.animName ) != 0  )										// если в списке анимаций скелета найдена новая
				{
					if( secondAnim.animName == "none" || entitySkeletonPtr->sklAnimations.count( secondAnim.animName ) == 0 ) 	// и вторая - none либо не найдена
					{
						//~ DEBUG_INFO
						// суем новую в стек
						//~ printMessage("only one")
						if( newAnim.repeatCount == -1 ) // если новую анимацию надо отыгрывать бесконечно
						{
							// XXX нужна очистка всего стека, за исключением 3 верхних элементов!
							
							///animConfigs[channelIndex].clear();
							//while( !animConfigs[channelIndex].empty() ) { animConfigs[channelIndex].pop(); } // то очистим стек
						}
						// а если число отыгрышей конечно, значит после надо будет вернуться к предыдущей и потмоу очистка стека не нужна
						
						// в качестве предпоследнего элемента либо запишем nullptr если обойма пуста, либо ее текущий топовый элемент
						//animConfig_tempPtr[channelIndex] = animConfigs[channelIndex].empty() ? nullptr : &animConfigs[channelIndex].top();			
					}
					else 																									// следующая анимация задана явно
					{
						//~ DEBUG_INFO
						//~ printMessage("Next and Second online!")
						//сначала суем в стек следующую 
						if( secondAnim.repeatCount == -1 ) // если следующую анимацию надо отыгрывать бесконечно (а так должно быть почти всегда!)
						{
							// XXX нужна очистка всего стека, за исключением 3 верхних элементов!
							
							///animConfigs[channelIndex].clear();
							//while( !animConfigs[channelIndex].empty() ) { animConfigs[channelIndex].pop(); } // то очистим стек
						}
						
						/// внимание, тут возможны ошибки
						secondAnim.animPtr = entitySkeletonPtr->sklAnimations[secondAnim.animName];						// обновим указатель на анимацию
						animConfigs[channelIndex].push(  secondAnim );								// и только теперь внесем в обойму новый элемент
						
						//animConfig_tempPtr[channelIndex] = &animConfigs[channelIndex].top();		// запомним следующую анимацию в стеке (почти правильно)
						// этот указатель надо заполнить после отыгрыша новой! Пока новая не отыграна, он должен быть nullptr
						
						// в качестве предпоследнего элемента либо запишем nullptr если обойма пуста, либо ее текущий топовый элемент
						//animConfig_tempPtr[channelIndex] = animConfigs[channelIndex].empty() ? nullptr : &animConfigs[channelIndex].top();
						
						// и лишь теперь суем новую	
					}
					
					//~ DEBUG_INFO
					newAnim.animPtr = entitySkeletonPtr->sklAnimations[newAnim.animName];								// обновим указатель на анимацию
					animConfigs[channelIndex].push( newAnim );									// и только теперь внесем в обойму новый элемент
						
					// на данный момент новая анимация в стеке и захвачена предыдущая, можно запустить смешивание
					///canUpdate[channelIndex] = false;
					///Flow.CallSequence( {"MicroWorldup channel animation", mixAnim, this, &canUpdate[channelIndex], channelIndex } );
					///canUpdate[channelIndex] = false;	// на данный момент новая анимация в стеке и захвачена предыдущая, можно запустить смешивание
					
					///printMessage("Decomment!")
					///DEBUG_INFO
					BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;
					datas.var = channelIndex;
						std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function 	= std::bind( &AnimationController::mixAnim, this, datas);	// главная вращающая функция
						std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> start 		= std::bind( &AnimationController::lockChannelUpdating, this, datas);	// один запуск заблокирует
						std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> end 		= std::bind( &AnimationController::lockChannelUpdating, this, datas);	// второй запуск разблокирует
					Flow.CallSequence( { std::move(function), std::move(start), std::move(end), std::move(datas) } );
					/// надо по завершении микса снять запрет на обновление!!!!
					///Flow.CallSequence( , mixAnim, this, &canUpdate[channelIndex], channelIndex } );
					
					//~ DEBUG_INFO
					
					
					directionOfMixing = true; // разрешаем смешивание "вперед"
				}
				
									
				// если указана только новая, 
					// старая может быть а может не быть
				// если указана еще и следующая, то
				
				// если явно указана еще и следующая, то сначала в стек заносится следующая, чтобы когда новая отыграет, ее попнуть и т.о. перейти к следующей!
				
				
			
				//~ if( newAnim.repeatCount == -1 && !animConfigs[channelIndex].empty() ) // если новую анимацию надо отыгрывать бесконечно, значит, все имеющиеся можно удалить
				//~ {
					//~ for(int i =0; i<1; i++) { animConfigs[channelIndex].pop(); } // 1 раз срезать верхние элементы должно быть достаточно
					//~ // ну либо заменить другим кодом очистки стека
					
					
				//~ }
									
				return true;
				/// КОНЕЦ СПОКОЙНОГО ТВОРЕНИЯ
					
			}	else 		{	printVarComment("No animation found: ", newAnim.animName) std::cout << __LINE__ << std::endl; return false;	}
		}		else		{	printVarComment("Wrong channel index: ", channelIndex) std::cout << __LINE__ << std::endl;	return false;	}
	}			else		{	return false;	}	
	
	
}


bool AnimationController::mixAnim(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas)
{	
	float mix = 0.0f;
	
	// смешаем позу старой и новой анимаций		
		
	// Если послеследующая не указана: если directionOfMixing == true, то смешиваем subtop с top, а если false, то смешиваем prevtop с top
	// Но! если указана послеследующая, то если directionOfMixing == true, то смешиваем subsubtop с top, а если false, то prevtop с top
	
	// как понять, что указана послеследующая? с помощью буля secondExist, который это проверяет в функции MicroWorldAnimation()
	
	// смешивание обратно должно происходить по маске от предыдущего конфига! ключ - directionOfMixing
	
	animStateConfig* first = directionOfMixing ? ( bSecondExist ? animConfigs[datas.var].subsubtop() : animConfigs[datas.var].subtop() )  : animConfigs[datas.var].prevtop();
			
	animStateConfig* second = animConfigs[datas.var].top();
	
	
	if( CHECK_PTR(second) )
	{
		animConfigs[datas.var].top()->timer += tick;
		mix = (float)animConfigs[datas.var].top()->timer/(float)animConfigs[datas.var].top()->blendingTimeIn ;	// определим текущий коэффициет поворота	
		
		
	}
	else {printMessage("Error! No top animation detected!") return true;}
	
	if(mix>1.01f)
	{				
		if( second != nullptr )	// CHECK_PTR(second) )
		{
			second->timer = 0;
			//second->iAnimTime_milsecs = 0;
		}
		
		if( first != nullptr )	//CHECK_PTR(first) ) // subtop
		{
			first->timer = 0;
			//first->iAnimTime_milsecs = 0;
		}
						
		//~ printMessage("mixAnim done")
		
		directionOfMixing = false;
		
		return true;
	}
	else
	{				
		//printVarComment("\ntranslateFromCurrToNext", mix)
				
		
		//~ printVarComment("Direction: ", controller->directionOfMixing)
		
		if(  second != nullptr && second->animPtr != nullptr )	// CHECK_PTR(second) && CHECK_PTR(second->animPtr) ) // если имеются конфиг и новая назначенная анимация
		{
			int frame2 = ( second->iAnimTime_milsecs / (second->animPtr->duration*1000)) * second->animPtr->frames; 
			
			if( first != nullptr )	//CHECK_PTR(first) ) // если есть старый конфиг
			{
				if( first->animPtr != nullptr )	//CHECK_PTR(first->animPtr))
				{
					//printVarComment("\nMixing with previos anim ", mix)
					// если текущая имеется, то смешаем ее с новой
					int frame = ( first->iAnimTime_milsecs / (first->animPtr->duration*1000)) * first->animPtr->frames;
					
					// если directionOfMixing == true, то маску брать от second, тк это переход "туда"
					// если directionOfMixing == false, то маску брать не от second, а от first, тк это возвращение обратно по той же маске
					
					//std::vector
					if(directionOfMixing)
					{
						// по одной маске
						mixByMask(datas.var, first, second, second->mask, mix, frame, frame2);
					}
					else
					{
						// по другой маске!
						mixByMask(datas.var, first, second, first->mask, mix, frame, frame2);
					}
					
				}
						
			}
			else
			{
				//printVarComment("\nMixing with T-pose ", mix)
				
				// если старая анимация отсутствует, то считаем, что там - Т-поза и смешаем ее с second->animPtr
				glm::mat4 tpose = glm::mat4(0); // пустая матрица для любой кости
				tpose[0] = glm::vec4(0,0,0,1); // кватернион в формате x y z w 
				
				if( second->mask.empty() )
				{
					for(int i = 0; i<entitySkeletonPtr->sklBonesCount; i++)		// обновим в канале ВСЕ кости
					{
						//printVarComment("Empty mask ", i)
						animChannels[datas.var] [i] = mixPosesForBone(mix,  tpose, 	second->animPtr->calculateBonePose(i, frame2)	);
					}
				}
				else
				{
					for(auto i : second->mask)							// обновим только МАСОЧНЫЕ кости
					{
						//printVarComment("Full mask ", i)
						animChannels[datas.var] [i] = mixPosesForBone(mix,  tpose, 	second->animPtr->calculateBonePose(i, frame2)	);
					}
				}
				
				
			}
			
			
			
		}
		else {printMessage("Error! Emergency exit") return true;}			
		
		return false;
						
		
	}


}





void AnimationController::switchAnimConfig(int channelIndex)
{	
	if( !animConfigs[channelIndex].empty() )
	{
		//~ std::cout << "Top (current) 0: " <<&animConfigs[channelIndex].top()->animPtr << std::endl;
		// currConfig вернет true если анимация отыграна до конца
		
		if( CHECK_PTR(animConfigs[channelIndex].top()->animPtr) && canUpdate[channelIndex] )
		{
			//~ printMessage("flag 1")
			bool animFinish = animConfigs[channelIndex].top()->animPtr->getPose( animConfigs[channelIndex].top()->iAnimTime_milsecs, animChannels[channelIndex], animConfigs[channelIndex].top()->mixCoeff, animConfigs[channelIndex].top()->mask);
			//~ ///bool animFinish = animConfigs[channelIndex].top()->animPtr->getPose( animConfigs[channelIndex].top()->iAnimTime_milsecs, sklPose, animConfigs[channelIndex].top()->mixCoeff, animConfigs[channelIndex].top()->mask);

			//~ printVarComment("switchAnimation() ", *entityNameCopyPtr)
		
			if(animFinish )																// если доиграли, то
			{					
				//~ printMessage("flag 2")							
				if(animConfigs[channelIndex].top()->repeatCount > 0 ) { animConfigs[channelIndex].top()->repeatCount -= 1; }
				
				if(animConfigs[channelIndex].top()->repeatCount == 0) // если отыграли нужное количество раз
				{			
					//~ printMessage("flag 3")				
					animConfigs[channelIndex].pop(); // удаляя текущую анимацию из стека, т.о. вернемся к предыдущей
											
					//~ canUpdate[channelIndex] = false;
					//~ Flow.CallSequence( {"MicroWorldup channel animation", mixAnim, this, &canUpdate[channelIndex], channelIndex } );
					///canUpdate[channelIndex] = false;	// на данный момент новая анимация в стеке и захвачена предыдущая, можно запустить смешивание
					
					///printMessage("Decomment!")
					///DEBUG_INFO
					BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;
					datas.var = channelIndex;
						std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function 	= std::bind( &AnimationController::mixAnim, this, datas);	// главная вращающая функция
						std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> start 		= std::bind( &AnimationController::lockChannelUpdating, this, datas);	// один запуск заблокирует
						std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> end 		= std::bind( &AnimationController::lockChannelUpdating, this, datas);	// второй запуск разблокирует
					Flow.CallSequence( { std::move(function), std::move(start), std::move(end), std::move(datas) } );
					/// надо по завершении микса снять запрет на обновление!!!!
					///Flow.CallSequence( , mixAnim, this, &canUpdate[channelIndex], channelIndex } );
					
				}
				// XXX printMessage()
				if(animConfigs[channelIndex].top()->repeatCount != -2)	// если не надо застывать на последнем кадре
				{
					animConfigs[channelIndex].top()->iAnimTime_milsecs = 0;							// сбросим счетчик времени для позы
				}
				
				// финальный колбек
				if( animConfigs[channelIndex].top()->end_clb)
				{
					animConfigs[channelIndex].top()->end_clb();	// вызвать выполнение колбека, повешенного на окончание отыгрыша одного цикла анимации. (В дальнейшем можно улучшить, уточняя число циклов)
					//~ animConfigs[channelIndex].top()->end_clb = [](){};
				}
			}
			else	
			{		
				// начальный колбек
				if( animConfigs[channelIndex].top()->iAnimTime_milsecs == 0 && animConfigs[channelIndex].top()->start_clb)
				{
					animConfigs[channelIndex].top()->start_clb();	// вызвать выполнение колбека, повешенного на начало отыгрыша одного цикла анимации
					//~ animConfigs[channelIndex].top()->start_clb = [](){};
				}
				
				// серединный колбек
				if( abs( animConfigs[channelIndex].top()->iAnimTime_milsecs - animConfigs[channelIndex].top()->animPtr->getDuration()/2 ) < 0.9*tick && animConfigs[channelIndex].top()->middle_clb )
				{
					animConfigs[channelIndex].top()->middle_clb();	// вызвать выполнение колбека, повешенного на начало отыгрыша одного цикла анимации
				}
				
				animConfigs[channelIndex].top()->iAnimTime_milsecs += tick;		
			}	// если еще проигрывается, то идем дальше по времени
		}			
	}
	else {printVarComment("Empty channel stack! Index:", channelIndex)}
	//~ printMessage("flag 4")
}








void AnimationController::updateState() //данная функция вызывается при каждом кадре для каждого объекта, независимо от того, скелетный он или нет
{	
		
	if( entitySkeletonPtr && entitySkeletonPtr->status) // && CHECK_PTR(sklPose) )	// если есть скелет и ему создан массив позы и можно обновляться
	{		
		
		if( animChannels[0] != nullptr )
		{
			sklPose = animChannels[0]; // XXX без этой строки на винде все запускается
			switchAnimConfig(0);		// обновим 0 канал
		} 
		
								
	}
}



void AnimationController::mixByMask (int& channelIndex, animStateConfig* first, animStateConfig* second, std::vector<int>& mask, float& mix, int& frame, int& frame2)
{
	//printMessage("mixByMask")
	if( mask.empty() )
	{
		for(int i = 0; i<entitySkeletonPtr->sklBonesCount; i++)
		{
			animChannels[channelIndex] [i] = mixPosesForBone(mix,  first->animPtr->calculateBonePose(i, frame), 	second->animPtr->calculateBonePose(i, frame2)	);
		}
	}
	else
	{
		for(auto i : mask)
		{
			animChannels[channelIndex] [i] = mixPosesForBone(mix,  first->animPtr->calculateBonePose(i, frame), 	second->animPtr->calculateBonePose(i, frame2)	);
		}
	}
}



void AnimationController::checkChannel(int ch)
{
	//~ //printVarComment("\n-------------------------------------------------------------------------------------------------\nCheck channel: ", ch)
	//~ std::cout << "\nCurrent name: " << currConfig[ch].animName << ", mix: " << currConfig[ch].mixCoeff << ", ms: " << currConfig[ch].iAnimTime_milsecs << std::endl;
	//~ if(!currConfig[ch].mask.empty()) { for(auto a : currConfig[ch].mask) {std::cout << a << " " ;} std::cout << std::endl; }
	
	//~ std::cout << "Next name: " << nextConfig[ch].animName << ", mix: " << nextConfig[ch].mixCoeff << ", ms: " << nextConfig[ch].iAnimTime_milsecs << std::endl;
	//~ if(!nextConfig[ch].mask.empty()){for(auto a : nextConfig[ch].mask) {std::cout << a << " " ;} std::cout << std::endl;}
	
	//~ std::cout << "Further name: " << furtherConfig[ch].animName << ", mix: " << furtherConfig[ch].mixCoeff << ", ms: " << furtherConfig[ch].iAnimTime_milsecs << std::endl;
	//~ if(!furtherConfig[ch].mask.empty()){for(auto a : furtherConfig[ch].mask) {std::cout << a << " " ;} std::cout << std::endl;}
	//~ //printMessage("-------------------------------------------------------------------------------------------------\n")		
}

void AnimationController::checkChannels()
{
	for(int i=0; i<1; i++)
	{
		checkChannel(i);
	}
}


glm::mat4 AnimationController::mixPosesForBone(float mixValue, glm::mat4 poseA, glm::mat4 poseB)
{
	//printMessage("test 1")
	glm::quat A = glm::quat( poseA[0].w, poseA[0].x, poseA[0].y, poseA[0].z);
	glm::quat B = glm::quat( poseB[0].w, poseB[0].x, poseB[0].y, poseB[0].z);
	glm::quat mixedAB = glm::slerp(A, B, mixValue);
	
	glm::mat4 mixedMatrix = glm::mat4(0);
	mixedMatrix[0] = glm::vec4( mixedAB.x, mixedAB.y, mixedAB.z, mixedAB.w);
	
	//printMessage("test 2")
	mixedMatrix[1] = glm::mix(poseA[1], poseB[1], mixValue);
	
	return mixedMatrix;
}


glm::mat4 AnimationController::mixPosesForChannels(float mixValue, glm::mat4 poseA, std::vector<int>& maskA, glm::mat4 poseB, std::vector<int>& maskB)
{
	
	
	glm::quat A = glm::quat( poseA[0].w, poseA[0].x, poseA[0].y, poseA[0].z);
	glm::quat B = glm::quat( poseB[0].w, poseB[0].x, poseB[0].y, poseB[0].z);
	glm::quat mixedAB = glm::slerp(A, B, mixValue);
	
	glm::mat4 mixedMatrix = glm::mat4(0);
	mixedMatrix[0] = glm::vec4( mixedAB.x, mixedAB.y, mixedAB.z, mixedAB.w);
	
	mixedMatrix[1] = glm::mix(poseA[1], poseB[1], mixValue);
	
	return mixedMatrix;
}




