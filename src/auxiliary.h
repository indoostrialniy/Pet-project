#pragma once
#ifndef AUXILIARY_H
#define AUXILIARY_H

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>
#include <string>

#include <chrono>
using hr_clock = std::chrono::high_resolution_clock;
using hr_time_point = hr_clock::time_point;
using hr_duration = hr_clock::duration;
using milliseconds = std::chrono::milliseconds;

std::string getValue(const std::string& strSource, int num); //возврат слова с порядковым номером num из строки strSource

std::string getValueOrNone(std::string& strSource, int num); //возврат слова с порядковым номером num из строки strSource. используется для загрузки скелета

char* 		StringToChar(std::string StringValue);	// XXX переделать с аргументом по ссылке

int 		countOfWords(std::string& line);

glm::quat 	vec4CastQuat(glm::vec4& vec);

glm::vec4 	blendVectors4(glm::vec4 A, glm::vec4 B, float mix2);

glm::quat 	blendQuat(glm::quat A, glm::quat B, float mix2);

glm::vec4 	CmulCXYZW(glm::vec4 a, glm::vec4 b);    //функция для перемножения комплексных чисел, например - кватернионов //a - xyzw, b- xyzw

glm::vec4 	RotByQuatXYZW(glm::vec4 coordinate, glm::vec4 quat1);    //функция возвращает координаты вектора, измененные после применения кватерниона

glm::vec4 	RotByQuatXYZW_1(glm::vec4 coordinate, glm::vec4 quat1);    //функция возвращает координаты вектора, измененные после применения кватерниона

glm::vec4 	RotByQuatXYZW_2(glm::vec4 u, glm::vec4 l);    //функция возвращает координаты вектора u, повернутые кватернионом l, оба аргумента в формате XYZW (SK_Attachments_6.sm)

std::string getOperationSystem();

void 		runtimePositionChange(glm::vec3& coordinates);


milliseconds since_last_call();

class LogDuration {
public:
    LogDuration(std::string id)
        : id_(std::move(id)) {
    }

    ~LogDuration() {
        const auto end_time = std::chrono::steady_clock::now();
        const auto dur = end_time - start_time_;
        std::cout << id_ << ": ";
        std::cout << "operation time"
                  << ": " << std::chrono::duration_cast<std::chrono::milliseconds>(dur).count()
                  << " ms" << std::endl;
    }

private:
    const std::string id_;
    const std::chrono::steady_clock::time_point start_time_ = std::chrono::steady_clock::now();
};



#endif