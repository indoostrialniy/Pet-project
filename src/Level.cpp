#include "Level.h"

#include "MicroWorld.h"
#include "macros.h"
#include "auxiliary.h"

#include <fstream>  //ifstream
#include "Entity.h"

Level::Level()
{
	#ifdef LEVEL_DEBUG
			std::cout << "Level constructor" << std::endl;
	#endif
}

Level::~Level()
{
	actors.clear();	 // очистка словаря приведет к удалению умных указателей, а значит, взовет их деструкторы								
}

void Level::levelUpdate()
{
	// 0. главная функция игры, вызывается в каждом цикле mainLoop, здесь устанавливаем порядок обновлений компонентов, цели рендера и прочее
	//~ printMessage("levelUpdate 1")
	
	// 1. вызов виртуальной функции, которая может быть переназначена в дочерних классах. Тут обязательный уровневый функционал
	update();	
	//~ printMessage("levelUpdate 2")
	
	// 2. если явно указан игрок, то обновим его в первую очередь
	if( actors.contains("Camera") )	
	{
		//~ printMessage("has actor camera")
		actors["Camera"]->MicroWorldUpdate(); //printMessage("update() camera")	DEBUG_INFO 
	}
	//~ printMessage("levelUpdate 3")
	
	// 3. обновление сетов запускаем после обновления уровня, т.к. на уровне может случаться что-то, влияющее на поведение сетов
	if(!actors.empty())													// проверим, что вообще есть акторы
	{
		for(auto act:actors)											// переберем все акторы. если статус создаваемого актора - false, то он просто не появится в actors!
		{
			if(act.second->MicroWorldName != "Camera")
			{
				act.second->MicroWorldUpdate(); 						// запуск главной функции сетов, из которой уже идет более мелкое обновление
			}
		}
	}
	else {printMessage("No actors on level!")}
}



std::shared_ptr<MicroWorld> Level::getActor(const std::string actorName)
{
	std::shared_ptr<MicroWorld> res;								// создадим новый shared_ptr для хранения ресурса
	
	if( actors.count(actorName) != 0 )									// если найден дубликат
	{
		#ifdef LEVEL_DEBUG
			std::cout << "\n\n\tObject [" << actorName << "] already has an instance, return shared_ptr reference." << std::endl;
		#endif
		
		res = actors[actorName];//.lock();									// отметим, что старого ресурса стало больше
	}
	else
	{
		std::cout << "\n\n\tNo actor [" << actorName << "] found! Return empty smartpointer. "; DEBUG_INFO
	}
	return res;																// и вернем указатель на запрашиваемый ресурс
}


std::shared_ptr<MicroWorld> Level::getTaggedActorNearPoint(const std::string& tagName, const glm::vec3& location, const float& radius)
{
	std::shared_ptr<MicroWorld> res;								// создадим новый shared_ptr для хранения ресурса
	
	bool found = false;
	for(auto act : actors)
	{
		//~ // XXX
		if( act.second->hasTag(tagName) && glm::length(act.second->getPosition() - location) < radius )
		{
			res = actors[act.first];//.lock();
			found = true;
			break;
		}
	}
	//~ if(!found) { printVarComment("No actors found with tag ", tagName) DEBUG_INFO }
	
	return res;																// и вернем указатель на запрашиваемый ресурс
}
	

void destroyActor(const std::string actorName)
{
	//~ if( actors.count(actorName) != 0 )
	//~ {
		//~ // XXX так как актер может быть присоединен к кому либо, надо грамотно отвязать его
		//~ actors[actorName]->MicroWorldDetach();
		
		//~ delete actors[actorName];
		//~ actors.erase(actorName);
		//~ printVarComment("Actor deleted: ", actorName)
	//~ }
}

//~ template <typename T>
//~ bool Level::spawnActor( std::string comboName, glm::vec3 pos, glm::quat rot)
//~ {
	//~ //printVarComment("spawnActor ", comboName)
	//~ if( actors.count(comboName) == 0)
	//~ {
		//~ auto newActor = std::make_shared<T>( pos, rot, comboName );

		//~ if( newActor->getStatus() )
		//~ {
			//~ //printVarComment("Spawn actor OK: ", comboName)
			//~ //DEBUG_INFO
			//~ actors[comboName] = newActor;
			//~ return true;
		//~ }
		//~ else
		//~ {
			//~ printVarComment("Fail to spawn actor. Error configuration ", comboName)
			//~ DEBUG_INFO
			//~ return false;
		//~ }
	//~ }
	//~ else
	//~ {
		//~ printVarComment("Fail to spawn actor. Already exists: ", comboName)
		//~ DEBUG_INFO
		//~ return false;
	//~ }
//~ }


//~ template <typename T>
//~ bool Level::spawnActor( std::string comboName )
//~ {
	//~ //printVarComment("spawnActor ", comboName)
	//~ if( actors.count(comboName) == 0)
	//~ {
		//~ auto newActor = std::make_shared<T>(comboName);
		//~ if( newActor->getStatus() )
		//~ {
			//~ actors[comboName] = newActor;
			//~ return true;
		//~ }
		//~ else
		//~ {
			//~ printVarComment("Fail to spawn actor. Error configuration ", comboName)
			//~ DEBUG_INFO
			//~ return false;
		//~ }
	//~ }
	//~ else
	//~ {
		//~ printVarComment("Fail to spawn actor. Already exists: ", comboName)
		//~ DEBUG_INFO
		//~ return false;
	//~ }
