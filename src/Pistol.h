#pragma once
#ifndef PISTOL_H
#define PISTOL_H

#include "Weapon.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Pistol : public Weapon
{
public:
	Pistol();
	
	Pistol(glm::vec3 worldLoc, glm::quat worldRot, std::string name );
};


#endif
