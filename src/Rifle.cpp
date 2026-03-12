#include "Rifle.h"
#include "CollisionDescriptor.h"
#include "SoundAsset.h"
#include "global.h"

Rifle::Rifle() 
{
	
}

Rifle::Rifle(glm::vec3 worldLoc = glm::vec3(0, 0, 0), glm::quat worldRot = glm::quat(1, 0, 0, 0), std::string name = "default_rifle")
{
	addTag("Weapon");
	addTag("Rifle");

	MicroWorldName = name;

	position = worldLoc;		//назначим координаты блюпринта
	rotation = worldRot;
	
	CollisionDescriptor emptyCollision;

	
	CollisionDescriptor rifleCollision;
		rifleCollision.collisionRadius = 0.09f;
		rifleCollision.collisionType = SPHERE_COLLISION;
		rifleCollision.complexCollisionFileName = "CB_Pistol_01";

	addEntity( "Rifle", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_ACR", rifleCollision, "MESH", { "M_ACR_01", },  "none",  "forward" );
	addEntity( "Rif", glm::vec3(0.1873, 0.0246, 0.1112), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_ACR_Rif", emptyCollision, "MESH", { "M_ACR_01", },  "none",  "forward" );
	addEntity( "Mag", glm::vec3(0.0561, 0.0002, -0.0295), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_ACR_Mag", emptyCollision, "MESH", { "M_ACR_Mag_01", },  "none",  "forward" );

	//~ addEntity( "Rifle", glm::vec3(0.0, 0.0, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_ACR", rifleCollision, "MESH", { "Rifle", },  "none",  "forward" );
	//~ addEntity( "Rif", glm::vec3(0.1873, 0.0246, 0.1112), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_ACR_Rif", {  }, "MESH", { "Rifle", },  "none",  "forward" );
	//~ addEntity( "Mag", glm::vec3(0.0561, 0.0002, -0.0295), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_ACR_Mag", {  }, "MESH", { "Pmag", },  "none",  "forward" );

	mainCollisionEntity = "Rifle";	// указываем, какая сущность будет являться основной для комбо оружия
	mainVisualEntity = "Rifle";
	//настраиваем уникальные характеристики конкретного типа оружия
	ammoMaxCap = 30;
	damage = 30;


	currentAmmoCount = ammoMaxCap;

	// укажем вектора ориентации сокета для привязки к руке ГГ
	socketPositionPlayer = glm::vec4(0.5431, -0.098, -0.072, 1);
	socketRotationPlayerXYZW = glm::vec4(-0.328, -0.025, 0.057, 0.943);
	
	// укажем вектора ориентации сокета для привязки к руке НПС с mixamo
	socketPosition = glm::vec4(0.009232, -0.85504, 1.345, 1);
	socketRotationXYZW = glm::vec4(-0.560, 0.476, -0.5, 0.458);

	idlePlayerAnimation = "Hands_Rifle_Idle";
	aimPlayerAnimation = "Hands_Rifle_Aim";
	takingPlayerAnimation = "Hands_Rifle_Overview";
	reloadPlayerAnimation = "Hands_Pistol_Reload";

	//~ // обозначим двигающиеся части
	//~ lidProcConfig ZatvorMovingPart;												// структура, кастомизирующая параметры трансформации указанной сущности
		//~ ZatvorMovingPart.name = "Zatvor";
		//~ ZatvorMovingPart.keyCode = GLFW_MOUSE_BUTTON_LEFT;
		//~ ZatvorMovingPart.transformationType = MMP_MOVABLE_REVERSE;
		//~ ZatvorMovingPart.targetPose = glm::vec3(-0.021, 0, 0);
		//~ ZatvorMovingPart.timeForMoving = 70;
	
	//~ lidProcConfig GuntriggerRotatingPart;
		//~ GuntriggerRotatingPart.name = "Guntrigger";
		//~ GuntriggerRotatingPart.mouse = true;
		//~ GuntriggerRotatingPart.keyCode = GLFW_MOUSE_BUTTON_LEFT;
		//~ GuntriggerRotatingPart.transformationType = MMP_ROTATABLE_REVERSE;
		//~ GuntriggerRotatingPart.targetQuat = glm::quat(0.957, 0, 0.291, 0);
		//~ GuntriggerRotatingPart.timeForRotation = 70;
	
	
	//~ mainMovingPartsMassive[MMP_ZERO]   = std::move(ZatvorMovingPart);			// обозначаем двигающуюся часть
	//~ mainMovingPartsMassive[MMP_FIRST]  = std::move(GuntriggerRotatingPart);		// двигающуюся часть

	
	weaponShotSoundPtr 		= SoundAssets.getAsset("weapons/colt/colt_shoot.ogg");
	weapponNoAmmoSoundPtr 	= SoundAssets.getAsset("weapons/colt/colt_empty.ogg");
	weaponReloadSoundPtr 	= SoundAssets.getAsset("weapons/colt/colt_reload.ogg");
	
	
	
	
	// добавим источник света для имитации вспышки выстрела
	//~ if(MicroWorldPersonalLightSourcePtr == nullptr) 
	//~ {
		//~ MicroWorldPersonalLightSourcePtr = new Dynamic_Light( worldLocation, worldRotation, "weaponFire" );  // создадим сет динамического света
		//~ MicroWorldPersonalLightSourcePtr->MicroWorldAttachTo(this, "Pistol", "none", glm::vec4(0.11, 0, 0.06, 1), glm::vec4(0,0,0,1)); //и привяжем его
	//~ }
	
	status = components.size()==3;	//true;
}
