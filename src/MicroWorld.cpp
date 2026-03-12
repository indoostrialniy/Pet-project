#include "MicroWorld.h"
#include "macros.h"
#include "Entity.h"
#include "CollisionDescriptor.h"
#include "CollisionShell.h"

#include "Level.h"
#include "Skeleton.h"
#include "auxiliary.h"

#include <cmath>


MicroWorld::MicroWorld() 
{
	//~ if( glewInit() != GLEW_OK)	// первым делом при создании ресурса, использующего openGL, проверим, инициализирован ли он
	//~ {
		//~ std::cout << "OpenGL context not exist until now. Create!"; DEBUG_INFO
		//~ initGL();
	//~ }
}

MicroWorld::MicroWorld( std::string n ) 
{
	MicroWorldName = n;
}								// конструкторы и деструкторы априори должны быть публичными


MicroWorld::MicroWorld(glm::vec3 worldLoc, glm::quat worldRot, std::string n ) 
{
	position = worldLoc;
	rotation = worldRot;
	MicroWorldName = n;
}

void MicroWorld::reset()
{
	position = startupPosition;
	rotation = startupRotation;
}


MicroWorld::~MicroWorld()
{
	//for(auto comp : components) { delete comp.second;}	//очистка всех Entity* через их собственные деструкторы
	components.clear();									//очистка словаря сущностей
	
	tags.clear();
	
	//if(MicroWorldPersonalLightSourcePtr != nullptr) { delete MicroWorldPersonalLightSourcePtr; MicroWorldPersonalLightSourcePtr = nullptr;}
}

bool MicroWorld::getStatus()
{
	return status;
}

bool MicroWorld::getVisibility()
{
	return visible;
}




void MicroWorld::MicroWorldUpdate()							
{
	#ifdef MICROWORLD_DEBUG
		printMessage("\n1 MicroWorldUpdate()")
	#endif
	
	// 1. функция для обеспечения привязки сетов к компонентам или корням других сетов
	attachMicroWorldToParent();				// там внутри обновляется собственный rotation
	
	m3VectorsForwardLeftUp = mat3_cast( rotation ) * m3DefaultVectorsForwardLeftUp;	// обязательно обновить матрицу векторов собственного пространства
	
	moveInDirectionWatchDog = 0;			// сбросить вачдог таймер (нужен при расчете коллизий в moveInDirection() )
	
	//printMessage("MicroWorldUpdate 1")
	
	// 2. обновление состояний и условий	
	update();								// запуск возможно дочерне переназначенной функции обновления условий
	
	//printMessage("MicroWorldUpdate 2")
	
	// // так как привязанный сет света не учтен в списке актеров уровня, то надо обновить его логику вручную
	// if(MicroWorldPersonalLightSourcePtr != nullptr) 
	// {
		// MicroWorldPersonalLightSourcePtr->MicroWorldUpdate(); //attachMicroWorldToParent();	
	// }
	
	
	// 3. реакция на обстоятельства
	logic();								// запуск функции реакции на изменившиеся условия
	
	//printMessage("MicroWorldUpdate 3")
	
	// 4. лишь после обновления комбо приходит черед обновлять его компоненты, т.к. они могут быть связаны с ним
	
	if(!components.empty())
	{
		
		#ifdef MICROWORLD_DEBUG
			printVarComment("Update components of combo: ", MicroWorldName)
		#endif
		
		
		for(auto MicroWorldUpdateComponent : components)		// 
		{
			if(CHECK_PTR(MicroWorldUpdateComponent.second))
			{
				MicroWorldUpdateComponent.second->updateState();
			}
		}
				
		for(auto MicroWorldUpdateComponent : components)		// запуск отрисовки всех компонентов сета
		{
			if(CHECK_PTR(MicroWorldUpdateComponent.second) && MicroWorldUpdateComponent.second->getVisibility() )
			{
				MicroWorldUpdateComponent.second->bRenderEntity();
			}
		}
		
		
	}
	//else {printVarComment("No components in MicroWorld: ", MicroWorldName)}
	
	
	prevPos = position;	// обновим значения предыдущей позиции
	prevRot = rotation;
	
	#ifdef MICROWORLD_DEBUG
		printVarComment("Combo updated: ", MicroWorldName)
	#endif
	
}

void MicroWorld::update()						// обновление состояний сета вызывается из функции MicroWorld::MicroWorldUpdate(). Может быть переопределено дочерне
{
	
}

void MicroWorld::logic()						// функция логики для сета вызывается из функции MicroWorld::MicroWorldUpdate(). Может быть переопределено дочерне
{
	
}


