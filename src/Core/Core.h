// Core.h: interface for the Lexems class.

#ifndef MYENGINE_CORE_H
#define MYENGINE_CORE_H

#include "types.h"
#include "FontManager.h"
#include "TextureManager.h"
#include "Object.h"
#include "../2dga_api/Animation.h"


class Core
    : public TextureManager
    , public FontManager
{
public:
    static bool InitWithXml(const char *fileName);
    static void SaveDataToXml(rapidxml::xml_node<> *files);
    static void Unload();
    static void Draw();
    static void Update(float deltaTime);
    static void Release();
    static bool DoLua(const char *code);
    static bool DoScript(const std::string &name);
    static Animation *getMyAnimation(const std::string &animationId);
    static void DrawBar(float x, float y, float width, float height, DWORD color);// Debug only
    static void OnMessage(const std::string &message);
    static char *Resource_MakePath(const char *filename);

    typedef QList<Object *> Objects;
    static std::string storeXML;
    static std::string storeDir;
    static std::string projectXML;
    static std::string atlasName;
    static std::string workDir;
    static std::string pathToGame;

    static std::string programPath;
protected:

    static Objects _objects;
    static QList<std::string> _messages;

};

#endif//MYENGINE_CORE_H
