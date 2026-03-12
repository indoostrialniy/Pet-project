#include "Collision.h"

#include "macros.h"
#include <iostream>
#include <nameof.hpp>

#include "global.h"
#include "CollisionShell.h"
#include <glm/gtx/intersect.hpp>


Collision::Collision( std::string eSysname, CollisionDescriptor initCollisionDescr, glm::vec3 ePos, glm::quat eRot, glm::vec3& cPos, glm::quat& cRot, glm::vec3& cDir )	
: entitySysname(eSysname), entityPosition(ePos), entityRotation(eRot), comboPosition(cPos), comboRotation(cRot), comboDirection(cDir)
{
	#ifdef COLLISION_DEBUG
		printMessage("Constructor of collision")
	#endif
		
	collisionDescriptor = initCollisionDescr;							// в момент создания сущности и ее коллизии необходимо сконфигурировать коллизию по описанию
		
	
	if( collisionDescriptor.collisionType == SPHERE_COLLISION && collisionDescriptor.collisionRadius > 0 )
	{
		enableCollision();
	}
	
	if( collisionDescriptor.collisionType == COMPLEX_COLLISION && collisionDescriptor.collisionRadius > 0 )
	{
		collisionDescriptor.complexCollisionShellPtr = CollisionShells.getAsset( collisionDescriptor.complexCollisionFileName );
		
		if( collisionDescriptor.complexCollisionShellPtr->status )
		{
			enableCollision();
		}
		else
		{
			printVarComment( "Error! Cannot configure COMPLEX_COLLISION object! Set NO_COLLISION! Corrupted entity: ", entitySysname )
			DEBUG_INFO
			disableCollision();
			collisionDescriptor.collisionType = NO_COLLISION;
		}
	}
	
	
}



Collision::~Collision()
{
	#ifdef COLLISION_DEBUG
		std::cout << "Destructor of collision [" << NAMEOF_TYPE(decltype(this)) << "] [" <<  entitySysname << "]" << std::endl;
	#endif
}


void 		Collision::enableCollision()
{
	if( collisionDescriptor.collisionType != NO_COLLISION )
	{
		collisionDescriptor.setCollisionValue(true);
	}
	else
	{
		printVarComment("Error! Cannot enable collision for entity with type NO_COLLISION, entity: ", entitySysname )
		DEBUG_INFO
	}
}

void 		Collision::disableCollision()
{
	if( collisionDescriptor.collisionType != NO_COLLISION )
	{
		collisionDescriptor.setCollisionValue(false);
	}
	else
	{
		printVarComment("Error! Cannot disable collision for entity with type NO_COLLISION, entity: ", entitySysname )
		DEBUG_INFO
	}
}

bool 		Collision::getCollision()
{
	// если тип НЕ NO_COLLISION, то смотрим актуальное значение буля, иначе - false!
	if( collisionDescriptor.collisionType != NO_COLLISION )
	{
		return collisionDescriptor.getCollisionValue();
	}
	else
	{
		//~ printVarComment( "Error! Trying to access collision of NO_COLLISION-entity: ", entitySysname )
		//~ DEBUG_INFO
		return false;
	}
	
}

float 		Collision::getCollisionRadius()
{
	if( collisionDescriptor.collisionType != NO_COLLISION )
	{
		return collisionDescriptor.collisionRadius;
	}
	else
	{
		printVarComment( "Error! Trying to access collision radius of NO_COLLISION-entity: ", entitySysname )
		DEBUG_INFO
		return -1.0f;
	}
	
}



glm::vec3 	Collision::getLocalPosition()
{
	return entityPosition;
}

glm::quat 	Collision::getLocalRotation()
{
	return entityRotation;
}

void 		Collision::setLocalPosition( glm::vec3 pos )
{
	entityPosition = pos;
}

void 		Collision::setLocalRotation( glm::quat rot )
{
	entityRotation = rot;
}



glm::vec3 	Collision::getWorldPosition()
{
	return ( (comboPosition) + (comboRotation) * entityPosition );
}

glm::quat 	Collision::getWorldRotation()
{
	return (comboRotation) * entityRotation;
}


void 		Collision::setWorldPosition( glm::vec3 worldPos )						// метод устанавливает такие локальные entityPosition и entityRotation, что, с учетом comboPosition и comboRotation метод getWorldPosition() станет возвращать worldPos
{
	 entityPosition = inverse(comboRotation)*(worldPos - comboPosition);
}					

void 		Collision::setWorldRotation( glm::quat worldRot )						// аналогично для ориентации
{
	entityRotation = inverse(comboRotation)*worldRot;
}





