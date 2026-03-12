#include "Mesh.h"

#include <iostream>
#include <nameof.hpp>
#include "macros.h"
#include "auxiliary.h"
#include <fstream>  //ifstream
#include <cstdio>

Mesh::Mesh(std::string meshName)
{
	
	
	#ifdef MESH_DEBUG
		LogDuration ld("Mesh constructor");
		std::cout << "Constructor of [" << NAMEOF_TYPE(decltype(this)) << "] [" <<  meshName << "]" << std::endl;
	#endif
	
	//~ if( glewInit() != GLEW_OK)	// первым делом при создании ресурса, использующего openGL, проверим, инициализирован ли он
	//~ {
		//~ std::cout << "OpenGL context not exist until now. Create!"; DEBUG_INFO
		//~ initGL();
	//~ }
	
	status = meshLoadBinary(meshName) ? true : false; //meshLoad(meshName);
	
	resourceName = meshName;
	
}



void Mesh::printResourceData()			// обязательная функция, но можно оставить тело пустым или попытаться скрыть
{
	std::cout << "\tDetails:\n";
	printVarComment("\t\t\t\tFaces ", meshFacesCount)
	printVarComment("\t\t\t\tFaces ", meshVerticesCount)
	
	if (meshSkeletal) { printVarComment("\t\t\t\t Skeleton: yes - ", meshSkeletonName) }
}

// пользовательские переменные и методы

Mesh::~Mesh()
{
	#ifdef MESH_DEBUG
		std::cout << "Destructor of [" << NAMEOF_TYPE(decltype(this)) << "] [" <<  resourceName << "]" << std::endl;
	#endif
	
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);
	
	//~ std::cout << "\nEnd of Destructor of [" << NAMEOF_TYPE(decltype(this)) << "] [" <<  resourceName << "]" << std::endl;
}

