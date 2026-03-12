#include "Entity.h"
#include "macros.h"
#include <nameof.hpp>
#include <iostream>
#include "global.h"

#include "Mesh.h"
#include "Shader.h"
#include "Material.h"
#include "Skeleton.h"

#include "auxiliary.h"


Entity::Entity(	std::string initSysname, 																				// имя
				glm::vec3 initPos, glm::quat initRot, glm::vec3& comboPos, glm::quat& comboRot, glm::vec3& comboDir,						// местоположение
				std::string initModelName, CollisionDescriptor initCollisionDescr, std::string initType, 				// конфигурация
				std::vector <std::string> initMaterialsNames, std::string lightmapName, std::string initShaderName )	// конфигурация 
				:	
				entityName(initSysname),
				Collision( initSysname, initCollisionDescr, initPos, initRot, comboPos, comboRot, comboDir ),	//сразу инициализируем родительский класс через определенный конструктор
				AnimationController( entityName )
				//entityPosition(initPos), entityRotation(initRot), comboPosition(comboPos), comboRotation(comboRot) // и текущему также проинициализируем переменные
{
	// printMessage("Constructor of entity")
	// printVec3(entityPosition)
	// printVec3(initPos)
	
	
	// entityName 		= initSysname;	// параметр name должен называться именно так - это нужно для корректного шаблонирования
	
	//entityPosition 	= initPos;
	//entityRotation 	= initRot;
	
	entityMeshName 	= initModelName;
	
	//entityCollision = std::make_shared<Collision>( initCollisionDescr, entityPosition, entityRotation, comboPosition, comboRotation );
	
	//Collision::Collision( CollisionDescriptor initCollisionDescr, glm::vec3& ePos, glm::quat& eRot, glm::vec3& cPos, glm::quat& cRot ) 

	
	//entityCollision = Collision();
	
	//entityCollision.collisionDescriptor = initCollisionDescr;
	
	entityMaterialsNamesVector = initMaterialsNames;
	entityShaderName = initShaderName;
	
	
	status = FUNC(configureEntity() )
}



Entity::Entity(	 const entityInitializationParameters& entityParameters, glm::vec3& comboPos, glm::quat& comboRot, glm::vec3& comboDir )
:	
				entityName(entityParameters.initSysname),
				Collision( entityParameters.initSysname, entityParameters.collisionType, entityParameters.initPos, entityParameters.initRot, comboPos, comboRot, comboDir ),	//сразу инициализируем родительский класс через определенный конструктор
				AnimationController( entityName )
{
	//~ printMessage("Advanced entity constructor!")
	
	entityMeshName 	= entityParameters.initMeshName;
	
	//entityCollision = std::make_shared<Collision>( initCollisionDescr, entityPosition, entityRotation, comboPosition, comboRotation );
	
	//Collision::Collision( CollisionDescriptor initCollisionDescr, glm::vec3& ePos, glm::quat& eRot, glm::vec3& cPos, glm::quat& cRot ) 

	
	//entityCollision = Collision();
	
	//entityCollision.collisionDescriptor = initCollisionDescr;
	
	entityMaterialsNamesVector = entityParameters.initMeshMaterialsVector;
	entityShaderName = entityParameters.shaderName;
	
	
	status = FUNC( configureEntity() )
}			




Entity::~Entity()
{
	#ifdef ENTITY_DEBUG
		std::cout << "Destructor of [" << NAMEOF_TYPE(decltype(this)) << "] [" <<  entityName << "]" << std::endl;
	#endif
}