bool 		Collision::bCheckAnotherAABB( std::shared_ptr<Collision>& otherAABB, glm::vec3 origin, glm::vec3 direction, glm::vec3& slideVector, float& slideCoeff )
{
	
	bool resultOfChecking = false;
	
	//~ std::cout << "\n\nCheck if other AABB type [" << otherAABB->collisionDescriptor.collisionType << "] intersects this shell [" << collisionDescriptor.collisionType << "]" << std::endl;
	
	if( this->collisionDescriptor.collisionType == SPHERE_COLLISION && otherAABB->collisionDescriptor.collisionType == SPHERE_COLLISION ) // если обе оболочки - простые сферы
	{
		// если расстояние между сферами меньше суммы их радиусов, то есть пересечение!
		if( glm::length( this->getWorldPosition() - otherAABB->getWorldPosition() ) <= ( this->getCollisionRadius() + otherAABB->getCollisionRadius() ) )
		{
			resultOfChecking = true;
			// XXX зарегистрируем столкновение и вычислим итоговый вектор отражения для обоих
			
			//~ glm::vec3 forward1 = comboDirection;//comboRotation*glm::vec3(1,0,0);
			//~ glm::vec3 forward2 = otherAABB->comboDirection;//comboRotation*glm::vec3(1,0,0);
			
			//~ glm::vec3 result = glm::normalize( forward1+forward2 );
			
			//~ glm::vec3 a = forward1;
			//~ glm::vec3 b = result;
			//~ float angle = glm::acos( glm::dot( a,b ) ) ; // угол в градусах между векторами		//float angle = glm::degrees(glm::acos( glm::dot( a,b ) ) ); // угол в градусах между векторами
			//~ glm::vec3 axis = glm::cross(a,b);
			//~ glm::quat dif = glm::angleAxis( angle, axis );
			//~ //printQuat( dif )
			//~ comboRotation = dif*comboRotation;
			
			//~ a = forward2;
			//~ b = result;
			//~ angle = glm::acos( glm::dot( a,b ) ) ; // угол в градусах между векторами		//float angle = glm::degrees(glm::acos( glm::dot( a,b ) ) ); // угол в градусах между векторами
			//~ axis = glm::cross(a,b);
			//~ dif = glm::angleAxis( angle, axis );
			//~ //printQuat( dif )
			//~ otherAABB->comboRotation = dif*otherAABB->comboRotation;

			
			//m3VectorsForwardLeftUp = mat3_cast( rotation ) * m3DefaultVectorsForwardLeftUp;	// обязательно
			
			//~ printVec3(forward1)
			//~ printVec3(forward2)
			//~ printVec3(result)
			
			//~ slideVector = result;//-slideVector;
			//~ slideCoeff = 1.0f;
		}
		
		//if( resultOfChecking) 	{ printMessage("Both shells are just spheres, check the length radius treshold") }
	}
	
	if( this->collisionDescriptor.collisionType == SPHERE_COLLISION && otherAABB->collisionDescriptor.collisionType == COMPLEX_COLLISION ) // если эта - сфера, а чужая - сложная
	{
		resultOfChecking = checkIntersectionBetweenSimpleSphereAndComplexAABB( *this, *otherAABB, 1, origin, direction, slideVector, slideCoeff );
		
		//~ if( resultOfChecking) { std::cout << "This shell is sphere with radius " << collisionDescriptor.collisionRadius << " and other - complex "  << otherAABB->collisionDescriptor.complexCollisionFileName << "]" << std::endl; }
	}
		
	if( this->collisionDescriptor.collisionType == COMPLEX_COLLISION && otherAABB->collisionDescriptor.collisionType == SPHERE_COLLISION ) // если эта - сложная, а чужая - сфера
	{
		resultOfChecking = checkIntersectionBetweenSimpleSphereAndComplexAABB( *otherAABB, *this, 0, origin, direction, slideVector, slideCoeff ) ;
		
		//~ if(resultOfChecking) { std::cout << "This shell is complex [ " << collisionDescriptor.complexCollisionFileName  << "] and other - sphere with radius " << otherAABB->collisionDescriptor.collisionRadius << std::endl; }
	}
	
	if( this->collisionDescriptor.collisionType == COMPLEX_COLLISION && otherAABB->collisionDescriptor.collisionType == COMPLEX_COLLISION ) // если обе - сложные
	{	
		// XXX ВАЖНО!!! тк речь про оболочку коллизии игрока (как это уточнить??), она не должна вращаться вместе с руками, поэтому каждый раз обнуляем ее вращение
		otherAABB->setWorldRotation( glm::quat(1,0,0,0) );
		
		
		//~ if( level->actors.count("Debugger") != 0 )	// визуализация вершин чужой коробки (игрока)
		//~ {				
			//~ level->actors["Debugger"]->components["Mark_00"]->setWorldPosition( otherAABB->getPointWorldSpacePosition( otherAABB->collisionDescriptor.complexCollisionShellPtr->getVertexCoordinates(0) ) );
			//~ level->actors["Debugger"]->components["Mark_01"]->setWorldPosition( otherAABB->getPointWorldSpacePosition( otherAABB->collisionDescriptor.complexCollisionShellPtr->getVertexCoordinates(1) ) );
			//~ level->actors["Debugger"]->components["Mark_02"]->setWorldPosition( otherAABB->getPointWorldSpacePosition( otherAABB->collisionDescriptor.complexCollisionShellPtr->getVertexCoordinates(2) ) );
			//~ level->actors["Debugger"]->components["Mark_03"]->setWorldPosition( otherAABB->getPointWorldSpacePosition( otherAABB->collisionDescriptor.complexCollisionShellPtr->getVertexCoordinates(3) ) );
			//~ level->actors["Debugger"]->components["Mark_04"]->setWorldPosition( otherAABB->getPointWorldSpacePosition( otherAABB->collisionDescriptor.complexCollisionShellPtr->getVertexCoordinates(4) ) );
			//~ level->actors["Debugger"]->components["Mark_05"]->setWorldPosition( otherAABB->getPointWorldSpacePosition( otherAABB->collisionDescriptor.complexCollisionShellPtr->getVertexCoordinates(5) ) );
			//~ level->actors["Debugger"]->components["Mark_06"]->setWorldPosition( otherAABB->getPointWorldSpacePosition( otherAABB->collisionDescriptor.complexCollisionShellPtr->getVertexCoordinates(6) ) );
			//~ level->actors["Debugger"]->components["Mark_07"]->setWorldPosition( otherAABB->getPointWorldSpacePosition( otherAABB->collisionDescriptor.complexCollisionShellPtr->getVertexCoordinates(7) ) );
			//~ level->actors["Debugger"]->components["Mark_08"]->setWorldPosition( otherAABB->getPointWorldSpacePosition( otherAABB->collisionDescriptor.complexCollisionShellPtr->getVertexCoordinates(8) ) );
		//~ }
		
		
		
		// 1. Возможно, избыточная функция для проверки, находится ли точка V0, представленная в мировых координатах, внутри чужой замкнутой комплексной оболочки
		//~ for(int i=0; i<collisionDescriptor.complexCollisionShellPtr->getVerticesCount(); i++) // переберем все вершины ЭТОЙ сущности лифта
		//~ {	
			//~ glm::vec3 V0 = getPointWorldSpacePosition( collisionDescriptor.complexCollisionShellPtr->getVertexCoordinates(i) ); // берем мировые координаты своей точки
			
			//~ // XXX пункты 2 и 3 духовно аналогичны соответствующим проверкам из checkIntersectionBetweenSimpleSphereAndComplexAABB()
			//~ if( otherAABB->bIsPointInsideCircledVolume(V0) )	
			//~ {							
				//~ // std::cout << "Point [" << i << "] of collision box [" << collisionDescriptor.complexCollisionFileName << "] intersects collision box of [" << otherAABB->collisionDescriptor.complexCollisionFileName << "]!" << std::endl;
				//~ resultOfChecking = true;
				//~ break;
			//~ }
		//~ }
		
		
		
		// 2. проверка пересечения чужой оболочки треугольниками этой оболочки
		// иными словами, можно проверить, не находится ли какая-либо точка ЧУЖОЙ оболочки ЗА каким-либо треугольником ЭТОЙ оболочки
		// при этом возможны 10 ситуаций:
		/*  все точки дают:
			A: 0,1 - no collision
			B: 0,1,2,3 - true collision
			C: 2,3 - no collision
			D: 0 - no collision
			E: 0,2 - true collision
			F: 2 - no collision
			G: 1 - no collision
			H: 1,3 - no collision
			I: 3 - no collision
			
			J: 0,1,2 - collision (когда под углом)
			Итого, коллизию дают лишь ситуации B и E
		*/
		
		if( !resultOfChecking ) 
		{ 
			//~ printMessage("\nBoth shells are complex shapes, no functionality yet") 
			for(int t=0; t<collisionDescriptor.complexCollisionShellPtr->getPolygonsCount(); t++)						// переберем свои треугольники
			{
				//~ printVarComment("\nCheck triangle ", t)
				
				bool zero = false;
				bool one = false;
				bool two = false;
				bool three = false;
				
				
				for(int i=0; i<otherAABB->collisionDescriptor.complexCollisionShellPtr->getVerticesCount(); i++)
				{
					glm::vec3 otherVertex = otherAABB->collisionDescriptor.complexCollisionShellPtr->getVertexCoordinates(i);	// возьмем чужую локальную точку
					otherVertex = otherAABB->getPointWorldSpacePosition(otherVertex);											// переведем в мировое
					otherVertex = getPointLocalSpacePosition(otherVertex);														// переведем в свое локальное
					
					// новая версия, через универсальную функцию
					PointTriangleArgumentsPack args;
						args.point = otherVertex;
						args.triangle_index = t;
					pointToTriangleRelativeCalculator( args );
					
					int check = args.pointRelativePosition;
					
					// XXX старая версия, избавиться
					//int check2 = findPointRelativePosition( otherVertex, t );
					//if(check2 != check)	{std::cout << "Vertex " << i <<  ", new result: " << check << ", old result " << check2 << std::endl; }

					
					switch(check)	// разберем возникшую ситуацию (check): FRONT_INSIDE, FRONT_OUTSIDE, BACK_INSIDE, BACK_OUTSIDE : 0,1,2,3
					{
						case FRONT_INSIDE: 	zero=true; 	break;
						case FRONT_OUTSIDE: one=true; 	break;
						case BACK_INSIDE: 	two=true; 	break;
						case BACK_OUTSIDE: 	three=true; break;
					}				
					
				}
		
				
				// XXX оптимизировать условия, вспомнить 3 курс, МПУ!
				if( (zero && one && two && three) || (zero && !one && two && !three) || (zero && one && two && !three) ) // situation B, E, J
				{
					// std::cout << "\n Vertex [" << i << "] of other`s shell intersects triangle [" << t << "] of this shell" << std::endl;
					
					glm::vec3 Normal 	= collisionDescriptor.complexCollisionShellPtr->getPolygonNormal(t);
					glm::vec3 Dir 		= glm::reflect( direction, Normal );	
					
					//~ printVec3(Normal)
					//~ printVec3(direction)
					//~ printVec3(Dir)				
									
					float k = glm::length( glm::cross(-Normal, direction) ) / glm::length(-Normal);
					k = glm::length(Dir) / glm::length(Normal); // чтобы априори отраженный вектор был меньше падающего
					Dir = comboRotation * (entityRotation * Dir);				// переведем обратно в мировые координаты из локальных координат комбо, в которых они вычислены - т.е. collisionWithAABB
					slideVector = glm::normalize(Dir);			
					slideCoeff = k;
					
					
					//~ printMessage("reflect: ")
					//~ printVec3(slideVector)
				
					resultOfChecking = true;
					break;	//triangles
				}
				
				
				
			}
		
		}
		
		
		// 3. проверка пересечения чужой оболочки гранями этой оболочки
		// нужно перебрать все грани и проверить, пересекают ли они треугольники чужой оболочки
		if( !resultOfChecking ) // если по треугольникам коллизии не зафиксировано, то проверим по граням
		{
			//~ printMessage("\n\n p.3")
			for(int e=0; e<collisionDescriptor.complexCollisionShellPtr->getEdgesCount(); e++)	//e<1; e++)//
			{
				glm::vec3 V0, V1;
				collisionDescriptor.complexCollisionShellPtr->getEdgeVertexes(e, V0, V1);	//получили координаты вершин конкретной грани в ее локальном пространстве
				//~ printVarComment("\nEdge ", e)
				
				// переведем эти координаты в мировую систему
				V0 = getPointWorldSpacePosition( V0 );
				V1 = getPointWorldSpacePosition( V1 );
				
				for(int ot=0; ot<otherAABB->collisionDescriptor.complexCollisionShellPtr->getPolygonsCount(); ot++)//=5)
				{
					//~ printVarComment("Other triangle ", ot)
					
					glm::vec3 trv0, trv1, trv2;							// получили локальные координаты вершин треугольника
					otherAABB->collisionDescriptor.complexCollisionShellPtr->getPolygonVertexes(ot, trv0, trv1, trv2);
					
					glm::vec3 V00 = otherAABB->getPointLocalSpacePosition( V0 );	// переведем все в систему координат чужой оболочки
					glm::vec3 V11 = otherAABB->getPointLocalSpacePosition( V1 );
										
					glm::vec3 directionVec = glm::normalize(V11-V00);
					//~ float dist=1.0f;
						
				
					float edgeHalfLength = glm::length(V11-V00)*0.5f;	// начало луча - в середине грани, значит, надо знать длину половины грани
					
					PointTriangleArgumentsPack args;
						args.point = V00 + (V11-V00)*0.5f;	// середина грани
						args.direction = directionVec;
						args.triangle_index = ot;
										
					otherAABB->pointToTriangleRelativeCalculator( args );
					
					if(args.rayIntersectionLength < edgeHalfLength && args.rayInside)// && args.rayRelativeDirection != 0 )// && args.pointRelativePosition == ) 
					{
						//~ printMessage("COLLISION!")
						resultOfChecking = true;
						break; //other triangles
						break; //edges
						
					}
					
				}
				
			}
		}
	
	
	}
	return resultOfChecking;	
}


