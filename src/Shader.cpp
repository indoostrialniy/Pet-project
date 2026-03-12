#include "Shader.h"

#include <iostream>
#include <nameof.hpp>
#include <fstream>  //ifstream

#include "macros.h"
#include "auxiliary.h"

Shader::Shader(std::string shaderName)
{
	LogDuration ld("Shader constructor");
	#ifdef SHADER_DEBUG
		std::cout << "Constructor of [" << NAMEOF_TYPE(decltype(this)) << "] [" <<  shaderName << "]" << std::endl;
	#endif
		
	vertexShaderFile = getVShaderPath(shaderName);
		
	fragmentShaderFile = getFShaderPath(shaderName);
	
	bool status_tmp = compileNewShader();


	if (status_tmp)
	{
		configureUBO();
		#ifdef SHADER_DEBUG
			std::cout << "\tUBO`s configured" << std::endl;
		#endif
		
		configureUniformAdresses();
		#ifdef SHADER_DEBUG
			std::cout << "\tUniformAdresses configured" << std::endl;
		#endif
	}
	else
	{
		std::cout<< "Cannot compile shader: " << shaderName; 
		DEBUG_INFO
	}
	
	status = status_tmp;
	
	resourceName = shaderName;
}



void Shader::printResourceData()			// обязательная функция, но можно оставить тело пустым или попытаться скрыть
{
	std::cout << "\tDetails:\n";
	//std::cout << "\t\t\t Shadername: " << name << std::endl;
}


Shader::~Shader()
{
	#ifdef SHADER_DEBUG
	std::cout << "Destructor of [" << NAMEOF_TYPE(decltype(this)) << "] [" <<  resourceName << "]" << std::endl;
	#endif
	
	glDeleteProgram(shaderProgramID);

	glDeleteBuffers(1, &PVMBlock); 				//очищаем юниформ-блок
	glDeleteBuffers(1, &skeletonUniformBlock); 	//очищаем юниформ-блок
	glDeleteBuffers(1, &lightingUniformBlock); 	//очищаем юниформ-блок
	
}

// пользовательские переменные и методы

void Shader::updateShader()
{
	glDeleteProgram(shaderProgramID);

	bool OK = compileNewShader();

	if (OK == true) { configureUniformAdresses(); printMessage("Shader updated") }
	glUseProgram(shaderProgramID);
	

}


void Shader::configureUBO()
{
	///PV and M ubo
	glGenBuffers(1, &PVMBlock);
	glBindBuffer(GL_UNIFORM_BUFFER, PVMBlock);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4) + 2 * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);	//выделили 2* (4*4)*4b=128 байт для хранения матриц PV и M
	
	#define LAYOUT_BINDING_8 8
	
	glBindBufferBase(GL_UNIFORM_BUFFER, LAYOUT_BINDING_8, PVMBlock); ///8 - это явный адрес из layout(binding = 8, std140) uniform PVM_Matrices шейдера forward
	///привязали блок юниформов к шейдеру! далее по ходу надо только обновлять значения
	//~ //glBindBufferRange(GL_UNIFORM_BUFFER, 2, uboExampleBlock, 0, 2*sizeof(glm::mat4) ); //привязали конкретную часть блока
	//~ //glUniformBlockBinding(testShader, glGetUniformBlockIndex(testShader, "PVM_Matrices"), 2);//привязали блок
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	///skeleton ubo
	glGenBuffers(1, &skeletonUniformBlock);
	glBindBuffer(GL_UNIFORM_BUFFER, skeletonUniformBlock);
	glBufferData(GL_UNIFORM_BUFFER, (70 * 5 + 140) * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);	//выделили место для 490 матриц 4х4 float - примерно 31360 байт
	//~ mat4 IERARCHY[70];   mat4 TRANSFORMINTO[70];   mat4 TRANSFORMNEXT[70];    mat4 LSK_Matrices[70];      mat4 TRANSFORMMATRICES[140];    mat4 POSES[70];  
	
	#define LAYOUT_BINDING_9 9
	
	glBindBufferBase(GL_UNIFORM_BUFFER, LAYOUT_BINDING_9, skeletonUniformBlock); ///9 - это явный адрес из layout(binding = 9, std140) uniform SKELETON_UBO шейдера forward
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	///lighting ubo	
	glGenBuffers(1, &lightingUniformBlock);
	glBindBuffer(GL_UNIFORM_BUFFER, lightingUniformBlock);
	glBufferData(GL_UNIFORM_BUFFER, 20 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);	//выделили место для 20 матриц 4х4 float - примерно 1280 байт
	//~ mat4 LIGHT[20];    //LIGHT[0] - Position;    //LIGHT[1] - Direction;    //LIGHT[2] - Color;    //LIGHT[3] - TypeSectorRadiusPower;   // x - тип (omni, spot, etc), y-ширина конуса луча, если тип=spot,  z- радиус освещения (для оптимизации), w-мощность               
	
	#define LAYOUT_BINDING_10 10
	glBindBufferBase(GL_UNIFORM_BUFFER, LAYOUT_BINDING_10, lightingUniformBlock); 	///10 - это явный адрес из layout(binding = 10, std140) uniform DYNAMIC_LIGHTS_UBO шейдера forward
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

}


