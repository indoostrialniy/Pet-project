#include "Laboratory.h"

#include "SoundAsset.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "auxiliary.h"
#include "macros.h"

#include "Elevator.h"
#include "Camera.h"
#include "Pistol.h"
#include "Door_01.h"
#include "Debugger.h"
#include "Scene_01.h"
#include "Dynamic_Ball.h"
#include "ElectricBox.h"
#include "Box_01.h"
#include "Transporter.h"
#include "WallBreach.h"
#include "Gate_01.h"
#include "Orc.h"
#include "EXO_Soldier.h"
#include "Rifle.h"
#include "Turniket.h"

Laboratory::Laboratory()	
{	
	
}



Laboratory::Laboratory(std::string name)
{
	levelName = name;

	#ifdef LABORATORY_DEBUG
		printVarComment("\t Loading level:", levelName)
	#endif
	
	//~ labScreamer1SoundPtr 	= SoundAssets.getAsset("priblijenie-neizvestnogo.wav"); 
	//~ lightSourceHint 		= SoundAssets.getAsset("lightSourceHint.wav"); 
	
	spawnActor<Camera>("Camera");		//actors["Camera"] = std::make_shared<Camera>("Camera");
	spawnActor<Scene_01>("Background");	//actors["Background"] = std::make_shared<Scene_01>();
	//~ spawnActor<Debugger>("Debugger");
	
	
	spawnActor<Gate_01> ( "Gate_01", glm::vec3(-8.05, 4.45, 2.5), glm::quat(0, 0,0, 1));
	spawnActor<WallBreach_01> ( "WallBreach_01", glm::vec3(-9.15, 2, 2.5), glm::quat(0.707, 0,0,0.707));
	spawnActor<Elevator>( "Elevator_01", glm::vec3(-9.25, 0.15, 0), glm::quat(1, 0, 0, 0) );
	spawnActor<Door_01>	( "Door_01", glm::vec3(0, 13.75, -2.5), 	glm::quat(1,0,0,0) );	// простая дверь в генераторную
	spawnActor<Electric_Box>("Electric_Box_01", glm::vec3(7, 6.85, -1.5), glm::quat(-0.707, 0, 0, -0.707) );
	// XXX что то ошибка 
	spawnActor<Pistol>("Pistol_01", glm::vec3(-6.47, 14.29, -1.76), glm::quat(0.347, -0.347, 0.616, -0.616));	
	//~ std::shared_ptr<Pistol> pistol_01 = dynamic_pointer_cast<Pistol>( getActor("Pistol_01") );	if(pistol_01!=nullptr) {pistol_01->canBeTaken = false;}	// конкретно данному пистолету временно отключим возможность подбора
	spawnActor<Box_01>	( "ArmyBox_01", glm::vec3(7.44275, 14.72, -2.5), glm::quat(0.42267, 0,0, -0.906284 ) );	// стандартный ящик
	
	spawnActor<Turniket_01>("Turniket_01", glm::vec3(-5.55314, 11.2992, -2.5), glm::quat(0.707, 0, 0, 0.707));	

	
	spawnActor<Door_01>	( "Door_02", glm::vec3(8.95, 12, -2.5), 	glm::quat(1,0,0,0) );	// электродверь на склад генераторную 
	spawnActor<Orc>		( "Orc_01", glm::vec3(-6, 2.5, 2.6), glm::quat(1,0,0,0) );

	spawnActor<Door_01>	( "Door_03", glm::vec3(30, 1.7, -2.5), 	glm::quat(-0.707,0,0,-0.707) );	// дверь к напарнику
	spawnActor<EXO_Soldier>	( "EXO_01", glm::vec3(30, -0.1, -2.4), glm::quat(0.707,0,0,0.707) );
	spawnActor<Rifle>	( "Rifle_01", glm::vec3(30, 0, -2.4), glm::quat(0.707,0,0,0.707) );

	spawnActor<Rifle>	( "Rifle_02", glm::vec3(10.3353, 7.0642, -2.2252 ), glm::quat(0.688, 0.209, -0.666, -0.202) );

	spawnActor<Door_01>	( "Door_04", glm::vec3(19, -2, -2.5), 	glm::quat(0.707, 0, 0, 0.707) );	// электродверь на склад генераторную 

	
	//~ std::shared_ptr<Box_01> box2 = std::dynamic_pointer_cast<Box_01>(getActor("ArmyBox_02"));						// а вот второму ящику назначим другие функции
	//~ if(box2 != nullptr)
	//~ {
		//~ printMessage("\n\n Found box2 \n\n")
		//~ // я точно знаю, что в классе Box_01 только 1 двигающийся компонент - крышка ящика, под индексом 0
		//~ box2->mainMovingPartsMassive[0].startFunction = std::bind(&Box_01::box_open_script_2, box2);
	//~ }
	
	
	//~ actors["Ward_01"] = new EXO_Soldier( glm::vec3(13,-1,0.1), glm::quat(0.707,0,0,0.707), "Ward_01" );		
	//~ labAmbient.openFromFile( getSoundPath("amb01.ogg") );	//Pashina igra_2.wav") );
	//~ labAmbient.play();		// Play it
	
	FUNC( loadLevel(true) )
}


