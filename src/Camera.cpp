#include "Camera.h"

#include "CollisionDescriptor.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Weapon.h"

#include "AnimationController.h"
#include "macros.h"
#include "global.h"
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "SoundAsset.h"

#include "Entity.h"
#include "Level.h"


Camera::Camera(std::string n )	
{	
	MicroWorldName = n;
	
	addTag("Camera");
	addTag("Player");
	
	//~ if( glewInit() != GLEW_OK)	// первым делом при создании ресурса, использующего openGL, проверим, инициализирован ли он
	//~ {
		//~ std::cout << "OpenGL context not exist until now. Create!"; DEBUG_INFO
		//~ initGL();
	//~ }
	
	//~ CollisionConfigurator
	//~ {}
				
		
	//sphere, radius 1.0f "CB_PLAYER_01.001"
																											
	CollisionDescriptor PlayerHandsCollision;
	//~ PlayerHandsCollision.collisionRadius = 0.5f;
	//~ PlayerHandsCollision.collisionType = SPHERE_COLLISION;
	PlayerHandsCollision.collisionRadius = 1.8f;
	PlayerHandsCollision.collisionType = COMPLEX_COLLISION;
	PlayerHandsCollision.complexCollisionFileName = "CB_Player_02";
	
	// PLAYER_COLLISION в функции update ежекадрово принимает позицию на 1 метр ниже координат комбо!
	addEntity( "PLAYER_COLLISION", glm::vec3(0,0,0), glm::quat(1.0, 0.0, 0.0, 0.0),  "CB_Player_Hands_02_Visual", PlayerHandsCollision, "MESH", {"M_PlayerHands_01",},  "none",  "forward" );	//таким образом можно одновременно и визуализировать оболочку
	
	setEntityVisibility( "PLAYER_COLLISION", false );				// components["PLAYER_COLLISION"]->visible = false;
	
	
	mainCollisionEntity = "PLAYER_COLLISION";
	
	
	
	//wpn->attachTo( caster, "Player_Hands_MESH", "Bone.028", wpn->getSocketPosition(), wpn->getSocketRotationXYZW() ); //vec4(0.5317, -0.087, -0.089, 1), vec4(-0.272, 0.066, 0.156, 0.947));

	
	///addEntity( "PLAYER_COLLISION_FOOTS", glm::vec3(0,0,-1.5), glm::quat(1.0, 0.0, 0.0, 0.0),  "CB_Player_Hands_02_Visual", PlayerHandsCollision, "MESH", {},  "none",  "forward" );	//таким образом можно одновременно и визуализировать оболочку
	///components["PLAYER_COLLISION_FOOTS"]->visible = false;
	
	//~ addEntity("Player_Hands_MESH", glm::vec3(0.072, 0.0, -0.151), glm::quat(1.0, 0.0, 0.0, 0.0), "SKM_Ork_01", {}, "MESH", { "M_PlayerHands_01", }, "none", "forward");
	addEntity("Player_Hands_MESH", glm::vec3(0.072, 0.0, -0.151), glm::quat(1.0, 0.0, 0.0, 0.0), "SKM_PlayerMesh", {}, "MESH", { "M_PlayerHands_01", }, "none", "forward");
	mainVisualEntity = "Player_Hands_MESH";
	
	// вместо этой функции надо как то сгенерировать сущность и назначить ей дубликаты position, rotation
	
	
	//~ DEBUG_INFO
	
	//~ addEntity( "PLAYER_COLLISION", glm::vec3(0.072, 0.0, -0.151), glm::quat(1.0, 0.0, 0.0, 0.0),  "Icosphere", {}, "MESH", { "M_PlayerHands_01", },  "none",  "forward" ); // визуализация сферы с радиусом 0.58
	
	animStateConfig handsAnimation;
		handsAnimation.mixCoeff = 0.0f;
		handsAnimation.mask = {};
		handsAnimation.animName = "Hands_Idle";
		handsAnimation.repeatCount = -1;
		handsAnimation.blendingTimeIn = 500;
	
	FUNC( setComponentAnimation(	mainVisualEntity, 0, handsAnimation, handsAnimation ) )		//components["Player_Hands_MESH"]->setAnimation(0, handsAnimation, handsAnimation	);	
	
	// { 0.0f, {}, "Hands_Idle", 1, 500 }, { 0.0f, {}, "Hands_Idle", -1, 500 } );	

	//~ DEBUG_INFO

	CameraPositionPtr 	= &position;			// закинем в глобальный указатель адрес координат камеры
	CameraAxisPtr 		= &m3VectorsForwardLeftUp;	// закинем в глобальный указатель адрес на матрицу осей камеры
	CameraQuaternion	= &rotation;			// закинем в глобальный указатель адрес на кватеринон камеры
	
	//~ CameraPrevPosPtr	= &worldPrevLoc;			// закинем в глобальный указатель адрес на предыдущую координату камеры
	
	status = true;
}
	
	
Camera::~Camera()
{
	std::cout << "Destructor of combo [" << NAMEOF_TYPE(decltype(this)) << "] [" <<  MicroWorldName << "]" << std::endl;

	
}
		


