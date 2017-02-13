// Core.cpp: implementation of the Lexems class.

#include "types.h"
#include "Core.h"
#include "ogl/render.h"
#include "Messager.h"
#include "Math.h"
#include <QCoreApplication>
#include "../Gamee/TileEditor.h"
#include "../Gamee/GMessageBox.h"

Core::Objects Core::_objects;
QList<std::string> Core::_messages;
std::string Core::storeXML = "levels.xml";
std::string Core::storeDir = "";
std::string Core::projectXML = "descriptions.xml";
std::string Core::atlasName = "data/atlas";
std::string Core::workDir = "data\\";
std::string Core::pathToGame;
std::string Core::programPath = ":main/";

bool Core::InitWithXml(const char *fileName)
{
    std::ifstream myfile(Core::Resource_MakePath(fileName));
    std::vector<char> buffer((std::istreambuf_iterator<char>(myfile)), std::istreambuf_iterator<char>( ));

    if (buffer.size() == 0)
    {
        LOG(std::string("file not found: ") + fileName);
        return false;
    }

    buffer.push_back('\0');
    rapidxml::xml_document<> doc;
    if (!Parse(doc, buffer, fileName))
    {
        return false;
    }

    rapidxml::xml_node<> *rootCoreInit = doc.first_node();//.RootElement();
    if (!rootCoreInit)
    {
        LOG(std::string("incorrect project file: ") + fileName);
        return false;
    }
    std::string name = rootCoreInit->name();
    rapidxml::xml_node<> *filesCoreInit = rootCoreInit->first_node();
    if (!filesCoreInit)
    {
        LOG(std::string("incorrect project file: ") + fileName);
        return false;
    }
    name = filesCoreInit->name();
    if (filesCoreInit->first_attribute("storeXml") == NULL
        || filesCoreInit->first_attribute("atlasName") == NULL
        || filesCoreInit->first_attribute("workDir") == NULL)
    {
        LOG(std::string("incorrect project file: ") + fileName);
        return false;
    }
    projectXML = fileName;
    storeXML = filesCoreInit->first_attribute("storeXml")->value();
    storeDir = (filesCoreInit->first_attribute("storeDir") ? filesCoreInit->first_attribute("storeDir")->value() : "");
    atlasName = filesCoreInit->first_attribute("atlasName")->value();
    workDir = filesCoreInit->first_attribute("workDir")->value();
    pathToGame = filesCoreInit->first_attribute("pathToGame") == NULL ? "" : filesCoreInit->first_attribute("pathToGame")->value();
    TileEditor::Instance()->LoadLevelSet();
    TileEditor::Instance()->LoadTemplates(doc);
    return true;
}

void Core::SaveDataToXml(rapidxml::xml_node<> *files)
{
    files->append_attribute(files->document()->allocate_attribute("storeXml", files->document()->allocate_string(storeXML.c_str())));
    if (storeDir.size())
    {
        files->append_attribute(files->document()->allocate_attribute("storeDir", files->document()->allocate_string(storeDir.c_str())));
    }
    files->append_attribute(files->document()->allocate_attribute("atlasName", files->document()->allocate_string(atlasName.c_str())));
    files->append_attribute(files->document()->allocate_attribute("workDir", files->document()->allocate_string(workDir.c_str())));
    files->append_attribute(files->document()->allocate_attribute("pathToGame", files->document()->allocate_string(pathToGame.c_str())));
}

Animation *Core::getMyAnimation(const std::string &animationId) {
    return AnimationManager::getAnimation(animationId);
}

void Core::OnMessage(const std::string &message)
{
    _messages.push_back(message);
}

void Core::Draw()
{
    for (Objects::iterator i = _objects.begin(), e = _objects.end(); i != e; i++) {
        (*i)->Draw();
    }
}

void Core::Update(float deltaTime)
{
    Messager::CoreSendMsgs(deltaTime);
    for (Objects::iterator i = _objects.begin(), e = _objects.end(); i != e; i++) {
        (*i)->Update(deltaTime);
    }
}

void Core::Release()
{
    Unload();
    ReleaseAllTextures();
    ReleaseAllFonts();
}

void Core::Unload() {
    for (Objects::iterator i = _objects.begin(), e = _objects.end(); i != e; i++) {
        delete (*i);
        (*i) = NULL;
    }
    _objects.clear();
}

char szTmpFilename[256];
char szAppPath[256];

char *Core::Resource_MakePath(const char *filename)
{
    int i;

    if(!filename)
        strcpy(szTmpFilename, szAppPath);
    else if(filename[0]=='\\' || filename[0]=='/' || filename[1]==':')
        strcpy(szTmpFilename, filename);
    else
    {
        strcpy(szTmpFilename, szAppPath);
        if(filename) strcat(szTmpFilename, filename);
    }

    for(i=0; szTmpFilename[i]; i++) { if(szTmpFilename[i]=='\\') szTmpFilename[i]='/'; }
    return szTmpFilename;
}
