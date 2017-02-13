#include "DataContainer.h"
#include "../Core/ogl/render.h"
#include "../Core/Core.h"
#include "../Core/Math.h"
#include <algorithm>
#include "AtlasWin.h"
#include "../Core/rapidxml/rapidxml_print.hpp"
#include "../Gamee/GMessageBox.h"

DataContainer::~DataContainer() {
    if (data)
        delete [] data;
}

DWORD DataContainer::GetData(int x, int y) {
    assert(y * w + x < w * h);
    return data[y * w + x];
}

DataContainer::DataContainer(const char *fileName)
:link(fileName)
,data(NULL)
,textureWidth(0)
,textureHeight(0)
,failed(false)
{
    GLTexture2D * hTex = new GLTexture2D(Core::Resource_MakePath(fileName));
    if (hTex->failed())
    {
        delete hTex;
        failed = true;
        return;
    }
    textureWidth = hTex->Width();
    textureHeight = hTex->Height();
    if (textureWidth > 1024)
    {
        delete hTex;
        failed = true;
        return;
    }
    DWORD *data = (DWORD *)hTex->CreateBitmapCopy();
    
    int contentX = 0;
    int contentY = 0;
    int contentWidth = fileWidth = hTex->Width();//   -      -         
    int contentHeight = fileHeight = hTex->Height();
    bool transparent = true;
    for (int i = contentX; i < contentX + contentWidth && transparent; ++i) {
        for (int j = contentY; j < contentY + contentHeight && transparent; ++j) {
            transparent &= ( (data[j * textureWidth + i] & 0xFF000000) == 0 );
        }
        if (transparent) {
            ++contentX;
            --contentWidth;
        }
    }
    transparent = true;
    for (int i = contentX + contentWidth - 1; i > contentX && transparent; --i) {
        for (int j = contentY; j < contentY + contentHeight && transparent; ++j) {
            transparent &= ( (data[j * textureWidth + i] & 0xFF000000) == 0 );
        }
        if (transparent) {
            --contentWidth;
        }
    }
    transparent = true;
    for (int j = contentY; j < contentY + contentHeight && transparent; ++j) {
        for (int i = contentX; i < contentX + contentWidth && transparent; ++i) {
            transparent &= ( (data[j * textureWidth + i] & 0xFF000000) == 0 );
        }
        if (transparent) {
            ++contentY;
            --contentHeight;
        }
    }
    transparent = true;
    for (int j = contentY + contentHeight - 1; j > contentY && transparent; --j) {
        for (int i = contentX; i < contentX + contentWidth && transparent; ++i) {
            transparent &= ( (data[j * textureWidth + i] & 0xFF000000) == 0 );
        }
        if (transparent) {
            --contentHeight;
        }
    }
    
    this->data = new DWORD[contentWidth * contentHeight];

    this->offX = contentX;
    this->offY = contentY;
    this->x = -1;
    this->y = -1;
    this->w = contentWidth;
    this->h = contentHeight;
    for (int i = 0; i < contentHeight; ++i) {
        for (int j = 0; j < contentWidth; ++j) {
            DWORD adress = i * contentWidth + j;
            assert((int)adress < contentWidth * contentHeight);
            assert((contentY + i) * textureWidth + contentX + j < textureWidth * textureHeight);
            this->data[adress] = data[(contentY + i) * textureWidth + contentX + j];
        }
    }

    delete [] data;
    delete hTex;
}


NameList Agregator::_fileNames;
QVector<DataContainer *> Agregator::_spritesCollection;


void Agregator::Reset() {
    for (unsigned int i = 0; i < _spritesCollection.size(); ++i) {
        delete _spritesCollection[i];
    }
    _spritesCollection.clear();
    _fileNames.clear();
}

void Agregator::PushFileName(std::string file)
{
    ReplaceSlash(file);
    assert(file.find("\\") == std::string::npos);
    _fileNames[file]  = 1;
}

