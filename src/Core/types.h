#ifndef MYENGINE_TYPES_H
#define MYENGINE_TYPES_H

#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include <map>
#include "../Core/rapidxml/rapidxml.hpp"

#ifndef WIN32
typedef unsigned int DWORD;
#endif

#define M_PI       3.14159265358979323846
#define M_PI_2     1.57079632679489661923

#include "ogl/fpoint.h"

#include "rapidxml/rapidxml.hpp"
#include <QMessageBox>

extern QWidget *mainWindow;

#define LOG(a) {     QMessageBox::about(mainWindow, mainWindow->tr("Warning : "), mainWindow->tr((std::string("message : ") + a + "\n").c_str())); }
bool Parse(rapidxml::xml_document<> &doc, std::vector<char> &buffer, const char *fileName);

#include "Object.h"

#endif//MYENGINE_TYPES_H
