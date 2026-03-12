// enabling debug messages:

//~ #define CORE_DEBUG
//~ #define LEVEL_DEBUG
//~ #define LABORATORY_DEBUG
//~ #define MICROWORLD_DEBUG
//~ #define MESH_DEBUG
//~ #define MOVINGPARTS_DEBUG
//~ #define SKELETON_DEBUG
//~ #define SHADER_DEBUG
//~ #define ANIMATION_CONTROLLER_DEBUG
//~ #define COLLISION_DEBUG
//~ #define SOUNDASSET_DEBUG
//~ #define MATERIAL_DEBUG
//~ #define ENTITY_DEBUG

//~ // макрос позволяет отслеживать вызовы функций
//~ #define TRACE_CALLS

#include <iostream>

#include "Core.h"

int main()
{
    std::cout << "fhMarch2026 12_03!\n";
    
    Core Game;
    Game.run();
    
	std::cout << "End of program. Quit\n";
	
	//system("pause");
    return 0;
}


// XXX НЕ ЗАБЫВАТЬ: подключаем код вручную
// либо подключаем исходники через makefile
//~ #include "auxiliary.cpp"		
//~ #include "Core.cpp"
//~ #include "global.cpp"

//~ // ресурсы
//~ #include "MicroWorld.cpp"
//~ #include "Shader.cpp"
//~ #include "Material.cpp"
//~ #include "SoundAsset.cpp"

//~ // #include "CollisionCalculator.cpp"
//~ #include "Mesh.cpp"
//~ #include "Collision.cpp"
//~ #include "Entity.cpp"
//~ #include "Skeleton.cpp"
//~ #include "MovingPartsController.cpp"
//~ #include "SkeletonAnimation.cpp"
//~ #include "AnimationController.cpp"
//~ #include "CollisionDescriptor.cpp"
//~ #include "CollisionShell.cpp"

//~ // уровни
//~ #include "Level.cpp"
//~ #include "Laboratory.cpp"

//~ // акторы
//~ #include "Camera.cpp"
//~ #include "Elevator.cpp"
//~ #include "Weapon.cpp"
//~ #include "Pistol.cpp"
//~ #include "Door_01.cpp"
//~ #include "Debugger.cpp"
//~ #include "Scene_01.cpp"
//~ #include "ElectricBox.cpp"
//~ #include "Box_01.cpp"
//~ #include "Transporter.cpp"
//~ #include "WallBreach.cpp"
//~ #include "Gate_01.cpp"
//~ #include "DynamicLight.cpp"
//~ #include "AI_Controller.cpp"
//~ #include "NPC.cpp"
//~ #include "Orc.cpp"
//~ #include "EXO_Soldier.cpp"
//~ #include "Rifle.cpp"
//~ #include "Turniket.cpp"