bool Entity::configureEntity()
{
	#ifdef ENTITY_DEBUG
		printVarComment("\nConfigure entity: ", entityName )
	#endif
	
		
	///entityNameCopyPtr = &entityName;	//скопируем в родительский класс AnimationController имя этой сущности для упрощения дальнейшей отладки
	
	entityShaderPtr = Shaders.getAsset(entityShaderName);
	if ( !entityShaderPtr->status) {printVarComment("\tFail to create shader ", entityShaderName) return false;}
	
	#ifdef ENTITY_DEBUG
		printMessage("\tShader configured!" )
	#endif
	
	
	entityMeshPtr = Meshes.getAsset(entityMeshName);
	if ( !entityMeshPtr->status) {printVarComment("\tFail to create mesh ", entityMeshName) return false;}
	else 
	{
		#ifdef ENTITY_DEBUG
			printMessage("\tMesh configured!" )
		#endif
		
		if( entityMeshPtr->isMeshSkeletal() )
		{
			entitySkeletonPtr = Skeletons.getAsset( entityMeshPtr->getMeshSkeletonName() );
			
			if(entitySkeletonPtr->status)
			{
				#ifdef ENTITY_DEBUG
					printMessage("\tSkeleton configured!" )
				#endif
				
				//~ sklPose = new glm::mat4 [entitySkeletonPtr->sklBonesCount]; // раз меш скелетный, то ему нужна поза
				//~ sklPose = std::make_unique<glm::mat4[]>(entitySkeletonPtr->sklBonesCount);			
				
				for(int i=0; i< ANIM_CHANNELS_COUNT; i++) 
				{
					//~ chPose[i] = new glm::mat4[entitySkeletonPtr->sklBonesCount];
					//std::unique_ptr<glm::mat4[]> sklIerarchyMatrices = std::make_unique<glm::mat4[]>( entitySkeletonPtr->sklBonesCount );
					animChannels[i] = std::make_shared<glm::mat4[]>( entitySkeletonPtr->sklBonesCount );
				}
				
				#ifdef ENTITY_DEBUG
					printVarComment("Create massive of bones: ",entitySkeletonPtr->sklBonesCount)
				#endif
			}
			else {printVarComment("Fail to create skeleton for skeletable mesh: ", entityMeshName) return false;}
		}
		//~ else { printVarComment("Not skeletal entity: ", entityName) DEBUG_INFO }
	
		// XXX здесь настройка коллизий
		//~ printVariable( collisionDescriptor.collisionType )
		//~ if( collisionDescriptor.collisionType != 0 ) //NO_COLLISION
		//~ {
			//~ collisionable = true;
		//~ }
		
		//~ if( collisionDescriptor.collisionType == 3 ) // COMPLEX_COLLISION
		//~ {
			//~ printVariable( collisionDescriptor.complexCollisionFileName )
			
			//~ entityCollisionShellPtr = CollisionShells.getAsset( collisionDescriptor.complexCollisionFileName );
			
			//~ if( entityCollisionShellPtr->status )
			//~ {
				//~ collisionable = true;
			//~ }
			//~ else
			//~ {
				//~ printMessage( "Problems with collision" )
				//~ DEBUG_INFO
				//~ collisionable = false;
			//~ }
		//~ }
		
		
	}
	
						
			
	// materials
	for (auto material : entityMaterialsNamesVector)
	{
		// XXX
		std::shared_ptr<Material> newMat = Materials.getAsset(material);	//создаем новый материал либо получаем указатель на готовый
		if (newMat->status) 
		{ 
			entityMaterials.push_back(newMat); 
		}
		else 
		{ 
			printVarComment("Fail to create material ", material) 
			return false;
		}
	}
	
	#ifdef ENTITY_DEBUG
		printMessage("\tMaterials configured!" )
	#endif
	
	
	return true;
}


