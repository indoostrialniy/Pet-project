cmake_minimum_required(VERSION 3.20)
project(PETPROJECT VERSION 0.1.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Отключаем разделение по конфигурациям
set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "" FORCE)

# 2. Устанавливаем единый выходной каталог
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Для мультиконфигурационных генераторов (Visual Studio, Xcode)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

# Определим источники и заголовки один раз
set(HEADERS src/global.h src/auxiliary.h src/AI_Controller.h src/AnimationController.h src/Box_01.h src/Camera.h src/Collision.h src/CollisionCalculator.h src/CollisionDescriptor.h src/CollisionShell.h src/Core.h src/Debugger.h src/Door_01.h src/DynamicLight.h src/ElectricBox.h src/Elevator.h src/Entity.h src/EXO_Soldier.h src/Gate_01.h src/Laboratory.h src/Level.h src/macros.h src/Material.h src/Mesh.h src/MicroWorld.h src/MovingPartsController.h src/NPC.h src/Orc.h src/Pistol.h src/Rifle.h src/Scene_01.h src/Shader.h src/Skeleton.h src/SkeletonAnimation.h src/SoundAsset.h src/Transporter.h src/Turniket.h src/WallBreach.h src/Weapon.h )
# src/headers.h
set(SOURCES src/main.cpp src/global.cpp src/auxiliary.cpp src/Core.cpp src/MicroWorld.cpp src/Shader.cpp src/Material.cpp src/SoundAsset.cpp src/Mesh.cpp src/Collision.cpp src/Entity.cpp src/Skeleton.cpp src/MovingPartsController.cpp src/SkeletonAnimation.cpp src/AnimationController.cpp src/CollisionDescriptor.cpp src/CollisionShell.cpp src/Level.cpp src/Laboratory.cpp src/Camera.cpp src/Elevator.cpp src/Weapon.cpp src/Pistol.cpp src/Door_01.cpp src/Debugger.cpp src/Scene_01.cpp src/ElectricBox.cpp src/Box_01.cpp src/Transporter.cpp src/WallBreach.cpp src/Gate_01.cpp src/DynamicLight.cpp src/AI_Controller.cpp src/NPC.cpp src/Orc.cpp src/EXO_Soldier.cpp src/Rifle.cpp src/Turniket.cpp )	# specify source code files

if(WIN32)
    message(STATUS "Target platform: Windows.")
	
	# specify include-folder at Windows
	include_directories( include )
	
	# specify libraries folder
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		message(STATUS "Compiler: GCC (version ${CMAKE_CXX_COMPILER_VERSION})")
		link_directories(libs/mingw/)				
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		message(STATUS "Compiler: MSVC (version ${CMAKE_CXX_COMPILER_VERSION})")
		link_directories(libs/msvc/)
	endif()
	
	add_executable(fhEngine ${SOURCES} ${HEADERS})									
	
	target_link_libraries(fhEngine glew32 opengl32 glfw3 gdi32  sfml-system sfml-audio)	
	
endif()	
	
if(UNIX)
    message(STATUS "Target platform: Linux!")

    include_directories(include	)
	
	# SFML
	find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
	if(NOT SFML_FOUND)
		message(FATAL_ERROR "SFML 2.5 not found! Install it!")
	endif()

	# Выводим информацию о найденном SFML
	message(STATUS "SFML found: ${SFML_VERSION}")
	#message(STATUS "SFML libraries: ${SFML_LIBRARIES}")

	#ищем glew
	#find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

	# Создаем цель
    add_executable(fhEngine ${SOURCES} ${HEADERS})
	
	target_link_directories(fhEngine PRIVATE /usr/lib/)  # Путь к SFML
    # Указываем библиотеки, которые нужно линковать с проектом

    target_link_libraries(fhEngine
		GLEW #glew
		#GL
		glfw
        #sfml-window
        #sfml-graphics
        sfml-system
        sfml-audio
    )

    # Дополнительные библиотеки для Linux
    target_link_libraries(fhEngine
        GL       # OpenGL
        pthread  # Потоки
        X11      # X11 для окон
    )

endif()
