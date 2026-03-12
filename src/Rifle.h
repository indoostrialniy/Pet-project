#pragma once
#ifndef RIFLE_H
#define RIFLE_H

#include "Weapon.h"
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Rifle : public Weapon
{
public:
	Rifle();
	
	Rifle(glm::vec3 worldLoc, glm::quat worldRot, std::string name );
};


#endif
