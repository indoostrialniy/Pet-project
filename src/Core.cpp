#include "Core.h"

#include <fstream>  //ifstream
#include <iostream>
#include <functional>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "macros.h"

#include "Level.h"
#include "Laboratory.h" //class Laboratory;

#include "auxiliary.h"

#include "Camera.h"

#include <memory>
#include "DynamicLight.h"


Core::~Core()
{
	///delete level;
}

void Core::run()
{
	
	bool configs = FUNC( coreLoadConfigs(true) )
	bool openGL = FUNC( initGL() )
	
	if( configs && openGL )
	{
		level = std::make_shared<Laboratory>("aerodrom.ltx");			//level = new Laboratory("aerodrom.ltx");
		
		printMessage("Level loaded, looping")
		
		mainLoop();
		
	}
	cleanUp();
}


bool Core::coreLoadConfigs(bool log)
{
	configurationFileName = getConfigPath("fhConfigs.ltx");
	
	//~ return false;
	printVarComment("Load settings... ", configurationFileName)			//	std::cout << "\nLoad settings..." << std::endl;
	
	std::ifstream configurationFileStream(configurationFileName); //инициализируем поток с именем configurationFileStream с данными из configurationFileName

	if (configurationFileStream.is_open())
	{
		std::string StrVar; //строкова¤ переменна¤ дл¤ хранени¤ строки текста из документа
		unsigned short int parametersFound = 0;

		//bool mediaFound = false;
		//читаемм весь файл и пытаемся распознать параметры
		while (getline(configurationFileStream, StrVar))
		{
			std::string Parameter = getValue(StrVar, 0);

			//position, циклом считали положение окна
			if (Parameter == "p") 		{ 	readFVec2( windowPosition, StrVar, 1 )		parametersFound++; 		}

			//resolution, циклом считали размер окна
			if (Parameter == "r")
			{
				readFVec2( windowResolution, StrVar, 1 )
				parametersFound++;
			}

			//определили уровень для загрузки
			if (Parameter == "l") 
			{  
				//~ std::cout << "first level catched\n"; /*coreLevelName = getValue(&StrVar, 1);*/	
				parametersFound++; 
			}

			// //playerDepthOfView
			// if (Parameter == "view_depth") 	{ Cam.fCameraViewDepth = stof(getValue(StrVar, 1));	parametersFound++; }

			// //playerFieldOfView
			// // if (Parameter == "fov") 		{ Cam.fCameraFOV = stof(getValue(StrVar, 1));	parametersFound++; }
			
			if (Parameter == "fullscreen") 	{ bFullscreenMode = stoi(getValue(StrVar, 1));	parametersFound++; }
		}

		configurationFileStream.close();

		std::cout << "\nfhConfigs:\n";
		printVec2(windowPosition)
		printVec2(windowResolution)
		std::cout << "\tfullscreen: " << bFullscreenMode << std::endl;
		
		return true;
		
	}
	else
	{
		std::cout << "Cannot read file engine configurations: " << configurationFileName << ", run with default MicroWorldtings" << std::endl;
		configurationFileStream.close();
		return 	false;
	}
}

bool Core::initGL()
{
	//~ return false;
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	engineWindow = glfwCreateWindow(windowResolution.x, windowResolution.y, "Demo build v0.1.1 feb 2025", nullptr, nullptr);

	if (engineWindow == nullptr) 
	{ 
		std::cout << "Failed to create GLWF window" << std::endl; 
		//glfwTerminate(); 
		return false; 
	}

	glfwMakeContextCurrent(engineWindow);

	glfwSetWindowPos(engineWindow, 100, 200);

	FUNC( glEnable(GL_DEPTH_TEST) )
	//~ #ifdef CORE_DEBUG
		//~ printMessage("GL_DEPTH_TEST enabled" ) DEBUG_INFO
	//~ #endif
	
	//~ glEnable(GL_CULL_FACE);
	//~ #ifdef CORE_DEBUG
		//~ printMessage("GL_CULL_FACE enabled" ) DEBUG_INFO
	//~ #endif
	
	//~ //glEnable(GL_BLEND);
	//~ #ifdef CORE_DEBUG
		//~ printMessage("GL_BLEND enabled" ) DEBUG_INFO
	//~ #endif
	
	
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (glewInit() != GLEW_OK) 
	{ 
		std::cout << "Failed to initialize GLEW" << std::endl; DEBUG_INFO
		return false; 
	}
	else 
	{ 
		#ifdef CORE_DEBUG
			std::cout << "\t\nGAPI initialized!\n";
		#endif
		
		return true; 
	}
}






