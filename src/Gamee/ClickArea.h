#ifndef CLICKAREA_H
#define CLICKAREA_H

#include "BeautyBase.h"

//
// 
//   
//

class ClickArea : public BeautyBase
{
private:

	int _width;
	int _height;
	void CalcWidthAndHeight();

	QVector<FPoint> _dots; //   
	QVector<FPoint> _screenDots; 
	bool _mouseDown;
	int _dotUnderCursor;
	int _dotLastEdited;

	int SearchNearest(float x, float y);

	int CreateDot(float x, float y);
	void RemoveDot(int index);

public:

	virtual ~ClickArea();
	ClickArea(rapidxml::xml_node<> *xe);
	ClickArea(const ClickArea &c);

	virtual void Draw();
	virtual void DebugDraw(bool onlyControl);
    virtual void Update(float ) {}
	virtual void MouseDown(const FPoint &mouse);
    virtual bool MouseMove(const FPoint &mouse);
	virtual void MouseUp(const FPoint &mouse);
	virtual bool PixelCheck(const FPoint &point);
	virtual bool Command(const std::string &cmd);
	virtual void EncapsulateAllDots(Rect &rect);
	virtual void GetAllLocalDotsRect(Rect &rect);

	virtual void SaveToXml(rapidxml::xml_node<> *xe);
	virtual std::string Type();

	virtual int Width();
	virtual int Height();

	virtual const char *GetIconTexture();
private:
	std::string _icon;

};


#endif//CLICKAREA_H
