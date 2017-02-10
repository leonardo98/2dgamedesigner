#include "Complex.h"
#include "../Core/Core.h"
#include "../Core/Math.h"
#include "Beauty.h"
#include "ColoredPolygon.h"
#include "ClickArea.h"
#include "AnimationArea.h"
#include "SolidGroundLine.h"
#include "StretchTexture.h"
#include "GroundLine.h"
#include "TiledLine.h"
#include "TileEditor.h"
#include "LevelSet.h"

//
//       
//
void ComplexManager::UnloadAll() {
	for (ComplexMap::iterator it = _complexes.begin(); it != _complexes.end(); it++) {
		delete (*it).second;
	}
	_complexes.clear();
}

//
// (   delete  !)
//
LevelSet *ComplexManager::getComplex(const std::string &complexId) {
	ComplexMap::iterator it_find = _complexes.find(complexId);
	if (it_find != _complexes.end()) {
		return (*it_find).second;
	}
	LevelSet *level = new LevelSet();
	if (TileEditor::Instance()->LoadLevel(complexId, *level))
	{
		_complexes[complexId] = level;
		return level;
	}
	delete level;
    LOG("complex " + complexId + " not found.");
	return NULL;
}

bool ComplexManager::isComplex(const std::string &complexId) {
	ComplexMap::iterator it_find = _complexes.find(complexId);
	return (it_find != _complexes.end());
}

void ComplexManager::deleteComplex(const std::string &complexId)
{
	assert(_complexes.find(complexId) != _complexes.end());
	delete _complexes[complexId];
	_complexes.erase(_complexes.find(complexId));
}

ComplexManager::ComplexMap ComplexManager::_complexes;