void Core::mainLoop()
{
	printMessage("Run game")
	
	
	
	since_last_call(); // initialize functions internal static time_point
	//Flow.CallSequence( {"reminder", reminder, this} );
	
	BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas;		
	std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function = std::bind( &Core::dynamicLightSourcesUpdater, this, datas);	// главная вращающая функция		
	
	//printMessage("flag 0")
	
	Flow.CallSequence( { std::move(function), std::move(datas) } );
	// Flow.CallSequence( {"dynamicLightSourcesUpdater", dynamicLightSourcesUpdater, this} );
	
	//printMessage("flag 1")
	// Flow.CallSequence({ "startAmbientSound", startAmbientSound, this });
	
	//system("pause");
	
	while (!glfwWindowShouldClose(engineWindow))
	{
		
		if(glfwGetKey(engineWindow, GLFW_KEY_ESCAPE)) { DEBUG_INFO glfwSetWindowShouldClose(engineWindow, GLFW_TRUE);  } //условия закрытия программы
		
		//printMessage("flag 2")
		
					
		glfwPollEvents();
		
		int w, h;
		glfwGetWindowSize(engineWindow, &w, &h);
		
		glViewport(0, 0, w, h);
		glClearColor(0.05f, 0.5f, 0.05f, 0.0f);   //задаем значение цвета. Т.е. функция устанавливающая состояние
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //очищаем экран, заполнив буфера цвета и глубины заданными значениями. Т.е. функция использующая состояние, которая использует состояние для определения цвета заполнения экрана.

		//printMessage("flag 3")

		if(CHECK_PTR(level))
		{
			//printMessage("flag 4")
			level->levelUpdate();
		}
		
		//printMessage("flag 5")
		
		Flow.Execute();

		glfwSwapBuffers(engineWindow);
		
		
		milliseconds ms = since_last_call();							// print out the number of milliseconds
		
		tick = ms.count();												// std::cout << "time passed: " << ms.count() << " milliseconds.\n";
	}
}

void Core::cleanUp()
{
	#ifdef CORE_DEBUG
		printMessage("\n\n----------------------------------------------\nGame over, clenup all\n----------------------------------------------\n")
	#endif
	
	glfwTerminate();
}





bool Core::reminder( BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data )	//просто функция-напоминалка, но можно сюда повесить некоторые обновления, которые необязательно делать в каждом кадре
{
	static int time_counter = 0;
	// 5000 миллисекунд
	if(time_counter > 5000)
	{
		//std::cout << "5 s gone" << std::endl;
		time_counter = 0;
	}
	else
	{
		time_counter += tick;
	}
			
	return false;
}


