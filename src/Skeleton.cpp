#include "Skeleton.h"

#include <iostream>
#include <nameof.hpp>
#include "auxiliary.h"

#include "SkeletonAnimation.h"

#include <fstream>  //ifstream
#include <cstring>

#include "macros.h"

Skeleton::Skeleton(std::string skeletonName)
{
	#ifdef SKELETON_DEBUG
		std::cout << "Constructor of [" << NAMEOF_TYPE(decltype(this)) << "] [" <<  skeletonName << "]" << std::endl;
	#endif
	
	status = loadSkeleton(skeletonName);
	resourceName = skeletonName;
}






// пользовательские переменные и методы

Skeleton::~Skeleton()
{
	//~ LogDuration ld("Skeleton constructor");
	#ifdef SKELETON_DEBUG
		std::cout << "Destructor of [" << NAMEOF_TYPE(decltype(this)) << "] [" <<  resourceName << "]" << std::endl;
	#endif
	
	//~ delete[] sklIerarchyMatrices;
	//~ delete[] sklInputSpaceMatrices;
	//~ delete[] sklOutputSpaceMatrices;
	//~ delete[] sklLocalSpaceMatrices;
	//~ delete[] sklTransformMatrices;
	
	boneIndexesMap.clear();
	boneNamesMap.clear();
	
	//~ for (auto anim : sklAnimations) { delete anim.second; } sklAnimations.clear();
	
	unique_transfers.clear();
}

bool Skeleton::sklCheckMatrices()
{
	//~ if(sklLocalSpaceMatrices == nullptr) 	{ printVariable(sklLocalSpaceMatrices) 	return false;}
	//~ if(sklIerarchyMatrices == nullptr)		{ printVariable(sklIerarchyMatrices) 	return false;}
	//~ if(sklInputSpaceMatrices == nullptr)	{ printVariable(sklInputSpaceMatrices) 	return false;}
	//~ if(sklOutputSpaceMatrices == nullptr)	{ printVariable(sklOutputSpaceMatrices)	return false;}
	//~ if(sklTransformMatrices == nullptr)		{ printVariable(sklTransformMatrices) 	return false;}
	
	return true;
}



glm::vec4 Skeleton::rotate_point10(glm::vec4 point, int bone_index, int transformMatrixIndex, std::weak_ptr<glm::mat4[]>& pose)// glm::mat4* pose)
{
	glm::mat4 toLocal = sklTransformMatrices [transformMatrixIndex]; //матрица для пересчета в пространство текущей кости С УЧЕТОМ ПОЛОЖЕНИЯ ГОЛОВЫ В Т-ПОЗЕ!!!
	point = toLocal * point;
	if(bone_index >= 0)
	{
		glm::mat4 QOH = pose.lock()[bone_index];//POSES[bone_index];
		//здесь нужно получить кватернион для кости bone_index
		glm::vec4 rotator = QOH[0]; // rotator считывается из файла анимации как vec4(xyzw)    vec4(0,0,0,1);//
		
		//printVec4(rotator)
		glm::vec4 myoffset = glm::vec4(QOH[1][0],QOH[1][1], QOH[1][2], 0); //координаты смещения кости vec4(0,0,0,0);//
		point = RotByQuatXYZW_2(glm::vec4(point[0], point[1], point[2], 1), rotator); //вместо 1
		point += myoffset;
	}
	return point; 
}