void Camera::update()	// здесь происходит изменение параметров камеры, ОБЯЗАТЕЛЬНО делать поправку на ФПС!
{
	/// F
	//~ if (glfwGetKey(engineWindow, GLFW_KEY_F) && !flagF )
	//~ {
		//~ if(components.count("Player_Hands_MESH") != 0)
		//~ {
			//~ printMessage("Press F!")
			//~ //Flow.CallSequence( {"pressed", pressed, this, &flagF} );
			///components["Player_Hands_MESH"]->MicroWorldAnimation(0, { 0.0f, {}, "Hands_F", 2, 500, 500, } );
			
			//~ animStateConfig handsAnimation, noSecondAnimation;
				//~ handsAnimation.mixCoeff = 1.0f;
				//~ handsAnimation.mask = {PLAYER_LEFT_HAND};
				//~ handsAnimation.animName = "Player_Interact_Left";
				//~ handsAnimation.repeatCount = 1;
				//~ handsAnimation.blendingTimeIn = 500;
			
			//~ components["Player_Hands_MESH"]->setAnimation(0, handsAnimation, noSecondAnimation	);
			
			///components["Player_Hands_MESH"]->setAnimation(0, { 1.0f, {PLAYER_LEFT_HAND}, "Player_Interact_Left", 1, 500 } );//, { 0.0f, {}, "Hands_Rifle_Idle", -1, 50 } );
			
				//~ //{1.0f, {PLAYER_LEFT_HAND}, "Hands_LeftArm", 1000} );																	
		//~ }
		//~ flagF = true;
	//~ }	if (!glfwGetKey(engineWindow, GLFW_KEY_F) )		{	flagF = false;	}
	
	
	if (glfwGetKey(engineWindow, GLFW_KEY_L) && !flagL )
	{
		lightSourceActive = !lightSourceActive;
		sound_LightSwitch->playSound( [](){}, [](){} );
		
		
		//~ if(!lightSourceSpawned)	// однократно заспауним и привяжем источник света
		//~ {
			//~ lightSourceSpawned = true;
			//~ std::string name = MicroWorldName+"_light";
			//~ level->spawnActor<DynamicLight_01>( name, getPosition(), getRotation() );  // заспауним сет динамического света
			
			//~ auto light = level->getActor(name);
			
			//~ if(light)
			//~ {
				//~ level->getActor(name)->attachTo( this, "Player_Hands_MESH", "none", glm::vec4(0.11, 0, 0.06, 1), glm::vec4(0,0,0,1) ); 
				//~ std::shared_ptr<DynamicLight_01> dynamicLight = dynamic_pointer_cast<DynamicLight_01>(light);
				//~ if(dynamicLight) 
				//~ {
					//~ dynamicLight->colorVec = glm::vec4(1,0.8,1, 0);
					//~ dynamicLight->typeSectorRadiusPower = glm::vec4(1.0f, 0.87f, 1.0f, 1.0f);
				//~ }
			//~ }
			
		//~ }
		//~ if( childMicroWorld!=nullptr && childMicroWorld->hasTag("Light") ) 
		//~ {
			//~ sound_LightSwitch->playSound();
			//~ childMicroWorld->visible = !childMicroWorld->visible;
			
			//~ if( !lightSourceActive )
			//~ {
				//~ childMicroWorld->visible = true;
			//~ }
			//~ else
			//~ {
				//~ childMicroWorld->visible = false;
			//~ }
		//~ }
		
		flagL = true;
	}	if (!glfwGetKey(engineWindow, GLFW_KEY_L) )		{	flagL = false;	}
	
	
	
	
	/// H
	//~ if (glfwGetKey(engineWindow, GLFW_KEY_H) && !flagH )
	//~ {
		//~ if(components.count("Player_Hands_MESH") != 0)
		//~ {
			//~ printMessage("Press H!")
			//~ //components["Player_Hands_MESH"]->MicroWorldAnimations(1, 	{1.0f, {PLAYER_RIGHT_HAND}, "Hands_RightArm", 1000} );
			
			//~ animStateConfig handsAnimation, noSecondAnimation;
				//~ handsAnimation.mixCoeff = 0.0f;
				//~ handsAnimation.mask = {};	//PLAYER_RIGHT_HAND
				//~ handsAnimation.animName = "Hands_Knife_Open_Door";//"Hands_RightArm";
				//~ handsAnimation.repeatCount = 1;
				//~ handsAnimation.blendingTimeIn = 500;
			
			//~ components["Player_Hands_MESH"]->setAnimation(0, handsAnimation, noSecondAnimation	);
			///components["Player_Hands_MESH"]->setAnimation(0, { 1.0f, {PLAYER_RIGHT_HAND}, "Hands_RightArm", 1, 500 } );//, { 0.0f, {}, "Hands_Idle", -1, 50 } ); 
		//~ }
		//~ flagH = true;
	//~ }	if (!glfwGetKey(engineWindow, GLFW_KEY_H) )		{	flagH = false;		}
	
	
	
	/// E
	if(glfwGetKey(engineWindow, GLFW_KEY_E) && !takeE && childMicroWorld == nullptr)
	{
		//~ printMessage("\n\n PLAYER TAKE WEAPON CALLED! \n ")
		takeE = true;
		
		BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;		
		std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function = std::bind( &Camera::takeWeapon, this, datas);	// главная вращающая функция
		auto start = std::bind( &Camera::takeWeapon_start, this);		// колбек начала
		auto end = std::bind( &Camera::takeWeapon_end, this);		// колбек конца
			
		Flow.CallSequence( { std::move(function), std::move(start), std::move(end), std::move(datas) } );
		
		//~ Flow.CallSequence({"takeWeapon", takeWeapon, this, &takeE});
		
	}		//if(!glfwGetKey(engineWindow, GLFW_KEY_E))		{	takeE = false;		}
	
	/// G
	if(glfwGetKey(engineWindow, GLFW_KEY_G) && !throwG && childMicroWorld != nullptr )
	{
		throwG = true;
		//~ ///Flow.CallSequence({"throwWeapon", throwWeapon, this, &throwG});
		
		flagLMB = false;
		
		BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;		
		std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function = std::bind( &Camera::throwWeapon, this, datas);	// главная вращающая функция
		auto start = std::bind( &Camera::throwWeapon_start, this);		// колбек начала
		auto end = std::bind( &Camera::throwWeapon_end, this);		// колбек конца
			
		Flow.CallSequence( { std::move(function), std::move(start), std::move(end), std::move(datas) } );
		
	}		//if(!glfwGetKey(engineWindow, GLFW_KEY_G))		{	throwG = false;		}

	
	
	//weaponShot
	/// LMB запускается однократно при выполнении всех условий и повторный пуск возможен лишь когда flagLMB снова станет false. flagLMB инвертируется внутри последовательности из bWeaponShot
	if(glfwGetMouseButton(engineWindow, GLFW_MOUSE_BUTTON_LEFT) && !flagLMB )
	{
		flagLMB = true;
		
		if( childMicroWorld != nullptr )
		{
			Weapon* wpn = dynamic_cast<Weapon*>( childMicroWorld );

			if( wpn != nullptr ) 
			{
				wpn->bWeaponShot();			// то вызовем функцию стрельбы, которая включает в себя проверки, запуск звуков и т.д.
			}
			else
			{
				printMessage("Error shot")
				DEBUG_INFO
				//flagLMB = false; // защита от невыполнения при неудачном касте 
			}
		}
	}
	if( !glfwGetMouseButton(engineWindow, GLFW_MOUSE_BUTTON_LEFT) )
	{
		flagLMB = false;
	}
	
	// можно вкорячить удобную функцию GetCurrentWeapon(), возвращающую указатель на динамически кастованное оружие и уже ему вызывать метод выстрела и т.д.
	
	//weaponReload
	if(glfwGetKey(engineWindow, GLFW_KEY_R) && !flagR )
	{
		flagR = true;
		
		
		if( childMicroWorld != nullptr )
		{
			//~ printMessage("Player try shot")
			
			Weapon* wpn = dynamic_cast<Weapon*>( childMicroWorld );
			
			if( wpn != nullptr ) 
			{
				wpn->bWeaponReload();			// то вызовем функцию стрельбы, которая включает в себя проверки, запуск звуков и т.д.			
			}
			else
			{
				printMessage("Error reload")
				DEBUG_INFO
			}
		}
		
		
	}	if( !glfwGetKey(engineWindow, GLFW_KEY_R) )	{	flagR = false;		}
	
	//weaponAim
	if( childMicroWorld != nullptr)
	{
		if( glfwGetMouseButton(engineWindow, GLFW_MOUSE_BUTTON_RIGHT) && !bPlayerAiming )// !flagRMB)
		{
			bPlayerAiming = true;													// защита от повторного срабатывания
			//~ printMessage("Decomment")
			//~ DEBUG_INFO
			Weapon* wpn = dynamic_cast<Weapon*>( childMicroWorld );
			if( wpn != nullptr ) 
			{
				wpn->bWeaponAim(bPlayerAiming);			
			}
			else
			{
				printMessage("Error aim")
				DEBUG_INFO
				//~ flagRMB = false; // защита от невыполнения при неудачном касте 
			}
		}
		
		if( !glfwGetMouseButton(engineWindow, GLFW_MOUSE_BUTTON_RIGHT) && bPlayerAiming )
		{
			bPlayerAiming = false;												// защита от повторного срабатывания
		
			//~ printMessage("Decomment")
			//~ DEBUG_INFO
			Weapon* wpn = dynamic_cast<Weapon*>( childMicroWorld );
			if( wpn != nullptr ) 
			{
				wpn->bWeaponAim(bPlayerAiming);			
			}
			else
			{
				printMessage("Error aim")
				DEBUG_INFO
				//~ flagRMB = false; // защита от невыполнения при неудачном касте 
			}
		}
		
		
		//~ if()
		//~ {
			
		//~ }
		
		
		
	}
	
	
	//~ if( childMicroWorld != nullptr)
	//~ {
		
	//~ }


	
	
	float fMoveSpeedXY = 1.0f*fCameraMovingSpeedXY * float(tick)/1000.0f; // сдвиг 1 см за шаг
	
	float fMoveSpeedZ = 1.0f*fCameraMovingSpeedZ * float(tick)/1000.0f;
	
	float fRotationSpeed = fCameraRotationSpeed * float(tick)/1000.0f;
	
	//~ printVariable(fMoveSpeedZ)

	
	// определим вектора направления камеры в плоскости X-Y
	glm::vec3 forwardVec = glm::normalize(glm::vec3(m3VectorsForwardLeftUp[0].x, m3VectorsForwardLeftUp[0].y, 0.0f)); // нормализовали вектор в плоскости 
	glm::vec3 upVec = glm::vec3(0,0,1);
	glm::vec3 leftVec = glm::cross( upVec, forwardVec); //плоскостный вектор влево получим путем векторного произведения вертикального на впередсмотрящий
	
	
	if( glfwGetKey(engineWindow, GLFW_KEY_J) )	{	}
	
	
	// звуки щагов
	static int step = 0;
	if( glfwGetKey(engineWindow, GLFW_KEY_W) || glfwGetKey(engineWindow, GLFW_KEY_S) || glfwGetKey(engineWindow, GLFW_KEY_A) || glfwGetKey(engineWindow, GLFW_KEY_D) )
	{
		if(step > 300)
		{
			step=0;
			sound_walking->playSound( [](){}, [](){} );
		}
		step+=tick;
	}
	
	
	
	if(overview)		// если камере разрешены обзор и перемещение
	{
		// так как bCastSpaceInDirectionи используется для разрешения движения, надо, чтобы она не просто сообщала можно/нельзя, а еще и корректировала курс согласно углу направления
		if (glfwGetKey(engineWindow, GLFW_KEY_W) )							{	moveInDirection( "PLAYER_COLLISION", forwardVec, fMoveSpeedXY );	} 
		if (glfwGetKey(engineWindow, GLFW_KEY_S) )							{	moveInDirection( "PLAYER_COLLISION", -forwardVec, fMoveSpeedXY );	} 
		if (glfwGetKey(engineWindow, GLFW_KEY_A) )							{	moveInDirection( "PLAYER_COLLISION", leftVec, fMoveSpeedXY );		}
		if (glfwGetKey(engineWindow, GLFW_KEY_D) )							{	moveInDirection( "PLAYER_COLLISION", -leftVec, fMoveSpeedXY );		} 
	
		//перемещаемся вверх-вниз
		if (glfwGetKey(engineWindow, GLFW_KEY_SPACE) ) 	{	moveInDirection( "PLAYER_COLLISION", upVec, fMoveSpeedZ*1.5f );	}//{ 	position += glm::vec3(0,0,1) * fMoveSpeedZ*(2.0f); }
		//~ if (glfwGetKey(engineWindow, GLFW_KEY_C)  )		{	position += -upVec * fMoveSpeedZ; }	//moveInDirection( "PLAYER_COLLISION", -upVec, fMoveSpeedZ );	}
		
		glm::vec3 cameraAngles = glm::degrees(glm::eulerAngles( rotation ));
		
		//по хорошему здесь надо делать ограничение на такой угол, который чуть больше того, на который поворачивается камера при текущем ФПС за 1 кадр, но пока оставим -80 + 80 град.
		
		//вращаемся 
		if (glfwGetKey(engineWindow, GLFW_KEY_UP) && cameraAngles.y > -80) { rotation = glm::angleAxis(glm::radians(-fRotationSpeed), m3VectorsForwardLeftUp[1]) * rotation; }	//
		if (glfwGetKey(engineWindow, GLFW_KEY_DOWN) && cameraAngles.y < 80) { rotation = glm::angleAxis(glm::radians(fRotationSpeed), m3VectorsForwardLeftUp[1]) * rotation; }	//
		if (glfwGetKey(engineWindow, GLFW_KEY_LEFT)) { rotation = glm::angleAxis(glm::radians(fRotationSpeed), glm::vec3(0,0,1)) * rotation; }
		if (glfwGetKey(engineWindow, GLFW_KEY_RIGHT)) { rotation = glm::angleAxis(glm::radians(-fRotationSpeed), glm::vec3(0,0,1)) * rotation; }
		
		double xpos, ypos = 0.0;
			
		static bool lookAroundWithMouse = false;
		static bool lookAroundWithMouseFlag = true;

		if (glfwGetMouseButton(engineWindow, GLFW_MOUSE_BUTTON_MIDDLE) && lookAroundWithMouseFlag) { lookAroundWithMouse = !lookAroundWithMouse;  lookAroundWithMouseFlag = false; }
		if (!glfwGetMouseButton(engineWindow, GLFW_MOUSE_BUTTON_MIDDLE)) { lookAroundWithMouseFlag = true; }

		if (lookAroundWithMouse) // осматривание мышью
		{

			///более-менее рабочая версия
			glfwGetCursorPos(engineWindow, &xpos, &ypos);

			int w, h;
			glfwGetWindowSize(engineWindow, &w, &h);


			float xx = 0.0625f * (float(w) / 2.0f - xpos);
			float yy = 0.0625f * (float(h) / 2.0f - ypos);

			if (cameraAngles.y > -85.0f && yy > 0)
			{
				// XXX зачем я сюда пихнул указатель?
				rotation = glm::angleAxis(glm::radians(-yy * fRotationSpeed), (*CameraAxisPtr)[1]) * rotation;
			}
			if (cameraAngles.y < 85.0f && yy < 0)
			{
				rotation = glm::angleAxis(glm::radians(-yy * fRotationSpeed), (*CameraAxisPtr)[1]) * rotation;
				//~ DEBUG_INFO
			}	

			rotation = glm::angleAxis(glm::radians(xx * fRotationSpeed), glm::vec3(0,0,1)) * rotation;

			glfwSetCursorPos(engineWindow, float(w) / 2.0f, float(h) / 2.0f);
		}
	}
	
	
	//~ // gravity
	//~ printMessage("Gravity on") DEBUG_INFO	
	moveInDirection( "PLAYER_COLLISION", -upVec, fMoveSpeedZ*0.7f );
	
	
	
	
	
	//~ static glm::vec3 offfdfmsds = glm::vec3(0,0,1);
	//~ //runtimePositionChange(offfdfmsds);
	//~ //printVec3(offfdfmsds)
	
	//~ position -= glm::vec3(0,0,1) * fMoveSpeedZ*offfdfmsds.z;
	//~ glm::vec3 temp = glm::vec3(0,0,0);
	//~ float tempK = 0.0f;
	//~ bool cast = bCheckIntersections( "Player_Hands_MESH", position, -glm::vec3(0,0,1), temp, tempK  );	// проверяем, не стали ли пересекать чью-либо коллизию
	//~ if(cast) // если пересекли кого-то
	//~ {
		//~ position += glm::vec3(0,0,1) * fMoveSpeedZ*offfdfmsds.z; //сделаем такой же шаг назад
	//~ }
	
	
	// надо чтобы независимо от вращения сета, сфера коллизии всегда располагалась прямо под ногами, т.е. надо ежекадрово задавать ей позицию
	//~ printVec3(position)
	//~ components["PLAYER_COLLISION"]->setWorldPosition( position + glm::vec3(0,0,-1) );
	//~ printVec3( components["PLAYER_COLLISION"]->getWorldPosition() )
	
	//
	
	
	
	
	
	
	
	
	
	
	
	
	
	//~ printVariable(tick)
	
	//~ for(int i=0; i<30000000; i++) //искусственно занизить ФПС
	//~ {
		//~ int f = 212123525;
	//~ }
	
	
	

	
}





