#include "auxiliary.h"
#include "global.h"

#include <cstring>

std::string getValue(const std::string& strSource, int num) //возврат слова с порядковым номером num из строки strSource
{
	//cout << "Incoming line: " << *strSource << endl;
	std::istringstream potokIzStroki(strSource); //создали входной поток из строки StrVar
	std::string str = "none";
	for (int i = 0; i <= num; i++, potokIzStroki >> str) //num раз пропарсим строку
		str = i == num ? str : "none";    //когда дойдем до заданного слова, сохраним в переменной само слово, иначе - "0"
		
	//potokIzStroki.close();
	return str;
}

std::string getValueOrNone(std::string& strSource, int num) //возврат слова с порядковым номером num из строки strSource. используется для загрузки скелета
{
	int currWordIndex = 0;
	std::string word;

	for (char c : strSource)
	{
		if (c == ' ')
		{
			currWordIndex++;
			if (currWordIndex > num) { break; }
		}
		else if (currWordIndex == num) { word += c; }
	}
	if (currWordIndex < num) { return "none"; }
	return word;
}

char* StringToChar(std::string StringValue) 				// XXX переделать с аргументом по ссылке
{
	char* CharValue = new char[StringValue.length() + 1];
	strcpy(CharValue, StringValue.c_str());	//преобразуем string в char
	return CharValue;
	delete[] CharValue;
}

int countOfWords(std::string& line)
{
	int words = 0;
	for(int i=0; i<(int)line.length(); i++)
	{
		words = line[i] == ' ' ? words+1 : words;
	}
	return words;
}

glm::quat vec4CastQuat(glm::vec4& vec)
{
	return glm::quat(vec.w, vec.x, vec.y, vec.z);
}

glm::vec4 blendVectors4(glm::vec4 A, glm::vec4 B, float mix2)
{
	return A + glm::vec4(mix2 * (B[0] - A[0]), mix2 * (B[1] - A[1]), mix2 * (B[2] - A[2]), 1);
}

glm::quat blendQuat(glm::quat A, glm::quat B, float mix2)
{
	//return  glm::mix(A, B, mix2);
	return glm::slerp(A, B, mix2);
	//interpolated[0] = vec4(mix2.x, mix2.y, mix2.z, mix2.w);
}

glm::vec4 CmulCXYZW(glm::vec4 a, glm::vec4 b)    //функция для перемножения комплексных чисел, например - кватернионов //a - xyzw, b- xyzw
{
	return glm::vec4(a[3] * b[0] + b[3] * a[0] + a[1] * b[2] - b[1] * a[2],
					a[3] * b[1] + b[3] * a[1] + a[2] * b[0] - b[2] * a[0],
					a[3] * b[2] + b[3] * a[2] + a[0] * b[1] - b[0] * a[1],
					a[3] * b[3] - a[0] * b[0] - a[1] * b[1] - a[2] * b[2]);
}

glm::vec4 RotByQuatXYZW(glm::vec4 coordinate, glm::vec4 quat1)    //функция возвращает координаты вектора, измененные после применения кватерниона
{
	return CmulCXYZW(CmulCXYZW(quat1, coordinate), glm::vec4(-quat1[0], -quat1[1], -quat1[2], quat1[3])); //
}

glm::vec4 RotByQuatXYZW_1(glm::vec4 coordinate, glm::vec4 quat1)    //функция возвращает координаты вектора, измененные после применения кватерниона
{
	return CmulCXYZW(CmulCXYZW(glm::vec4(-quat1[0], -quat1[1], -quat1[2], quat1[3]), coordinate), quat1); //
}

glm::vec4 RotByQuatXYZW_2(glm::vec4 u, glm::vec4 l)    //функция возвращает координаты вектора u, повернутые кватернионом l, оба аргумента в формате XYZW (SK_Attachments_6.sm)
{
    //чуть более оптимизированная функция
    //12m 8a
    glm::vec4 m = glm::vec4(0); //xyzw
    m[0] =  u[0]*l[3] - u[1]*l[2] + u[2]*l[1];  //i
    m[1] =  u[0]*l[2] + u[1]*l[3] - u[2]*l[0];  //j
    m[2] = -u[0]*l[1] + u[1]*l[0] + u[2]*l[3];  //k
    m[3] =  u[0]*l[0] + u[1]*l[1] + u[2]*l[2]; //

    //12m 9a
    glm::vec4 v = glm::vec4(0); //xyzw
    v[0] = l[3]*m[0] + l[0]*m[3] + l[1]*m[2] - l[2]*m[1];   //i
    v[1] = l[3]*m[1] - l[0]*m[2] + l[1]*m[3] + l[2]*m[0];   //j
    v[2] = l[3]*m[2] + l[0]*m[1] - l[1]*m[0] + l[2]*m[3];   //k
    v[3] = 1;

    //24m 17a
    return v;
}


