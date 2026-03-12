#include "CollisionCalculator.h"


void CollisionCalculator::loadCollisionShell( const std::vector<glm::mat4>& faces )
{
	// функция инициализирует этот экземпляр класса коллизионной оболочки назначенными полигонами из faces
	
	if( !faces.empty() )
	{
		collisionTrianglesCount = faces.size();
		
		collisionTriangles = std::make_unique<glm::mat4[]>( collisionTrianglesCount );	// в умный указатель поместим массив матриц, описывающих коллизионные треугольники
		
		int i=0;
		for(auto f : faces)
		{
			//~ f calculate coefficients
			float a,b,c,d;
			glm::vec3 P0 = glm::vec3(f[0]);
			glm::vec3 P1 = glm::vec3(f[1]);
			glm::vec3 P2 =  glm::vec3(f[2]);
			
			calculateTriangleCoefficients(a, b, c, d, P0, P1, P2 );	// вычислим коэффициенты a,b,c,d
			
			f[0][3] = a;
			f[1][3] = b;
			f[2][3] = c;
			f[3][3] = d;
			
			collisionTriangles[i] = f;
			i++;
		}
		
		#ifdef COLLISION_DEBUG
			//~ checkCollisionTriangles();
		#endif
		
		
	}
	else
	{
		printMessage("Error load collision shell!") DEBUG_INFO
	}
	
	
}


void CollisionCalculator::checkCollisionTriangles()
{
	#ifdef COLLISION_DEBUG
		if(collisionTriangles)
		{
			printMessage("\nCheck collision triangles:")
		
			for(int i=0; i<collisionTrianglesCount; i++)
			{
				std::cout << "\t" << i << std::endl;
				std::cout << "\t\t Vertexes: " << std::endl;
				printVec3(collisionTriangles[i][0]) // 1 вершина
				printVec3(collisionTriangles[i][1])	// 2 вершина
				printVec3(collisionTriangles[i][2])	// 3 вершина
				std::cout << "\t\t And normal: " << std::endl;
				printVec3(collisionTriangles[i][3])	// нормаль
				
				std::cout << "\tA: " <<  collisionTriangles[i][0][3] << ", B: " <<  collisionTriangles[i][1][3] << ", C: " <<  collisionTriangles[i][2][3] << ", D: " <<  collisionTriangles[i][3][3] << std::endl;
			}
		}
		else
		{
			std::cout << "Error! Empty collision shell";
			DEBUG_INFO
		}
	#endif
	
}


// XXX проверка нахождения точки внутри замкнутого объема
bool CollisionCalculator::bCheckPoint( glm::vec3& point, glm::vec3& entityPos, glm::quat& entityRot, glm::vec3& setPos, 	glm::quat& setRot  )
{
	// point - в мировых координатах
	// а остальные аргументы характеризуют ориентацию оболочки в мире
	
	// нижеприведенный случай подходит для полностью замкнутой фигуры с нормалями наружу, состоящей минимум из 4 треугольников
	if(collisionTriangles && collisionTrianglesCount >= 4) // объемная ЗАМКНУТАЯ (!) фигура состоит минимум из 4 треугольников
	{
		// надо бросить луч из point в точку примерно посередине 0-го треугольника и проверить, скомпенсированы ли нормали "нанизанных" на этот луч треугольников
		
		// определим точку посередине 0-го треугольника
		glm::vec3 P0 = glm::vec3(collisionTriangles[0][0]);
		glm::vec3 P1 = glm::vec3(collisionTriangles[0][1]);
		glm::vec3 P2 = glm::vec3(collisionTriangles[0][2]);
		
		//~ printVec3(P0)
		//~ printVec3(P1)
		//~ printVec3(P2)
		
		glm::vec3 temp1 = P0+(P1-P0)*0.5f;
		
		glm::vec3 temp2 = P2+(temp1-P2)*0.5f; // нужные координаты точки примерно (!) в центре плоскости треугольника 0
		
		// теперь надо бросить вектор из point в точку temp2 и определить, какие треугольники нанижутся
		// как их определить?
		// надо вычислить t_param для вектора луча и найти точку пересечения с плоксотью треугольника
		// надо модифицировать функцию findPointRelativePosition, чтоб либо она искала точку вертикальной проекции, либо точку пересечения с плоскостью
		// как понять, что луч пересекает плоскость?
		// если t_param > 0, то луч пересекает плокость. Если < 0, то не пересекает. Но нужно еще проверить нахождение в границах!

		glm::vec3 axis = temp2 - point;	// вектор из точки point в 0-й треугольник
		
		
		int inters = 0;
		bool inside = false;
			
		
		// переберем все треугольники
		for(int i=0; i<collisionTrianglesCount; i++)
		{
			float tempDistance = 100.0f;
			float tempSinusK = 0.0f;
			glm::vec3 tempRicochet = glm::vec3(0,0,0);
			
			int res  = iCheckRayTriangleIntersection( point, axis, collisionTriangles[i], tempDistance, tempSinusK, tempRicochet); // 0 - нет пересечения, 1 - сверху-вниз, 2 - снизу-вверх
			
			if( res == 1 )	// пересечение треугольника "изнутри"
			{
				std::cout << "\nRay intersect triangle UP-DOWN " << i << std::endl;
				inters++;
			}
			if( res == 2 )	// пересечение "снаружи"
			{
				std::cout << "\nRay intersect triangle DOWN-UP " << i << std::endl;
				inters--;
			}
		}
		
		if( inters <  0) // если пересечений "изнутри" было больше (такие случаи декрементируют счетчик inters)
		{
			inside = true;
		}
		
		return inside;
	}
	else
	{
		return false;
	}
	
}
	
	
bool CollisionCalculator::bCheckAnotherAABB(CollisionCalculator& otherAABB)
{
	
}
	