glm::vec4 Skeleton::calculateBranchRotation11(std::string bone_name, glm::vec4 rotated_point, std::weak_ptr<glm::mat4[]>& pose)// glm::mat4* pose)
{
	//на входе функции имеем координаты точки (в мире) и индекс кости, по цепи которой надо пройтись
	//надо определить цепь костей, включая эту

	//~ std::cout << "\n\n\nStart calculating by bone " << bone_index << " point: "; 	printVec4(rotated_point)
	int bone_index = boneIndexesMap[bone_name];
	// первым этапом провернем по самой старшей кости и в качестве матрицы перехода ей укажем ВХОДНУЮ (TRANSFORMINTO)! 
	int currentBoneIndex = bone_index; // циклом for перебираем кости ветви IERARCHY[bone_index]
	int currentBoneTransformSpaceMatrix     = int(sklInputSpaceMatrices[bone_index][0][0]); // получим индекс матрицы для перевода в систему кости
	
	//std::cout << "First turn from world to bone [" << currentBoneIndex << "] with matrix [" << currentBoneTransformSpaceMatrix << "]" << std::endl;
	
	//~ printMessage("Calculate input")
	//~ printVariable(currentBoneIndex)
	//~ printVariable(currentBoneTransformSpaceMatrix)
	
	rotated_point = rotate_point10(rotated_point, currentBoneIndex,  currentBoneTransformSpaceMatrix, pose);
	//~ printMessage("Rotated input:")
	//std::cout << "Point in space of bone [" << bone_index << "] is: ";	printVec4(rotated_point)
	
	for(int i=1; i<16; i++)
	{
		
		int c = i/4;
		int s = i%4;

		int currentBoneIndex = int(sklIerarchyMatrices[bone_index][c][s]); // циклом for перебираем кости ветви IERARCHY[bone_index]
		//~ printVariable(currentBoneIndex)
		//индекс-указатель на матрицу перевода в пространство кости currentBoneIndex
		int currentBoneTransformSpaceMatrix  = int(sklOutputSpaceMatrices[bone_index][c][s]);    //индекс-указатель на матрицу перевода в пространство кости currentBoneIndex
		//~ printVariable(currentBoneTransformSpaceMatrix)		
		// а далее, при переборе всей ветви, проворачиваем уже по родительским костям и в качестве переходных им указываем матрицы ВЫХОДНЫЕ (TRANSFORMNEXT)
		
		
		//~ rotated_point = rotate_point10(rotated_point, currentBoneIndex-1,  currentBoneTransformSpaceMatrix, entity);
		//~ printVec4(rotated_point)
		
		if(currentBoneIndex != 0) //пока есть кости в ветке (пока индексы ненулевые)
		{
			//std::cout << "\n\t Now turn point to space of bone [" << currentBoneIndex-1 << "] with matrix [" << currentBoneTransformSpaceMatrix << "]" << std::endl;
			rotated_point = rotate_point10(rotated_point, currentBoneIndex-1,  currentBoneTransformSpaceMatrix, pose );
			//std::cout << "Point in space of bone [" << currentBoneIndex-1 << "] is: ";	printVec4(rotated_point)
		}
		else //как только дошли до конца
		{
			//std::cout << "\n\t Now turn point to world space with matrix [" << currentBoneTransformSpaceMatrix << "]" << std::endl;
			rotated_point = rotate_point10(rotated_point, currentBoneIndex-1,  currentBoneTransformSpaceMatrix, pose );
			//std::cout << "Point in world space is: ";	printVec4(rotated_point)
			break;
		}
	}

	//~ std::cout << "\n\n\nEnd calculating by bone " << bone_index << std::endl;
	//~ printVec4(rotated_point)

	return rotated_point;
}


glm::vec4 Skeleton::update_normal_quat(glm::vec4 quaternion, int bone_index, int transformMatrixIndex, std::weak_ptr<glm::mat4[]>& pose)//  
{
	//чтобы добавить кватернион поворота к кватерниону нормали, сначала домножить на него ЧИСТУЮ матрицу ЛСК кости
	glm::mat4 matrix_clear = glm::mat4(0.0);
		matrix_clear[0] = sklLocalSpaceMatrices[bone_index][0];
		matrix_clear[0][3] = 0;
		matrix_clear[1] = sklLocalSpaceMatrices[bone_index][1];
		matrix_clear[1][3] = 0;
		matrix_clear[2] = sklLocalSpaceMatrices[bone_index][2];
		matrix_clear[2][3] = 0;
		
		matrix_clear[3][3] = 1.0f;
	
	quaternion = CmulCXYZW( glm::inverse(matrix_clear) * pose.lock()[bone_index][0], quaternion );	//поворачиваем вокруг кости // xyzw

	return quaternion;
}