bool Entity::bRenderEntity()
{		
	//~ printMessage("Render entity")
	// 1. Используем нашу шейдерную программу
	if( entityShaderPtr->status )
	{	
		glUseProgram(entityShaderPtr->getShaderProgram() ); 
		
		#ifdef ENTITY_DEBUG
			printMessage("Render entity: Shader checked")
		#endif
		
		
		
		// 2. Настроим материалы в шейдере
		int mat = 20; //20
		for (auto materialPtr : entityMaterials ) // перебор вектора указателей на материалы
		{
			if( CHECK_ASSET(materialPtr) )
			{
				glBindTextureUnit(mat, materialPtr->getTexture2DArray() );
				char* material_N = StringToChar("material_" + std::to_string(mat));
				char* material_N_configs = StringToChar("material_" + std::to_string(mat) + "_Configs");

				GLuint material = glGetUniformLocation( entityShaderPtr->getShaderProgram(), material_N);
				//cout << "Material " << mat << " : "  << material << endl; //513, и далее
				glUniform1i(material, mat);

				GLuint config = glGetUniformLocation( entityShaderPtr->getShaderProgram(), material_N_configs);
				//cout << "Config " << mat << " : " << config << endl; //514 и далее
				glUniformMatrix4fv(config, 1, GL_FALSE, &materialPtr->getMatInfo()[0][0]);

				delete[] material_N;
				delete[] material_N_configs;

				mat++;
			}
		}
		#ifdef ENTITY_DEBUG
			printMessage("Render entity: Materials checked")
		#endif
		
		// 3. Позиция и точка взгляда по-умолчанию
		glm::vec3 POV = glm::vec3(-5, 0, 1);
		glm::vec3 DOV = glm::vec3(5, 0, 0);
		
		// 3.1 Синхронизируемся с камерой
		if( CameraPositionPtr != nullptr ) 
		{
			POV = *CameraPositionPtr;
		}
		else
		{
			printMessage("POV fail")
			POV = glm::vec3(-9.1397705078125, 3.6825764179229736, 3.902055263519287);
		}
		
		
		if( CameraAxisPtr != nullptr )
		{
			DOV = (*CameraAxisPtr)[0];
		}
		else
		{
			printMessage("DOV fail")
			DOV = glm::vec3(0,0,0);
		}
		
		#ifdef ENTITY_DEBUG
			printMessage("\tRender entity: Camera checked")
		#endif
		
		int w, h;
		glfwGetWindowSize(engineWindow, &w, &h);
		
		// 4. Занесем UBO матриц (PV, M) и параметров камеры (position, forwardVec)
		glm::mat4 ProjViewMatrix = glm::perspective(glm::radians(64.1f), float(w) / float(h), 0.01f, 7000.0f) * glm::lookAt(POV, POV + DOV, glm::vec3(0,0,1));
	
		glm::mat4 matrix = glm::mat4(1.0f);
		
		
		//~ configureModelMatrix(matrix, setWorldLocation, setWorldRotation);
		
		configureModelMatrix(matrix, comboPosition, comboRotation );
		#ifdef ENTITY_DEBUG
			printMessage("\tRender entity: Model matrix checked")
		#endif
		
		glBindBuffer( GL_UNIFORM_BUFFER, entityShaderPtr->getMatricesUBO() );				// привязали UBO
		//						target,	offMicroWorld,	data length, &variable
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &ProjViewMatrix[0][0]); 	//обновили в юниформ-блоке значения матрицы проекции-вида
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &matrix[0][0]); 	//обновили в юниформ-блоке значения матрицы модели
		glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::vec4), &POV[0]); 	//обновили в юниформ-блоке значения для позиции камеры
		glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4) + sizeof(glm::vec4), sizeof(glm::vec4), &DOV[0]); 	//обновили в юниформ-блоке значения для впередсмотрящего вектора камеры
		glBindBuffer(GL_UNIFORM_BUFFER, 0);	// отвязали UBO
		// Закончили занесение UBO матриц (PV, M) и параметров камеры (position, forwardVec)
		#ifdef ENTITY_DEBUG
			printMessage("\tRender entity: PVM UBO checked")
		#endif
		
		
		

		// 5. Обработка скелета
		glUniform1i(glGetUniformLocation(entityShaderPtr->getShaderProgram(), "skeletal"), 0);	//по умолчанию любой объект - нескелетный
		
		//~ printMessage("BrenderEntity 1")
		
		//~ // но если объект - скелетный и все матрицы сконфигурированы, то закидываем в шейдер дополнительные параметры
		if ( entitySkeletonPtr && entitySkeletonPtr->status  )	// у некоторых объектов нет скелета, так что проверку надо выполнить вручную, во избежание ненужных отчетов о якобы "ошбках"
		{
			//~ printMessage("BrenderEntity 2")
			
			if( entitySkeletonPtr->sklCheckMatrices() )
			{
				//~ printMessage("BrenderEntity 3")
				//~ printVariable(entitySkeletonPtr->resourceName)
				
				glUniform1i(glGetUniformLocation(entityShaderPtr->getShaderProgram(), "skeletal"), 1); 	//указываем шейдеру, что объект - скелетный

				//~ printMessage("BrenderEntity 4")
				//~ ///mat4 IERARCHY[70];   mat4 TRANSFORMINTO[70];   mat4 TRANSFORMNEXT[70];    mat4 LSK_Matrices[70];      mat4 TRANSFORMMATRICES[140];    mat4 POSES[70];    
				
				glBindBuffer(GL_UNIFORM_BUFFER, entityShaderPtr->getSkeletonUBO() );
				//~ printMessage("BrenderEntity 4.5")
				glBufferSubData(GL_UNIFORM_BUFFER, 0, 						   70 * sizeof(glm::mat4), &entitySkeletonPtr->sklIerarchyMatrices[0][0]); 			//обновили в юниформ-блоке значения IERARCHY
				//~ printMessage("BrenderEntity 4.7")
				glBufferSubData(GL_UNIFORM_BUFFER, 1 * 70 * sizeof(glm::mat4), 70 * sizeof(glm::mat4), &entitySkeletonPtr->sklInputSpaceMatrices[0][0]); 			//обновили в юниформ-блоке значения TRANSFORMINTO
				//~ printMessage("BrenderEntity 4.75")
				glBufferSubData(GL_UNIFORM_BUFFER, 2 * 70 * sizeof(glm::mat4), 70 * sizeof(glm::mat4), &entitySkeletonPtr->sklOutputSpaceMatrices[0][0]); 			//обновили в юниформ-блоке значения TRANSFORMNEXT
				//~ printMessage("BrenderEntity 4.8")
				glBufferSubData(GL_UNIFORM_BUFFER, 3 * 70 * sizeof(glm::mat4), 70 * sizeof(glm::mat4), &entitySkeletonPtr->sklLocalSpaceMatrices[0][0]); 			//обновили в юниформ-блоке значения LSK_Matrices
				glBufferSubData(GL_UNIFORM_BUFFER, 4 * 70 * sizeof(glm::mat4), 140 * sizeof(glm::mat4), &entitySkeletonPtr->sklTransformMatrices[0][0]); 			//обновили в юниформ-блоке значения TRANSFORMMATRICES
				
				//~ printMessage("BrenderEntity 5")
				
				//~ if( CHECK_PTR(sklPose) )
				{	
					#ifdef ENTITY_DEBUG
						printMessage("\tRender entity: Pose checked")
					#endif
					
					glBufferSubData(GL_UNIFORM_BUFFER, 420 * sizeof(glm::mat4), 70 * sizeof(glm::mat4), &sklPose.lock()[0][0]);

				}
				
				glBindBuffer(GL_UNIFORM_BUFFER, 0);
				
				
				
				
				
				// отладим расчеты на процессоре, чтобы затем перенести этот алгоритм в шейдер
				//~ // XXX OLD glm::vec4 coord = glm::vec4(0.456, -0.089, -0.0385, 1); // xyzw
				//~ // XXX OLD coord = calculateBranchRotation11(28, coord ); // просчитаем по 28 кости - 28 27 26 25
			}
			else {printMessage("Skeletal mesh has invalid skeleton matrices")}
			
		}
		
		#ifdef ENTITY_DEBUG
			printMessage("Render entity: Skeleton checked")
		#endif
		
		

		//~ // 7. Обработка освещения
		glBindBuffer(GL_UNIFORM_BUFFER, entityShaderPtr->getLightingUBO());
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 20 * sizeof(glm::mat4), &lights[0][0]); 	//обновили в юниформ-блоке значения
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		
		// 8. Теперь уже отрисовываем объект     
		if(CHECK_ASSET(entityMeshPtr))
		{
			glBindVertexArray(entityMeshPtr->getVAO() );
			glDrawArrays(GL_TRIANGLES, 0, entityMeshPtr->getVerticesCount() );
			glBindVertexArray(0);
		}
		
		
		#ifdef ENTITY_DEBUG
			printMessage("Render entity: Render done")
		#endif
		
	
	
	}
	else
	{
		printMessage("Shader status offline")
		DEBUG_INFO
	}
	
	
	
	return false;
}










