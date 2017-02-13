#ifndef DATACONTAINER_H
#define DATACONTAINER_H

#include "Atlas.h"
#include <string>
#include <map>
#include <vector>
#include "../Core/ogl/render.h"

struct DataContainer {
    int x, y, w, h;
    int textureWidth;
    int textureHeight;
    int fileWidth;
    int fileHeight;
    int offX, offY;
    const std::string link;
    ~DataContainer();
    DWORD GetData(int x, int y);
    DataContainer(const char *fileName);
    bool failed;
private:
    DWORD *data;
};

typedef std::map<std::string, int> NameList;

class Agregator
{
private:
    static NameList _fileNames;
    static QVector<DataContainer *> _spritesCollection;
    static Atlas *TryCreateAtlas(unsigned int altasWidth, unsigned int atlasHeight);
    static unsigned int _maxAtlasSize;
public:
    static void SetMaxAtlasSize(unsigned int size);
    static void Reset();
    static void PushFileName(std::string file);
    static void Process(std::string exportFileName);
    static bool TrimAndSave(std::string inputFile, std::string outputFile);
};

#endif//DATACONTAINER_H
