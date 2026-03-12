#pragma once
#ifndef SCENE_01_H
#define SCENE_01_H

#include <string>

#include "MicroWorld.h"


class Scene_01 : public MicroWorld
{
public:	
	Scene_01( std::string name  );
	
	void update() override;
	
	bool changeCollisionVisibility = false;

};

#endif
