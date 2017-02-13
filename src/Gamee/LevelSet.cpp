#include "LevelSet.h"
#include "../Core/Math.h"
#include "../Core/Core.h"
#include "Beauty.h"
#include "BeautyText.h"
#include "ColoredPolygon.h"
#include "ClickArea.h"
#include "AnimationArea.h"
#include "SolidGroundLine.h"
#include "StretchTexture.h"
#include "GroundLine.h"
#include "LinkToComplex.h"
#include "TiledLine.h"

void LevelSet::Clear() {
    for (unsigned int i = 0; i < beauties.size(); ++i) {
        delete beauties[i];
    }
    beauties.clear();
}

bool LevelSet::LoadFromFile(const std::string &fileName)
{
    std::vector<char> bufferForDoc;
    rapidxml::xml_document<> doc;

    std::ifstream myfile(Core::Resource_MakePath(fileName.c_str()));
    bufferForDoc.clear();
    bufferForDoc = std::vector<char>(std::istreambuf_iterator<char>(myfile), std::istreambuf_iterator<char>());
    if (bufferForDoc.size() == 0)
    {
        //LOG("Error! Level file not found! " + Core::Resource_MakePath(fileName.c_str()));
        return false;
    }

    bufferForDoc.push_back('\0');
    if (!Parse(doc, bufferForDoc, fileName.c_str()))
    {
        return false;
    }

    LoadFromXml(doc.first_node(), false);
    name = fileName.substr(fileName.rfind("/"));
    return true;
}

void LevelSet::LoadFromXml(rapidxml::xml_node<> *xe, bool readName) {
    Clear();
    assert(xe != NULL);
    if (readName)
    {
        name = xe->first_attribute("id")->value();
    }
    // level loading
    rapidxml::xml_node<> *beautyList = xe->first_node("Beauties");
    if (beautyList) {
        rapidxml::xml_node<> *elem = beautyList->first_node();
        std::string typeName;
        while (elem != NULL) {
            BeautyBase *beauty = NULL;
            typeName = elem->name();
            if (typeName == "ColoredPolygon") {
                beauty = new ColoredPolygon(elem);
            } else if (typeName == "ClickArea") {
                beauty = new ClickArea(elem);
            } else if (typeName == "Beauty") {
                beauty = new Beauty(elem);
            } else if (typeName == "BeautyText") {
                beauty = new BeautyText(elem);
            } else if (typeName == "Animation") {
                AnimationArea *a;
                beauty = a = new AnimationArea(elem);
                if (a->GetAnimation() == NULL)
                {
                    LOG("Can't load animation " + a->Name());
                    delete a;
                    beauty = NULL;
                }
            } else if (typeName == "LinkToComplex") {
                LinkToComplex * c;
                beauty = c = new LinkToComplex(elem);
                if (c->GetComplex() == NULL)
                {
                    delete c;
                    beauty = NULL;
                }
            } else if (typeName == "GroundSpline") {
                beauty = new SolidGroundLine(elem);
            } else if (typeName == "GroundLine") {
                beauty = new GroundLine(elem);
            } else if (typeName == "SolidGroundLine") {
                beauty = new SolidGroundLine(elem);
            } else if (typeName == "StretchTexture") {
                beauty = new StretchTexture(elem);
            } else if (typeName == "TiledLine") {
                beauty = new TiledLine(elem);
            } else {
                assert(false);
            }
            if (beauty)
            {
                beauties.push_back(beauty);
            }
            elem = elem->next_sibling();
        }
    }
}

LevelSet::LevelSet(const LevelSet &l) {
    for (unsigned int i = 0; i < l.beauties.size(); ++i) {
        beauties.push_back(MakeCopy(l.beauties[i]));
    }
}

LevelSet::~LevelSet() {
    Clear();
}

const LevelSet &LevelSet::operator=(const LevelSet &l) {
    Clear();
    for (unsigned int i = 0; i < l.beauties.size(); ++i) {
        beauties.push_back(MakeCopy(l.beauties[i]));
    }
    return *this;
}

void LevelSet::MakeCopyOfBeauties(BeautyList &beautyList)
{
    for (unsigned int i = 0; i < beauties.size(); ++i) {
        BeautyBase *beauty = MakeCopy(beauties[i]);
        beautyList.push_back(beauty);
    }
}

void LevelSet::Draw() {
    for (unsigned int i = 0; i < beauties.size(); ++i) {
        beauties[i]->Draw();
    }
}

bool LevelSet::GeometryCheck(const FPoint &point)
{
    for (unsigned int i = 0; i < beauties.size(); ++i) {
        if (beauties[i]->GeometryCheck(point))
        {
            return true;
        }
    }
    return false;
}

void LevelSet::EncapsulateAllDots(Rect &rect) {
    for (unsigned int i = 0; i < beauties.size(); ++i) 
    {
        //    -      
        //       , 
        // ..       
        if (beauties[i]->Type() != "Animation") 
        {
            beauties[i]->EncapsulateAllDots(rect);
        }
    }
}

void LevelSet::GetAllLocalDotsRect(Rect &rect) {
    for (unsigned int i = 0; i < beauties.size(); ++i) 
    {
        //    -      
        //       , 
        // ..       
        if (beauties[i]->Type() != "Animation") 
        {
            beauties[i]->GetAllLocalDotsRect(rect);
        }
    }
}

BeautyBase *MakeCopy(BeautyBase *origin)
{
    BeautyBase *b;
    if (origin->Type() == "Beauty") {
        b = new Beauty(*(Beauty *)origin);
    } else if (origin->Type() == "BeautyText") {
        b = new BeautyText(*(BeautyText *)origin);
    } else if (origin->Type() == "ColoredPolygon") {
        b = new ColoredPolygon(*(ColoredPolygon *)origin);
    } else if (origin->Type() == "ClickArea") {
        b = new ClickArea(*(ClickArea *)origin);
    } else if (origin->Type() == "GroundLine") {
        b = new GroundLine(*(GroundLine *)origin);
    } else if (origin->Type() == "Animation") {
        b = new AnimationArea(*(AnimationArea *)origin);
    } else if (origin->Type() == "LinkToComplex") {
        b = new LinkToComplex(*(LinkToComplex *)origin);
    } else if (origin->Type() == "SolidGroundLine") {
        b = new SolidGroundLine(*(SolidGroundLine *)origin);
    } else if (origin->Type() == "StretchTexture") {
        b = new StretchTexture(*(StretchTexture *)origin);
    } else if (origin->Type() == "TiledLine") {
        b = new TiledLine(*(TiledLine *)origin);
    } else {
        assert(false);
    }
    return b;
}
