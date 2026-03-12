#include "CollisionShell.h"

#include "macros.h"
#include <iostream>
#include <nameof.hpp>
#include <fstream>  //ifstream
#include "auxiliary.h"

CollisionShell::CollisionShell( std::string collisionName )
{
	#ifdef COLLISION_DEBUG
		std::cout << "Constructor of [" << NAMEOF_TYPE(decltype(this)) << "] [" <<  collisionName << "]" << std::endl;
	#endif
	
	status = collisionLoad(collisionName) ? true : false; //meshLoad(meshName);
	
	resourceName = collisionName;
}


bool 			CollisionShell::collisionLoad(std::string name)
{
	std::string collisionFileSource = collisionGetPath(name);

	#ifdef COLLISION_DEBUG
		printVarComment("Load collision ", collisionFileSource)
	#endif
	
	
	std::ifstream collisionStream(collisionFileSource);									// открываем поток с кодом шейдерной программы из файла
	
	if ( collisionStream.is_open() )
	{
		std::string StrVar = "";
		
		int v = 0;
		int e = 0;
		int p = 0;
		//bool counts_catched = false;
		
		while (getline(collisionStream, StrVar)) 										// записываем в VertexShaderStream строки из файла, пока они есть
		{
			std::string Parameter = getValue(StrVar, 0);
						
			//~ printVariable( StrVar )
			
			//while( Parameter != "data" && !counts_catched)
			//{
			//getline(collisionStream, StrVar);
			
			if( Parameter == "vertices" )
			{
				vertices_count = stoi( getValue(StrVar, 1) );
				//~ printVariable(vertices_count)
				vertices = std::make_unique<glm::vec3[]>(vertices_count);
			}
			if( Parameter == "edges" )
			{
				edges_count = stoi( getValue(StrVar, 1) );
				//~ printVariable(edges_count)
				edges = std::make_unique<glm::vec2[]>(edges_count);
			}
			if( Parameter == "polygons" )
			{
				polygons_count = stoi( getValue(StrVar, 1) );
				//~ printVariable(polygons_count)
				polygons = std::make_unique<glm::mat3[]>(polygons_count);
			}
			//}
				
				
			
			if(Parameter == "v")
			{
				readFVec3( vertices[v], StrVar, 2 ) 
				v++;
			}
			
			if(Parameter == "e")
			{
				readFVec2( edges[e], StrVar, 2 ) 
				e++;
			}
			if(Parameter == "p")
			{
				readFVec3( polygons[p][0], StrVar, 2 ) // считали индексы вершин треугольника
				readFVec3( polygons[p][1], StrVar, 5 ) // считали нормаль треугольника
				
				float d = 1.0f;
				calculateTriangleCoefficients(polygons[p][2][0], polygons[p][2][1], polygons[p][2][2], d, vertices[ polygons[p][0][0] ], vertices[ polygons[p][0][1] ], vertices[ polygons[p][0][2] ] );

				
				
				p++;
			}
			
						
		}
		
		//~ loadCollisionShell( triangles );
		
		//~ triangles.clear();
		
		if( vertices_count > 0 && edges_count > 0 && polygons_count > 0 )
		{
			//~ printVariable(vertices_count)
			//~ printVariable(edges_count)
			//~ printVariable(polygons_count)
			//~ counts_catched = true;
		}
		else
		{
			printMessage("Error by loading collision!")
			DEBUG_INFO
			return false;
		}
		
			
		//collisionCheck();
		
		#ifdef COLLISION_DEBUG
			printMessage("Collision loaded successfully!")
		#endif
		
		collisionStream.close();	
		
		return true;
	}
	else
	{
		std::cout << "Cannnot find collision shell source file: " << collisionFileSource; 
		DEBUG_INFO 																		// выведем в консоль эту точку исходника
		return false;
	}
	
}

std::string 	CollisionShell::collisionGetPath(std::string filename)			
{	
	return getOperationSystem() == "Windows" ? "datas\\meshes\\" + filename + ".collision" : "./datas/meshes/" + filename + ".collision" ;	//тернарный определитель пути	
}