void MicroWorld::moveInDirection( std::string collisionEntityName, glm::vec3 directionVec, float movingSpeed )
{
	//~ printVarComment("\n\nmoveInDirection func iteration: ", moveInDirectionWatchDog)
	
	directionVec = glm::normalize(directionVec);
	
	if(0)
	{
		//~ printMessage("Collision disabled")
		//~ DEBUG_INFO
		position += directionVec * movingSpeed * 1.0f; // сдвигаем комбо в указанном направлении на заданную величину - 1 шаг
	}
	else
	////for(auto physicalEntity : components)								// переберем все компоненты актора
	{
		////if( physicalEntity.second->getCollision() )						// и из них возьмем только те, что с коллизиями
		{
			//printVariable(physicalEntity.second->entitySysname)
			position += directionVec * movingSpeed * 1.0f; // сдвигаем комбо в указанном направлении на заданную величину - 2 шага
		
			// XXX													vec3(0,0,0) это заглушка!
			bool cast = bCheckIntersections( collisionEntityName, glm::vec3(0,0,0), directionVec, v3Reflect, fSlideCoeff  );	// по сущности с оболочкой проверяем, не стали ли пересекать чью-либо чужую коллизию

			//~ printVec3(directionVec)
			//~ printVec3( v3Reflect )
			
			if(cast) // если пересекли кого-то
			{
				position -= directionVec * movingSpeed*1.0f; //сделаем 2 шага назад
				// и снова прокастуем в новом направлении через рекурсию
				glm::vec3 alongsideVector =  glm::length( directionVec + v3Reflect ) > 0.001f ? glm::normalize( directionVec + v3Reflect ) : directionVec; 			// не будем нормализовывать во избежание деления на ноль в случаях, когда вектор отражения противоположен направлению
				
				//moveDirection = glm::normalize(directionVec + v3Reflect);
				//~ printMessage("Casted!, print reflection: ")
				//~ printVec3( alongsideVector )
				
				
				//~ position += alongsideVector * movingSpeed * fSlideCoeff; // сдвигаем комбо в указанном направлении на заданную величину - 2 шага
				//~ float tempK = 1.0f;
				//~ glm::vec3 tempV = glm::vec3(0,0,1);
				//~ bool cast2 = bCheckIntersections( collisionEntityName, position, alongsideVector, tempV, tempK  );
				//~ if( cast2 )
				//~ {
					//~ position -= alongsideVector * movingSpeed * fSlideCoeff; 
				//~ }
				if( moveInDirectionWatchDog <= 0 && directionVec != glm::vec3(0,0,-1) )
				{
					moveInDirectionWatchDog++;
					moveInDirection( collisionEntityName, alongsideVector, movingSpeed );
					
				}
				else
				{
					//~ printMessage("Watchdog cast limit reached")
				}
				
			}
			//~ else // если основной каст ничего не дал, то...
			//~ {
				//~ position -= directionVec * movingSpeed; //сделав шаг назад, итого переместимся на 1 шаг вперед
			//~ }
			
		}
		
		
	}
}	





glm::vec3 MicroWorld::getPosition() 	
{
	return position;
}
	
glm::quat MicroWorld::getRotation()	
{
	return rotation;
}

void MicroWorld::setPosition(glm::vec3 worldLoc) 	
{ 
	position = worldLoc;
}

void MicroWorld::setRotation(glm::quat worldRot)	
{ 
	rotation = worldRot;
}


bool MicroWorld::bIsTargetInRadius( std::string targetComboName, float radius)
{
	if(level->getActor( targetComboName ) != nullptr )	//actors.count(targetComboName) != 0 )
	{
		return glm::length(level->getActor(targetComboName)->getPosition() - position) < radius ? true : false;
	}
	else
	{
		return false;
	}
}



void MicroWorld::addTag(std::string tag) 
{ 
	tags.push_back(tag); 
}

bool MicroWorld::hasTag(std::string tag)							// публичный метод, чтобы проверить наличие тега
{
	//bool res = false;

	for (auto t : tags)
	{
		if (t == tag)
		{
			return true;
		}
	}
	//cout << "Check for having tag: " << tag << ": " << res << endl;
	return false;
}

void MicroWorld::removeTag(std::string tag)
{
	
}

void MicroWorld::setEntityVisibility( const std::string& entityName, bool visibility = true)
{
	if( components.count(entityName) != 0 )
	{
		components[entityName]->setVisibility( visibility );
	}
	else
	{
		std::cout << "No entity [" << entityName << "]!"; DEBUG_INFO
	}
}