void Shader::configureUniformAdresses()	//находим и запоминаем адреса юниформов в шейдере
{
	#ifdef SHADER_DEBUG
		std::cout << "\tConfiguring adresses in shader\n";
	#endif
	
	
	//PV_matrix_adress 			= glGetUniformLocation(testShader, "PV");
	//M_matrix_adress 			= glGetUniformLocation(testShader, "M");
	Skeletal_adress = glGetUniformLocation(shaderProgramID, "skeletal");
	//Highlight_adress			= glGetUniformLocation(testShader, "highlightColor");
	//SetHighlighting_adress		= glGetUniformLocation(testShader, "setHighlightColor");

	//cout << "\n\n\n\n PV_matrix_adress " << PV_matrix_adress << "\n\n\n\n"<< endl;
	//cout << "\n\n\n\n M_matrix_adress " << M_matrix_adress << "\n\n\n\n"<< endl;
	//cout << "\n\n\n\n Skeletal_adress " << Skeletal_adress << "\n\n\n\n" << endl;
	//cout << "\n\n\n\n Highlight_adress " << Highlight_adress << "\n\n\n\n"<< endl;
	//cout << "\n\n\n\n SetHighlighting_adress " << SetHighlighting_adress << "\n\n\n\n"<< endl;
}



bool Shader::compileNewShader()//string VertexShader, string FragmentShader)
{
	#ifdef SHADER_DEBUG
		std::cout << "\tCompiling shaders: [" << vertexShaderFile << "] [" << fragmentShaderFile << "]" << std::endl;
	#endif
	
	//из тела класса берем адреса файлов шейдеров, компилируем
	
	
	//ВЕРШИННЫЙ
	#ifdef SHADER_DEBUG
		printMessage("\tCreate vertex shader")
	#endif
	
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);									// создаем объект вершинной шейдерной программы

	std::string VertexShaderCode;
	
	std::ifstream VertexShaderStream(vertexShaderFile);									// открываем поток с кодом шейдерной программы из файла
	
	if ( VertexShaderStream.is_open() )
	{
		std::string Line = "";
		while (getline(VertexShaderStream, Line)) 										// записываем в VertexShaderStream строки из файла, пока они есть
		{
			VertexShaderCode += "\n" + Line;
		}
	}
	else
	{
		std::cout << "Cannnot find shader source file: " << vertexShaderFile; 
		DEBUG_INFO 																		// выведем в консоль эту точку исходника
		return false;
	}
	VertexShaderStream.close();															// закрываем поток с кодом шейдерной программы

	char const* VertexSourcePointer = VertexShaderCode.c_str();							// переводим текст шейдерной программы из string в char* для подготовки к компиляции шейдерной программы
	// XXX удалять VertexSourcePointer не нужно, выскочит ошибка!

	glShaderSource(vShader, 1, &VertexSourcePointer, NULL);								// записываем код по указателю vertexShaderSource в шейдерную программу
	
	glCompileShader(vShader);															// компилируем
	
	//~ shadCheckShaderCompileStatus(vShader);												// проверяем
	




	//ФРАГМЕНТНЫЙ
	#ifdef SHADER_DEBUG
		printMessage("\tCreate fragment shader")
	#endif
	
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);								// создаем фрагментный шейдер, далее - по аналогии с вершинным шейдером

	std::string FragmentShaderCode;
	
	std::ifstream FragmentShaderStream(fragmentShaderFile);
	
	if (FragmentShaderStream.is_open())
	{
		std::string Line2 = "";
		while (getline(FragmentShaderStream, Line2)) //записываем в FragmentShaderStream строки из файла, пока они есть
		{
			FragmentShaderCode += "\n" + Line2;
		}
	}
	else 
	{ 
		std::cout << "Cannnot find shader source file: " << fragmentShaderFile; 
		DEBUG_INFO 
		return false; 
	}
	FragmentShaderStream.close(); 
	
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();

	glShaderSource(fShader, 1, &FragmentSourcePointer, NULL);  
	
	glCompileShader(fShader);									
	//shadCheckShaderCompileStatus(fShader);							// проверяем
	



	//СОЗДАЕМ ПРОГРАММУ
	#ifdef SHADER_DEBUG
		printMessage("\tCreate shader in general ")
	#endif
	
	shaderProgramID = glCreateProgram();							// создаем программу
	
	glAttachShader(shaderProgramID, vShader);							// присоединяем к программе вершинный шейдер
	glAttachShader(shaderProgramID, fShader);							// и фрагментный
	//glAttachShader(program, gShader);
	glLinkProgram(shaderProgramID);										// линкуем
	//glUseProgram(program);										// начинаем использовать
	GLint shaderStatus;
	glGetProgramiv( shaderProgramID, GL_LINK_STATUS, &shaderStatus );			// получаем статус программы

	glDeleteShader(vShader);
	glDeleteShader(fShader);

	

	if (shaderStatus == GL_FALSE)										// проверяем статус программы
	{
		GLint infoLogLength;
		glGetProgramiv( shaderProgramID , GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* infoLog = new GLchar[infoLogLength];
		glGetProgramInfoLog(shaderProgramID, infoLogLength, NULL, infoLog);
		std::cerr << "Link log 2: " << infoLog;
		DEBUG_INFO
		
		delete[] infoLog;
		return false;
	}
	else
	{
		#ifdef SHADER_DEBUG
			std::cout << "\tReady-to-use shaders: [" << vertexShaderFile << "] [" << fragmentShaderFile << "]" << std::endl;
		#endif
		return true;
	}

	//~ printMessage("vlkdnkvdsnvsl")

	//glDeleteShader(gShader);


	//VertexShaderStream.close();
	//FragmentShaderStream.close();
	//GeometryShaderStream.close();

	//return program;
}


std::string Shader::getVShaderPath(std::string filename)	
{	
	return getOperationSystem() == "Windows" ? "datas\\shaders\\" + filename + ".vs" : "./datas/shaders/" + filename + ".vs" ;		
}	//тернарный определитель пути

std::string Shader::getFShaderPath(std::string filename)	
{	
	return getOperationSystem() == "Windows" ? "datas\\shaders\\" + filename + ".fs" : "./datas/shaders/" + filename + ".fs" ;		
}	//тернарный определитель пути



bool Shader::initGL()
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


//~ #include "auxiliary.cpp"
//~ int main()
//~ {
	
	//~ Shader* rookie = new Shader("forward");
	
	//~ if(rookie->status)
	//~ {
		//~ printMessage("Ok")
	//~ }
	//~ else
	//~ {
		//~ printMessage("Fail")
	//~ }
	
	//~ delete rookie;
//~ }


