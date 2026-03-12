#include "Material.h"

#include <iostream>
#include <nameof.hpp>

#include <fstream>  //ifstream

#ifndef STB_IMAGE_IMPLEMENTATION
	#define STB_IMAGE_IMPLEMENTATION
	#include <stb_image.h>
#endif

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
	#define STB_IMAGE_WRITE_IMPLEMENTATION
	#include <stb_write.h>
#endif

#include "auxiliary.h"


Material::Material()
{

};


Material::Material(std::string materialName)
{
	//~ LogDuration ld("Material constructor");
	
	
	status = LoadMaterial(materialName);
	resourceName = materialName;
	
	#ifdef MATERIAL_DEBUG
		std::cout << "Constructor of [" << NAMEOF_TYPE(decltype(this)) << "] [" <<  materialName << "], status: " << status << std::endl;
	#endif
}



Material::~Material()
{
	#ifdef MATERIAL_DEBUG
		std::cout << "Destructor of [" << NAMEOF_TYPE(decltype(this)) << "] [" <<  resourceName << "]" << std::endl;
	#endif
	
	glDeleteTextures(1, &texture2DArray);
	
	textureSubMapsNames.clear();
}


GLuint& Material::getTexture2DArray() 
{	
	return texture2DArray; 
}	//возможно здесь надо возвращать ссылочную переменную


glm::mat4 Material::getMatInfo()	
{ 
	return matInfo; 
}



bool  Material::LoadMaterial(std::string& name)
{
	//при создании класса в конструктор приходит только строка с названием файла материала, например, "datas\\materials\\BRICK.ltx"
	//загружаться и настраиваться материал будет тут в классе
	//если пришли сюда, значит, дубликатов нет, значит надо прочитать файл и загрузить все текстуры и настроить матрицу конфигурации

	//cout << "\nLoading and configurating material: " << Name << endl;
	//Name = getPathCorrectedBySystem(Name);
	std::string materialFile = getMaterialPath(name);

	std::ifstream materialStream(materialFile);
	std::string StrVar;

	bool sizesEqual = true; //вспомогательная переменная для проверки равенства размеров текстур в карте

	int material_index = 1; //начинаем с нуля, чтобы в шейдере идентифицировать отсутствие карт
	if (materialStream.is_open())
	{
		while (getline(materialStream, StrVar))
		{
			if (getValue(StrVar, 0) == "BaseColor")
			{
				//cout << "catch color" << endl;
				textureSubMapsNames.push_back(getValue(StrVar, 1));
				while (getValue(StrVar, 0) != "END")
				{
					if (getValue(StrVar, 0) == "scale") { matInfo COLOR_SCALE = stof(getValue(StrVar, 1)); }
					if (getValue(StrVar, 0) == "size")
					{
						//textureSize.x = stoi(getValue(&StrVar, 1)); textureSize.y = stoi(getValue(&StrVar, 2)); 
						if (textureSize.x == 128 && textureSize.y == 128)	//если пока не обновили минимальный размер карты
						{
							textureSize.x = stoi(getValue(StrVar, 1)); textureSize.y = stoi(getValue(StrVar, 2));	//обновляем минимальный размер карты
						}
						else //если минимальный размер карты уже обновлен ( больше чемм 128х128)
						{
							if (stoi(getValue(StrVar, 1)) != textureSize.x || stoi(getValue(StrVar, 2)) != textureSize.y) { sizesEqual = false; }
						}
					} //размер карт ориентирован на размер карты цвета (диффуз)
					getline(materialStream, StrVar);
				}
				matInfo COLOR_ADDRES = material_index;
				material_index++;
			}

			if (getValue(StrVar, 0) == "Normal")
			{
				//cout << "catch normal" << endl;
				textureSubMapsNames.push_back(getValue(StrVar, 1));
				while (getValue(StrVar, 0) != "END")
				{
					if (getValue(StrVar, 0) == "scale") { matInfo NORMAL_SCALE = stof(getValue(StrVar, 1)); }
					if (getValue(StrVar, 0) == "size")
					{
						//if (stoi(getValue(&StrVar, 1)) != textureSize.x || stoi(getValue(&StrVar, 2)) != textureSize.y) { sizesEqual = false; } 
						if (textureSize.x == 128 && textureSize.y == 128)	//если пока не обновили минимальный размер карты
						{
							textureSize.x = stoi(getValue(StrVar, 1)); textureSize.y = stoi(getValue(StrVar, 2));	//обновляем минимальный размер карты
						}
						else //если минимальный размер карты уже обновлен ( больше чемм 128х128)
						{
							if (stoi(getValue(StrVar, 1)) != textureSize.x || stoi(getValue(StrVar, 2)) != textureSize.y) { sizesEqual = false; }
						}
					}
					getline(materialStream, StrVar);
				}
				matInfo NORMAL_ADDRES = material_index;
				material_index++;
			}

			if (getValue(StrVar, 0) == "Lightmap")
			{
				//cout << "catch lightmap" << endl;
				textureSubMapsNames.push_back(getValue(StrVar, 1));
				while (getValue(StrVar, 0) != "END")
				{
					if (getValue(StrVar, 0) == "scale") { matInfo LIGHTMAP_SCALE = stof(getValue(StrVar, 1)); }
					if (getValue(StrVar, 0) == "size")
					{
						if (textureSize.x == 128 && textureSize.y == 128)	//если пока не обновили минимальный размер карты
						{
							textureSize.x = stoi(getValue(StrVar, 1)); textureSize.y = stoi(getValue(StrVar, 2));	//обновляем минимальный размер карты
						}
						else //если минимальный размер карты уже обновлен ( больше чемм 128х128)
						{
							if (stoi(getValue(StrVar, 1)) != textureSize.x || stoi(getValue(StrVar, 2)) != textureSize.y) { sizesEqual = false; }
						}
					}
					getline(materialStream, StrVar);
				}
				matInfo LIGHTMAP_ADDRES = material_index;
				material_index++;
			}

		}
		materialStream.close(); //как только спарсили весь документ, закрываем поток
		if (sizesEqual) { return createTextureArray(); } //если размеры всех карт одинаковы, то пытаемся сделать массив //true or false	//create textureArray for this material
		else { std::cout << "\t ERROR: Sizes of maps in material [" << name << "] are not equal!" << std::endl; return false; } //если карты не соразмерны, то false
	}
	else
	{
		std::cout << "Cannot open material source file: " << name << ", LoadMaterial (class MATERIAL)" << std::endl;
		return false;
	}

}