bool MicroWorld::getEntityVisibility( const std::string& entityName )
{
	if( components.contains(entityName) )
	{
		return components[entityName]->getVisibility();	//visible;
	}
	else
	{
		std::cout << "No entity [" << entityName << "]! Return false"; DEBUG_INFO
		return false;
	}
}

glm::vec3 MicroWorld::getEntityPosition( const std::string& entityName )
{
	if( components.count(entityName) != 0 )
	{
		return components[entityName]->entityPosition;
	}
	else
	{
		std::cout << "\nNo entity [" << entityName << "], return zero vec3!"; DEBUG_INFO		
		return glm::vec3(0,0,0);
	}
}

void MicroWorld::setComponentAnimation(	const std::string& entityName, int channelIndex, animStateConfig& newAnim, animStateConfig& secondAnim 	)
{
	if( components.count(entityName) != 0 )
	{
		//~ printVarComment("Class: ", MicroWorldName)
		//~ printVarComment("\tSet anim to component: ", entityName)
		components[entityName]->setAnimation(0, newAnim, secondAnim	);	
		//~ DEBUG_INFO
	}
	else
	{
		std::cout << "\nNo entity [" << entityName << "]!"; DEBUG_INFO		
	}
}



bool MicroWorld::bCheckIntersections( std::string componentName, glm::vec3 origin, glm::vec3 direction, glm::vec3& slide, float& slideK )
{
	// функция определяет, с кем пеесекается сущность с именем componentName при условии, что ее родительское комбо направлено direction и находится в origin
	
	//~ printMessage("\n")
	
	if( components.count( componentName ) != 0 )						// проверили, что такая сущность вообще существует
	{
		bool intersect = false;
		
		for(auto actor : level->actors)									// перебираем ВСЕ комбо на уровне
		{
			if( actor.second->MicroWorldName != MicroWorldName )		// исключаем из списка самого себя			//if( actor.second->getVisibility() && actor.second->MicroWorldName != MicroWorldName ) // переберем всех актеров, за исключением ЭТОГО САМОГО
			{	// т.к. локально сущность внутри своего сета может быть сдвинута очень сильно, то нецелесообразно пихать проверку на дальность до КОМБО сюда. Придется перебирать именно все сущности, благо их тоже можно быстро просеять					
				
				for(auto component : actor.second->components)			// перебираем сущности очередного комбо
				{
					//~ printMessage("All collisions disabled!") DEBUG_INFO
					//~ return false;
					
					if( component.second->getCollision() )				// если у сущности есть активная коллизия
					{
						float radiusSum = components[componentName]->getCollisionRadius() + component.second->getCollisionRadius();
						if( radiusSum < 0 )		// если вдруг сумма радиусов меньше нуля
						{
							printMessage("Error! Soomething went wrong: One of the entity`s collisions is broken, < 0")
							DEBUG_INFO
							continue;
						}
						
						radiusSum*=10.0f;
						
						if( glm::length( components[componentName]->getWorldPosition() - component.second->getWorldPosition() ) < radiusSum )	// и расстояние между сущностями меньше суммы их радиусов оболочек 
						{
							
							
							//component.second->getCollisionability() && 		// если у компонента чужого сета активирован просчет коллизий
							 //~ && // collisionDescriptor.collisionActive && //collisionType != 0 && // и есть оболочка коллизии какого-либо типа
							 
							//~ )
							
							
							 //~ printVarComment("Check: ", component.first  )
							// добрались до сущности чужого комбо, которая имеет оболочку, активна и может быть пересечена
							
							std::shared_ptr<Collision> AABB = std::dynamic_pointer_cast<Collision> ( components[componentName] ); // кастуем компонент entity к родительскому классу collision
							
							intersect = component.second->bCheckAnotherAABB( AABB, origin, direction, slide, slideK ); // проверим, не пересекается ли она с оболочкой ЭТОГО САМОГО
							
							if( intersect )
							{
								//~ std::cout << "Detected collision of " << components[componentName]->entityName << " (class " << MicroWorldName << ") with " << component.second->entityName  << " (class " << actor.second->MicroWorldName << ")!" << std::endl;
								//printMessage("\n\nCOLLISION DETECTED\n\n")
								return true;
							}
						}
						
						
						

					}
					
				}
			}
			
			
			
		}
		return intersect;
	}
	else
	{
		printVarComment("Cannot found component entity: ", componentName)
		DEBUG_INFO
		return false;
	}
}



