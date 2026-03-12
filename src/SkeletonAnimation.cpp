#include "SkeletonAnimation.h"

#include <iostream>
#include <nameof.hpp>
#include <fstream>  //ifstream

#include "auxiliary.h"
#include "macros.h"

SkeletonAnimation::SkeletonAnimation() 
{
	
}

SkeletonAnimation::SkeletonAnimation(std::string animationName)
{
	#ifdef constructors
		std::cout << "Constructor of [" << NAMEOF_TYPE(decltype(this)) << "] [" <<  animationName << "]" << std::endl;
	#endif
	resourceName = animationName;
	
	status = loadAnimation(animationName);
}


SkeletonAnimation::~SkeletonAnimation()
{
	#ifdef destructors
		std::cout << "Destructor of [" << NAMEOF_TYPE(decltype(this)) << "] [" <<  resourceName << "]" << std::endl;
	#endif
	
	for(int i=0; i< bonesCount; i++)
	{
		for(auto channel : data[i])
		{
			channel.second.clear();	// очистим ключевые кадры канала
		}
		data[i].clear();			// очистим сами каналы
	}
		
	delete[] data;					// очистим скелет
}


bool SkeletonAnimation::getFullPose(int iSAtime, glm::mat4* pose)
{
	if(CHECK_VALUE(frames) && CHECK_VALUE(iSAtime))
	{
		iSAtime = (iSAtime/ (duration*1000)) * frames;
		
		if(CHECK_VALUE(bonesCount))
		{
			for(int boneIndex=0; boneIndex<bonesCount; boneIndex++)
			{	
				pose[boneIndex] = calculateBonePose(boneIndex, iSAtime);
			}
			
		}
		return iSAtime > frames ? true : false;
	}
	return false;
}


// функция возвращает true, когда отыграна полностью ((iSAtime/ (duration*1000)) * frames > frames)
// принимает момент времени, указатель на массив позы и вектор-маску костей, для которых надо вычислить позу
bool SkeletonAnimation::getPose( int iSAtime, std::weak_ptr<glm::mat4[]> pose,  float& mixCoeff, std::vector<int> mask) //)// glm::mat4* pose,
{
	if(CHECK_VALUE(frames) && CHECK_VALUE(iSAtime))
	{
		iSAtime = (iSAtime/ (duration*1000)) * frames;
		
		//if(pose != nullptr)	// валидность массива позы проверяется в вызывающей функции update() класса AnimationController
		//{		
			if(CHECK_VALUE(bonesCount))
			{
				if( mask.empty() )	// если маска пустая, то выполним расчет для ВСЕХ костей
				{
					for(int boneIndex=0; boneIndex<bonesCount; boneIndex++)
					{	
						pose.lock()[boneIndex] = calculateBonePose(boneIndex, iSAtime);
					}
				}
				else 				// а если в маске указаны конкретные кости, то выполним расчеты ТОЛЬКО ДЛЯ НИХ  и смешаем
				{
					for(auto boneIndex : mask)
					{
						if(boneIndex < bonesCount && boneIndex >= 0)
						{
							//~ printVariable(boneIndex)
							glm::quat defQuat = glm::quat( pose.lock()[boneIndex][0].w, pose.lock()[boneIndex][0].x, pose.lock()[boneIndex][0].y, pose.lock()[boneIndex][0].z); // возьмем текущий кватернион из позы кости
						
							glm::mat4 cpose = calculateBonePose(boneIndex, iSAtime); //вычислим новую QOH-матрицу для кости
							
							glm::quat mixQuat = glm::quat( cpose[0].w, cpose[0].x, cpose[0].y, cpose[0].z ); // определим новый кватернион кости
							
							glm::quat mixed = glm::slerp(defQuat, mixQuat, mixCoeff);	//смешаем текущий и новый кватернионы
							
							cpose[0] = glm::vec4(mixed.x, mixed.y, mixed.z, mixed.w);	//итоговый кватернион в виде вектора
							
							cpose[1] = glm::mix( pose.lock()[boneIndex][1], cpose[1], mixCoeff); //итоговое смещение
							
							pose.lock()[boneIndex] = cpose; //возвращаем итоговые значения в позу
						}
						else
						{
							printMessage("Mask vector bones indexes out of bounds!")
						}
						
						
					}
				}
				

			}
		//}
		else {printMessage("pose massive is nullptr, cannot calculate pose!")}
		
		return iSAtime > frames ? true : false;
	}
	
	return false;
}