// вспомогательная функция для отладки шейдера
glm::vec4 rotate_point9(glm::vec4 point, int bone_index, int transformMatrixIndex)
{
	//~ std::cout << "Receive coords: ";	printVec4(point)
	
	//~ printVariable(bone_index)
	//~ printVariable(transformMatrixIndex)
	
	
	//~ glm::mat4 toLocal = entitySkeletonPtr->sklTransformMatrices[transformMatrixIndex]; //матрица для пересчета в пространство текущей кости С УЧЕТОМ ПОЛОЖЕНИЯ ГОЛОВЫ В Т-ПОЗЕ!!!
	//~ printMat4("matrix into : ", toLocal)

	//~ point = toLocal * point;
	//~ std::cout << "Converted coord: ";	printVec4(point)
	

	//здесь нужно получить кватернион для кости bone_index
	//     vec4 rotator = 	vec4(0,0,0,1);//POSES[bone_index][0]; // rotator считывается из файла анимации как vec4(xyzw)
	//
	//
	// 	vec4 myoffMicroWorld =  vec4(0,0,0,0); //vec4(POSES[bone_index][1].xyz, 0); //координаты смещения кости
	//
	//     //if( rotator.w < 0.999)
	//     {
	//         point = RotByQuatXYZW(vec4(point.xyz, 1), rotator);
	//     }
	//
	// 	point += myoffMicroWorld;
	
	//~ printVec4(point)
	
	return point; //vec4(p,1); //
}