bool CmpContainer1(DataContainer *one, DataContainer *two) {
    return (one->h > two->h);
}

bool CmpContainer2(DataContainer *one, DataContainer *two) {
    return (one->w > two->w);
}

bool CmpContainer3(DataContainer *one, DataContainer *two) {
    return (one->h * one->w > two->h * two->w);
}

typedef std::map<std::string, std::string> SpritesMap;

Atlas *Agregator::TryCreateAtlas(unsigned int altasWidth, unsigned int atlasHeight)
{
    Atlas *oAtlasWin = new Atlas(altasWidth, atlasHeight);
    for(unsigned int i = 0; i < _spritesCollection.size() && oAtlasWin != NULL; ++i)
    {
        DataContainer &rect = *_spritesCollection[i];
        //    
        Atlas::Rect *area = oAtlasWin->Alloc(rect.w + 1, rect.h + 1);
        if (area) {
            rect.x = area->x;
            rect.y = area->y;
        } else {
            delete oAtlasWin;
            oAtlasWin = NULL;
        }
    }
    return oAtlasWin;
}

void Agregator::Process(std::string exportFileName)
{
    if ((exportFileName.find(".png") + 4) == exportFileName.size())
    {
        exportFileName.erase(exportFileName.find(".png"));
    }
    else if ((exportFileName.find(".aml") + 4) == exportFileName.size())
    {
        exportFileName.erase(exportFileName.find(".aml"));
    }
    unsigned long square = 0;
    for (NameList::iterator i = _fileNames.begin(), e = _fileNames.end(); i != e; ++i) {
        DataContainer *dc = new DataContainer(i->first.c_str());
        if (dc->failed == false && dc->textureWidth < _maxAtlasSize 
            && ((dc->fileWidth | (dc->fileWidth - 1)) != (dc->fileWidth * 2 - 1) 
                || (dc->fileHeight | (dc->fileHeight - 1)) != (dc->fileHeight * 2 - 1)
                )//       
            )
        {
            _spritesCollection.push_back(dc);
            square += (_spritesCollection.back()->h * _spritesCollection.back()->w);
        }
        else
        {
            delete dc;
        }
    }
    
    int rectHeight = (int)sqrtl(square);
    int rectHeight2Pow = 1;
    while (rectHeight2Pow * 2 < rectHeight) {
        rectHeight2Pow *= 2;
    }

    rectHeight = rectHeight2Pow;
    int rectWidth = rectHeight * 2;
    Atlas *oAtlasWin = NULL;
    do {
        for (int k = 1; k <= 3 && oAtlasWin == NULL; ++k)
        {
            if (k == 1)
            {
                std::sort(_spritesCollection.begin(), _spritesCollection.end(), CmpContainer1);
            }
            else if (k == 2)
            {
                std::sort(_spritesCollection.begin(), _spritesCollection.end(), CmpContainer2);
            }
            else if (k == 3)
            {
                std::sort(_spritesCollection.begin(), _spritesCollection.end(), CmpContainer3);
            }
            oAtlasWin = TryCreateAtlas(rectWidth, rectHeight);
            if (oAtlasWin == NULL && rectWidth != rectHeight)
            {
                //      
                oAtlasWin = TryCreateAtlas(rectHeight, rectWidth);
                if (oAtlasWin)
                {
                    int tmp = rectHeight;
                    rectHeight = rectWidth;
                    rectWidth = tmp;
                }
            }
        }
        if (oAtlasWin == NULL)
        {
            if (rectWidth < rectHeight)
            {
                rectWidth *= 2;
            }
            else
            {
                rectHeight *= 2;
            }
        }
    } while (oAtlasWin == NULL);

    if ((rectWidth > _maxAtlasSize || rectHeight > _maxAtlasSize)
            && GMessageBoxYesNoShow(("Too many sprites!\nImpossible pack its into " + Math::IntToStr(_maxAtlasSize) + "x" + Math::IntToStr(_maxAtlasSize) + " Proceed any way?").c_str())
                != QMessageBox::Yes)
    {
        return;
    }

    //
    SpritesMap spritesMap;
    CSurfaceWin atlasMap(rectWidth, rectHeight);
    for(unsigned int i = 0; i < _spritesCollection.size(); ++i)
    {    //
        DataContainer &rect = *_spritesCollection[i];
        char buff[100];
        sprintf(buff, "%i:%i:%i:%i:%i:%i", rect.x, rect.y, rect.w, rect.h, rect.offX, rect.offY);
        spritesMap[rect.link] = buff;

        for (int y = 0; y < rect.h; ++y) {
            for (int x = 0; x < rect.w; ++x) {
                DWORD d = rect.GetData(x, y);
                atlasMap.SetPixel(rect.x + x, rect.y + y, d);
            }
        }

    }

    std::string::size_type f = exportFileName.rfind("xml");
    std::string::size_type s = exportFileName.length();
    if (f == s - 3) {
        exportFileName = exportFileName.substr(0, exportFileName.length() - 4);
    }
    atlasMap.Save((exportFileName + ".png").c_str());

    rapidxml::xml_document<> doc;

    {
        rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
        doc.append_node(decl);
        decl->append_attribute(doc.allocate_attribute("version", "1.0"));
        decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
        decl->append_attribute(doc.allocate_attribute("standalone", "no"));
    }

    rapidxml::xml_node<>* root = doc.allocate_node(rapidxml::node_element, "root");
    doc.append_node(root);

    for (unsigned int i = 0; i < _spritesCollection.size(); ++i)
    {
        rapidxml::xml_node<> *sprite = doc.allocate_node(rapidxml::node_element, "sprite");
        root->append_node(sprite);
        sprite->append_attribute(doc.allocate_attribute("name", _spritesCollection[i]->link.c_str()));
        Math::Write(sprite, "x", _spritesCollection[i]->x);
        Math::Write(sprite, "y", _spritesCollection[i]->y);
        Math::Write(sprite, "w", _spritesCollection[i]->w);
        Math::Write(sprite, "h", _spritesCollection[i]->h);
        Math::Write(sprite, "shiftx", _spritesCollection[i]->offX);
        Math::Write(sprite, "shifty", _spritesCollection[i]->offY);
    }
    // Save to file
    std::ofstream file_stored((exportFileName + ".aml").c_str());
    file_stored << doc;
    file_stored.close();
    doc.clear();

    delete oAtlasWin;

    for (unsigned int i = 0; i < _spritesCollection.size(); ++i) {
        delete _spritesCollection[i];
    }
    _spritesCollection.clear();

    return;
}