// flow.sequence последовательность для взятия в руки оружия
bool Camera::takeWeapon(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)								// XXX возможно однажды эта функция будет использоваться и НПС-ми
{
	
	// собираюсь работать с руками и их скелетом с анимациями
	if( components.count("Player_Hands_MESH") != 0 && CHECK_PTR(components["Player_Hands_MESH"]->entitySkeletonPtr) )
	{
		float radius = 2.0f;
		
		std::shared_ptr<Weapon> wpn = std::dynamic_pointer_cast<Weapon>( level->getTaggedActorNearPoint("Weapon", position, radius) );
		
		//~ ///Weapon* wpn = dynamic_cast<Weapon*>( level->getTaggedActorNearPoint("Weapon", caster->position, radius) );
		
		if( wpn != nullptr && wpn->parentMicroWorld == nullptr && wpn->canBeTaken )		// XXX внимательнее с условием (wpn->parentMicroWorld == nullptr)
		{
			if(takeWeaponTimer == 0)
			{
				//~ std::cout << "Casted orphan weapon [" << wpn->MicroWorldName << "] in radius " << radius << " m around [" << MicroWorldName << "]" << std::endl;
				
				animStateConfig handsAnimation;
					handsAnimation.mixCoeff = 0.0f;
					handsAnimation.mask = {};
					handsAnimation.animName = wpn->takingPlayerAnimation;
					handsAnimation.repeatCount = 1;
					handsAnimation.blendingTimeIn = 500;
					
					
				animStateConfig SecondAnimation;
					SecondAnimation.mixCoeff = 0.0f;
					SecondAnimation.mask = {};
					SecondAnimation.animName = wpn->idlePlayerAnimation;
					SecondAnimation.repeatCount = -1;
					SecondAnimation.blendingTimeIn = 500;
					
				components["Player_Hands_MESH"]->setAnimation(0, handsAnimation, SecondAnimation );	
				
				
				//components["Player_Hands_MESH"]->setAnimation(0, { 0.0f, {}, wpn->takingPlayerAnimation, 1, 500 } , { 0.0f, {}, wpn->idlePlayerAnimation, -1, 500 } );
			}
			
			if(takeWeaponTimer > 1000 && !bPlayerhasWeapon)
			{					
				wpn->attachTo( this, "Player_Hands_MESH", "Bone.028", wpn->getSocketPositionPlayer(), wpn->getSocketRotationPlayerXYZW() ); //vec4(0.5317, -0.087, -0.089, 1), vec4(-0.272, 0.066, 0.156, 0.947));
				bPlayerhasWeapon = !bPlayerhasWeapon;
				
				wpn->components[ wpn->mainCollisionEntity ] -> disableCollision();	// при подборе оружию отключаем коллизии
				
				sound_takeWeapon->playSound( std::bind(&Camera::coolGunPhrase_start, this), std::bind(&Camera::coolGunPhrase_end, this)  );
				
				takeWeaponTimer = 0;
				//~ printMessage("taken!")
				return true;
			}
			
			// XXX нижеприведенные функции могут не выполнится после привязки ствола
			// if(caster->takeWeaponTimer > 4000 )//caster->components["Player_Hands_MESH"]->entitySkeletonPtr->sklAnimations[wpn->takingPlayerAnimation]->getDuration()*1000 + 500-tick)
			// {
				// caster->takeWeaponTimer = 0;
				// printMessage("taken!")
				// return true;
			// }
			// else
			// {
				takeWeaponTimer += tick;
				return false;
			// }
			
		}
		else
		{
			//~ printMessage("Cast failed. Maybe weapon already have parent")
			//~ DEBUG_INFO
			return true;
		}
		
	}
	else 
	{
		printMessage("No Player_Hands_MESH or no skeleton for it!")
		DEBUG_INFO
		return true;												// в случае неудачи сразу отвязываем
	}	
	
}