//~ }






//~ #include "Camera.h"
//~ class Scene_01;
//~ class Gate_01;
//~ class WallBreach_01;
//~ class Elevator;

// ЯВНАЯ ИНСТАНЦИАЦИЯ для всех нужных типов
//~ template bool Level::spawnActor<Camera>(const std::string&);
//~ template bool Level::spawnActor<Scene_01>(const std::string&);
//~ template bool Level::spawnActor<Gate_01>(const std::string&, const glm::vec3&, const glm::quat&);
//~ template bool Level::spawnActor<WallBreach_01>(const std::string&, const glm::vec3&, const glm::quat&);
//~ template bool Level::spawnActor<Elevator>(const std::string&, const glm::vec3&, const glm::quat&);
//~ template bool Level::spawnActor<Door_01>(const std::string&, const glm::vec3&, const glm::quat&);
//~ template bool Level::spawnActor<Electric_Box>(const std::string&, const glm::vec3&, const glm::quat&);
//~ template bool Level::spawnActor<Pistol>(const std::string&, const glm::vec3&, const glm::quat&);
//~ template bool Level::spawnActor<Box_01>(const std::string&, const glm::vec3&, const glm::quat&);
//~ template bool Level::spawnActor<Turniket_01>(const std::string&, const glm::vec3&, const glm::quat&);
//~ template bool Level::spawnActor<Orc>(const std::string&, const glm::vec3&, const glm::quat&);
//~ template bool Level::spawnActor<EXO_Soldier>(const std::string&, const glm::vec3&, const glm::quat&);
//~ template bool Level::spawnActor<Rifle>(const std::string&, const glm::vec3&, const glm::quat&);
//~ template bool Level::spawnActor<DynamicLight_01>(const std::string&, const glm::vec3&, const glm::quat&);
//~ template bool Level::spawnActor<Transporter>(const std::string&, const glm::vec3&, const glm::quat&);




std::string Level::getLevelPath(const std::string Name)	
{	
	return getOperationSystem() == "Windows" ? "datas\\" + Name + "" : "./datas/" + Name + "" ;		//тернарный определитель пути
}	





void Level::addComponentToMicroWorld( const std::string& actorName, const entityInitializationParameters& newEntity )
{	
	if(actors.contains(actorName))
	{
		#ifdef LEVEL_DEBUG
			printVarComment("Adding entity to actor: ", actorName)
		#endif
		
		actors[actorName]->addEntity( newEntity );
	}
	else
	{
		printVarComment("No actors found: ", actorName)
		DEBUG_INFO
	}
}