// вспомогательная функция для отладки шейдера
glm::vec4 calculateBranchRotation11(int bone_index, glm::vec4 rotated_point)
{
	//~ //на входе функции имеем координаты точки (в мире) и индекс кости, по цепи которой надо пройтись
	//~ //надо определить цепь костей, включая эту

	//~ std::cout << "\n\n\nStart calculating by bone " << bone_index << std::endl;
	//~ printVec4(rotated_point)

	//~ // первым этапом провернем по самой старшей кости и в качестве матрицы перехода ей укажем ВХОДНУЮ (TRANSFORMINTO)! 
	//~ int currentBoneIndex = bone_index; // циклом for перебираем кости ветви IERARCHY[bone_index]
	//~ int currentBoneTransformSpaceMatrix     = int(entitySkeletonPtr->sklInputSpaceMatrices[bone_index][0][0]); // получим индекс матрицы для перевода в систему кости
	
	//~ rotated_point = rotate_point9(rotated_point, currentBoneIndex,  currentBoneTransformSpaceMatrix);
	
	//~ for(int i=1; i<16; i++)
	//~ {
		//~ std::cout << "\t step" << std::endl;
		//~ int c = i/4;
		//~ int s = i%4;

		//~ int currentBoneIndex = int(entitySkeletonPtr->sklIerarchyMatrices[bone_index][c][s]); // циклом for перебираем кости ветви IERARCHY[bone_index]
		
		//~ //индекс-указатель на матрицу перевода в пространство кости currentBoneIndex
		//~ int currentBoneTransformSpaceMatrix  = int(entitySkeletonPtr->sklOutputSpaceMatrices[bone_index][c][s]);    //индекс-указатель на матрицу перевода в пространство кости currentBoneIndex
				
		//~ // а далее, при переборе всей ветви, проворачиваем уже по родительским костям и в качестве переходных им указываем матрицы ВЫХОДНЫЕ (TRANSFORMNEXT)
		//~ if(currentBoneIndex != 0) //пока есть кости в ветке (пока индексы ненулевые)
		//~ {
			//~ rotated_point = rotate_point9(rotated_point, currentBoneIndex-1,  currentBoneTransformSpaceMatrix);
		//~ }
		//~ else //как только дошли до конца
		//~ {
			//~ rotated_point = rotate_point9(rotated_point, currentBoneIndex-1,  currentBoneTransformSpaceMatrix);
			//~ break;
		//~ }
	//~ }

	//~ std::cout << "\n\n\nEnd calculating by bone " << bone_index << std::endl;
	//~ printVec4(rotated_point)

	return rotated_point;
}


bool Entity::getStatus() 
{
	return status;
}
	

void Entity::updateShader()
{
	// XXX	entityShaderPtr->updateShader();
}


void Entity::setVisibility( bool visibility )
{
	visible = visibility;
}


bool Entity::getVisibility()
{
	return visible;
}

/*
* 
* name: Entity::configureModelMatrix
* @param На входе ссылочные аргументы матрицы модели, мировые координаты сета, которому принадлежит сущность Entity и мировая ориентация этого же сета
* @return Не возвращает ничего, результирующая матрица записывается в ссылочный аргумент
* 
*/
void Entity::configureModelMatrix(glm::mat4& matrix, glm::vec3& setWorldLocation, glm::quat setWorldRotation)
{
	#ifdef ENTITY_DEBUG
			printMessage("\tRender entity: configureModelMatrix")
	#endif
	
	//matrix = translate( matrix, getWorldPosition() ) * mat4_cast( getWorldRotation() );
	
	matrix = translate(matrix, setWorldLocation + setWorldRotation * entityPosition ) 	//	матрицу ориентации домножаем
			* mat4_cast(setWorldRotation * entityRotation );  							//	на матрицу переноса и оставляем это в ссылочном аргументе
}







//~ #include "Shader.cpp"
//~ #include "auxiliary.cpp"
//~ #include "Mesh.cpp"
//~ #include "CollisionCalculator.cpp"
//~ #include "Skeleton.cpp"
//~ #include "Material.cpp"


//~ int main()
//~ {
	//~ Entity rookie("rookie", glm::vec3(0,0,0), glm::quat(1,0,0,0), "SKM_PlayerMesh", "MESH", {"M_Cube_01"}, "no_lightmap", "forward");
	
	//~ if( rookie.getStatus() )
	//~ {
		//~ printMessage("Entity load successfull")
	//~ }
	//~ else
	//~ {
		//~ printMessage("Fail to load entity")
	//~ }
	
//~ }