void Camera::coolGunPhrase_start(){}
void Camera::coolGunPhrase_end()
{
	if(!colgunSayed)
	{
		colgunSayed = true;
		sound_CoolGun->playSound( [](){}, [](){} );
	}
}
	

void Camera::takeWeapon_start()
{
	
}

void Camera::takeWeapon_end()
{
	takeE = false;
}



bool Camera::throwWeapon(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
{
		if( components.count("Player_Hands_MESH")!=0 && CHECK_PTR(components["Player_Hands_MESH"]->entitySkeletonPtr) ) 
		{
			if(throwWeaponTimer == 0)
			{
				//printMessage("MicroWorldup change animation")
				
				animStateConfig handsAnimation;
					handsAnimation.mixCoeff = 0.0f;
					handsAnimation.mask = {};
					handsAnimation.animName = "Hands_Change_Item";
					handsAnimation.repeatCount = 1;
					handsAnimation.blendingTimeIn = 500;
					
					
				animStateConfig SecondAnimation;
					SecondAnimation.mixCoeff = 0.0f;
					SecondAnimation.mask = {};
					SecondAnimation.animName = "Hands_Idle";
					SecondAnimation.repeatCount = -1;
					SecondAnimation.blendingTimeIn = 500;
					
				components["Player_Hands_MESH"]->setAnimation(0, handsAnimation, SecondAnimation );	
				
				//components["Player_Hands_MESH"]->setAnimation(0, { 0.0f, {}, "Hands_Change_Item", 1, 500 } , { 0.0f, {}, "Hands_Idle", -1, 500 } );
			}
			
			if(throwWeaponTimer > 1000 && bPlayerhasWeapon)
			{			
				if( CHECK_PTR(childMicroWorld ) )
				{	
					////caster->childMicroWorld->MicroWorldDetach();
					
					//~ printMessage("detach weapon")
					Weapon* wpn = dynamic_cast<Weapon*>(childMicroWorld);
					if(wpn!=nullptr)
					{
						wpn->components[ wpn->mainCollisionEntity ] -> enableCollision(); // при выбрасывании оружию включаем коллизии
					}
					
					childMicroWorld->detach();
					
					bPlayerhasWeapon = !bPlayerhasWeapon;
				}		
			}
			
			if(throwWeaponTimer > 4000 )//caster->components["Player_Hands_MESH"]->entitySkeletonPtr->sklAnimations["Hands_Change_Item"]->getDuration()*1000 + 500-tick)
			{
				throwWeaponTimer = 0;
				//~ printMessage("throwed!")
				return true;
			}
			else
			{
				//~ printVariable(takeWeaponTimer)
				throwWeaponTimer += tick;
				return false;
			}			
		}
		else
		{
			printMessage("No Player_Hands_MESH or no skeleton for it! throwWeapon()")
			return true;
		}		
}

void Camera::throwWeapon_start()
{
	
}

void Camera::throwWeapon_end()
{
	throwG = false;
}


//~ #include "Shader.cpp"
//~ #include "auxiliary.cpp"
//~ #include "Mesh.cpp"
//~ #include "Skeleton.cpp"
//~ #include "CollisionCalculator.cpp"
//~ #include "Entity.cpp"
//~ #include "MicroWorld.cpp"

//~ int main()
//~ {
	
	//~ MicroWorld* cam = new Camera("MicroWorld");
	
	//~ if( cam->getStatus() )
	//~ {
		//~ printMessage("MicroWorld load successfull")
	//~ }
	//~ else
	//~ {
		//~ printMessage("Fail to load MicroWorld")
	//~ }
	
//~ }
