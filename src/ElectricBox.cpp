#include "ElectricBox.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "macros.h"

#include "CollisionDescriptor.h"
#include "Door_01.h"
#include "Level.h"


Electric_Box::Electric_Box( glm::vec3 worldLoc = glm::vec3(0, 0, 0), glm::quat worldRot = glm::quat(1, 0, 0, 0), std::string n = "Door_Wood")
{
	MicroWorldName = n;
	
	position = worldLoc;		//назначим координаты
	rotation = worldRot;

	//~ addEntity( "Elbox_01", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Electricbox_01_Box", {} ,"MESH", { "rusty_metal_02",  "rust_coarse_01", },  "none",  "forward" );
	//~ addEntity( "Fuses", glm::vec3(-0.0, 0.0162, 0.2757), glm::quat(0.9833, 0.0, 0.1821, 0.0),  "SM_Electircbox_01_Fuses", {}, "MESH", { "M_Default_1k", },  "none",  "forward" );
	//~ addEntity( "Door_Electro", glm::vec3(0.1065, -0.1991, 0.5416), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Electricbox_01_Door", {}, "MESH", { "rusty_metal_03",  "rust_coarse_01", },  "none",  "forward" );
	//~ addEntity( "Handle", glm::vec3(-0.0059, 0.1965, 0.3719), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Electricbox_01_Handle", {}, "MESH", { "rust_coarse_01", },  "none",  "forward" );
	
	//~ addEntity( "Elbox_01", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Electricbox_01_Box", {  }, "MESH", { "M_Metal_Rust_002_0_5k_x2",  "M_Default_0_5k", },  "none",  "forward" );
	//~ addEntity( "Fuses", glm::vec3(-0.0, 0.0162, 0.2757), glm::quat(0.9833, 0.0, 0.1821, 0.0),  "SM_Electircbox_01_Fuses", {  }, "MESH", { "M_Default_0_5k", },  "none",  "forward" );
	//~ addEntity( "Door_Electro", glm::vec3(0.1065, -0.1991, 0.5416), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Electricbox_01_Door", {  }, "MESH", { "M_Metal_Rust_002_0_5k_x1",  "M_Metal_Rust_002_0_5k_x1", },  "none",  "forward" );
	//~ addEntity( "Handle", glm::vec3(-0.0059, 0.1965, 0.3719), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Electricbox_01_Handle", {  }, "MESH", { "M_Metal_Rust_001_0_5k_x1", },  "none",  "forward" );
	
	CollisionDescriptor collisionEmpty;

	
	std::vector<std::string> elboxMat = { "M_Metal_Rust_002_0_5k_x2",  "M_Default_0_5k" };
	std::vector<std::string> doorelectroMat = { "M_Metal_Rust_002_0_5k_x1",  "M_Metal_Rust_002_0_5k_x1" };
	
	addEntity( "Elbox_01", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Electricbox_01_Box", collisionEmpty, "MESH", elboxMat,  "none",  "forward" );
	addEntity( "Door_Electro", glm::vec3(0.1065, -0.1991, 0.5416), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Electricbox_01_Door", collisionEmpty, "MESH", doorelectroMat,  "none",  "forward" );

	addEntity( "Fuses", glm::vec3(-0.0, 0.0162, 0.2757), glm::quat(0.9833, 0.0, 0.1821, 0.0),  "SM_Electircbox_01_Fuses", collisionEmpty, "MESH", { "M_Default_0_5k", },  "none",  "forward" );
	addEntity( "Handle", glm::vec3(-0.0059, 0.1965, 0.3719), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Electricbox_01_Handle", collisionEmpty, "MESH", { "M_Metal_Rust_001_0_5k_x1", },  "none",  "forward" );

	
	
	
	
	treshold = 1.8;
	
	lidProcConfig Door_Electro;	
		Door_Electro.name = "Door_Electro";	
		Door_Electro.transformationType = MMP_ROTATABLE;	
		Door_Electro.defaultQuat = glm::quat(1,0,0,0);	
		Door_Electro.targetQuat = glm::quat(0.383, 0,0,-0.924);	
		Door_Electro.timeForRotation = 1000;
	
	lidProcConfig Handle;	
		Handle.name = "Handle";	
		Handle.transformationType = MMP_ROTATABLE;	
		Handle.defaultQuat = glm::quat(1,0,0,0);	
		Handle.targetQuat = glm::quat(0.924, 0,0.383, 0);	
		Handle.timeForRotation = 1000;

	lidProcConfig Fuses;	
		Fuses.name = "Fuses";	
		Fuses.transformationType = MMP_ROTATABLE;	
		Fuses.targetQuat = glm::quat(1,0,0,0);	
		Fuses.defaultQuat = glm::quat(0.983, 0, 0.182, 0);	
		Fuses.timeForRotation = 1000;
	
	// так как родительский класс - MovingPartsController, то для него указываем, какая сущность каким компонентом является и уточняем целевой кватернион
	mainMovingPartsMassive[MMP_ZERO]  = Door_Electro;	
	mainMovingPartsMassive[MMP_FIRST] = Handle;	
	mainMovingPartsMassive[MMP_SECOND] = Fuses;	

	// добавим источник света для имитации вспышки выстрела
	//~ if(MicroWorldPersonalLightSourcePtr == nullptr) 
	//~ {
		//~ MicroWorldPersonalLightSourcePtr = new Dynamic_Light_Blink( worldLocation, worldRotation, "Electric_BoxFire" );  // создадим сет динамического света
		//~ MicroWorldPersonalLightSourcePtr->MicroWorldAttachTo(this, "Elbox_01", "none", glm::vec4(0.2, 0, 0.7, 1), glm::vec4(0,0,0,1)); //и привяжем его
	//~ }
	
	//~ lightBlinkSound.openFromFile( "./datas/audio/pishuschaya-mashinka-elektricheskaya-medlennaya-skorost-pechati-27682.wav" );
	//~ generatorSound.openFromFile( "./datas/audio/dizelnyiy-generator-27410.wav" );

	status = true;

}







void Electric_Box::logic()
{
	// if press e and player is near
	// лямбда функция, если копия камеры валидна, то смотреть разницу расстояния, иначе - тру
	auto chk = [&]()	{ 	if( CHECK_PTR(CameraPositionPtr) ) 		{	float length = glm::length(*CameraPositionPtr-position);		return  length < treshold ? true : false;		} 
							else 			{	printMessage("Camera ptr is invalid")			return true;	} 		};
	
	
	if(glfwGetKey(engineWindow, GLFW_KEY_E) && !flag_E && chk() && electricBoxActive)
	{		
		//~ printMessage("Open electric box")	
		if( !doorOpen )
		{
			launchMainMovingPartMovement(MMP_ZERO, std::bind(&Electric_Box::open_1_start, this), std::bind(&Electric_Box::open_1_end, this));
			//Flow.CallSequence({"turnOnElectroBox", turnOnElectroBox , this, &doorOpen});	//
		}
		else
		{
			//Flow.CallSequence({"turnOffElectroBox", turnOffElectroBox , this, &doorOpen});	//
		}
		flag_E = true;
	}
	if(!glfwGetKey(engineWindow, GLFW_KEY_E))		{		flag_E = false;		}
	
	
	// так как привязанный сет света не учтен в списке актеров уровня, то надо обновить его логику вручную
	//~ if(MicroWorldPersonalLightSourcePtr != nullptr) 
	//~ {
		//~ MicroWorldPersonalLightSourcePtr->MicroWorldUpdate(); //attachMicroWorldToParent();	
	//~ }			
}



//~ bool Electric_Box::openSequence_1(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
//~ {
	
//~ }


void Electric_Box::open_1_start()
{
	sound_dvercaOpen->playSound( [](){}, [](){} );
}

void Electric_Box::open_1_end()
{
	// открыли крышку
	launchMainMovingPartMovement(MMP_FIRST, [](){}, [](){} );
	launchMainMovingPartMovement(MMP_SECOND, std::bind(&Electric_Box::open_2_start, this), std::bind(&Electric_Box::open_2_end, this));

}



void Electric_Box::open_2_start()
{
	sound_Fuses->playSound( [](){}, [](){} );
}

void Electric_Box::open_2_end()
{
	//~ printMessage("Electric box opened!")
	
	std::shared_ptr<Door_01> door = dynamic_pointer_cast<Door_01>( level->getActor("Door_02") );
	if(door)
	{
		door->electicaOn = true;
	}
	
	//~ auto orc = dynamic_pointer_cast<Orc>( level->getActor("Orc_01") );
	//~ if(orc) {orc->spotted = true;}
	
	//launchMainMovingPartMovement(MMP_ZERO, std::bind(&Electric_Box::open_1_start, this), std::bind(&Electric_Box::open_1_end, this));
}













/////////////////////////////
//~ bool Electric_Box::turnOnElectroBox(void* ptr, int var)
//~ {
	//~ IF_DYNAMIC_CAST(Electric_Box*, elbox, ((Electric_Box*)ptr))
	//~ {
		//~ static bool step1 = false;
		//~ static bool step2 = false;
		//~ if(!step1)
		//~ {
			//~ Flow.CallSequence({"rotateMainComponent_0", elbox->rotateMainComponent, elbox, &step2, MMP_ZERO});
			//~ step1 = true;
		//~ }
		
		//~ static bool step3 = false;
		
		//~ if( step2 )
		//~ {
			//~ step2 = false;
			//~ Flow.CallSequence({"rotateMainComponent_1", elbox->rotateMainComponent , elbox, nullptr, MMP_FIRST});	//
			//~ Flow.CallSequence({"rotateMainComponent_2", elbox->rotateMainComponent , elbox, &step3, MMP_SECOND});	//
		//~ }
		
		//~ if(step3)
		//~ {
			//~ step1 = false;
			//~ step3 = false;
			//~ //printMessage("turnOnElectroBox done")
			
			//~ //MicroWorld* doorTorch = level->getActor("ArmoryRoom_Torch");
			//~ // IF_DYNAMIC_CAST(MicroWorld*, torch, level->getActor("ArmoryRoom_Torch") )
			//~ // {
				//~ // torch->visible = true;
			//~ // }
			
			//~ // elbox->lightBlinkSound.play();		// Play it
			//~ // elbox->generatorSound.play();
			
			//~ // IF_DYNAMIC_CAST(Dynamic_Light_Blink*, torch, elbox->MicroWorldPersonalLightSourcePtr )
			//~ // {
				//~ // torch->active = true;
			//~ // }
			
			
			
			//~ return true;
		//~ }
		//~ else
		//~ {
			//~ return false;
		//~ }				
	//~ }
	//~ else
	//~ {
		//~ return true; // в случае неудачи приведения типов возвращение true позволит сразу отвязать последовательность от Секвенсора
	//~ }
//~ }



//~ bool Electric_Box::turnOffElectroBox(void* ptr, int var)
//~ {
	//~ IF_DYNAMIC_CAST(Electric_Box*, elbox, ((Electric_Box*)ptr))
	//~ {
		//~ static bool step1 = false;
		//~ static bool step2 = false;
		//~ if(!step1)
		//~ {
			//~ Flow.CallSequence({"rotateMainComponent_1", elbox->rotateMainComponent , elbox, nullptr, MMP_FIRST});	//
			//~ Flow.CallSequence({"rotateMainComponent_2", elbox->rotateMainComponent , elbox, &step2, MMP_SECOND});	//
			//~ step1 = true;
		//~ }
		
		//~ static bool step3 = false;
		
		//~ if( step2 )
		//~ {
			//~ step2 = false;
			//~ Flow.CallSequence({"rotateMainComponent_0", elbox->rotateMainComponent, elbox, &step3, MMP_ZERO});
			//~ // elbox->lightBlinkSound.stop();		// Play it
			//~ // elbox->generatorSound.stop();
			
			//~ // IF_DYNAMIC_CAST(Dynamic_Light_Blink*, torch, elbox->MicroWorldPersonalLightSourcePtr )
			//~ // {
				//~ // torch->active = false;
			//~ // }
			
		//~ }
		
		//~ if(step3)
		//~ {
			//~ step1 = false;
			//~ step3 = false;
			//~ //printMessage("turnOffElectroBox DONE")
			//~ // IF_DYNAMIC_CAST(MicroWorld*, torch, level->getActor("ArmoryRoom_Torch") )
			//~ // {
				//~ // torch->visible = false;
			//~ // }
			//~ return true;
		//~ }
		//~ else
		//~ {
			//~ return false;
		//~ }				
	//~ }
	//~ else
	//~ {
		//~ return true; // в случае неудачи приведения типов возвращение true позволит сразу отвязать последовательность от Секвенсора
	//~ }
//~ }