void MicroWorld::attachMicroWorldToParent()
{
	// если родитель назначен, то привязка будет производиться по мере указания аргументов 
	// parentMicroWorld -> component -> skeleton -> bone
	// просчет сокета выполняется методами класса Skeleton, а остальные преобразования - в данной функции
	//~ auto parentMW = parentMicroWorld.lock();
	
	if( parentMicroWorld != nullptr)													// явно указан родительский сет
	{
		// Entity* parentEntity = parentMicroWorld->getEntity( parentMicroWorldComponentName );
		
		// 
		
		glm::vec4 socketQuaternionXYZW = ownQuatXYZW; //x y z w
		glm::vec4 socketCoordinates = ownCoordXYZ;
		
		if( parentMicroWorld->components.count( parentMicroWorldComponentName ) != 0) 											// явно указан компонент родительского сета
		{
			std::shared_ptr<Entity> parentEntity = parentMicroWorld->components[parentMicroWorldComponentName];
			
			if(parentEntity->entitySkeletonPtr &&
				parentEntity->entitySkeletonPtr->boneIndexesMap.count(parentMicroWorldComponentSocketName) != 0	)	// явно указан скелет и он содержит кость
			{
				//привязка к конкретной кости компонента родительского сета
			
				// 1 - вычисление итоговой точки по скелету
				// calculateBranchRotation11 позволяет вычислить итоговую точку после прогона по скелету, который, считается, начинается в начале координат
				socketCoordinates = parentEntity->entitySkeletonPtr->calculateBranchRotation11(parentMicroWorldComponentSocketName , socketCoordinates , parentEntity->sklPose);
				
				// 2 - приведение к мировой системе координат
				// так как в сете Камера скелет (руки) имеет смещение относительно координат сета, надо добавить это смещение к вычисленным по скелету координатам
				glm::vec4 parentComponentOffMicroWorld = glm::vec4(parentEntity->getLocalPosition(), 0.0f);
				socketCoordinates += parentComponentOffMicroWorld;
				
				// теперь пистолет лежит в руке четко и можно добавить вращение родительского сета (parentMicroWorld->worldRotation)
				socketCoordinates = parentMicroWorld->getRotation() * socketCoordinates;
				
				// а после вращения родителем осталось добавить смещение родителя
				glm::vec4 parentOffMicroWorld = glm::vec4(parentMicroWorld->getPosition(), 0.0f);
				//~ printVariable(level->actors["Camera"]->MicroWorldName)
				//~ printVec3( level->actors["Camera"]->getPosition() )
				//~ printVec4(parentOffMicroWorld)
				socketCoordinates += parentOffMicroWorld;
				
				// 3 - вычисление итогового кватерниона
				// просчитаем кватернион по ветке костей
				socketQuaternionXYZW = parentEntity->entitySkeletonPtr->calculateBranchRotationNormal(parentMicroWorldComponentSocketName, socketQuaternionXYZW, parentEntity->sklPose); //вкидываем индекс начальной кости и кватернион сокета				
			}
			else
			{
				// скелет не указан либо не имеет требуемой кости, привязка будет к компоненту родительского сета - entityLocation и entityRotation (с учетом worldLocation и worldRotation!)
				socketCoordinates += glm::vec4(parentEntity->getLocalPosition(), 1);
				socketCoordinates = parentMicroWorld->rotation * socketCoordinates;
				socketCoordinates += glm::vec4(parentMicroWorld->position, 0.0f);
			}
		}
		else
		{
			// компонент не указан, привязка будет к корню родительского сета - worldLocation и worldRotation с параметрами сокета
			socketCoordinates = parentMicroWorld->getRotation() * socketCoordinates;
			socketCoordinates += glm::vec4(parentMicroWorld->getPosition(), 0.0f);
		}
		// так или иначе вычислив новые socketCoordinates и socketQuaternionXYZW, выводим их обратно
		position = socketCoordinates;	// вычисленные координаты сета, привязанного к указанному сокету parentMicroWorldComponentSocketName компонента parentMicroWorldComponentName
		rotation = parentMicroWorld->rotation * vec4CastQuat(socketQuaternionXYZW);	// вычислив кватернион сокета по скелету, добавим к нему вращение родительского сета
		
		//~ std::cout << MicroWorldName << " position:"; printVec3(position)
		//~ std::cout << MicroWorldName << " rotation:"; printQuat(rotation)
	}
	else	{	/* никаких привязок не осуществляется*/		}

}