// проверка пересечения лучом треугольников
bool CollisionCalculator::bCheckRay( const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const float& range, const glm::vec3& entityPos, const glm::quat& entityRot, const glm::vec3& setPos, 	const glm::quat& setRot, glm::vec3& ricochet, float& sinusK )
{
	// rayOrigin и rayDirection  - в мировых координатах
	// а остальные аргументы характеризуют ориентацию оболочки в мире
	
	// самый нормальный способ - выявить все треугольники, которые пересекает луч Сверху-Вниз, из них выявить ближайший к источнику и
	// если дистанция до точки пересечения больше порога и проекция высоты вне треугольника либо дальше порога, то можно двигаться
	if(collisionTriangles)
	{
		//~ printMessage("Calculate collision")
		
		//~ printVec3(rayOrigin)
		//~ printVec3(rayDirection)
		
		// параметры, характеризующие взаимодействие с ближайшим "лицевым" треугольником
		float 	nearestTriangleIntersection = 100.0f; // переменная для хранения дистанции и определения ближайшего треугольника
		int 	nearestTriangleIndex = -1; // индекс ближайшего треугольника, определяемый путем сравнения дистанции до текущего треугольника с nearestTriangleIntersection
		//float 	sinusK = 0.0f;						// синус угла падения луча к нормали
		//glm::vec3 ricochet = glm::vec3(0,0,0);		// вектор вдоль направления плоскости
		
		
		
		// поочередно: повернуть - сместить, потом выше по иерархии: повернуть-сместить
		//~ glm::vec3 	originInLocal = setRot * ( entityRot * rayOrigin + entityPos ) + setPos;
		//~ glm::vec3 	directionInLocal = entityRot * rayDirection;
					//~ directionInLocal = setRot * directionInLocal;
		
		// переведем параметры луча в локальное пространство оболочки
		glm::vec3 originInLocal = glm::inverse(entityRot)*(glm::inverse(setRot)*(rayOrigin - setPos) - entityPos); // координаты рассматриваемой точки в локальном пространстве треугольника, но с учетом его расположения в мире
		// т.е. проводим обратное преобразование - искому точку трансформируем, весь мир состоит только из этой точки и треугольника
		glm::vec3 directionInLocal = glm::inverse(entityRot)*( glm::inverse(setRot)*rayDirection ); // координаты рассматриваемой точки в локальном пространстве треугольника, но с учетом его расположения в мире

	///glm::vec3 rayDirInLocalSpace = glm::inverse(entityRot)*( glm::inverse(setRot)*rayDir ); // координаты рассматриваемой точки в локальном пространстве треугольника, но с учетом его расположения в мире

		//~ printVec3(originInLocal)
		//~ printVec3(directionInLocal)
		
		float tempDistance = 100.0f;
		float tempSinusK = 0.0f;
		glm::vec3 tempRicochet = glm::vec3(0,0,0);
		
		for(int i=0; i< collisionTrianglesCount; i++)
		{
			int res = iCheckRayTriangleIntersection( originInLocal, directionInLocal, collisionTriangles[i], tempDistance, tempSinusK, tempRicochet);
			
			if( res == 1 )	// пересечение треугольника "изнутри"
			{
				//~ std::cout << "\nRay intersect triangle UP-DOWN " << i << std::endl;
				if( tempDistance < nearestTriangleIntersection )
				{
					nearestTriangleIntersection = tempDistance;
					nearestTriangleIndex = i;
					
					sinusK = tempSinusK;
					ricochet = tempRicochet;
					
				}
			}
			//~ if( res == 2 )	// пересечение "снаружи"
			//~ {
				//~ std::cout << "\nRay intersect triangle DOWN-UP " << i << std::endl;
				
				//~ //if()
			//~ }
		}
		
		if(nearestTriangleIndex >= 0)
		{
			printVarComment("Nearest UP_DOWN triangle:", nearestTriangleIndex)
			printVarComment("Distance: ", nearestTriangleIntersection)
			
			printVariable(sinusK)
			printVec3(ricochet)
			
			// теперь надо вернуть вектор рикошета обратно в мировое пространство
			ricochet = setRot * ( entityRot * ricochet   ) ; // переведем в глобальное и запишем в переменную из аргумента
			
			// XXX надо еще определить где точка проекции высоты!
			
			//~ if( nearestTriangleIntersection < range )
			//~ {
				
			//~ }
			//~ else
			//~ {
				//~ return false;
			//~ }
			
			
			
			return nearestTriangleIntersection < range ? true : false;
			//~ return false;
		}
		else
		{
			//~ printMessage("No collisions!")
			return false;
		}
		//~ return false;
		
		
		
	}
	else
	{
		//~ printMessage("No collisionTriangles found!")
		//~ DEBUG_INFO
		return false;
	}
	
}
	
	
	
	
	
	
	
	
	
	