void 			CollisionShell::calculateTriangleCoefficients(float& a, float&b, float&c, float&d, glm::vec3& P1, glm::vec3& P2, glm::vec3& P3)	//вызывается из загрузчика меша
{
	///d = 1.0f;
	
	///float delta = P1.x*P2.y*P3.z + P2.x*P3.y*P1.z + P3.x*P1.y*P2.z - 
	
	// координаты трех точек треугольника
	//~ float x1 = P1.x + 0.001f;
	//~ float y1 = P1.y + 0.001f;
	//~ float z1 = P1.z + 0.001f;
	
	//~ float x2 = P2.x + 0.002f;
	//~ float y2 = P2.y + 0.002f;
	//~ float z2 = P2.z + 0.002f;
	
	//~ float x3 = P3.x + 0.003f;
	//~ float y3 = P3.y + 0.003f;
	//~ float z3 = P3.z + 0.003f;
	
	
	float x1 = P1.x ;
	float y1 = P1.y ;
	float z1 = P1.z ;
	
	float x2 = P2.x ;
	float y2 = P2.y ;
	float z2 = P2.z ;
	
	float x3 = P3.x ;
	float y3 = P3.y ;
	float z3 = P3.z ;
	


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

void 			CollisionShell::collisionCheck()
{
	
	for(int i=0; i<vertices_count; i++)
	{
		printVec3(vertices[i])
	}
	
	for(int i=0; i<edges_count; i++)
	{
		printVec2(edges[i])
	}
	
	for(int i=0; i<polygons_count; i++)
	{
		printMat3("faces ", polygons[i])
	}
}




int 			CollisionShell::getVerticesCount()										// вернуть количество вершин оболочки
{
	return vertices_count;
}

int 			CollisionShell::getEdgesCount()										// вернуть количество граней оболочки
{
	return edges_count;
}

int 			CollisionShell::getPolygonsCount()										// вернуть количество треугольников оболочки
{
	return polygons_count;
}

glm::vec3 		CollisionShell::getVertexCoordinates(int vertex_index)					// вернуть координаты вершины с заданным индексом 
{
	return vertices[vertex_index];
}

void 			CollisionShell::getEdgeVertexes(int edge_index, glm::vec3& v0, glm::vec3& v1)	// в присланные v0 и v1 записать координаты вершин, составляющих указанную грань
{
	v0 = vertices[ edges[edge_index].x ];
	v1 = vertices[ edges[edge_index].y ];
}

void 			CollisionShell::getPolygonVertexes(int polygon_index, glm::vec3& v0, glm::vec3& v1, glm::vec3& v2)	// // в присланные v0, v1 и v2 записать координаты вершин, составляющих указанный треугольник
{
	v0 = vertices[ polygons[polygon_index][0].x ];
	v1 = vertices[ polygons[polygon_index][0].y ];
	v2 = vertices[ polygons[polygon_index][0].z ];
}

glm::vec3 		CollisionShell::getPolygonNormal(int polygon_index)					// вернуть нормаль указанного треугольника
{
	return polygons[polygon_index][1];
}

void 			CollisionShell::getPolygonCoefficients(int polygon_index, float& a, float& b, float& c)	// в прсисланные a,b,c записать соответствующие коэффициенты треугольника
{
	a = polygons[polygon_index][2].x;
	b = polygons[polygon_index][2].y;
	c = polygons[polygon_index][2].z;
}

glm::mat3		CollisionShell::getPolygon(int polygon_index)
{
	return polygons[polygon_index];
}





	
	
//~ void CollisionShell::loadCollisionShell( const std::vector<glm::mat4>& faces )
//~ {
	//~ // функция инициализирует этот экземпляр класса коллизионной оболочки назначенными полигонами из faces
	
	//~ if( !faces.empty() )
	//~ {
		//~ collisionTrianglesCount = faces.size();
		
		//~ collisionTriangles = std::make_unique<glm::mat4[]>( collisionTrianglesCount );	// в умный указатель поместим массив матриц, описывающих коллизионные треугольники
		
		//~ int i=0;
		//~ for(auto f : faces)
		//~ {
			//f calculate coefficients
			//~ float a,b,c,d;
			//~ glm::vec3 P0 = glm::vec3(f[0]);
			//~ glm::vec3 P1 = glm::vec3(f[1]);
			//~ glm::vec3 P2 =  glm::vec3(f[2]);
			
			//~ calculateTriangleCoefficients(a, b, c, d, P0, P1, P2 );	// вычислим коэффициенты a,b,c,d
			
			//~ f[0][3] = a;
			//~ f[1][3] = b;
			//~ f[2][3] = c;
			//~ f[3][3] = d;
			
			//~ collisionTriangles[i] = f;
			//~ i++;
		//~ }
		
		//~ #ifdef COLLISION_DEBUG
			//~ checkCollisionTriangles();
		//~ #endif
		
		
	//~ }
	//~ else
	//~ {
		//~ printMessage("Error load collision shell!") DEBUG_INFO
	//~ }
	
	
//~ }


//~ void CollisionShell::checkCollisionTriangles()
//~ {
	//~ #ifdef COLLISION_DEBUG
		//~ if(collisionTriangles)
		//~ {
			//~ printMessage("\nCheck collision triangles:")
		
			//~ for(int i=0; i<collisionTrianglesCount; i++)
			//~ {
				//~ std::cout << "\t" << i << std::endl;
				//~ std::cout << "\t\t Vertexes: " << std::endl;
				//~ printVec3(collisionTriangles[i][0]) // 1 вершина
				//~ printVec3(collisionTriangles[i][1])	// 2 вершина
				//~ printVec3(collisionTriangles[i][2])	// 3 вершина
				//~ std::cout << "\t\t And normal: " << std::endl;
				//~ printVec3(collisionTriangles[i][3])	// нормаль
				
				//~ std::cout << "\tA: " <<  collisionTriangles[i][0][3] << ", B: " <<  collisionTriangles[i][1][3] << ", C: " <<  collisionTriangles[i][2][3] << ", D: " <<  collisionTriangles[i][3][3] << std::endl;
			//~ }
		//~ }
		//~ else
		//~ {
			//~ std::cout << "Error! Empty collision shell";
			//~ DEBUG_INFO
		//~ }
	//~ #endif
	
//~ }


//~ void CollisionShell::calculateTriangleCoefficients_new()
//~ {
	
//~ }





CollisionShell::~CollisionShell()
{
	
}