bool  Material::createTextureArray()
{
	//cout << "\tCreating and configurating texture array" << endl;
	texture2DArrayMapsCount = textureSubMapsNames.size();

	glGenTextures(1, &texture2DArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture2DArray);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, textureSize.x, textureSize.y, texture2DArrayMapsCount, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); //зарезервировали память

	stbi_set_flip_vertically_on_load(true);

	int currentlayer = 0;
	for (auto tex : textureSubMapsNames)
	{
		//cout << "\t " << tex << endl;
		//tex = getPathCorrectedBySystem(tex);
		std::string texture = getTexturePath(tex);

		unsigned char* data = stbi_load(texture.c_str(), &textureSize.x, &textureSize.y, &textureColorChannels, STBI_rgb_alpha);

		if (data == nullptr) { std::cout << "\t Error: Cannot open image file [" << texture << "]" << std::endl; delete[] data; return false; }

		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, currentlayer, textureSize.x, textureSize.y, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		delete[] data;

		currentlayer++;
	}

	return true;

}

std::string  Material::getMaterialPath(std::string& filename)	
{	
	return getOperationSystem() == "Windows" ? "datas\\materials\\" + filename + ".ltx" : "./datas/materials/" + filename + ".ltx" ;		
}	//тернарный определитель пути


std::string  Material::getTexturePath(std::string& filename)	
{	
	return getOperationSystem() == "Windows" ? "datas\\textures\\" + filename + "" : "./datas/textures/" + filename + "" ;		
}	//тернарный определитель пути




//~ #include "auxiliary.cpp"
//~ int main()
//~ {
	//~ return 0;
//~ }


