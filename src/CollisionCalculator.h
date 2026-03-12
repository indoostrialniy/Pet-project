#pragma once
#ifndef COLLISION_CALCULATOR_H
#define COLLISION_CALCULATOR_H

//~ #define COLLISION_DEBUG

class CollisionCalculator // каждому мешу соответствует своя собственная коробка. Потери памяти незначительны, т.к на самую простую - коробку 12 треугольников - уйдет 12*3*6*4 = 864 байта. На 10000 мешей с простейшими коробками уйдет всего 9 МБ оперативки, а 10000 мешей на сцене это уже солидно. К тому же, меши как раз-таки регулируются по дубликатам
{
public:
	CollisionCalculator(){}
		
	
	void loadCollisionShell( const std::vector<glm::mat4>& faces ); // загрузка оболочки
	
	void checkCollisionTriangles();		// потенциально можно сделать защищенной. Обычная проверка корректности загрузки оболочки
	
	// проверка нахождения точки внутри замкнутой оболочки
	// принимает параметры ориентации оболочки и переводит систему координат
	bool bCheckPoint( glm::vec3& point, glm::vec3& entityPos, glm::quat& entityRot, glm::vec3& setPos, 	glm::quat& setRot  );
	
	
	bool bCheckAnotherAABB(CollisionCalculator& otherAABB);
	
	// кастование оболочки лучом
	// принимает параметры ориентации оболочки и переводит систему координат
	bool bCheckRay( const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const float& range, const glm::vec3& entityPos, const glm::quat& entityRot, const glm::vec3& setPos, 	const glm::quat& setRot, glm::vec3& ricochet, float& sinusK );
		
	
	int iCheckRayTriangleIntersection( glm::vec3& origin, glm::vec3& direction, glm::mat4& triangle, float& distance, float& sinusK, glm::vec3& ricochet );
	
protected:
		
	float cosa(const glm::vec3& U, const glm::vec3& V);

	void calculateTriangleCoefficients(float& a, float&b, float&c, float&d, glm::vec3& P1, glm::vec3& P2, glm::vec3& P3);
	

	std::unique_ptr<glm::mat4[]> collisionTriangles;	// умный указатель для хранения динамического массива коллизионных треугольников в количестве collisionTrianglesCount
/*
	один коллизионный треугольник хранит 3 xyz-координаты точек v0-v2, вектор нормали N, коэффициенты a,b,c,d треугольника и описывается так:
	mat4 triangle;
	triangle[0] = vec4( v0x, v0y, v0z, 	a );
	triangle[1] = vec4( v1x, v1y, v1z, 	b );
	triangle[2] = vec4( v2x, v2y, v2z, 	c );
	triangle[3] = vec4( Nx, Ny, Nz, 	d );
*/

	int collisionTrianglesCount = 0;			// количество коллизионных треугольников
	
	// 0 1 2 3 - перечисление возможных положений рассматриваемой точки
	// итого есть 4 возможные ситуации расположения точки point относительно треугольника: 
	// 0 - точка перед треугольником и в его границах 					(FRONT_INSIDE)	intersection = false
	// 1 - точка перед треугольником, но вне его границ 				(FRONT_OUTSIDE)	intersection = false
	// 2 - точка за треугольником и в его границах 						(BACK_INSIDE)	intersection = true (чистейшее пересечение)
	// 3 - точка за треугольником, но вне его границ 					(BACK_OUTSIDE)	intersection = false
	// сохраним их в виде перечисления:
	enum situations{ FRONT_INSIDE, FRONT_OUTSIDE, BACK_INSIDE, BACK_OUTSIDE};
	
	float calculateTriangleSquare(glm::vec3 P0, glm::vec3 P1, glm::vec3 P2);			// вспомогательная функция для вычисления площади треугольника формулой Герона
	
	int findPointRelativePosition( const glm::vec3& point, const glm::mat4& triangle );
	
	
//----------------------------------------------------------------------	


	//~ bool bInVolume( const glm::vec3& point ); 


	//~ glm::mat4* AABB_Box = nullptr;
	
	//~ int AABB_triangles = 0;					// количество треугольников в ААВВ-коробке
	//~ int	meshAABBCount = 0;					// количество флоатов для ААВВ_коробки
	
//~ public:
	// коробка относится к мешу, меш - это сущность, а сет состоит из сущностей, соответственно, нужно в аргумент передать мировые координаты сущности 
	//~ bool bCheckRayAABBIntersection(glm::vec3& slider, float& k, const glm::vec3& rayDir, const glm::vec3& rayOrigin, float rangeLimit, const glm::vec3& entityPos, const glm::quat& entityRot, const glm::vec3& MicroWorldPos, const glm::quat& MicroWorldRot);
	
	
	//~ bool bCheckRayCollisionTrianglesIntersection( 	const glm::vec3& origin, 	const glm::vec3& direction, const float& range, const glm::vec3& entityPos, const glm::quat& entityRot, const glm::vec3& setPos, 	const glm::quat& setRot  );
	
	
	
	
	//~ int checkTriangleIntersection(const glm::mat4& triangle, const glm::vec3 point, float& k, glm::vec3&slideVector, float& h_length, const glm::vec3& rayDir, const glm::vec3& entityPos, const glm::quat& entityRot, const glm::vec3& MicroWorldPos, const glm::quat& MicroWorldRot);
	
	
	//~ float getDistanceToTriangle(float A, float B, float C, float D, 
								//~ glm::vec3 Point, const glm::vec3& entityPos, const glm::quat& entityRot, const glm::vec3& MicroWorldPos, const glm::quat& MicroWorldRot);
	
	//~ void checkAABB();


//~ protected:
	
	
};


#endif