// функция определяет коллизию между двумя объектами, одни из которых сфера, а другой - сложная оболочка.
bool 		Collision::checkIntersectionBetweenSimpleSphereAndComplexAABB( Collision& collisionWithSphere, Collision& collisionWithAABB, int leader, glm::vec3 origin, glm::vec3 direction, glm::vec3& slideVector, float& slideCoeff )
{
	if( &collisionWithAABB == this ) // тут все чики-пуки
	{
		//~ printMessage("This is COMPLEX!!!!")
		// ААВВ- своя, сфера - чужая
		
		origin = collisionWithAABB.getPointLocalSpacePosition( collisionWithSphere.getWorldPosition() ); // поэтому центр чужой сферы (в мировых) переводим в локальное для этой комплексной		
		direction = glm::inverse(collisionWithAABB.getWorldRotation() ) * direction;
		
	}
	
	if( &collisionWithSphere == this ) // XXX direction неправильна!
	{
		//~ printMessage("This is SPHERE!!!!")
		// сфера - своя, ААВВ-чужая
		
		origin = collisionWithAABB.getPointLocalSpacePosition( collisionWithSphere.getWorldPosition() ); //  поэтому центр собственной сферы (в мировых) переводим в локальное для этой комплексной
	}
	
	//~ printMessage("Collision origin: ")
	//~ printVec3( origin )
	//~ printVec3( direction )
	
	bool resultOfChecking = false;
	
	// 1. Отобразим все вершины коллизии	
	//~ for(int i=0; i<CollisionShellPtr->vertices_count; i++)	
	//~ {
		//~ std::string name = "Mark_0" + std::to_string(i);
		//~ level->actors["Debugger"]->components[ name ]->setLocalPosition( CollisionShellPtr->vertices[i] );
	//~ }
	
	// 2. Проверим, не находится ли какая-либо вершина комплексной коллизии внутри сферы
	for(int i=0; i<collisionWithAABB.collisionDescriptor.complexCollisionShellPtr->getVerticesCount(); i++)	
	{
		if( glm::length(collisionWithAABB.collisionDescriptor.complexCollisionShellPtr->getVertexCoordinates(i) - origin) < collisionWithSphere.collisionDescriptor.collisionRadius )
		{
			//~ std::cout << "Vertex " << i << " of [" << collisionWithAABB.collisionDescriptor.complexCollisionFileName << "] is inside alien sphere!" << std::endl;			
			return true;	// раз есть точка внутри, то сразу закончим расчет и вернем true
		}
	}
	
	///	printMessage("flag 2")
	
	// 3. Если вершин внутри нет, проверим треугольники
	for(int i=0; i<collisionWithAABB.collisionDescriptor.complexCollisionShellPtr->getPolygonsCount(); i++)
	{
		// сначала определить кратчайшее расстояние до плоскости треугольника и координаты точки пересечения
		// затем по барицентрическим координатам определить, внутри ли треугольника она
		// после - сравнить с радиусом сферы и сделать вывод
		
		//printVarComment("\nTriangle ", i)
		
		float dist;
		//~ glm::vec3 V1 = glm::vec3(0,0,0);
		float direct = glm::dot(direction, collisionWithAABB.collisionDescriptor.complexCollisionShellPtr->getPolygon(i)[1]); // если вектора направления комбо (direction) и нормали треугольника (polygons[i][1]) противонаправлены (т.е.direct < 0), значит к треугольнику приходим извне

		
		// старый вариант
		//int res = getDistanceToTriangle( origin, i, dist, V1, collisionWithAABB.collisionDescriptor.complexCollisionShellPtr); // вернет показатель, с какой стороны находимся от треугольника
		//printMessage("Old parameters")
		//printVariable(res)
		//printVariable(direct)
		//printVariable(dist)//
		
		
		
		PointTriangleArgumentsPack args;
			args.point = origin;
			args.triangle_index = i;
			
		collisionWithAABB.pointToTriangleRelativeCalculator( args );
		
		dist = args.heightProjectionLength;
		
		
		//~ printMessage("\nNew way params")
		//~ printVariable(dist) // == dist	= abs(t_param_Point)
		//~ printVariable(args.heightInside)
		//~ printVariable(args.rayInside)
		
		//~ printVariable(args.t_param_Point)
		
		// t_param_Point < 0
		
		// из точки пересечения высоты до поинта
		//~ float cosinus = cosa(h, collisionWithAABB.collisionDescriptor.complexCollisionShellPtr->getPolygonNormal(i));
		//~ int res2 = 0;
		
		//~ if(args.heightInside)
		//~ {
			//~ res2 = cosinus > 0 ? 1 : 2;	// 1 снаружи, 2 - изнутри
		//~ }
		//~ else
		//~ {
			//~ return 0; // нет пересечения, т.к. точка пересечения ВНЕ треугольника
		//~ }
		
		//~ printVec3(res2)
		
		
		
		if( args.t_param_Point < 0 && args.heightInside == 1 && direct <= 0 && dist < collisionWithSphere.collisionDescriptor.collisionRadius ) // res == 1  // если искомый полигон пересекает сферу оболочки игрока снаружи
		{
			// надо определить вектор вдоль этого треугольника
			// V1 - точка проекции высоты на треугольник
			
			glm::vec3 Normal = collisionWithAABB.collisionDescriptor.complexCollisionShellPtr->getPolygonNormal(i);
			
			glm::vec3 Dir = glm::reflect( direction, Normal );					
			
			float k = glm::length( glm::cross(-Normal, direction) ) / glm::length(-Normal);
			k = glm::length(Dir) / glm::length(Normal); // чтобы априори отраженный вектор был меньше падающего
			
			Dir = collisionWithAABB.comboRotation * (collisionWithAABB.entityRotation * Dir);				// переведем обратно в мировые координаты из локальных координат комбо, в которых они вычислены - т.е. collisionWithAABB
		
			slideVector = glm::normalize(Dir);			
			slideCoeff = k;
			
			///printMessage("flag 2.2")
			
			resultOfChecking = true;
			
			//~ printVec3(Normal)
			//~ printVec3(direction)
			//~ printVec3(H)
			//~ printVec3(Dir)
			//~ printVariable(k)
			//~ printVarComment("Intersect triangle ", i)
			
			break;
		}
					
	}
	
	///printMessage("flag 3")
	
	// 4. Если этот кусок не закомментирован, то можно пройти сквозь треугольник с обратной стороны
	if( !resultOfChecking )
	{
		//return false;
	}
	
	///printMessage("flag 4")
	
	// 5. В зависимости от пункта 4 дополнительно проверим, пересекают ли какие-либо грани сложной оболочки сферу - т.е. сравним расстояние от центра сферы до грани с радиусом сферы
	for(int i=0; i<collisionWithAABB.collisionDescriptor.complexCollisionShellPtr->getEdgesCount(); i++)
	{
		glm::vec3 P0, P1;
		
		collisionWithAABB.collisionDescriptor.complexCollisionShellPtr->getEdgeVertexes(i, P0, P1);	// локальные координаты вершины грани
		
		P0 -= glm::vec3(0.0001f, 0.0002f, 0.0003f);
		P1 += glm::vec3(0.0003f, 0.0002f, 0.0001f);
		
		///printMessage("flag 4.1")

		glm::vec3 Pm = origin + glm::vec3(0.00005f, 0.00005f, 0.00005f); // origin - локальные координаты центра чужой сферы
		
		glm::vec3 V1 = glm::vec3(0,0,0);
		glm::vec3 V2 = glm::vec3(0,0,0);
		
		glm::vec3 N1 = glm::vec3(0,0,1);
		glm::vec3 N2 = glm::vec3(0,0,1);
		
		bool cast = glm::intersectLineSphere( P0, P1, Pm, collisionWithSphere.getCollisionRadius(), V1, N1, V2, N2 ); // это проверка с прямой, а надо с отрезком!
						
		if(cast) // поэтому, если зарегистрировано пересечение 
		{
			glm::vec3 intersect = V1 + (V2-V1)*0.5f; // средняя точка пересечения
							
			if(  abs( glm::length(intersect-P0) + glm::length(intersect-P1)  - glm::length(P1-P0) ) <= 0.01 ) // если точка пересечения сферы линией находятся внутри отрезка P0-P1
			{
				//~ printVarComment("Intersect edge ", i)
				
				//printMessage("flag 4.2")
				
				//~ printVec3(N1)
				//~ printVec3(N2)
				glm::vec3 N = glm::normalize(N1+N2);				
				//~ printMessage("Average N:")
				//~ printVec3(N)
				
				N = glm::normalize(-N+direction);
				//~ printMessage("Corrected N:")
				//~ printVec3(N)
				
				N = collisionWithAABB.comboRotation * (collisionWithAABB.entityRotation * N);				// переведем обратно в мировые координаты из локальных координат комбо, в которых они вычислены - т.е. collisionWithAABB			
				
				if(!resultOfChecking)
				{
					slideVector = glm::normalize(slideVector + N);			
					slideCoeff = 1.0f;
					resultOfChecking = true;
					// break;
				}
				
			}
		}
	}

	///printMessage("flag 5")
	/// конец обсчетов, возвращаем результат
	
	return resultOfChecking;
}