// вспомогательная функция для bCheckRay(). 0 - нет пересечения, 1 - сверху-вниз, 2 - снизу-вверх
int CollisionCalculator::iCheckRayTriangleIntersection( glm::vec3& origin, glm::vec3& direction, glm::mat4& triangle, float& distance, float& sinusK, glm::vec3& ricochet )
{
	// функция определит, пересекает ли луч из точки origin в направлении direction треугольник triangle и не выступает ли за геометрические пределы
		
	float a = triangle[0][3];															// коэффициент a плоскости
	float b = triangle[1][3];															// b
	float c = triangle[2][3];															// c 
	float d = triangle[3][3];															// коэффициент d равен 1
		
	glm::vec3 N = glm::vec3(triangle[3]);												// нормаль треугольника	

	// значение t_param позволит определить, имеет ли место пересечение ПЛОСКОСТИ, но надо еще уточнить, находится ли в пределах треугольника
	float t_param = -(a*origin.x + b*origin.y + c*origin.z + d)/(a*direction.x + b*direction.y + c*direction.z); 	// коэффициент параметризации плоскости (для вектора луча!)
	
	
	glm::vec3 intersectionPoint = direction*t_param + origin;   						// координаты точки пересечения лучом треугольника triangle
	glm::vec3 h = origin - intersectionPoint;											// вектор из точки пересечения до point
	
	distance = glm::length(h);															// расстояние до точки пересечения
	float cosinus = cosa(h, N);	
	
	
	// способ 2 - методом сравнения площадей по формуле Герона
	float Sall = calculateTriangleSquare( glm::vec3(triangle[0]), glm::vec3(triangle[1]), glm::vec3(triangle[2]) );	// площадь коллизионного треугольника
	
	float S0 = calculateTriangleSquare( glm::vec3(triangle[0]), glm::vec3(triangle[1]), intersectionPoint );		// площадь треугольника, образованного двумя вершинами и точкой проекции
	float S1 = calculateTriangleSquare( glm::vec3(triangle[1]), glm::vec3(triangle[2]), intersectionPoint );
	float S2 = calculateTriangleSquare( glm::vec3(triangle[2]), glm::vec3(triangle[0]), intersectionPoint );
	
	bool insideArea = abs(S0+S1+S2 - Sall) < 0.05f ? true : false;	// если разница суммы площадей и общей площади меньше 0.05, считается, что они равны, т.е. точка внутри треугольника
	
	
	
	
	float t_param_N = -(a*origin.x + b*origin.y + c*origin.z + d)/(a*N.x + b*N.y + c*N.z); 	// коэффициент параметризации плоскости (для вектора луча!)
	glm::vec3 intersectionPointN = N * t_param_N + origin;   								// координаты точки вертикальной проекции на плоскость
	//float distanceN = glm::length()
	
	
	//~ printMessage("\n\tcheckTriangleIntersection()")
	sinusK = cosa(direction, -N);
	sinusK = sqrt(1-sinusK*sinusK);	// пользуясь основным тригонометрическим тождеством, вычислим синус
	
	//~ printVarComment("Sinus k: ", sinusK)
	glm::vec3 H = glm::normalize(glm::cross(-N, direction));
	ricochet = glm::normalize(glm::cross(N, H)); // вектор вдоль треугольника в его локальном пространстве
	
	//~ printVec3(N)
	
	// XXX еще надо определеть направление вдоль стены
	// понадобятся нормаль и вектор взгляда
	//~ printMessage("Vector in local space")
	//~ printVec3(H)		
	//~ printVec3(ricochet)
	
	
	
	//~ printVariable(insideArea)
	//~ printVariable(cosinus)
	//~ printVariable(t_param)
	
	if(t_param < 0)
	{
		return 0;	// нет пересечения, т.к. вектор направлен в другую сторону от треугольника
	}
	else 			// есть пересечение - вектор направлен в сторону треугольника, уточним, с какой стороны он его пронизывает
	{
		
		
		
		
		if(insideArea)
		{
			return cosinus > 0 ? 1 : 2;	// 1 снаружи, 2 - изнутри
		}
		else
		{
			return 0; // нет пересечения, т.к. точка пересечения ВНЕ треугольника
		}
	}	
}







