#pragma once
#ifndef DYNAMIC_BALL_H
#define DYNAMIC_BALL_H

class Dynamic_Ball : public MicroWorld
{
public:	
	Dynamic_Ball( glm::vec3 worldLoc, glm::quat worldRot, std::string n );

	void update() override;
	
	//glm::vec3()
	
	glm::vec3 v3Reflect = glm::vec3(0,0,0);
	float fSlideCoeff = 0.0f;
	
	float v = 0.0f;
	
	int t=0;
	
	float mass = 1.0f;
	float g = 9.8f;
	
	float RadiusOfCollision = 0.5f;
	
	float velocityZ = 0.0f;
	
	float Eh = mass * g * position.z;									// стартовая потенциальная энергия
	float Ek = mass * velocityZ*velocityZ / 2;							// стартовая кинетическая энергия
	
	float speed = 0.0f;
	
};


//~ // определим скорость смещения
		//~ float moveSpeed = 0.05f * slideK;
		//~ //найдем вектор влево, имея вектор вверх и вектор направления
		//~ glm::vec3 forward = glm::normalize( glm::vec3(slide.x, slide.y, 0.0f) );
		//~ glm::vec3 left = glm::cross( glm::vec3(0,0,1), forward );
		//~ rotation = glm::angleAxis( moveSpeed, left ) * rotation;

#endif
