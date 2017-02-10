#ifndef LEVELSET_H
#define LEVELSET_H

#include "../Core/ogl/render.h"
#include "BeautyBase.h"

struct CurrentBlock {
    QVector<BeautyBase *> beauty;
	BeautyBase *beautyUnderCursor;
};

struct LevelSet {
	std::string name;
	BeautyList beauties;
	void Clear();
    void LoadFromXml(rapidxml::xml_node<> *xe, bool readName = true);
    bool LoadFromFile(const std::string &fileName);
    LevelSet(const LevelSet &l);
	LevelSet() {}
	~LevelSet();
	const LevelSet & operator= (const LevelSet &l);
	void MakeCopyOfBeauties(BeautyList &beautyList);
	void Draw();
	virtual void EncapsulateAllDots(Rect &rect);
	virtual void GetAllLocalDotsRect(Rect &rect);
	bool GeometryCheck(const FPoint &point);
};

BeautyBase *MakeCopy(BeautyBase *);

#endif//LEVELSET_H
