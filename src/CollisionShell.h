#pragma once
#ifndef COLLISION_SHELL_H
#define COLLISION_SHELL_H

#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>


// класс CollisionShell служит для обработки комплексной коллизии

class CollisionShell 
{
public:
	// обязательные методы и переменные класса ресурса публичны
	CollisionShell();
	CollisionShell(std::string collisionName);
	
	bool status = false;
	
	std::string resourceName = "none";
	
	void printResourceData();			// обязательная функция, но можно оставить тело пустым или попытаться скрыть
	
	~CollisionShell();
	
private:
	
	// пользовательские переменные и методы
	
	int vertices_count = -1;											// число вершин, должно быть >= 1
	int edges_count = -1;												// число граней
	int polygons_count = -1;											// число треугольников
	
	std::unique_ptr<glm::vec3[]>		vertices;						// массив вершин
	std::unique_ptr<glm::vec2[]>		edges;							// массив индексов вершин граней
	std::unique_ptr<glm::mat3[]>		polygons;						// массив индексов вершин треугольников (0 столб), нормалей этих треугольников (1 столб) и место для хранения коэффициентов a,c,d плоскости (2 столб, d = 1 всегда)
	
	/*
	Формат хранения данных:
		vertices: 0) xyz		координаты вершины
				1) xyz
				...
				n-1) xyz
				
		edges: 	0) v0_i v1_i	индексы обеих вершин грани
				1) v0_i v1_1
				...
				n-1) v0_i v1_i
				
		polygons: 0) v0_i v1_i v2_i
					 nx ny nz
					 a b c
	  
	*/
	
public:
	int 		getVerticesCount();																		// вернуть количество вершин оболочки
	
	int 		getEdgesCount();																		// вернуть количество граней оболочки
	
	int 		getPolygonsCount();																		// вернуть количество треугольников оболочки


	glm::vec3 	getVertexCoordinates(int vertex_index);													// вернуть координаты вершины с заданным индексом 
	
	void 		getEdgeVertexes(int edge_index, glm::vec3& v0, glm::vec3& v1);							// в присланные v0 и v1 записать координаты вершин, составляющих указанную грань
	
	void 		getPolygonVertexes(int polygon_index, glm::vec3& v0, glm::vec3& v1, glm::vec3& v2);		// в присланные v0, v1 и v2 записать координаты вершин, составляющих указанный треугольник
	
	glm::vec3 	getPolygonNormal(int polygon_index);													// вернуть нормаль указанного треугольника
	
	void 		getPolygonCoefficients(int polygon_index, float& a, float& b, float& c);				// в прсисланные a,b,c записать соответствующие коэффициенты треугольника
	
	glm::mat3	getPolygon(int polygon_index);
	
	
private:
	// все для загрузки и конфигурации оболочки
	bool collisionLoad(std::string name);

	std::string collisionGetPath(std::string filename);
	
	void calculateTriangleCoefficients(float& a, float&b, float&c, float&d, glm::vec3& P1, glm::vec3& P2, glm::vec3& P3);

	void collisionCheck();												// метод проверки загруженной сложной оболочки
	
	
	
	
	
	
	
	//void loadCollisionShell( const std::vector<glm::mat4>& faces ); // загрузка оболочки
	
	//void checkCollisionTriangles();		// потенциально можно сделать защищенной. Обычная проверка корректности загрузки оболочки
	
	//~ void calculateTriangleCoefficients_new();
	
	
	//int collisionTrianglesCount = 0;			// количество коллизионных треугольников
	
	//std::unique_ptr<glm::mat4[]> collisionTriangles;	// умный указатель для хранения динамического массива коллизионных треугольников в количестве collisionTrianglesCount
	
	
};

//~ template <typename D>
//~ class resManAsset
//~ {
	
//~ };




#endif
