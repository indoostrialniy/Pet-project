#pragma once
#ifndef COLLISION_DESCRIPTOR_H
#define COLLISION_DESCRIPTOR_H

#include <string>
#include <memory>

class CollisionShell;

enum COLLISION_TYPE{ NO_COLLISION, SPHERE_COLLISION, CUBE_COLLISION, COMPLEX_COLLISION }; // 0 1 2 3

// структура CollisionDescriptor определяет параметры коллизии сущности
struct CollisionDescriptor	
{
public:

	int 			collisionType = NO_COLLISION; 						// по умолчанию коллизия отсутствует В ПРИНЦИПЕ. Тогда нижеприведенные параметры тоже не нужны
	
	// но если collisionType != NO_COLLISION, то в дело вступает уточняющий параметр collisionRadius
	float 			collisionRadius = 1.0f;								// по умолчанию радиус 1 метр
	
	// а если коллизия явно указана как комплексная, то уточняем еще и название файла с ее геометрией и умный указатель на нее как на ресурс
	std::string 						complexCollisionFileName = "default";
	std::shared_ptr<CollisionShell>		complexCollisionShellPtr;		// класс CollisionShell служит для обработки комплексной коллизии как ресурса, аналогично Mesh, Material etc


	void setCollisionValue(bool state)
	{
		collisionActive = state;
	}
	
	bool getCollisionValue()
	{
		return collisionActive;
	}



	// потенциально ненужное
	// конструктор принимает тип (0,1,2,3), радиус (если требуется) и имя файла сложного меша оболочки (тоже если требуется)
	CollisionDescriptor(int type = NO_COLLISION, float radius = 1.0f, std::string name = "default")	
	{
		collisionType = type;
		collisionRadius = radius;
		complexCollisionFileName = name;
	}
	
private:
	bool 			collisionActive = false;							// параметр, отвечающий за то, учитывать коллизию или нет
	
	
};

#endif