bool MicroWorld::attachTo( MicroWorld* parent, std::string componentName, std::string socketName, glm::vec4 socketCoordXYZ, glm::vec4 socketQuatXYZW)
{
	// ??? std::shared_ptr<MicroWorld> casted = std::dynamic_pointer_cast<MicroWorld>( parent );
	// IF_DYNAMIC_CAST(MicroWorld*, casted, parent) // можно дополнительно кастануть
	
	if( CHECK_PTR(parent) )												// проверили родителя
	{
		parentMicroWorld = parent;										// этому комбо прописали родительский комбо
		
		parentMicroWorldComponentName = componentName;					// далее прописали имена компонентов и ориентацию сокета
		parentMicroWorldComponentSocketName = socketName;
		ownCoordXYZ = socketCoordXYZ;
		ownQuatXYZW = socketQuatXYZW;
		
		parent->childMicroWorld = this;									// родительскому комбо прописали этот this как дочерний
		
		return true;
	}
	else
	{
		std::cout << "Fail to attach [" << MicroWorldName << "] to parent" << std::endl; 
		DEBUG_INFO
		return false;
	}
}

bool MicroWorld::detach()
{
	//auto PARENT = parentMicroWorld.lock();
	if (CHECK_PTR(parentMicroWorld) && CHECK_PTR(parentMicroWorld->childMicroWorld) )// != nullptr)
	{
		//~ printMessage("detaching!")
		//в зависимости от того, является ли родитель игроком, конечная ориентация отвязываемого оружия будет разная
		glm::quat dovorot = parentMicroWorld->hasTag("Player") ? glm::quat(0.707f, -0.707f, 0.0f, 0.0f) : rotation;
		//z +90
		// выбрасываемый сет будет иметь азимут как у камеры на момент выбразывания
		rotation = dovorot;

		//и находиться у самой "земли"
		//~ auto chk = [](){ if(CHECK_PTR(CameraAxisPtr)) { return (*CameraAxisPtr)[0];} else {return glm::vec3(0,0,0);} };
		auto chk = [](){ return glm::vec3(0,0,0); };
		
		
		position += parentMicroWorld->hasTag("Player") ? (chk())*0.2f : glm::vec3(0, 0, 0);
		//position[2] = 0.1;
		//worldLocation.z = parentMicroWorld->MicroWorldHasTag("Player") ? 0.15 : worldLocation.z;

		//~ printMessage("Flag x1")
		//у родителя отвязываем child
		//~ std::shared_ptr <MicroWorld> empty;
		parentMicroWorld->childMicroWorld = nullptr;
		//~ printMessage("Flag x2")
		//отвязываем самого родителя от текущего сета
		//~ std::shared_ptr <MicroWorld> empty2;
		parentMicroWorld = nullptr;
		//~ printMessage("Flag x3")
		parentMicroWorldComponentName = "none";
		parentMicroWorldComponentSocketName = "none";
		ownCoordXYZ = glm::vec4(0, 0, 0, 0);
		ownQuatXYZW = glm::vec4(0, 0, 0, 1);

		

		return true;
	}
	else { std::cout << "No parent for MicroWorld [" << MicroWorldName << "]" << std::endl; DEBUG_INFO return false;}
}



//~ void MicroWorld::addComponentToMap( std::string name, std::shared_ptr<Entity> objectPtr )
//~ {
	//~ printMessage("Adding component")
	//~ DEBUG_INFO
	
	//~ printVarComment("Check object ptr:", &objectPtr )
	//~ printVarComment("Check map ptr:", &components )

	//~ printVarComment("Check map size:", components.empty() )
	//~ components[name] = objectPtr;
	
	//~ printMessage("Ok")

//~ }

bool MicroWorld::addEntity( const entityInitializationParameters& newEntity )
{
	#ifdef MICROWORLD_DEBUG
		printVarComment("Adding entity new-style", newEntity.initSysname)
		printVec3(newEntity.initPos)
		printQuat(newEntity.initRot)
	#endif
		
	
	// передадим полученную инициализационную структуру newEntity  в виде rvalue в конструктор класса сущности
	std::shared_ptr<Entity> rookie = std::make_shared<Entity>(	newEntity, position, rotation, moveDirection ) ;
	

	if ( rookie && rookie->getStatus() ) // если статус сконфигурированной сущности - да, то занесем ее в словарь компонентов
	{ 
		//~ printMessage("try to add entity")
		//~ DEBUG_INFO
		
		// XXX тут ошибка!
		//components["rookie_entityName"] = rookie; // ERROR!!!	в случае успешного создания заносим в список компонентов комбо
		components.insert( {newEntity.initSysname, rookie} );
		
		#ifdef MICROWORLD_DEBUG
			printMessage("Created successfully: ")	//, newEntity.initSysname) DEBUG_INFO
		#endif
		return true;
	}		
	else 
	{ 
		printVarComment("Fail to create entity, delete pointer to entity: ", newEntity.initSysname) 
		DEBUG_INFO
		return false;
	}
}