float CollisionCalculator::calculateTriangleSquare(glm::vec3 P0, glm::vec3 P1, glm::vec3 P2)
{
	float A = glm::length(P1 - P0);	// сторона треугольника
	float B = glm::length(P2 - P1);
	float C = glm::length(P0 - P2);
	float P = (A+B+C)/2;
	
	return sqrt( P*(P-A)*(P-B)*(P-C) );
}















// с какой стороны находится точка относительно треугольника
int CollisionCalculator::findPointRelativePosition( const glm::vec3& point, const glm::mat4& triangle )
{
	// функция определит, как расположена точка point относительно треугольника triangle ( т.е. с какой стороны от него она находится и не выступает ли за геометрические пределы)
		
	float a = triangle[0][3];															// коэффициент a плоскости
	float b = triangle[1][3];															// b
	float c = triangle[2][3];															// c 
	float d = triangle[3][3];															// коэффициент d равен 1
	
	glm::vec3 N = glm::vec3(triangle[3]);												// нормаль треугольника
	
	// XXX надо модифицировать, чтоб находить точку пересечения лучом и критерий пересечения - 
	float t_param = -(a*point.x + b*point.y + c*point.z + d)/(a*N.x + b*N.y + c*N.z); 	// коэффициент параметризации плоскости (для нормали!)
	
	glm::vec3 intersectionPoint = N*t_param + point;   									// координаты точки проекции point-а на плоскость треугольника triangle
	
	glm::vec3 h = point - intersectionPoint;											// вектор из точки проекции вертикально до point. Его длина = расстояние от точки point до плоскости
	
	// способ 2 - методом сравнения площадей по формуле Герона
	float Sall = calculateTriangleSquare( glm::vec3(triangle[0]), glm::vec3(triangle[1]), glm::vec3(triangle[2]) );	// площадь коллизионного треугольника
	
	float S0 = calculateTriangleSquare( glm::vec3(triangle[0]), glm::vec3(triangle[1]), intersectionPoint );		// площадь треугольника, образованного двумя вершинами и точкой проекции
	float S1 = calculateTriangleSquare( glm::vec3(triangle[1]), glm::vec3(triangle[2]), intersectionPoint );
	float S2 = calculateTriangleSquare( glm::vec3(triangle[2]), glm::vec3(triangle[0]), intersectionPoint );
	
	bool insideArea = abs(S0+S1+S2 - Sall) < 0.05f ? true : false;	// если разница суммы площадей и общей площади меньше 2, считается, что они равны, т.е. точка в "воздушном пространстве" коллизионной оболочки
	
	float cosinus = cosa(h, N);										// определим скалярное произведение между вектором нормали и вектором проекции точки на плоскость
	
	//~ printVariable(a)
	//~ printVariable(b)
	//~ printVariable(c)
	//~ printVariable(d)
	//~ printVariable(t_param)
	//~ printVec3(point)
	//~ printVec3(N)
	//~ printVec3(intersectionPoint)
	//~ printVariable(insideArea)
	//~ printVariable(cosinus)
	
	
	// возвращая int = situations{ FRONT_INSIDE, FRONT_OUTSIDE, BACK_INSIDE, BACK_OUTSIDE}, определим, какое взаимное расположение точки и треугольника имеет место быть
	
	if( cosinus > 0 )				// спереди
	{
		return insideArea ? 0 : 1;	// тернарно определим, внутри/вне
	}
	else 							// сзади
	{
		return insideArea ? 2 : 3;	// тернарно определим, внутри/вне
	}
	
}