bool Agregator::TrimAndSave(std::string inputFile, std::string outputFile)
{
    DataContainer *image = NULL;
    {
        DataContainer *dc = new DataContainer(inputFile.c_str());
        if (dc->failed == false && dc->textureWidth <= 1024 
            && ((dc->fileWidth | (dc->fileWidth - 1)) != (dc->fileWidth * 2 - 1) 
                || (dc->fileHeight | (dc->fileHeight - 1)) != (dc->fileHeight * 2 - 1)
                )//       
            )
        {
            image = dc;
        }
        else
        {
            delete dc;
        }
    }
    if (!image)
    {
        return false;
    }
    
    DataContainer &rect = *image;
    //    
    CSurfaceWin atlasMap(rect.w, rect.h);
    {    //   

        for (int y = 0; y < rect.h; ++y) {
            for (int x = 0; x < rect.w; ++x) {
                DWORD d = rect.GetData(x, y);
                atlasMap.SetPixel(x, y, d);
            } 
        } 

    }

    atlasMap.Save(outputFile.c_str());

    return true;
}

void Agregator::SetMaxAtlasSize(unsigned int size) 
{ 
    assert(size == 512 || size == 1024 || size == 2048); 
    _maxAtlasSize = size; 
}

unsigned int Agregator::_maxAtlasSize = 2048;