glm::mat4 SkeletonAnimation::calculateBonePose(int boneIndex, int keyFrameIndex)
{
	
	glm::mat4 bPose = glm::mat4(0.0f);	//создадим нулевую матрицу - в случае отсутствия каналов или ключевых кадров, в pose пойдет Т-поза
	bPose[0] = glm::vec4(0,0,0,1); // quat xyzw
	
	float lx, ly, lz = 0.0f;
	float qx,qy,qz = 0.0f;
	float qw = 1.0f;
	
	
	//вспомогательное для интерполяции
	int prev = 0;		// индексы соседних ключевых элементов
	int next = 0;
	float p = 0.0f;		// коэффициент интерполяции между соседними ключевыми кадрами
						
	// loc x
	if( data[boneIndex].count("loc_x") != 0 && data[boneIndex]["loc_x"].count(keyFrameIndex) != 0 ) //если для кости boneIndex есть канал X и в нем есть запрашиваемый кадр itime, значит он - ключевой
	{
		lx = data[boneIndex] ["loc_x"] [keyFrameIndex];		}
	else // иначе - интерполируем
	{			
		defineNearestKeyframes(boneIndex, "loc_x", keyFrameIndex, prev, next, p);
		lx = data[boneIndex] ["loc_x"] [prev] + p*( data[boneIndex] ["loc_x"] [next] - data[boneIndex] ["loc_x"] [prev] );
	}
	
	// loc y
	if( data[boneIndex].count("loc_y") != 0 && data[boneIndex]["loc_y"].count(keyFrameIndex) != 0 ) //если для кости boneIndex есть канал Y и в нем есть запрашиваемый кадр itime, значит он - ключевой
	{
		ly = data[boneIndex] ["loc_y"] [keyFrameIndex];		}
	else // иначе - интерполируем
	{
		defineNearestKeyframes(boneIndex, "loc_y", keyFrameIndex, prev, next, p);
		ly = data[boneIndex] ["loc_y"] [prev] + p*( data[boneIndex] ["loc_y"] [next] - data[boneIndex] ["loc_y"] [prev] );
	}
	
	// loc z
	if( data[boneIndex].count("loc_z") != 0 && data[boneIndex]["loc_z"].count(keyFrameIndex) != 0 ) //если для кости boneIndex есть канал Z и в нем есть запрашиваемый кадр itime, значит он - ключевой
	{
		lz = data[boneIndex] ["loc_z"] [keyFrameIndex];		}
	else // иначе - интерполируем
	{
		defineNearestKeyframes(boneIndex, "loc_z", keyFrameIndex, prev, next, p);
		lz = data[boneIndex] ["loc_z"] [prev] + p*( data[boneIndex] ["loc_z"] [next] - data[boneIndex] ["loc_z"] [prev] );
	}
						
	if(	data[boneIndex].count("quat_x") != 0 && data[boneIndex]["quat_x"].count(keyFrameIndex) != 0  &&
		data[boneIndex].count("quat_y") != 0 && data[boneIndex]["quat_y"].count(keyFrameIndex) != 0  &&
		data[boneIndex].count("quat_z") != 0 && data[boneIndex]["quat_z"].count(keyFrameIndex) != 0  &&
		data[boneIndex].count("quat_w") != 0 && data[boneIndex]["quat_w"].count(keyFrameIndex) != 0 ) //если есть все каналы и они ключевые, то
	{
		qx = data[boneIndex] ["quat_x"] [keyFrameIndex];
		qy = data[boneIndex] ["quat_y"] [keyFrameIndex];
		qz = data[boneIndex] ["quat_z"] [keyFrameIndex];
		qw = data[boneIndex] ["quat_w"] [keyFrameIndex];
	}
	else // иначе - интерполируем
	{
		int key = keyFrameIndex;	// заданный кадр
		int prev = 0;	// индексы ключевых элементов
		int next = 0;
		for(auto w : data[boneIndex] ["quat_w"])
		{
			if(w.first < key) {prev = w.first;}
			if(w.first > key) {next = w.first; break;} 
			next = w.first;
		}
		// есть индексы, интерполируем
		float p = (key - prev) / (next - prev + 0.0001f) + 0.0001f;
		if (p > 1) { p = 1; }
		
		glm::quat leftQ = glm::quat(data[boneIndex] ["quat_w"] [prev], data[boneIndex] ["quat_x"] [prev], data[boneIndex] ["quat_y"] [prev], data[boneIndex] ["quat_z"] [prev]);
		glm::quat rightQ = glm::quat(data[boneIndex] ["quat_w"] [next], data[boneIndex] ["quat_x"] [next], data[boneIndex] ["quat_y"] [next], data[boneIndex] ["quat_z"] [next]);
		
		glm::quat mixed = glm::slerp( leftQ, rightQ, p);
		
		qx = mixed.x;
		qy = mixed.y;
		qz = mixed.z;
		qw = mixed.w;
	}

						
	bPose[0] = glm::vec4(qx,qy,qz,qw); // quat xyzw
	bPose[1] = glm::vec4(lx, ly, lz, 0.0f); // offMicroWorld
	
	return bPose;
}