float CollisionCalculator::cosa(const glm::vec3& U, const glm::vec3& V) // dot
{
	// XXX упростить формулу
	// получение косинуса угла между векторами U и V
	return (U.x*V.x + U.y*V.y + U.z*V.z)    / ( sqrt(U.x*U.x + U.y*U.y + U.z*U.z) * sqrt(V.x*V.x + V.y*V.y + V.z*V.z) );
}





void CollisionCalculator::calculateTriangleCoefficients(float& a, float&b, float&c, float&d, glm::vec3& P1, glm::vec3& P2, glm::vec3& P3)	//вызывается из загрузчика меша
{
	///d = 1.0f;
	
	///float delta = P1.x*P2.y*P3.z + P2.x*P3.y*P1.z + P3.x*P1.y*P2.z - 
	
	// координаты трех точек треугольника
	float x1 = P1.x + 0.001f;
	float y1 = P1.y + 0.001f;
	float z1 = P1.z + 0.001f;
	
	float x2 = P2.x + 0.001f;
	float y2 = P2.y + 0.001f;
	float z2 = P2.z + 0.001f;
	
	float x3 = P3.x + 0.001f;
	float y3 = P3.y + 0.001f;
	float z3 = P3.z + 0.001f;


	//вычисляем коэффициенты a,b,c,d плоскости методом Крамера
	d = 1.0f;	// d принимаем равным единице

	// основной определитель
	float delta = x1*y2*z3 + x2*y3*z1 + x3*y1*z2  - z1*y2*x3 - z2*y3*x1 - z3*y1*x2;
	if(abs(delta) < 0.0001f) delta = 0.0001f;
	//~ printVarComment("delta  ", delta)

	// определитель А
	float deltaA = (-d)*y2*z3 + (-d)*y3*z1 + (-d)*y1*z2  -  z1*y2*(-d)  - z2*y3*(-d)  - z3*y1*(-d);
	if(abs(deltaA) < 0.0001f) deltaA = 0.0001f;
	//~ printVarComment("deltaA ", deltaA)
	
	// определитель В
	float deltaB = x1*(-d)*z3 + x2*(-d)*z1 + x3*(-d)*z2  -  z1*(-d)*x3 - z2*(-d)*x1 - z3*(-d)*x2;
	if(abs(deltaB) < 0.0001f) deltaB = 0.0001f;
	//~ printVarComment("deltaB ", deltaB) 
	
	// определитель С
	float deltaC = x1*y2*(-d) + x2*y3*(-d) + x3*y1*(-d)  -  (-d)*y2*x3 - (-d)*y3*x1 - (-d)*y1*x2;
	if(abs(deltaC) < 0.0001f) deltaC = 0.0001f;
	//~ printVarComment("deltaC ", deltaC)
	
	a = deltaA/delta;
	b = deltaB/delta;
	c = deltaC/delta;
	
	// XXX на линуксе этот код выполняется чаще, что приводит к неочевидным ошибкам! См. скриншот сравнения ОС
	//~ if(abs(a) < 0.0001) a = 0.0001;
	//~ if(abs(b) < 0.0001) b = 0.0001;
	//~ if(abs(c) < 0.0001) c = 0.0001;
	
	// методом Крамера
}






#ifndef MAIN_POINT
//~ #include "auxiliary.cpp"

