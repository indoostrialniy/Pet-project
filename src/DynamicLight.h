#pragma once
#ifndef DYNAMIC_LIGHT_H
#define DYNAMIC_LIGHT_H

#include "MicroWorld.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class DynamicLight_01 : public MicroWorld
{
public:
	DynamicLight_01();
	DynamicLight_01(glm::vec3 worldLoc = glm::vec3(0, 0, 0), glm::quat worldRot = glm::quat(1, 0, 0, 0), std::string name = "default_light");
	
	//LIGHTS[0] - Position;
    //LIGHTS[1] - Direction;
    //LIGHTS[2] - Color;
    //LIGHTS[3] - TypeSectorRadiusPower;   // x - тип (omni, spot, etc), y-ширина конуса луча, если тип=spot,  z- радиус освещения (для оптимизации), w-мощность
	
	
	
	glm::vec4 colorVec = glm::vec4(1, 0.64, 0, 0);
	glm::vec4 typeSectorRadiusPower = glm::vec4(0, 0, 1, 1);
	
};



#endif