//~ void Laboratory::spawnAllActors()
//~ {
	
//~ }



void Laboratory::update()
{
	//~ std::cout << "Update " << levelName << "\n";
	
	// looting sound
	//~ DEBUG_INFO
	if( !sound1done && glm::length( getActor("Camera")->getPosition() - glm::vec3(-6, 14, 1)) < 2 )// getActor("Door_01")->getWorldLocation() ) < 4 )
	{
		//playMusic("./datas/audio/walking_on_graviy.wav");
		
		sound1done = true;
	}
	
	//~ DEBUG_INFO
	// скример чьих-то шагов
	if( !sound2done && glm::length( getActor("Camera")->getPosition() - glm::vec3(-6, 7, 1)) < 2 )// getActor("Door_01")->getWorldLocation() ) < 4 )
	{
		labScreamer1SoundPtr->playSound( [](){}, [](){} );
		sound2done = true;
	}
	
	//~ DEBUG_INFO
	// камеры нет, а попытка взять ее адрес (который так то нулевой) - есть
	// подсказка о фонарике
	if( !sound3done && glm::length( getActor("Camera")->getPosition() - glm::vec3(-10, 5, 3)) < 2 )// getActor("Door_01")->getWorldLocation() ) < 4 )
	{
		lightSourceHint->playSound( [](){}, [](){} );
		sound3done = true;
	}
	
	// рык из другой комнаты 
	//~ if( !sound4done && glm::length( getActor("Camera")->getPosition() - glm::vec3(-1.5, 13, 1)) < 2 )// getActor("Door_01")->getWorldLocation() ) < 4 )
	//~ {
		//~ labScreamer2SoundPtr->playSound();
		//~ sound4done = true;
	//~ }
	
	//~ if( glfwGetKey(engineWindow, GLFW_KEY_O) )
	//~ {
		//~ destroyActor("Rifle_03");
	//~ }
	
	//~ if( glfwGetKey(engineWindow, GLFW_KEY_U) )
	//~ {
		//~ level->actors[""]->visible = false;
	//~ }
	
	
}


std::string Laboratory::getSoundPath(std::string filename)	
{	
	return getOperationSystem() == "Windows" ? "datas\\audio\\" + filename + "" : "./datas/audio/" + filename + "" ;		
}	//тернарный определитель пути


Laboratory::~Laboratory()
{
	std::cout << "Destructor of level " << levelName << std::endl;
}



//~ #include "MicroWorld.cpp"
//~ #include "Shader.cpp"
//~ #include "auxiliary.cpp"
//~ #include "Mesh.cpp"
//~ #include "CollisionCalculator.cpp"
//~ #include "Entity.cpp"
//~ #include "Skeleton.cpp"
//~ #include "Level.cpp"
//~ #include "Camera.cpp"

//~ int main()
//~ {
	
	
//~ return 0;
//~ }



