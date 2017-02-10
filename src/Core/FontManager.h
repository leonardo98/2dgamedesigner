#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include "types.h"
#include "CoreFont.h"

class FontManager
{
public:
    typedef std::map<std::string, CoreFont*> FontMap;
    static CoreFont *getFont(const std::string &fontFileName);
    static void ReleaseAllFonts();
private:
    static FontMap _fontsMap;
};

#endif // FONTMANAGER_H
