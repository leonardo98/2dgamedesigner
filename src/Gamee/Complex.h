#ifndef COMPLEX_H
#define COMPLEX_H

#include "../Core/ogl/render.h"
#include "BeautyBase.h"
#include "LevelSet.h"

class ComplexManager {

	typedef std::map<std::string, LevelSet *> ComplexMap;
	static ComplexMap _complexes;

public:

	static void UnloadAll();
	static LevelSet *getComplex(const std::string &complexId);
	static bool isComplex(const std::string &complexId);
	static void deleteComplex(const std::string &complexId);

};

#endif//COMPLEX_H