bool MicroWorld::addEntity(std::string initSysname, glm::vec3 initPos, glm::quat initRot, std::string initModelName, CollisionDescriptor initCollisionType, std::string initType, std::vector <std::string> initMaterialsNames, std::string lightmapName, std::string initShaderName)	// метод, чтобы добавить сущность в сет
{
	#ifdef MICROWORLD_DEBUG
		printVarComment("Adding entity old-style", initSysname)
		printVec3(initPos)
		printQuat(initRot)
	#endif
	
	// XXX надо в сущность передать позицию и ориентацию комбо
	
	std::shared_ptr<Entity> rookie = std::make_shared<Entity>(initSysname, initPos, initRot, position, rotation, moveDirection, initModelName, initCollisionType, initType, initMaterialsNames, lightmapName, initShaderName); //создаем сущность с аргументированными параметрами

	if ( rookie->getStatus() ) 
	{ 
		//~ printVarComment("Try to write entity ", initSysname)
		//~ rookie->setComboPositionPtr( &position );	//теперь указатель в сущности хранит знает, где посмотреть позицию и ориентацию комбо
		//~ rookie->setComboRotationPtr( &rotation );
		
		//~ if( rookie->entityCollisionPtr )
		//~ {
			//~ rookie->entityCollisionPtr->setEntityPositionPtr( &rookie->entityPosition );
			//~ rookie->entityCollisionPtr->setEntityRotationPtr( &rookie->entityRotation );
			//~ rookie->entityCollisionPtr->setComboPositionPtr( &position );
			//~ rookie->entityCollisionPtr->setComboRotationPtr( &rotation );
		//~ }
		
		//addComponentToMap( initSysname, rookie );

		
		//components[initSysname] = rookie; //в случае успешного создания заносим в список компонентов комбо
		components.insert( {initSysname, rookie} );
		//~ DEBUG_INFO

		return true;
	}		
	else 
	{ 
		printVarComment("Fail to create entity, delete pointer to entity: ", initSysname) 
		DEBUG_INFO
		return false;
	}
}
	
	
	
bool MicroWorld::initGL()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	GLFWwindow* engineWindow = glfwCreateWindow(800, 600, "local", nullptr, nullptr);

	if (engineWindow == nullptr) { std::cout << "Failed to create GLWF window" << std::endl; glfwTerminate(); return false; }

	glfwMakeContextCurrent(engineWindow);


	if (glewInit() != GLEW_OK) { std::cout << "Failed to initialize GLEW" << std::endl; return false; }
	else 
	{ 
		if (0) { std::cout << "\tGAPI bWallBreachInitialized!\n"; }	
		return true; 
	}
}



