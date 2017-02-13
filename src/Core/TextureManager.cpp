// TextureManager.cpp: implementation of the TextureManager class.

#include "TextureManager.h"
#include "Messager.h"
#include "../Atlas/DataContainer.h"
#include "../Core/Core.h"

void ReplaceSlash(std::string &textureId)
{
    for (std::string::size_type i = 0, n = textureId.size(); i < n; ++i)
    {
        if (textureId[i] == '\\')
        {
            textureId[i] = '/';
        }
    }
}

bool TextureManager::isTexture(std::string textureId)
{
    ReplaceSlash(textureId);
    return (_texturesMap.find(textureId) != _texturesMap.end());
}

GLTexture2D *TextureManager::getTexture(std::string textureId)
{
    ReplaceSlash(textureId);
    if (_texturesMap.find(textureId) == _texturesMap.end()) {
        //     -     ?
        TextureState ts;
        ts.group = "";
        ts.path = textureId.c_str();
        ts.texture = new GLTexture2D(ts.path.c_str());
        _texturesMap[textureId] = ts;
        return ts.texture;
    } else {
        return _texturesMap[textureId].texture;
    }
}

void TextureManager::ReleaseAllTextures() 
{
    for (;_texturesMap.begin() != _texturesMap.end();) {
        if (_texturesMap.begin()->second.texture != NULL) {
            delete _texturesMap.begin()->second.texture;
        }
        _texturesMap.erase(_texturesMap.begin());
    }
}

void TextureManager::PackAllTexturesToAtlases(std::string exportFileName)
{
    Agregator::Reset();
    for (TextureMap::iterator i = _texturesMap.begin(), e = _texturesMap.end(); i != e; ++i) {
        Agregator::PushFileName(i->first);
    }
    Agregator::Process(exportFileName);
}

TextureManager::TextureMap TextureManager::_texturesMap;