void SkeletonAnimation::defineNearestKeyframes(int& SAboneIndex, std::string SAchannelName, int& SAkey, int& SAprev, int& SAnext, float& SAcoeff)
{
	for(auto w : data[SAboneIndex] [SAchannelName])
	{
		if(w.first < SAkey) {SAprev = w.first;}
		if(w.first > SAkey) {SAnext = w.first; break;} 
		SAnext = w.first;
	}
	// есть индексы, интерполируем
	SAcoeff = (SAkey - SAprev) / (SAnext - SAprev + 0.0001f) + 0.0001f;
	if (SAcoeff > 1) { SAcoeff = 1; }
	//~ printVariable(key)
	//~ printVariable(prev)
	//~ printVariable(next)
}


bool SkeletonAnimation::loadAnimation(std::string animationName)
{
	std::string animationFile = animGetPath(animationName);
	std::ifstream animationFileStream(animationFile); //инициализируем поток с именем modelFileStream с данными из objModelName

	if (animationFileStream.is_open())
	{
		
		std::string StrVar;
		
		int bi = 0; //bone index
		
		//читаем весь файл и пытаемся распознать параметры
		while (getline(animationFileStream, StrVar))
		{
			std::string Parameter = getValue(StrVar, 0);
			
			if(Parameter == "bones")
			{
				bonesCount = stoi(getValue(StrVar, 1));
									
				data = new std::map<std::string, std::map<int, float>> [bonesCount];
			}
			
			if(Parameter == "frames")			{		frames = stoi(getValue(StrVar, 1));		}
			
			if(Parameter == "seconds")			{		duration = stof(getValue(StrVar, 1));	}
			
			if(Parameter == "bone")				{		bi++;									}
			
			if(Parameter == "loc_x" || Parameter == "loc_y" || Parameter == "loc_z" ||
				Parameter == "quat_w" || Parameter == "quat_x" || Parameter == "quat_y" || Parameter == "quat_z")
			{
				std::string channelName = getValue(StrVar, 0);
				//~ printVariable(countOfWords(StrVar))
				
				for(int k=1; k<countOfWords(StrVar)-0; k+=2) // циклом переберем все слова в строке (определив их число функцией countOfWords)
				{
					int key = stoi(getValue(StrVar, k));
					float value = stof(getValue(StrVar, k+1));
					//~ std::cout << " key[" << key << "] value[" << value << "],";
					
					//к конкретной кости	//к каналу
					(data[bi-1]) 				[channelName] [key] = value;
					
				}	//std::cout << std::endl;
			}
			
		}
		
		
		#ifdef SKELETON_ANIMATION_REPORT
			std::cout << "Load new animation: " << animationName << std::endl;
			printVarComment("Bones: ", bonesCount )
			printVarComment("Frames: ", frames  )
			printVarComment("seconds: ", duration  )
		#endif
		
		
		// проверки ключевых кадров
		#ifdef SKELETON_ANIMATION_CHECK
			printMessage("\n\nCheck anim data")
			for(int b = 0; b<bonesCount; b++)
			{
				std::cout << "Bone index: " << b << std::endl;
				for(auto channel : data[b])	
				{
					std::cout << "Channel: " << channel.first << "\n";
					for(auto keyframe : channel.second)//data[b][channel.first] ) //cha)
					{
						std::cout << "K:" << keyframe.first << " v: " << keyframe.second << ", ";
					} 	std::cout << std::endl;
				}	
				std::cout << std::endl;
			}
		#endif

		return true;
	}
	
	else { std::cout << "Cannot find file: " << animationName << "(class SkeletonAnimation). Break!" << std::endl; return false;}


}

std::string SkeletonAnimation::animGetPath(std::string& filename)
{
	return getOperationSystem() == "Windows" ? "datas\\animations\\" + filename + ".anim" : "./datas/animations/" + filename + ".anim";
}



int SkeletonAnimation::getDuration()
{
	return (int)duration*1000;
}
