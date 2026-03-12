#include "Transporter.h"

#include "CollisionDescriptor.h"
#include "macros.h"
#include "Entity.h"

Transporter::Transporter( glm::vec3 worldLoc, glm::quat worldRot, std::string n)
{
	MicroWorldName = n;
	
	position = worldLoc;		//назначим координаты
	rotation = worldRot;
	
	CollisionDescriptor MarkerCollision;
		MarkerCollision.collisionType = NO_COLLISION;

	// добавим невидимую сущность
	bool ent1 = addEntity( "Mark_00", glm::vec3(-1, 0, 0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Mark_01", MarkerCollision, "MESH", { "M_Mark_01", },  "none",  "forward" );
		setEntityVisibility( "Mark_00", false );	//components["Mark_00"]->visible = false;
	
	//~ setEntityVisibility( "Mark_00", true);
	
	// так как родительский класс - MovingPartsController, то для него указываем, какая сущность каким компонентом является и уточняем целевой кватернион
	
	treshold = 3.5; // порог по расстоянию до игрока, меньше которого срабатывают клавиши E, R
	
	
	
	lidProcConfig DoorRight;
		DoorRight.name = "Mark_00";
		DoorRight.keyCode = GLFW_KEY_E;
		DoorRight.transformationType = MMP_MOVABLE;
		DoorRight.defaultPose = glm::vec3(-1.2, 0, 0);
		DoorRight.targetPose = glm::vec3(1.2,   0, 0);
		DoorRight.timeForMoving = 2000;
	
	
	mainMovingPartsMassive[MMP_ZERO]  = std::move(DoorRight);						//lidProcConfig("Door", 		false, MMP_MOVABLE, glm::vec3(1.04, -0.44, 0.1), glm::vec3(1.04, -1.24, 0.1), 1160);



	//~ movePartSoundPtr = std::make_unique<SoundAsset>( "zvuk_rgavoy_metallicheskoy_dveri.wav" ); 

	//~ movePartBuffer.loadFromFile("./datas/audio/zvuk_rgavoy_metallicheskoy_dveri.wav");
	//~ movePartSound = sf::Sound(movePartBuffer);

	status = ent1;

}



void Transporter::logic() 
{
	//~ // if press e and player is near
	//~ // лямбда функция, если копия камеры валидна, то смотреть разницу расстояния, иначе - тру
	auto playerIsNear = [&]()	{ 	if( CHECK_PTR(CameraPositionPtr) ) 		{	float length = glm::length(*CameraPositionPtr-position);		return  length < treshold ? true : false;		} 
							else 			{	printMessage("Camera ptr is invalid")			return true;	} 		};
	
	//printMessage("Transporter logic")
	
	if(glfwGetKey(engineWindow, GLFW_KEY_E) && !triggered && playerIsNear() ) // нажав клавишу вблизи неактивированного транспортера
	{
		//~ printMessage("PRESS E")
		triggered = true;		//активируем его однократно		
		startPos = *CameraPositionPtr;		//обновим координаты камеры
		preparing = true; //разрешим подготовку к транспортировке
	} 
	
	if(preparing)
	{
		//printVariable(timer)
		
		*CameraPositionPtr = glm::mix( startPos, (components["Mark_00"]->getWorldPosition() + glm::vec3(0,0,1.67)) , float(timer)/1000.0f) ;	// за 1 с плавно подведем камеру к точке старта
		
		if(timer > 1000)
		{
			timer = 0;
			preparing = false;	
			//~ printMessage("Decomment")
			//~ DEBUG_INFO
			//~ triggered = false;
			processing = true;
			
			launchMainMovingPartMovement( 	MMP_ZERO, std::bind(&Transporter::startTransporting, this), std::bind(&Transporter::endTransporting, this) );	// створка гермоворот

			
			//launchMainMovingPartMovement( &processing, MMP_ZERO);	//запустим перемещение компонента
			
			// запуск нового события	
			//~ BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;	
			//~ datas.var = MMP_ZERO;
			
			//~ std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function = std::bind( &MovingPartsController::rotateUniversal, this, datas);	// главная вращающая функция
			//~ auto start = std::bind( &Transporter::startTransporting, this);		// колбек начала
			//~ auto end = std::bind( &Transporter::endTransporting, this);		// колбек конца
				
			//~ Flow.CallSequence( { std::move(function), std::move(start), std::move(end), std::move(datas) } );
		}
		timer+= tick;
	}
	
	if( processing )	// во время перемещения ассоциируем координаты игрока с мировыми координатами компонента
	{
		//~ printMessage("Transporting player")
		*CameraPositionPtr = components["Mark_00"]->getWorldPosition()+glm::vec3(0,0,1.67);
	}
	
	
		
}

void Transporter::startTransporting()
{
	printMessage("Start transporting")
	
	sound_Transporting->playSound( [](){}, [](){} );
}

void Transporter::endTransporting()
{
	printMessage("End transporting")
	processing = false;
	
	sound_Transporting->stopSound();
}



/////////////////

	//~ printVariable(active)
	
	//processMMP(GLFW_KEY_Y, active, MMP_ZERO, chk() );
	
	//~ 
	
	
	
	//~ //if(glfwGetKey(engineWindow, GLFW_KEY_E) && !flag_E && chk() )
	//~ if(chk() && !flag_E )
	//~ {	
		//~ printMessage("\n\nGO TO GATES LAUNCHED!\n")
		
		//~ flag_E = true; // true-флаг позволит избежать множественного вызова последовательности elevatorOpenGates, которая по завершении снова инвертирует его и можно будет повторить заново
		//~ Flow.CallSequence({"goToGates", goToGates , this, nullptr});	//
	//~ }