glm::vec4 Skeleton::calculateBranchRotationNormal(std::string bone_name, glm::vec4 quaternion, std::weak_ptr<glm::mat4[]>& pose)// glm::mat4* pose)
{
	//на вход получаем имя кости, к которой привязываем, кватернион сокета и указатель на сущность, к которой привязываем (из нее берем позу)
	int bone_index = boneIndexesMap[bone_name];	// определим индекс кости

	for(int i=0; i<16; i++)
	{
		// пройдемся по матрице иерархии, хранящей индексы костей ветви, идущей от кости bone_index
		// и по параллельной матрице, хранящей номера матриц переходов в пространства следующих по иерархии костей
		// идти будем ВПЛОТЬ до тех пор, пока не дойдем до нулевого индекса. Индексы костей в иерархии увеличены на 1, поэтому нулевой кости не существует в принципе, это позволяет
		// идентифицировать конец ветки
		int c = i/4;
		int s = i%4;

		int currentBoneIndex = int( sklIerarchyMatrices[bone_index][c][s]); // текущая кость ветви

		int currentBoneTransformSpaceMatrix     = int(sklOutputSpaceMatrices[bone_index][c][s]);    // индекс-указатель на матрицу перевода в пространство кости bone_index

		if(currentBoneIndex != 0) // пока есть кости в ветке (пока индексы ненулевые)
		{
			quaternion = update_normal_quat(quaternion, currentBoneIndex-1,  currentBoneTransformSpaceMatrix, pose );
		}
		else // как только дошли до конца
		{
			break;
		}
	}

	// к этому моменту скопили полный кватернион сокета, преобразованный по скелету 
	//normal = RotByQuatXYZW_2(glm::vec4(normal.xyz, 1), quaternion);    //и повернем нормаль в итоговом направлении

	return quaternion;
}



bool Skeleton::getStatus()
{
	return status;
}


std::string Skeleton::sklGetPath(std::string filename)		
{	
	return getOperationSystem() == "Windows" ? "datas\\animations\\" + filename + ".arm" : "./datas/animations/" + filename + ".arm" ;		
}	//тернарный определитель пути


std::string Skeleton::getValueOrNone(std::string strSource, int num) //возврат слова с порядковым номером num из строки strSource. используется для загрузки скелета
{
	int currWordIndex = 0;
	std::string word;

	for (char c : strSource)
	{
		if (c == ' ')
		{
			currWordIndex++;
			if (currWordIndex > num) { break; }
		}
		else if (currWordIndex == num) { word += c; }
	}
	if (currWordIndex < num) { return "none"; }
	return word;
}


void Skeleton::sklAddNewAnimation(std::string animationName)	
{					
	if (sklAnimations.count( animationName ) == 0)  //если данная анимация не найдена среди существующих у данного скелета
	{
		#ifdef SKELETON_DEBUG
			std::cout << "\tAdding new animation: " << animationName << std::endl;
		#endif

		SkeletonAnimation* newAnim = new SkeletonAnimation(animationName);

		if (newAnim->status == true)
		{
			sklAnimations[animationName] = newAnim;
		}
		else
		{
			std::cout << "\nError by loading animation [" << animationName << "]!\n" << std::endl;
			delete newAnim;
		}
	}
	else
	{
		#ifdef SKELETON_DEBUG
			std::cout << "\nAnimation [" << animationName << "] already exist!\n" << std::endl;
		#endif
	}
}


int Skeleton::getUniqueTransferIndex( std::tuple<int, int> pair )
{
	int i=0;
	for(auto cort : unique_transfers)
	{
		if( cort == pair )
		{
			return i;
		}
		else
		{
			i++;
		}
	}
	return 0;
}