std::shared_ptr<MicroWorld> MicroWorld::traceRay( const glm::vec3& traceDirectionWS )
{
	// тут надо прокастовать пространство по направлению луча из ствола и определить, в кого попали
		
	float nearest = 1000.0f;
	
	std::shared_ptr<MicroWorld> nearestCombo;
	//~ printMessage("Trace ray")

	for(auto actor : level->actors)									// перебираем ВСЕ комбо на уровне
	{
		if( actor.second->MicroWorldName != MicroWorldName || ( actor.second->MicroWorldName != MicroWorldName && parentMicroWorld != nullptr && actor.second->MicroWorldName != parentMicroWorld->MicroWorldName ) )		// исключаем из списка самого себя	и оболочку хозяина
		{	// т.к. локально сущность внутри своего сета может быть сдвинута очень сильно, то нецелесообразно пихать проверку на дальность до КОМБО сюда. Придется перебирать именно все сущности, благо их тоже можно быстро просеять					
			
			//~ printVarComment("Check actor: ", actor.second->MicroWorldName)
			for(auto component : actor.second->components)			// перебираем сущности очередного комбо
			{
				//~ printVarComment("\tCheck component: ", actor.second->MicroWorldName)

				if( component.second->getCollision() )				// если у сущности есть активная коллизия
				{
					float radiusSum = components[mainCollisionEntity]->getCollisionRadius() + component.second->getCollisionRadius();
					if( radiusSum < 0 )		// если вдруг сумма радиусов меньше нуля
					{
						printMessage("Error! Soomething went wrong: One of the entity`s collisions is broken, < 0")
						DEBUG_INFO
						continue;
					}
					
					radiusSum*=10.0f;
					
					if( glm::length( components[mainCollisionEntity]->getWorldPosition() - component.second->getWorldPosition() ) < radiusSum )	// и расстояние между сущностями меньше суммы их радиусов оболочек 
					{	// вот и добрались до сущности чужого комбо, которая имеет оболочку, активна и может быть пересечена
						
						std::shared_ptr<Collision> AABB = std::dynamic_pointer_cast<Collision> ( component.second ); // кастуем компонент entity к родительскому классу collision
						
						if(AABB->collisionDescriptor.collisionType == COMPLEX_COLLISION)	// если чужая оболочка - сложная
						{
							//~ printVarComment("Check: ", AABB->entitySysname)
							
							for(int ot=0; ot<AABB->collisionDescriptor.complexCollisionShellPtr->getPolygonsCount(); ot++)
							{
								
								glm::vec3 worldPosition = getPosition();
								glm::vec3 worldView = traceDirectionWS;	//m3VectorsForwardLeftUp[0];
								
								glm::vec3 a = worldPosition + worldView;
								glm::vec3 b = worldPosition;
								
								//~ printVec3( worldPosition )
								//~ printVec3( worldView )
								
								//~ printMessage("Local: ")
								glm::vec3 localPosition = AABB->getPointLocalSpacePosition ( worldPosition );
								a = AABB->getPointLocalSpacePosition( a  );
								b = AABB->getPointLocalSpacePosition( b );
								glm::vec3 localView = a - b;
									
								PointTriangleArgumentsPack args;
									args.point =  localPosition;	
									args.direction = localView;
									args.triangle_index = ot;
								AABB->pointToTriangleRelativeCalculator( args );
								// printVarComment("Relative position ", args.rayRelativeDirection)

								if(args.rayInside && args.t_param_Ray > 0 && args.rayRelativeDirection == 1)
								{
									
									if(args.t_param_Ray < nearest)
									{
										nearest = args.t_param_Ray;
										args.rayIntersectionCoordinates = AABB->getPointWorldSpacePosition( args.rayIntersectionCoordinates );
										//~ printVec3(args.rayIntersectionCoordinates)
										//~ printVarComment("Distance to hole ", args.rayIntersectionLength )
										//~ printVarComment("Penetrated triangle ", args.triangle_index)
										//~ printVarComment("Dir ", args.t_param_Ray )
										
										//~ level->actors["Debugger"]->components["Mark_09"]->setWorldPosition( args.rayIntersectionCoordinates  );	
										
										//~ printVarComment("\n Cast something! ", actor.second->MicroWorldName)
										
										nearestCombo = actor.second;
										//~ return actor.second;
										//~ std::shared_ptr<Monster> monster = dynamic_pointer_cast<Monster>(actor.second);	// обратимся к комбо монстра
										//~ if(monster)
										//~ {
											//~ monster->takeDamage(damage);
										//~ }
										
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	return nearestCombo;
}


// for biCycle 1.3
bool MicroWorld::focuse(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
{		
	focuseTimer += tick;

	float mix = (float)focuseTimer/(float)data.focuseDuration;	// определим текущий коэффициет поворота
				
	if(mix>1.01f)
	{
		focuseTimer = 0;								
		return true;
	}
	else
	{
		setPosition( glm::mix( data.currentPoint, data.focusedPoint, mix ) );
		setRotation( glm::slerp( data.currentRot, data.focusedRot, mix ) );
		//~ setRotation( glm::mix( data.currentRot, data.focusedRot, mix ) );
		return false;
	}
}



glm::quat MicroWorld::getQuaternionFocusedToPoint( const glm::vec3 point )	// XXX вроде как ненужная пока функция
{
	// point - координаты точки в мировом пространстве, куда надо навести это комбо
	glm::vec3 a = glm::normalize( m3DefaultVectorsForwardLeftUp[0] );
	glm::vec3 b = glm::normalize( point-position );
	
	glm::vec3 axis = glm::cross( a, b );	// ось вращения
	axis = glm::normalize(axis);
	//~ printVec3(a)
	//~ printVec3(b)
	//~ printVec3(axis)
	
	float cos_y = glm::dot( a, b ) / ( glm::length(a) * glm::length(b) );	// косинус угла между векторами
	
	
	#ifdef __linux__
		return glm::angleAxis( std::acos(cos_y), axis);
	#endif

	#ifdef _WIN32
		return glm::angleAxis( std::acosf(cos_y), axis);
	#endif	
}
	

glm::quat MicroWorld::getHorizontalQuaternionFocusedToPoint( const glm::vec3 point )
{
	//~ printMessage("\n getHorizontalQuaternionFocusedToPoint")

	// point - координаты точки в мировом пространстве, куда надо навести это комбо
	glm::vec3 a = glm::normalize( m3DefaultVectorsForwardLeftUp[0] );
	glm::vec3 b = glm::normalize(point-position);
	
	b = glm::vec3(b.x, b.y, 0);
	
	glm::vec3 axis = glm::cross( a, b );	// ось вращения
	axis = glm::normalize(axis);
	
	//~ printVec3(a)
	//~ printVec3(b)
	//~ printVec3(axis)
	
	float cos_y = glm::dot( a, b ) / ( glm::length(a) * glm::length(b) );	// косинус угла между векторами
	
	//~ return glm::angleAxis( std::acosf(cos_y), axis);
	#ifdef __linux__
		return glm::angleAxis( std::acos(cos_y), axis);
	#endif

	#ifdef _WIN32
		return glm::angleAxis( std::acosf(cos_y), axis);
	#endif	
}


glm::quat MicroWorld::getVerticalQuaternionFocusedToPoint( const glm::vec3 point )	// XXX вроде как ненужная пока функция
{
	//~ printMessage("\n getVerticalQuaternionFocusedToPoint")
	// point - координаты точки в мировом пространстве, куда надо навести это комбо
	glm::vec3 a = glm::normalize( m3DefaultVectorsForwardLeftUp[0] );
	glm::vec3 b = glm::normalize(point-position);
	
	b = glm::vec3(0, 0, b.z);
	
	glm::vec3 axis = glm::cross( a, b );	// ось вращения
	axis = glm::normalize(axis);
	//~ printVec3(a)
	//~ printVec3(b)
	//~ printVec3(axis)
	
	float cos_y = glm::dot( a, b ) / ( glm::length(a) * glm::length(b) );	// косинус угла между векторами
	
	//~ return glm::angleAxis( std::acosf(cos_y), axis);
	#ifdef __linux__
		return glm::angleAxis( std::acos(cos_y), axis);
	#endif

	#ifdef _WIN32
		return glm::angleAxis( std::acosf(cos_y), axis);
	#endif	
}


	
	
void MicroWorld::focuseToSpecifiedPoint( const glm::vec3& POV, int duration, std::function<void()> start_clb = [](){}, std::function<void()> end_clb = [](){} )
{
	//~ printMessage("Focuse to specified world-space point:")
	//~ printVec3(POV)
	
	
	BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;
		datas.focuseDuration = duration;
		datas.currentPoint = getPosition();	//player->getPosition();
		datas.currentRot = getRotation();	//player->getRotation();
		datas.focusedPoint = getPosition();	//components["Gate_01"]->getPointWorldSpacePosition( glm::vec3(1.0482, 0.3313, 1.3922) );
		datas.focusedRot = getHorizontalQuaternionFocusedToPoint( POV );	// WTF, но вроде работает нормально
		//getVerticalQuaternionFocusedToPoint( point );
		
	// запустим приближение				
	std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function 	= std::bind( &MicroWorld::focuse, 	this, datas);	// фокусирующая функция
	//~ std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> start;// 		= std::bind( &Gate_01::approachingForOpeningGate_start, this, datas); 
	//~ std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> end;// 		= std::bind( &Gate_01::approachingForOpeningGate_end, this, datas) ; 
	
	Flow.CallSequence( { std::move(function), std::move(start_clb), std::move(end_clb), std::move(datas) } );
	
}

void MicroWorld::goAndFocuseToSpecifiedPoint( const glm::vec3& POV, const glm::vec3& pos, int duration, std::function<void()> start_clb = [](){}, std::function<void()> end_clb = [](){} )
{
	printMessage("Focuse to specified world-space point at position:")
	printVec3(POV)
	printVec3(pos)
	
	BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;
		datas.focuseDuration = duration;
		datas.currentPoint = getPosition();	//player->getPosition();
		datas.currentRot = getRotation();	//player->getRotation();
		datas.focusedPoint = pos;	//components["Gate_01"]->getPointWorldSpacePosition( glm::vec3(1.0482, 0.3313, 1.3922) );
		datas.focusedRot = getHorizontalQuaternionFocusedToPoint( POV );	// WTF, но вроде работает нормально
		//getVerticalQuaternionFocusedToPoint( point );
		
	// запустим приближение				
	std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function 	= std::bind( &MicroWorld::focuse, 	this, datas);	// фокусирующая функция
	//~ std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> start;// 		= std::bind( &Gate_01::approachingForOpeningGate_start, this, datas); 
	//~ std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> end;// 		= std::bind( &Gate_01::approachingForOpeningGate_end, this, datas) ; 
	
	Flow.CallSequence( { std::move(function), std::move(start_clb), std::move(end_clb), std::move(datas) } );
	
}
	
	
//~ #include "Shader.cpp"
//~ #include "auxiliary.cpp"
//~ #include "Mesh.cpp"
//~ #include "Skeleton.cpp"
//~ #include "CollisionCalculator.cpp"
//~ #include "Entity.cpp"






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
