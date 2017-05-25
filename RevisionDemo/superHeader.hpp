#pragma once

#define WINVER _WIN32_WINNT_WINXP

#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "Ws2_32") 
#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <glm.hpp>
#include <gtx/transform.hpp>
#include <memory>
#include <exception>
#include <cstdio>
#include <string>
#include <stdarg.h>
#include "../sync/sync.h"
#include "../bass.h"

using namespace std;
using namespace glm;

#define sizeofa(array) sizeof array / sizeof array[ 0 ]

#define OFFSET_FIX(i) ((char *) NULL + (i))
extern HSTREAM stream;