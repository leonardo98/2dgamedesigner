#ifndef BEAUTY_BASE_H
#define BEAUTY_BASE_H

#include "../Core/ogl/render.h"
#include "../Core/ogl/sprite.h"
#include "../Core/rect.h"

//
//     -        
//
class BeautyBase 
{
protected:
	//      
	bool _canBeRotated;
	bool _canBeScaled;
	bool _mouseDown;

	//     
	std::string _inGameType;
	std::string _uid;
	FPoint _pos;
	float _angle;
	DWORD _color;
	float _sy, _sx;
	bool _visible;
	FPoint _pointForAnything;

	//     
	FPoint _mousePos;
	Matrix parent;
	bool _debugDraw;
	Sprite *scaleSide;
	Sprite *scale;
	Sprite *square;
	FPoint rotateBtn;
	FPoint scaleBtn[4];
	FPoint sideScaleBtn[4];

private:
	float _scalar;
	bool _scaleX;
	bool _scaleY;
	FPoint _beforeDragScale;
	Matrix rotateMatrix;
	Matrix scaleMatrix;

	FPoint _mouseDownPos;
	enum MoveStates{
		edit_none,
		edit_rotate,
		edit_scale,
		edit_move,
		edit_pointForAnything
	};

	MoveStates _state;
	Sprite *rotate;
	Sprite *center;

	bool _needRegenerateIcon;

	bool CheckScale(const FPoint &mouse);
	bool CheckSideScale(const FPoint &mouse);
    void InitCorners();

public:

	std::string &GetUnicOrLinkedId() { return _uid; }
	virtual ~BeautyBase();
    BeautyBase(const FPoint &pos, float sx, float sy, float angle);
    BeautyBase(rapidxml::xml_node<> *xe);
    BeautyBase(const BeautyBase &b);

	bool &Visible() { return _visible; }
    virtual void Update(float) {}
	virtual void Draw();
	virtual void DebugDraw(bool onlyControl = false);
	void DebugDrawPointForAnything();
	virtual void MouseDown(const FPoint &mouse);
    virtual bool MouseMove(const FPoint &mouse);
	virtual void MouseUp(const FPoint &mouse);
	virtual bool PixelCheck(const FPoint &point) = 0;
	virtual bool ControlsCheck(const FPoint &point);
    virtual bool GeometryCheck(const FPoint &) { return false; }
	virtual float DistanceTo(const FPoint &point) { return (point - _pos).Length(); }
	virtual int Width() = 0;
	virtual int Height() = 0;
	virtual const char *GetIconTexture() = 0;
	virtual bool Command(const std::string &cmd);
    virtual bool Selection(const Rect&, bool = true) { return false; }
	virtual void EncapsulateAllDots(Rect &rect) = 0; 
	virtual void GetAllLocalDotsRect(Rect &rect) = 0; 
	virtual bool NeedRegenerateIcon() { return _needRegenerateIcon; }
	virtual void SetNeedRegenerateIcon(bool v) { _needRegenerateIcon = v; }
	FPoint &PointData() { return _pointForAnything; }
	float &FloatData() { return _scalar; }
	bool Rotated() { return _canBeRotated; }
	bool Scaled() { return _canBeScaled; }

	virtual std::string Type() = 0;
    std::string &UserString() { return _inGameType; }
    virtual const std::string &GetTexturePath();
	virtual void SaveToXml(rapidxml::xml_node<> *xe);

	FPoint GetPos() { return _pos; }
	float GetAngle() { return _angle; }
	FPoint GetScale() { return FPoint(_sx, _sy); }
    void SetPosition(float x, float y);
	void SetAngle(float a);
	void SetScale(float sx, float sy);
	void ShiftTo(float dx, float dy);//   
	
	virtual void SetColor(DWORD color);
	virtual DWORD GetColor() { return _color; }

	virtual float SomeFloatValue() { return 0.f; }
};

typedef QVector<BeautyBase *> BeautyList;

#endif//BEAUTY_BASE_H