// XXX нет загрузчика коллизий
bool Mesh::meshLoad(std::string name)
{
	std::string meshFileSource = meshGetPath(name);

	std::ifstream meshFileStream(meshFileSource); //инициализируем поток с именем meshFileStream с данными из meshFileSource

	if (meshFileStream.is_open())
	{
		std::string StrVar; //строкова¤ переменна¤ дл¤ хранени¤ строки текста из документа

		GLfloat* vertices = nullptr;
		int vert = 0; //counter for correct filling the vertices massive
		//int attrib = 0; //переменная для счета аттрибутов вершин. Нужна для дополнительной проверки количества считанных атрибутов. Во избежание битых моделей
		//читаем весь файл и пытаемся распознать параметры
		while (getline(meshFileStream, StrVar))
		{
			std::string Parameter = getValue(StrVar, 0);

			if (Parameter == "skeleton")
			{
				meshSkeletal = true;
				meshAttributesCount = 23;
				meshSkeletonName = getValue(StrVar, 1);

			}	//должен быть считан в самом начале

			if (Parameter == "faces") { meshFacesCount = stoi(getValue(StrVar, 1)); }
			if (Parameter == "vertices")
			{
				meshVerticesCount = stoi(getValue(StrVar, 1));
				vertices = new GLfloat[meshVerticesCount * meshAttributesCount]; //attributesCount values for each vertex
				for (GLuint i = 0; i < meshVerticesCount * meshAttributesCount; i++)
				{
					vertices[i] = 0.0f;
				}
			}
			if (Parameter == "v")
			{
				for (int i = 0; i < meshAttributesCount; i++)	//в зависимости от наличия скелета считаем разное число значений: 9 или 21
				{
					vertices[meshAttributesCount * vert + i] = stof(getValue(StrVar, i + 1));
				}
				vert++;
			}
		}
		
		meshFileStream.close();

		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, 4 * meshVerticesCount * meshAttributesCount, vertices, GL_STATIC_DRAW);	//4 байта на флоат для attributesCount флоатов для ModelVerticesCount вершин

		delete[] vertices;					// сразу удаляем более ненужный массив вершин модели

		#define XYZ_ATTRIB 0
		#define NORMAL_ATTRIB 1
		#define UV_ATTRIB 2
		#define MATERIAL_ATTRIB 3

		glVertexAttribPointer(XYZ_ATTRIB, 3, GL_FLOAT, GL_FALSE, meshAttributesCount * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(XYZ_ATTRIB);
		glVertexAttribPointer(NORMAL_ATTRIB, 3, GL_FLOAT, GL_FALSE, meshAttributesCount * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
		glEnableVertexAttribArray(NORMAL_ATTRIB);
		glVertexAttribPointer(UV_ATTRIB, 2, GL_FLOAT, GL_FALSE, meshAttributesCount * sizeof(GLfloat), (GLvoid*)(6 * sizeof(float)));
		glEnableVertexAttribArray(UV_ATTRIB);
		glVertexAttribPointer(MATERIAL_ATTRIB, 1, GL_FLOAT, GL_FALSE, meshAttributesCount * sizeof(GLfloat), (GLvoid*)(8 * sizeof(float)));
		glEnableVertexAttribArray(MATERIAL_ATTRIB);

		if (meshSkeletal == true)		//если меш - скелетный, добавим еще атрибутов
		{
			#define Bone1_Bone2 4
			#define Bone3_Bone4 5
			#define Bone5_Bone6 6
			#define Bone7 7

			glVertexAttribPointer(Bone1_Bone2, 4, GL_FLOAT, GL_FALSE, meshAttributesCount * sizeof(GLfloat), (GLvoid*)(9 * sizeof(float)));
			glEnableVertexAttribArray(Bone1_Bone2);
			glVertexAttribPointer(Bone3_Bone4, 4, GL_FLOAT, GL_FALSE, meshAttributesCount * sizeof(GLfloat), (GLvoid*)(13 * sizeof(float)));
			glEnableVertexAttribArray(Bone3_Bone4);
			glVertexAttribPointer(Bone5_Bone6, 4, GL_FLOAT, GL_FALSE, meshAttributesCount * sizeof(GLfloat), (GLvoid*)(17 * sizeof(float)));
			glEnableVertexAttribArray(Bone5_Bone6);
			glVertexAttribPointer(Bone7, 2, GL_FLOAT, GL_FALSE, meshAttributesCount * sizeof(GLfloat), (GLvoid*)(21 * sizeof(float)));
			glEnableVertexAttribArray(Bone7);
		}

		glBindVertexArray(0);

		//ДОБАВИТЬ проверку корректности загрузки модели 

		return true;
	}
	else
	{
		std::cout << "\t Error: Cannot find file [" << meshFileSource << "] (class Mesh). Status of " << name << " FALSE!" << std::endl;
		return false;
	}

}


bool Mesh::meshLoadBinary(std::string name)
{
	std::string meshFileSource = meshGetPathBinary(name);

	#ifdef MESH_DEBUG
		printVarComment("Load binary ", meshFileSource)
	#endif

	
	
	FILE* fp;
	fp = fopen(meshFileSource.c_str(), "rb");

	if (fp != nullptr)
	{
		
		
		// 1. считаем два первых float файла, хранящие числа треугольников и вершин;
		float ch[2];							
		fread(&ch, sizeof(float), 2, fp);			

		meshFacesCount = ch[0];
		meshVerticesCount = ch[1];
		
		#ifdef MESH_DEBUG
			printVariable(meshFacesCount)
			printVariable(meshVerticesCount)
		#endif
		
		
		
		
		// 2. считаем следующие 32 байта, где должно быть записано имя скелета (при наличии)
		std::string skeleton = "noskeleton";		// по умолчанию скелет отсутствует
		char stroka[32];
		fread(&stroka, sizeof(char), 32, fp);		// считаем 32 символа, в которых может быть обозначен используемый скелет
		skeleton = stroka;							// запишем вероятно-обозначенный скелет в строку
		
		if (getValue(skeleton, 0) != "noskeleton")	// имя скелета всегда будет самым первым в строке из 32 символов, поэтому если там НЕ дефолтное слово "noskeleton"
		{
			meshSkeletal = true;
			meshAttributesCount = 23;
			meshSkeletonName = getValue(skeleton, 0);
		}
		#ifdef MESH_DEBUG
			printVariable(skeleton)
			printVariable(meshSkeletonName)
			printVariable(meshAttributesCount)
		#endif
		
		
		
		// 3. считаем еще 1 float, хранящий число float-ов для ААВВ-коробки
		float aabb[1];
		fread(&aabb, sizeof(float), 1, fp);
		
		int AABBfloats = aabb[0];	// число флоатов, описывающих коробку
		
		if(AABBfloats != 0)			// если флоатов больше 0
		{
			float* AABB_Verts = new float[AABBfloats];

			fread(AABB_Verts, sizeof(float), AABBfloats, fp);
			
			/*
				для триангулированной коробки (6 граней куба, деленных пополам = 12 треугольников), хранятся вершины для каждого треугольника, а каждая вершина имеет координату и нормаль
				формат коробки: 	face_0_vert_0_x face_0_vert_0_y face_0_vert_0_z face_0_vert_0_nx face_0_vert_0_ny face_0_vert_0_nz 
									face_0_vert_1_x face_0_vert_1_y face_0_vert_1_z face_0_vert_1_nx face_0_vert_1_ny face_0_vert_1_nz 
									face_0_vert_2_x face_0_vert_2_y face_0_vert_2_z face_0_vert_2_nx face_0_vert_2_ny face_0_vert_2_nz
									..перечисление всех треугольников..
									face_n_vert_0_x face_n_vert_0_y face_n_vert_0_z face_n_vert_0_nx face_n_vert_0_ny face_n_vert_0_nz  
				
				при этом нормаль у вершин одного треугольника - одна и та же (да, это ведет к небольшой избыточности, но как есть)
				соответственно, коробку можно сохранить как массив glm::mat4, в которой 3 первых столбца отведены под координаты, а 4-й под нормаль
				обрабатывать коробку можно в отдельном родительском классе, гже можно реализовать собственные алгоритмы просчета пересечений
				путем перевода координат внешней точки в локальное пространство коробки, чтобы излишне не усложнять формулу
				
				Оставшиеся 4 пустых ячейки можно использовать для хранения a,b,c,d=1 компонентов треугольника (вычисляемых методом Крамера), которые в дальнейшем понадобятся для определения высоты до данного треугольника
			*/		
			//~ collisionTrianglesCount  = AABBfloats/18; // число треугольников коробки AABB_triangles
			
			std::vector <glm::mat4> triangles;		// вектор, хранящий описывающие треугольники
			
			//~ for(int tris=0; tris<collisionTrianglesCount; tris++) // проход по треугольникам (матрицам)
			//~ {
				//~ glm::vec3 P1 = glm::vec3( AABB_Verts[tris*18+0], AABB_Verts[tris*18+1], AABB_Verts[tris*18+2] );	// xyz 0 вершины треугольника
				//~ glm::vec3 P2 = glm::vec3( AABB_Verts[tris*18+6], AABB_Verts[tris*18+7], AABB_Verts[tris*18+8] );	// xyz 1 вершины треугольника
				//~ glm::vec3 P3 = glm::vec3( AABB_Verts[tris*18+12], AABB_Verts[tris*18+13], AABB_Verts[tris*18+14] );	// xyz 2 вершины треугольника
				//~ glm::vec3 N  = glm::vec3(AABB_Verts[tris*18+3], AABB_Verts[tris*18+4], AABB_Verts[tris*18+5] );
								
				//~ glm::mat4 tr0;	// объявим матрицу и внесем в нее точки и нормаль. Коэффициенты рассчитаются автоматически после вызова функции loadCollisionShell
				//~ tr0[0] = glm::vec4( P1, 0 ); // xyz 0 вершины треугольника
				//~ tr0[1] = glm::vec4( P2, 0 ); // xyz 1 вершины треугольника
				//~ tr0[2] = glm::vec4( P3, 0 ); // xyz 2 вершины треугольника
				//~ tr0[3] = glm::vec4( N, 0 ); // xyz нормали треугольника
				
				//~ triangles.push_back(tr0);	
			//~ }
			
			
			//~ std::cout << "\nAABB: " << std::endl;
			//~ for(int a=0; a<AABBfloats; a++)
			//~ {
				//~ std::cout << AABB_Verts[a] << " ";
			//~ }
			
			delete[] AABB_Verts; // удалим уже ненужный нерасфасованный массив
			
			//~ loadCollisionShell( triangles );
			
		}
		
		
		
		// 4. считаем оставшийся файл для загрузки геометрии модели
		float* vertices = new float[meshVerticesCount * meshAttributesCount];
		fread(vertices, sizeof(float), meshVerticesCount * meshAttributesCount, fp);

		
		fclose(fp);

		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * meshVerticesCount * meshAttributesCount, vertices, GL_STATIC_DRAW);	//4 байта на флоат для attributesCount флоатов для ModelVerticesCount вершин
		
		
		delete[] vertices;					// сразу удаляем более ненужный массив вершин модели
		
		#define XYZ_ATTRIB 0
		#define NORMAL_ATTRIB 1
		#define UV_ATTRIB 2
		#define MATERIAL_ATTRIB 3

		glVertexAttribPointer(XYZ_ATTRIB, 3, GL_FLOAT, GL_FALSE, meshAttributesCount * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(XYZ_ATTRIB);
		glVertexAttribPointer(NORMAL_ATTRIB, 3, GL_FLOAT, GL_FALSE, meshAttributesCount * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
		glEnableVertexAttribArray(NORMAL_ATTRIB);
		glVertexAttribPointer(UV_ATTRIB, 2, GL_FLOAT, GL_FALSE, meshAttributesCount * sizeof(GLfloat), (GLvoid*)(6 * sizeof(float)));
		glEnableVertexAttribArray(UV_ATTRIB);
		glVertexAttribPointer(MATERIAL_ATTRIB, 1, GL_FLOAT, GL_FALSE, meshAttributesCount * sizeof(GLfloat), (GLvoid*)(8 * sizeof(float)));
		glEnableVertexAttribArray(MATERIAL_ATTRIB);

		if (meshSkeletal == true)		//если меш - скелетный, добавим еще атрибутов
		{
			#define Bone1_Bone2 4
			#define Bone3_Bone4 5
			#define Bone5_Bone6 6
			#define Bone7 7

			glVertexAttribPointer(Bone1_Bone2, 4, GL_FLOAT, GL_FALSE, meshAttributesCount * sizeof(GLfloat), (GLvoid*)(9 * sizeof(float)));
			glEnableVertexAttribArray(Bone1_Bone2);
			glVertexAttribPointer(Bone3_Bone4, 4, GL_FLOAT, GL_FALSE, meshAttributesCount * sizeof(GLfloat), (GLvoid*)(13 * sizeof(float)));
			glEnableVertexAttribArray(Bone3_Bone4);
			glVertexAttribPointer(Bone5_Bone6, 4, GL_FLOAT, GL_FALSE, meshAttributesCount * sizeof(GLfloat), (GLvoid*)(17 * sizeof(float)));
			glEnableVertexAttribArray(Bone5_Bone6);
			glVertexAttribPointer(Bone7, 2, GL_FLOAT, GL_FALSE, meshAttributesCount * sizeof(GLfloat), (GLvoid*)(21 * sizeof(float)));
			glEnableVertexAttribArray(Bone7);
		}

		glBindVertexArray(0);

		//ДОБАВИТЬ проверку корректности загрузки модели 
		
		#ifdef MESH_DEBUG
			printMessage("Mesh loaded successfully!")
		#endif
		
		return true;
	}
	else
	{
		std::cout << "\t Error: Cannot open file [" << meshFileSource << "] (class Mesh). Status of " << name << " FALSE!" << std::endl;
		DEBUG_INFO
		return false;
	}

}


std::string Mesh::meshGetPath(std::string filename)			
{	
	return getOperationSystem() == "Windows" ? "datas\\meshes\\" + filename + ".ltx" : "./datas/meshes/" + filename + ".ltx" ;	//тернарный определитель пути	
}	

std::string Mesh::meshGetPathBinary(std::string filename)	
{	
	return getOperationSystem() == "Windows" ? "datas\\meshes\\" + filename + ".model" : "./datas/meshes/" + filename + ".model" ;	//тернарный определитель пути	
}	
	
bool Mesh::initGL()
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
		if (0) { std::cout << "\tGAPI initialized!\n"; }	
		return true; 
	}
}
	
	
//~ #include "CollisionCalculator.cpp"
//~ #include "auxiliary.cpp"
//~ int main()
//~ {
	
	//~ Mesh* rookie = new Mesh("SM_Wall_01");
	
	//~ if(rookie->status)
	//~ {
		//~ printMessage("Ok")
	//~ }
	//~ else
	//~ {
		//~ printMessage("Fail")
	//~ }
	
	//~ delete rookie;
	
	//~ return 0;
//~ }
	
