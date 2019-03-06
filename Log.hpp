#pragma once//日志

#include <iostream>
#include <string>

#define NORMAL 0
#define WARNING 1
#define ERROR 2

const char* log_level[] = {"Normal", "Warning", "Error", NULL};

//信息，级别，哪个文件，哪一行
void Log(std::string msg, int level, std::string file, int line)
{
    std::cout << '[' << msg << ']' << "[" << log_level[level] << "]" << " : " << \
        file << " : " << line << std::endl;
}

#define LOG(msg, level) Log(msg, level, __FILE__, __LINE__)