int main()
{
	CollisionCalculator aabb;
	
	glm::mat4 tr0;
	tr0[0] = glm::vec4(-1.136400,3.041400,3.337600,0);
	tr0[1] = glm::vec4(2.875400,3.041400,0.663100,0);
	tr0[2] = glm::vec4(-1.136400,0.366900,-0.674200,0);
	tr0[3] = glm::vec4(-0.346844,0.780399,-0.520266,0);


	glm::mat4 tr1;
	tr1[0] = glm::vec4(0.200900,-0.970400,3.337600,0);
	tr1[1] = glm::vec4(-1.136400,0.366900,-0.674200,0);
	tr1[2] = glm::vec4(2.875400,-0.970400,-0.674200,0);
	tr1[3] = glm::vec4(-0.309426,-0.928279,-0.206284,0);


	glm::mat4 tr2;
	tr2[0] = glm::vec4(-1.136400,3.041400,3.337600,0);
	tr2[1] = glm::vec4(0.200900,-0.970400,3.337600,0);
	tr2[2] = glm::vec4(2.875400,3.041400,0.663100,0);
	tr2[3] = glm::vec4(0.545455,0.181818,0.818182,0);


	glm::mat4 tr3;
	tr3[0] = glm::vec4(0.200900,-0.970400,3.337600,0);
	tr3[1] = glm::vec4(-1.136400,3.041400,3.337600,0);
	tr3[2] = glm::vec4(-1.136400,0.366900,-0.674200,0);
	tr3[3] = glm::vec4(-0.928279,-0.309426,0.206284,0);


	glm::mat4 tr4;
	tr4[0] = glm::vec4(2.875400,3.041400,0.663100,0);
	tr4[1] = glm::vec4(0.200900,-0.970400,3.337600,0);
	tr4[2] = glm::vec4(2.875400,-0.970400,-0.674200,0);
	tr4[3] = glm::vec4(0.818182,-0.181818,0.545455,0);


	glm::mat4 tr5;
	tr5[0] = glm::vec4(2.875400,3.041400,0.663100,0);
	tr5[1] = glm::vec4(2.875400,-0.970400,-0.674200,0);
	tr5[2] = glm::vec4(-1.136400,0.366900,-0.674200,0);
	tr5[3] = glm::vec4(0.104828,0.314485,-0.943456,0);
	
	std::vector <glm::mat4> shell;
	shell.push_back(tr0);
	shell.push_back(tr1);
	shell.push_back(tr2);
	shell.push_back(tr3);
	shell.push_back(tr4);
	shell.push_back(tr5);
	
	
	aabb.loadCollisionShell( shell );
	
	aabb.checkCollisionTriangles();
	
	// FRONT_INSIDE, FRONT_OUTSIDE, BACK_INSIDE, BACK_OUTSIDE
	printMessage("\nstep 0")
	std::cout << aabb.bCheckCollisionTrianglesIntersection(glm::vec3(0,0,3) ) << std::endl;
	
	printMessage("\nstep 1")
	std::cout << aabb.bCheckCollisionTrianglesIntersection(glm::vec3(2,2,3) ) << std::endl;
	
	printMessage("\nstep 2")
	std::cout << aabb.bCheckCollisionTrianglesIntersection(glm::vec3(0,0,0) ) << std::endl;
	
	printMessage("\nstep 3")
	std::cout << aabb.bCheckCollisionTrianglesIntersection(glm::vec3(2,2,0) ) << std::endl;
		
	printMessage("\nstep 4")
	std::cout << aabb.bCheckCollisionTrianglesIntersection(glm::vec3(0,0,-3) ) << std::endl;
	
	printMessage("\nstep 5")
	std::cout << aabb.bCheckCollisionTrianglesIntersection(glm::vec3(2,2,-3) ) << std::endl;
	
	printMessage("\nstep 6")
	std::cout << aabb.bCheckCollisionTrianglesIntersection(glm::vec3(-0.96655, 0.53449,0.82723) ) << std::endl;
	
	printMessage("\nstep 7")
	std::cout << aabb.bCheckCollisionTrianglesIntersection(glm::vec3(-0.83775,0.63437,0.82822) ) << std::endl;
	
	printMessage("\nstep 8")
	std::cout << aabb.bCheckCollisionTrianglesIntersection(glm::vec3(-1.0053, 2.4481,-1.0914) ) << std::endl;
	
	printMessage("\nstep 9")
	std::cout << aabb.bCheckCollisionTrianglesIntersection(glm::vec3(1.99, 1.41, 0.75) ) << std::endl;
	
	printMessage("\nstep 10")
	std::cout << aabb.bCheckCollisionTrianglesIntersection(glm::vec3(1.49, 1.29, 0.76) ) << std::endl;
	
	printMessage("\nstep 11")
	std::cout << aabb.bCheckCollisionTrianglesIntersection(glm::vec3(-2.28, 0.43, -4.71) ) << std::endl;
	
	printMessage("\nstep 12")
	std::cout << aabb.bCheckCollisionTrianglesIntersection(glm::vec3(-4.19, -0.54, 0.22) ) << std::endl;
	
	system("pause");
	return 0;
}

#endif