bool Core::dynamicLightSourcesUpdater(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
{
	
		
	//printMessage("Update lights")
	
	//LIGHTS[0] - Position;
	//LIGHTS[1] - Direction;
	//LIGHTS[2] - Color;
	//LIGHTS[3] - TypeSectorRadiusPower;   // x - тип (omni, spot, etc), y-ширина конуса луча, если тип=spot,  z- радиус освещения (для оптимизации), w-мощность
	
	for(int a=0; a<20; a++) {lights[a] = glm::mat4(0); }	//очищаем матрицы светов
	
	//~ ///static glm::vec3 playerLightParam = glm::vec3(1.0f,0.5f,10.0f);
	//~ ///runtimePositionChange(playerLightParam);
	
	std::shared_ptr<Camera> player = dynamic_pointer_cast<Camera>( level->getActor("Camera") );
	
	if( player && player->lightSourceActive )					//CHECK_PTR(CameraAxisPtr) && CHECK_PTR(CameraPositionPtr) )
	{
		glm::mat4 playerTorch = glm::mat4(0);
		
		playerTorch[0] = glm::vec4(*CameraPositionPtr,0);
		playerTorch[1] = glm::vec4((*CameraAxisPtr)[0], 0);
		playerTorch[2] = glm::vec4(1,0.8,1, 0);
		
		/// playerTorch[3] = glm::vec4( playerLightParam, 1);
		playerTorch[3][0] = 1;	// type = spot
			playerTorch[3][1] = 0.87;	// conus angle dot coefficient
		playerTorch[3][2] = 1;		// range
		playerTorch[3][3] = 1;		// power
		
		lights[0] = playerTorch;
		//printMat4("player torch: ", playerTorch)
	}
	
	
	int i = player ? 1 : 0;
			
	//~ i = 0;		
	
	//for(int i=0; i< level->getActorsCount(); i++)						// пройдемся по числу известных акторов	//
	for(auto act : level->actors)
	{
		//~ // и еще искать по дочерним сетам
		//~ if(act.second->childMicroWorld != nullptr && act.second->childMicroWorld->MicroWorldHasTag("Light") && act.second->childMicroWorld->visible )
		//~ {
			//~ //printVarComment("Active light:", act.first)
			//~ lights[i][0] = glm::vec4(act.second->childMicroWorld->getWorldLocation(), 0);
			//~ lights[i][1] = glm::vec4(act.second->childMicroWorld->m3VectorsForwardLeftUp[0], 0);
			//~ lights[i][2] = glm::vec4(1, 0.94, 0.8, 0);
			//~ //lights[3] - TypeSectorRadiusPower;
			//~ i++;
			
			///printVarComment("MicroWorld with attached dyn light:", act.first)
		//~ }
		//auto actor = level->getActor( i );
		
		if( act.second->hasTag("Light") && act.second->getVisibility() )
		{
			std::shared_ptr<DynamicLight_01> lightSource = dynamic_pointer_cast<DynamicLight_01>( act.second );
			if( lightSource )
			{
				lights[i][0] = glm::vec4(act.second->getPosition(), 0);
				lights[i][1] = glm::vec4(act.second->m3VectorsForwardLeftUp[0], 0);
				lights[i][2] = lightSource->colorVec;// glm::vec4(1, 0.64, 0, 0);
				lights[i][3] = lightSource->typeSectorRadiusPower;
			}
			//printVarComment("Active light:", act.first)
			
			//lights[3] - TypeSectorRadiusPower;
			i++;
		}
	}
	
	//~ ///glm::mat3* CameraAxisPtr;
	//~ ///glm::vec3* CameraPositionPtr;
	
	//~ ///lights[1][0] = vec4(CAMERA.cameraPosition[0], CAMERA.cameraPosition[1], CAMERA.cameraPosition[2], 0) + vec4(CAMERA.cameraVectorsForwardAndRight[0][0], CAMERA.cameraVectorsForwardAndRight[0][1], CAMERA.cameraVectorsForwardAndRight[0][2], 0);	//у 0-й матрицы 0-й вектор - позиция
	//~ ///lights[1][1] = vec4(CAMERA.cameraVectorsForwardAndRight[0][0], CAMERA.cameraVectorsForwardAndRight[0][1], CAMERA.cameraVectorsForwardAndRight[0][2], 0);	//у 0-й матрицы 1-й вектор - направление
	//~ ///lights[1][3][0] = 0; // значит, всенаправленный
	
		
			
		
	return false;
	
}




std::string Core::getConfigPath(std::string filename)	
{	
	return getOperationSystem() == "Windows" ? "datas\\" + filename + "" : "./datas/" + filename + "" ;		//тернарный определитель пути
}	

