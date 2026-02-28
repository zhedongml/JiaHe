// Copyright 2025 MLOptic
#include "MLUtilCommon.h"

#include <io.h>
#include <qstring.h>
#include <qstringlist.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <list>
#include <mutex>

using namespace MLUtils;

MLUtilCommon* MLUtilCommon::m_instance = nullptr;

MLUtilCommon::MLUtilCommon() {}

MLUtilCommon* MLUtilCommon::instance() {
    if (!m_instance) {
        static std::mutex mutex;
        std::unique_lock<std::mutex> locker(mutex);
        if (!m_instance) {
            m_instance = new MLUtilCommon();
        }
    }
    return m_instance;
}

MLUtilCommon::~MLUtilCommon() {}


Size MLUtilCommon::TransStrToSize(std::string str)
{
    std::string result;
    result.resize(str.size());
    transform(str.begin(), str.end(), result.begin(), ::tolower);
    if (result == "large") {
        return Size::Large;
    }
    else if (result == "small") {
        return Size::Small;
    }
    else {
        return Size::Size_UnKnown;
    }
}

Size MLUtilCommon::TransIntToSize(int size)
{
    if (size == 0) {
        return Size::Large;
    }
    else if (size == 1) {
        return Size::Small;
    }
    else {
        return Size::Size_UnKnown;
    }
}

std::string MLUtilCommon::TransSizeToStr(Size size)
{
    if (size == Size::Large) {
        return "Large";
    }
    else if (size == Size::Small) {
        return "Small";
    }
    else {
        return "";
    }
}

EyeType MLUtilCommon::TransStrToEyeType(std::string str)
{
    std::string result;
    result.resize(str.size());
    transform(str.begin(), str.end(), result.begin(), ::tolower);
    if (result == "left") {
        return EyeType::Left;
    }
    else if (result == "right") {
        return EyeType::Right;
    }
    else {
        return EyeType::EyeType_UnKnown;
    }
}

EyeRelief MLUtilCommon::TransStrToEyeRelief(std::string str)
{
    std::string result;
    result.resize(str.size());
    transform(str.begin(), str.end(), result.begin(), ::tolower);
    if (result == "far") {
        return EyeRelief::Far;
    }
    else if (result == "medium") {
        return EyeRelief::Medium;
    }
    else if (result == "near") {
        return EyeRelief::Near;
    }
    else{
        return EyeRelief::EyeRelief_UnKnown;
    }
}

EyeRelief MLUtilCommon::TransIntToEyeRelief(int index)
{
    if (index == 0) {
        return EyeRelief::Far;
    }
    else if (index == 1) {
        return EyeRelief::Medium;
    }
    else if (index == 2) {
        return EyeRelief::Near;
    }
    else {
        return EyeRelief::EyeRelief_UnKnown;
    }
}

std::vector<std::string> MLUtilCommon::split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}
