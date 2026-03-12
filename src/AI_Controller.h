#pragma once
#ifndef AI_CONTROLLER_H
#define AI_CONTROLLER_H

#include "MicroWorld.h"


class AI_Controller : public MicroWorld
{
public:
	AI_Controller(){}

	void logic() override;
	
};


#endif