// перевести координату pointOwnedByEntity из локальной системы координат сущности в мировые координаты
glm::vec3	Collision::getPointWorldSpacePosition( glm::vec3 pointOwnedByEntity = glm::vec3(0,0,0) )
{
	return comboPosition + (comboRotation) * ( entityPosition + entityRotation * pointOwnedByEntity );
}

// перевести мировые координаты otherPointInWorldSpace в локальное пространство этой оболочки
glm::vec3	Collision::getPointLocalSpacePosition( glm::vec3 otherPointInWorldSpace )
{
	return glm::inverse( entityRotation) * ( glm::inverse(comboRotation) * (otherPointInWorldSpace - comboPosition) - entityPosition); // XXX WTF рабочая, но непонятно почему
}




// проверка нахождения точки внутри сложного замкнутого объема, характеризующего ЭТУ оболочку
bool 		Collision::bIsPointInsideCircledVolume( glm::vec3& point )
{	
	// нижеприведенный случай подходит для полностью замкнутой фигуры с нормалями наружу, состоящей минимум из 4 треугольников
	if( collisionDescriptor.collisionType == 3 ) 
	{
		// point приходит в мировых координатах, поэтому переведем в локальное пространство
		point = getPointLocalSpacePosition( point );
		
		// ЗАДАЧА: надо бросить луч из point в точку примерно посередине 0-го треугольника и проверить, скомпенсированы ли нормали "нанизанных" на этот луч треугольников
		
		// 1. определим координаты вершин 0-го треугольника
		glm::vec3 P0, P1, P2;
		#define ZERO_TRIANGLE_INDEX 0
		collisionDescriptor.complexCollisionShellPtr->getPolygonVertexes(ZERO_TRIANGLE_INDEX, P0, P1, P2);
		 
		// 2. определим точку посередине 0-го треугольника
		glm::vec3 temp1 = P0+(P1-P0)*0.5f;
		glm::vec3 temp2 = P2+(temp1-P2)*0.5f; // temp2 - это нужные координаты точки примерно (!) в центре плоскости треугольника 0
		
		// 3. теперь надо бросить вектор из point в точку temp2 и определить, какие треугольники нанижутся
		glm::vec3 axis = temp2 - point;	// вектор луча из точки point в 0-й треугольник 
		
		
		int inters = 0;			// счетчик пересечений сверху-вниз
		
		bool inside = false;	// по-умолчанию точка - снаружи объема
			
	
		// переберем все треугольники
		for(int i=0; i<collisionDescriptor.complexCollisionShellPtr->getPolygonsCount(); i++)
		{
			//printVarComment("Volume triangle: ", i)
			
			PointTriangleArgumentsPack args;
				args.point = point;
				args.direction = axis;
				args.triangle_index = i;
				
			pointToTriangleRelativeCalculator( args );
			
			printVariable(args.rayRelativeDirection)
			
			// XXX не перепутал ли, где снаружи, где изнутри?
			if( args.rayRelativeDirection == 1 )	// пересечение треугольника "изнутри"
			{
				// std::cout << "\nRay intersect triangle UP-DOWN " << i << std::endl;
				inters++;
			}
			if( args.rayRelativeDirection == 2 )	// пересечение "снаружи"
			{
				// std::cout << "\nRay intersect triangle DOWN-UP " << i << std::endl; 
				inters--;
			}		
		}
		
		if( inters <  0) // если пересечений "изнутри" было больше (такие случаи декрементируют счетчик inters), то...
		{
			// printMessage("inters < 0. Inside = true")
			inside = true; // делаем вывод, что точка - внутри объема
		}
		
		return inside;
	}
	else
	{
		printMessage("This is not a complex circled volume!")
		DEBUG_INFO
		return false;
	}
}
	