bool Skeleton::loadSkeleton(std::string skeletonName)
{
	/*
	 * 
	 * name: Skeleton::loadSkeleton()
	 * @param Имя файла скелета
	 * @return Успех/неудача
	 * Данная функция читает файл с описанием скелета и должна сформировать массив матриц (70 шт - т.е. для каждой кости) для перехода между пространствами
	 * У каждой кости есть только 1 родитель 
	 */
	 
	std::string skeletonFile = sklGetPath(skeletonName);

	std::ifstream skeletonFileStream(skeletonFile); //инициализируем поток с именем modelFileStream с данными из objModelName

	if (skeletonFileStream.is_open())
	{
		#ifdef SKELETON_REPORT
			std::cout << "\tLoading and configurating skeleton: " << skeletonName << std::endl;
		#endif
		
		std::string StrVar; //строкова¤ переменна¤ дл¤ хранени¤ строки текста из документа

		int m = 0; //вспомогательная переменная-счетчик для считки чистых ЛСК-матриц
		int h = 0; //вспомогательная переменная-счетчик для считки голов костей
		int ie = 0; //вспомогательная переменная-счетчик для считки иерархии костей

		//читаем весь файл и пытаемся распознать параметры
		while (getline(skeletonFileStream, StrVar))
		{
			std::string Parameter = getValue(StrVar, 0);

			if (Parameter == "bones")
			{
				sklBonesCount = stoi(getValue(StrVar, 1));
			
				//~ sklIerarchyMatrices = new glm::mat4[sklBonesCount] {};
				//~ sklInputSpaceMatrices = new glm::mat4[sklBonesCount] {};
				//~ sklOutputSpaceMatrices = new glm::mat4[sklBonesCount] {};
				//~ sklLocalSpaceMatrices = new glm::mat4[sklBonesCount] {};
				//~ sklTransformMatrices = new glm::mat4[sklBonesCount * 2]{};
				
				sklIerarchyMatrices = std::make_unique<glm::mat4[]>( sklBonesCount );
				sklInputSpaceMatrices = std::make_unique<glm::mat4[]>( sklBonesCount );
				sklOutputSpaceMatrices = std::make_unique<glm::mat4[]>( sklBonesCount );
				sklLocalSpaceMatrices = std::make_unique<glm::mat4[]>( sklBonesCount );
				sklTransformMatrices = std::make_unique<glm::mat4[]>( sklBonesCount*2 );
								
				
			}

			if (Parameter == "m")	//загружаем ЛСК-матрицы
			{
				//~ новая версия, ЛСК задается в файле кватернионом и из него просто кастуется матрица
				glm::quat lsk = glm::quat( stof(getValue(StrVar, 5)), stof(getValue(StrVar, 2)), stof(getValue(StrVar, 3)), stof(getValue(StrVar, 4))  );
				sklLocalSpaceMatrices[m] = glm::mat4_cast(lsk);
				
				//~ старая версия, когда ориентация ЛСК задавалась напрямую матрицей
				//~ readFVec3( sklLocalSpaceMatrices[m][0], StrVar, 2 )
				//~ readFVec3( sklLocalSpaceMatrices[m][1], StrVar, 6 )
				//~ readFVec3( sklLocalSpaceMatrices[m][2], StrVar, 10 )
			
				boneIndexesMap[ getValue(StrVar, 1) ] = m;	// занесем в словарь имя кости и соответствующий ей порядковый номер
				
				boneNamesMap[m] = getValue(StrVar, 1);	// занесем в словарь индекс кости и соответствующее ему имя
				
				m++;
			}

			if (Parameter == "h")	//загружаем координаты голов костей в Т-позе
			{
				// добавляем смещение головы к матрице кости
				readFVec3( sklLocalSpaceMatrices[h][3], StrVar, 2 ) // в 3 столбец записываем координаты голов костей
				
				sklLocalSpaceMatrices[h][3][3] = 1;	//в последнюю ячейку последнего столбца пишем 1

				//~ //printMatInfo("LSK with heads ", sklLocalSpaceMatrices[h]);

				//~ //на данном этапе матрица трансформации в пространство данной кости скомпонована, осталось ее инвертировать тут, чтобы не делать этого каждый кадр в шейдере
				sklLocalSpaceMatrices[h] = inverse(sklLocalSpaceMatrices[h]);	//получили матрицу для перевода координат в систему этой кости без дополнительных инверсий реал-тайм 
				
				//~ printMat4( getValue(StrVar, 1), sklLocalSpaceMatrices[h]); // ready
				
				h++;
			}

			if (Parameter == "ie")	//загружаем иерархию костей в ветвях
			{
				// 1 задача данной функции - присвоить уникальный серийный номер каждому переходу между пространствами костей и мира. Их число = 2 * число костей.
				// 2 задача - заполнить матрицы индексов иерархии и переходов
				
				// в sklIerarchyMatrices кладутся индексы костей скелета для каждой ветки иерархии 
				// в sklInputMatrices кладутся индексы на матрицы для перевода В локальную систему координат
				// в sklOutputMatrices кладутся индексы на матрицы для перевода ИЗ локальной системы координат
			
				
					
					//~ std::cout << "w_"<< getValueOrNone(StrVar, 1) << std::endl;
					//~ std::string tr = "w_"+ getValueOrNone(StrVar, 1);
					
					std::tuple <int, int> cortege(255, stoi(getValueOrNone(StrVar, 1)) );
					unique_transfers.insert(cortege);
					
					for(int i = 0; i<16; i++)
					{
						int c = i / 4; 	//частное от деления: 0 0 0 0 1 1 1 1 2 2 2 2 3 3 3 3
						int s = i % 4;	//остаток от деления: 0 1 2 3 0 1 2 3 0 1 2 3 0 1 2 3
						
						if( getValueOrNone(StrVar, i + 1) != "none") 
						{
							float index = stof(getValueOrNone(StrVar, i + 1));
							//std::cout << index << ", ";
							
							sklIerarchyMatrices[ie][c][s] = index + 1; //заносим в матрицу ierarchy индекс вершины, инкрементированный для обеспечения в дальнейшем в шейдере возможности идентифицировать конец ветки
						}
						
						if( getValueOrNone(StrVar, i + 2) != "none" )
						{
							//~ std::string tr = getValueOrNone(StrVar, i + 1) + "_" + getValueOrNone(StrVar, i + 2);
							std::tuple <int, int> cortege(stoi(getValueOrNone(StrVar, i+1)), stoi(getValueOrNone(StrVar, i+2)) );
							unique_transfers.insert(cortege);
							
							//~ std::cout << getValueOrNone(StrVar, i + 1)<<"_"<<getValueOrNone(StrVar, i + 2);
						}
						if(getValueOrNone(StrVar, i + 2) == "none")
						{
							//~ std::string tr = getValueOrNone(StrVar, i + 1) + "_w";
							std::tuple <int, int> cortege(stoi(getValueOrNone(StrVar, i+1)), 255 );
							unique_transfers.insert(cortege);
							
							//~ std::cout << getValueOrNone(StrVar, i + 1) << "_w";
							break;
						}
						//~ std::cout<<std::endl;
					}
					//~ std::cout << std::endl;
					
				ie++;
			}

			if (Parameter == "a")	//загружаем анимации
			{
				sklAddNewAnimation(getValue(StrVar, 1));
			}
		}
		skeletonFileStream.close();
		
		
		
		// после считки пора сконфигурировать все
		
		#ifdef SKELETON_CHECK
		// отображение списка и количества уникальных переходов между костями скелета
			printMessage("List of unique transforms")
			for(auto cort : unique_transfers)
			{	
				int a,b;
				std::tie(a,b) = cort;
				std::cout << a  << "-" << b << ", ";		
			}	std::cout << std::endl;
			printVariable( unique_transfers.size() )
		#endif
		
		
		// теперь надо пройтись по матрицам, снова определить номера переходов В ЭТУ кость и посмотреть, какой индекс будет у этого перехода
		// это надо делать именно сейчас, т.к. в помент парсинга файла список уникальных перхеодов еще не заполнен и не отсортирован
		// 255 - это число-заглушка, обозначающая мировое пространство, ибо максимальное число костей в движке ограничено 70-ю
		for(int b=0; b< sklBonesCount; b++)
		{							
			//~ std::cout << 255 << "-" << sklIerarchyMatrices[b] [0] [0] -1 ;//<< std::endl;
			std::tuple <int, int> cortege( 255, int(sklIerarchyMatrices[b] [0] [0] -1) );
			//~ std::cout << "\tInput index: " << getUniqueTransferIndex(cortege) << std::endl;	//индекс матрицы перехода в это пространство
			sklInputSpaceMatrices[b] [0] [0] = getUniqueTransferIndex(cortege);
			
			for(int i=0; i<16; i++)
			{
				int firstIndex = sklIerarchyMatrices[b] [i/4] [i%4] ;
				int secondIndex = sklIerarchyMatrices[b] [(i+1)/4] [(i+1)%4] ;
				
				int c = (i+1) / 4; 	//частное от деления: 0 0 0 0 1 1 1 1 2 2 2 2 3 3 3 3
				int s = (i+1) % 4;	//остаток от деления: 0 1 2 3 0 1 2 3 0 1 2 3 0 1 2 3
				
				if( secondIndex != 0 )
				{
					//~ std::cout << firstIndex-1 << "-" << secondIndex-1 ;//<< std::endl;
					std::tuple <int, int> cortege( firstIndex-1, secondIndex-1 );
					//~ std::cout << "\tOutput index: " << getUniqueTransferIndex(cortege) << std::endl;
					sklOutputSpaceMatrices[b] [c] [s] = getUniqueTransferIndex(cortege);	//индекс матрицы перехода в следующее пространство
				}
				else
				{
					//~ std::cout << firstIndex-1  << "-" << 255 ;//<< std::endl;
					std::tuple <int, int> cortege( firstIndex-1, 255 );
					//~ std::cout << "\tOutput index: " << getUniqueTransferIndex(cortege) << std::endl;
					sklOutputSpaceMatrices[b] [c] [s] = getUniqueTransferIndex(cortege);	//индекс матрицы перехода в следующее пространство
					
					break;
				}
			}
		}
		
		#ifdef SKELETON_CHECK
			printMessage("List of matrices")
			for(int bone=0; bone< sklBonesCount; bone++)
			{
				printVariable(bone)
				printMat4("ierarchy +1 ", sklIerarchyMatrices[bone] )	// помнить, что в ierarchy индексы увеличены на 1 (для облегчения парсинга в шейдере)
				printMat4("input ", sklInputSpaceMatrices[bone] )
				printMat4("output ", sklOutputSpaceMatrices[bone] )
			}
		#endif
		
		// теперь надо вычислить эти самые матрицы переходов
		//~ printMessage("Configuring matrices")
		int i=0;
		for(auto cort : unique_transfers)
		{	
			int a,b;
			std::tie(a,b) = cort;
			//std::cout << a  << "-" << b << ", ";	
			// 255 - это число-заглушка, обозначающая мировое пространство, ибо максимальное число костей в движке ограничено 70-ю
			if(a==255) // зеленые матрицы SK6.sm - для перехода из мира в систему кости "a"
			{
				sklTransformMatrices[i] = sklLocalSpaceMatrices[b];		}	
			
			if (a != 255 && b == 255)	//фиолетовая матрица SK6.sm - для перехода обратно в мир инвертируем эту же матрицу "a"
			{
				sklTransformMatrices[i] = inverse(sklLocalSpaceMatrices[a]); 		}
			
			if (a != 255 && b != 255)	//бирюзовые матрицы SK6.sm - для перехода от кости "a" в кость "b"
			{
				sklTransformMatrices[i] = sklLocalSpaceMatrices[b] * inverse(sklLocalSpaceMatrices[a]);		}
			
			i++;
		}	//std::cout << std::endl;
		
		
		#ifdef SKELETON_CHECK
			printMessage("\nTransform-matrices")
			for(int i=0; i< sklBonesCount*2; i++)
			{
				printVariable(i)
				printMat4("tr_mat ", sklTransformMatrices[i] )
			}
			
			//~ // проверка заполнения словаря имен и индексов костей
			for(auto b : boneIndexesMap)
			{	std::cout << b.first << ": " << b.second << std::endl;		}
			
			for(auto b : boneNamesMap)
			{	std::cout << b.first << ": " << b.second << std::endl;		}
		#endif
		
		
		
		
		
		

		
	}
	else
	{
		std::cout << "Cannot find file (class Skeleton). Status of [" << skeletonName << "] FALSE!" << std::endl;
		return false;
	}
	
	
	// возможные дополнительные проверки корректности загрузки и формирования скелета
	if( !sklCheckMatrices() ) {return false;}
		
	//~ if(sklAnimations.empty() ) 
	//~ {
		//~ printMessage("\tSkeleton load, but list of attached animations is empty, failure!")
		//~ return false;
	//~ }	
	
	return true;
	
	//~ return false;
}






//~ #include "auxiliary.cpp"
//~ int main()
//~ {
	//~ Skeleton rookie("ExoSoldier_Arm");

	//~ if( rookie.getStatus() )
	//~ {
		//~ printMessage("skeleton load successfull")
	//~ }
	//~ else
	//~ {
		//~ printMessage("Fail to load skeleton")
	//~ }

//~ }