bool Level::loadLevel(bool log)
{
	// XXX actors["backgroundobjects"] = new MicroWorld("backgroundObjects");
	
	#ifdef LEVEL_DEBUG
		std::cout << "\nLoading level " << levelName << "...\n";		// << std::endl;
	#endif

	std::ifstream levelToLoadStream( getLevelPath(levelName) );

	if (levelToLoadStream.is_open())
	{
		std::string StrVar; //строкова¤ переменна¤ дл¤ хранени¤ строки текста из документа
		unsigned short int parametersFound = 0;

		while (getline(levelToLoadStream, StrVar)) //записываем в StrVar строки из файла, пока они есть
		{
			//циклом считали позицию камеры и ее поворот
			if (getValue(StrVar, 0) == "p")
			{
				if(actors.count("Camera") != 0)
				{
					glm::vec3 position2;
					readFVec3( position2, StrVar, 1 )
					actors["Camera"]->setPosition( position2 );
				}
				parametersFound++;
			}
			
			if (getValue(StrVar, 0) == "v")
			{		
				if(actors.count("Camera") != 0)
				{
					glm::quat rotation2 = glm::quat(1,0,0,0);
					readQuatWXYZ(rotation2, StrVar, 1)	//считаем в rotation WXYZ кватернион из строки
					/*
						по умолчанию камера блендера смотрит в -Z, верх у нее в Y, а право - в X
						* Поэтому, повернув камеру там в нужную сторону, надо еще учесть, что оси всех остальных объектов до поворотов описываются единичной матрицей
						* Соответственно, надо внести поправку для кватерниона камеры, чтобы по умолчанию ее оси как бы тоже описывались единичной матрицей 
						* Это можно сделать домножением кватерниона камеры на glm::quat(0.5, -0.5, 0.5, 0.5)
					*/
					actors["Camera"]->setRotation(  rotation2 * glm::quat(0.5, -0.5, 0.5, 0.5)  );
				}
				parametersFound++;
			}
			if (getValue(StrVar, 1) == "MESH") //как только наткнулись на блок описания объекта.
			{
				entityInitializationParameters rookie;					// создадим инициализационную структуру
				
				//~ std::string initSysname; glm::vec3 initPos; glm::quat initRot;
				//~ std::string initMeshName; std::string initType;  std::string shaderName = "forward"; std::string lightmapName = "none";
				//~ std::vector <std::string> initMeshMaterialsVector;

				while (getValue(StrVar, 0) != "END") //пока не наткнемся на сигнал конца описания сущности, то читаем блок описания сущности
				{
					std::string Slovo = "";
					Slovo = getValue(StrVar, 0); //выдрали первое слово из строки

					if (StrVar[0] == '[') 	{								rookie.initType 	= getValue(StrVar, 1); }
					if (Slovo == "sysname") { 								rookie.initSysname 	= getValue(StrVar, 1); }
					if (Slovo == "position") { for (int i = 0; i < 3; i++) 	rookie.initPos[i] 	= stof(getValue(StrVar, i + 1)); }
					if (Slovo == "direction")      //выдираем кватернион поворота
					{
																			rookie.initRot.w 	= stof(getValue(StrVar, 1));
																			rookie.initRot.x 	= stof(getValue(StrVar, 2));
																			rookie.initRot.y 	= stof(getValue(StrVar, 3));
																			rookie.initRot.z 	= stof(getValue(StrVar, 4));
					}
					if (Slovo == "model") { 								rookie.initMeshName = getValue(StrVar, 1); }
					if (Slovo == "material") { 								rookie.initMeshMaterialsVector.push_back(getValue(StrVar, 1)); }
					if (Slovo == "shader") { 								rookie.shaderName 	= getValue(StrVar, 1); }

					if (Slovo == "Lightmap") { 								rookie.lightmapName = getValue(StrVar, 3); }
					
					
					
					//~ if (StrVar[0] == '[') 	{								initType 	= getValue(StrVar, 1); }
					//~ if (Slovo == "sysname") { 								initSysname 	= getValue(StrVar, 1); }
					//~ if (Slovo == "position") { for (int i = 0; i < 3; i++) 	initPos[i] 	= stof(getValue(StrVar, i + 1)); }
					//~ if (Slovo == "direction")      //выдираем кватернион поворота
					//~ {
																			//~ initRot.w 	= stof(getValue(StrVar, 1));
																			//~ initRot.x 	= stof(getValue(StrVar, 2));
																			//~ initRot.y 	= stof(getValue(StrVar, 3));
																			//~ initRot.z 	= stof(getValue(StrVar, 4));
					//~ }
					//~ if (Slovo == "model") { 								initMeshName = getValue(StrVar, 1); }
					//~ if (Slovo == "material") { 								initMeshMaterialsVector.push_back(getValue(StrVar, 1)); }
					//~ if (Slovo == "shader") { 								shaderName 	= getValue(StrVar, 1); }

					//~ if (Slovo == "Lightmap") { 								lightmapName = getValue(StrVar, 3); }

					getline(levelToLoadStream, StrVar);
				}

				//addEntity( "Wall_01.023_Wall_01_MESH", glm::vec3(-8.0, 3.75, 0.0), glm::quat(1.0, 0.0, 0.0, 0.0),  "SM_Wall_01", {  }, "MESH", { "concrete_wall_007", },  "none",  "forward" );

				// передадим структуру в виде rvalue в функцию
				FUNC( addComponentToMicroWorld("Background", std::move(rookie) )  )
				
				//FUNC( actors["Background"]->addEntity( std::move(rookie) ) )	//
				
				
				
				//actors["Background"]->addEntity(std::move(initSysname), std::move(initPos), std::move(initRot), std::move(initMeshName), {}, std::move(initType), std::move(initMeshMaterialsVector), std::move(lightmapName), std::move(shaderName) );//
				
				//printMessage("Entity loaded")
				//DEBUG_INFO
				
				
				//~ initMeshMaterialsVector.clear();

				//~ Entity* rookie = new Entity(initSysname, initPos, initRot, initMeshName, initType, initMeshMaterialsVector, lightmapName, shaderName); //создаем сущность со считанными параметрами

				//~ if (rookie->OK) 
				//~ { 
					//~ backgroundObjects->components[initSysname] = rookie; initMeshMaterialsVector.clear(); 
					//~ }
				//~ else { cout << "Fail to create entity [" << initSysname << "], delete pointer" << endl; delete rookie; initMeshMaterialsVector.clear(); }
			}
			//--тут закончили парсить блок данных объекта
		}


		levelToLoadStream.close();

		if (parametersFound == 2)
		{
			return true;
		}
		else
		{
			std::cout << "Some camera params not found! Position or rotation";	DEBUG_INFO return false;
		}
	}
	else
	{
		std::cout << "Cannot read level description: " << levelName; DEBUG_INFO
		levelToLoadStream.close();
		return false;
	}
	//~ return false;
}



void Level::update() // виртуальная функция, может быть переназначена в дочерних классах
{
	
}



//~ int main()
//~ {
	//~ return 0;
//~ }


//~ #include "MicroWorld.cpp"
//~ #include "Shader.cpp"
//~ #include "auxiliary.cpp"
//~ #include "Mesh.cpp"
//~ #include "CollisionCalculator.cpp"
//~ #include "Entity.cpp"
//~ #include "Skeleton.cpp"