void 		Collision::pointToTriangleRelativeCalculator( PointTriangleArgumentsPack& arguments )
{
	// все вычисления происходят в локальном пространстве

	if(collisionDescriptor.collisionType == COMPLEX_COLLISION)
	{
		// функция определит, как расположена точка point относительно треугольника triangle ( т.е. с какой стороны от него она находится и не выступает ли за геометрические пределы)
	
		float a,b,c,d = 1.0f;
		collisionDescriptor.complexCollisionShellPtr->getPolygonCoefficients(arguments.triangle_index, a,b,c);
		
		glm::vec3 N = collisionDescriptor.complexCollisionShellPtr->getPolygonNormal(arguments.triangle_index);		// нормаль треугольника
		
		///printVec3(N)
		
		arguments.t_param_Point	= -(a*arguments.point.x + b*arguments.point.y + c*arguments.point.z + d) / (a*N.x + b*N.y + c*N.z); 							// коэффициент параметризации плоскости (для нормали!)
		arguments.t_param_Ray 	= -(a*arguments.point.x + b*arguments.point.y + c*arguments.point.z + d) / (a*arguments.direction.x + b*arguments.direction.y + c*arguments.direction.z); 	// коэффициент параметризации плоскости (для вектора луча!)
		
		
		glm::vec3 heightProjectionVector = -arguments.t_param_Point * N;											// вектор в точку проекции
		glm::vec3 rayIntersectionVector  = -arguments.t_param_Ray * arguments.direction;							// вектор в точку пересечения лучом плоскости
		
		arguments.heightProjectionLength = abs(arguments.t_param_Point);											// длина вектора проекции (кратчайшее расстояние до треугольника)
		arguments.rayIntersectionLength  = abs(arguments.t_param_Ray);												// длина до пересечения
		
		arguments.heightProjectionCoordinates 	= arguments.point - heightProjectionVector;							// координаты точки вертикальной проекции origin-а на плоскость треугольника
		arguments.rayIntersectionCoordinates 	= arguments.point - rayIntersectionVector;							// координаты точки пересечения лучом плоскости треугольника


		// через барицентрические координаты определим, находятся ли точки проекции и пересечения внутри треугольника
		glm::vec3 Vec1, Vec2, Vec3 = glm::vec3(0,0,0);
		collisionDescriptor.complexCollisionShellPtr->getPolygonVertexes(arguments.triangle_index, Vec1, Vec2, Vec3);
		float znamenatel =  (Vec2.y-Vec3.y)*(Vec1.x-Vec3.x) + (Vec3.x-Vec2.x)*(Vec1.y-Vec3.y);	// не должен равняться нулю	
		
		float bary1 = ( (Vec2.y-Vec3.y)*(arguments.heightProjectionCoordinates.x-Vec3.x)+(Vec3.x-Vec2.x)*(arguments.heightProjectionCoordinates.y-Vec3.y) )/znamenatel;
		float bary2 = ( (Vec3.y-Vec1.y)*(arguments.heightProjectionCoordinates.x-Vec3.x)+(Vec1.x-Vec3.x)*(arguments.heightProjectionCoordinates.y-Vec3.y) )/znamenatel;
		float bary3 = 1.0f - bary1 - bary2;
		
		arguments.heightInside = bary1>=0 && bary1 <=1 && bary2>=0 && bary2<=1 && bary3>=0 && bary3<=1;
		
		bary1 = ( (Vec2.y-Vec3.y)*(arguments.rayIntersectionCoordinates.x-Vec3.x)+(Vec3.x-Vec2.x)*(arguments.rayIntersectionCoordinates.y-Vec3.y) )/znamenatel;
		bary2 = ( (Vec3.y-Vec1.y)*(arguments.rayIntersectionCoordinates.x-Vec3.x)+(Vec1.x-Vec3.x)*(arguments.rayIntersectionCoordinates.y-Vec3.y) )/znamenatel;
		bary3 = 1.0f - bary1 - bary2;
		
		arguments.rayInside = bary1>=0 && bary1 <=1 && bary2>=0 && bary2<=1 && bary3>=0 && bary3<=1;
		
		
		if (arguments.t_param_Point > 0) // BACK (from inside)
		{
			arguments.pointRelativePosition = arguments.heightInside ? 2 : 3;	// тернарно определим, внутри/вне
		}
		else 					// FRONT (from outside)
		{
			arguments.pointRelativePosition = arguments.heightInside ? 0 : 1;	// тернарно определим, внутри/вне
		}
		
		
		
		if(arguments.t_param_Ray < 0)	// ???
		{
			arguments.rayRelativeDirection = 0;	// нет пересечения, т.к. вектор направлен в другую сторону от треугольника
		}
		else 			// есть пересечение - вектор направлен в сторону треугольника, уточним, с какой стороны он его пронизывает
		{
			if(arguments.rayInside)
			{
				arguments.rayRelativeDirection = cosa(arguments.direction, N) > 0 ? 2 : 1;	// 1 снаружи, 2 - изнутри
			}
			else
			{
				arguments.rayRelativeDirection = 0; // нет пересечения, т.к. точка пересечения ВНЕ треугольника
			}
		}
		
	
	}
	else
	{
		printVarComment("Error at pointToTriangleRelativeCalculator()! No collision shell detected! Corrupted entity: ", entitySysname)
		DEBUG_INFO
	}	
}



// XXX упростить формулу - dot?
float 		Collision::cosa(const glm::vec3& U, const glm::vec3& V) // dot
{
	
	// получение косинуса угла между векторами U и V
	return (U.x*V.x + U.y*V.y + U.z*V.z)    / ( sqrt(U.x*U.x + U.y*U.y + U.z*U.z) * sqrt(V.x*V.x + V.y*V.y + V.z*V.z) );
}