std::string getOperationSystem()
{
	#ifdef _WIN32
		return "Windows";
	#elif __linux__
		return "Linux";
	#else
		return "Unknown";
	#endif
}

void runtimePositionChange(glm::vec3& coordinates)
{
	static bool f1 = false;
	static bool f2 = false;
	static bool f3 = false;
	static bool f4 = false;
	static bool f5 = false;
	static bool f6 = false;
	
	
	
	//bool stepped = glfwGetKey(engineWindow, GLFW_KEY_TAB) ? true : false;
	
	if( glfwGetKey(engineWindow, GLFW_KEY_TAB) )
	{
		if( glfwGetKey(engineWindow, GLFW_KEY_KP_8)) { coordinates.x += 1 * tick/1000.0f;}
		
		if( glfwGetKey(engineWindow, GLFW_KEY_KP_2)) { coordinates.x -= 1 * tick/1000.0f;}
		
		if( glfwGetKey(engineWindow, GLFW_KEY_KP_4)) { coordinates.y += 1 * tick/1000.0f;}
		
		if( glfwGetKey(engineWindow, GLFW_KEY_KP_6)) { coordinates.y -= 1 * tick/1000.0f;}
		
		if( glfwGetKey(engineWindow, GLFW_KEY_KP_9)) { coordinates.z += 1 * tick/1000.0f;}
		
		if( glfwGetKey(engineWindow, GLFW_KEY_KP_7)) { coordinates.z -= 1 * tick/1000.0f;}
	}
	else
	{
		if( glfwGetKey(engineWindow, GLFW_KEY_KP_8) && !f1) { coordinates.x += 1 * tick/1000.0f; f1 = true;}
		if( !glfwGetKey(engineWindow, GLFW_KEY_KP_8) ) {f1 = false;}
		
		if( glfwGetKey(engineWindow, GLFW_KEY_KP_2) && !f2) { coordinates.x -= 1 * tick/1000.0f; f2 = true;}
		if( !glfwGetKey(engineWindow, GLFW_KEY_KP_2) ) {f2 = false;}
		
		if( glfwGetKey(engineWindow, GLFW_KEY_KP_4) && !f3) { coordinates.y += 1 * tick/1000.0f; f3 = true;}
		if( !glfwGetKey(engineWindow, GLFW_KEY_KP_4) ) {f3 = false;}
		
		if( glfwGetKey(engineWindow, GLFW_KEY_KP_6) && !f4) { coordinates.y -= 1 * tick/1000.0f; f4 = true;}
		if( !glfwGetKey(engineWindow, GLFW_KEY_KP_6) ) {f4 = false;}
		
		if( glfwGetKey(engineWindow, GLFW_KEY_KP_9) && !f5) { coordinates.z += 1 * tick/1000.0f; f5 = true;}
		if( !glfwGetKey(engineWindow, GLFW_KEY_KP_9) ) {f5 = false;}
		
		if( glfwGetKey(engineWindow, GLFW_KEY_KP_7) && !f6) { coordinates.z -= 1 * tick/1000.0f; f6 = true;}
		if( !glfwGetKey(engineWindow, GLFW_KEY_KP_7) ) {f6 = false;}
	}

	
	//~ printVec3(coordinates)
	
}

milliseconds since_last_call()
{
	// retain time between calls (static)
	static hr_time_point previous = hr_clock::now();

	// get current time
	hr_time_point current = hr_clock::now();

	// get the time difference between now and previous call to the function
	milliseconds ms = std::chrono::duration_cast<milliseconds>(current - previous);

	// store current time for next call
	previous = current;

	// return elapsed time in milliseconds
	return ms;
}





//struct fhNovFlowData;

//biCycleSequencer<fhNovFlowData> Flow;
//~ 

//~ 



//~ std::shared_ptr<Level> level;	//Level* level = nullptr;



//~ 


//~ std::map<std::string, bool> conditions;						// словарь условий



