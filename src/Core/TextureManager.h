// TextureManager.h: interface for the TextureManager class.

#ifndef MYENGINE_TEXTUREMANAGER_H
#define MYENGINE_TEXTUREMANAGER_H

#include "types.h"
#include "ogl/render.h"
#include "ogl/textures.h"

class TextureManager  
{
public:
    struct TextureState
    {
        std::string path;
        std::string group;
        GLTexture2D *texture;
    };
    typedef std::map<std::string, TextureState> TextureMap;
    static GLTexture2D *getTexture(std::string textureId);
    static bool isTexture(std::string textureId);
    static void ReleaseAllTextures();
    static void PackAllTexturesToAtlases(std::string exportFileName);
private:
    static TextureMap _texturesMap;
};

void ReplaceSlash(std::string &textureId);

#endif//MYENGINE_TEXTUREMANAGER_H
