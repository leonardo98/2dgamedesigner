#include "FontManager.h"
#include "Core.h"

CoreFont *FontManager::getFont(const std::string &fontFileName)
{
    if (_fontsMap.find(fontFileName) == _fontsMap.end()) {
        CoreFont *font = new CoreFont(fontFileName.c_str());
        _fontsMap[fontFileName] = font;
        return font;
    } else {
        return _fontsMap[fontFileName];
    }
}

void FontManager::ReleaseAllFonts()
{
    for (;_fontsMap.begin() != _fontsMap.end();) {
        if (_fontsMap.begin()->second != NULL) {
            delete _fontsMap.begin()->second;
        }
        _fontsMap.erase(_fontsMap.begin());
    }
}

FontManager::FontMap FontManager::_fontsMap;
