/*
** 2D Game Animation HGE api 
** by Pakholkov Leonid am98pln@gmail.com
**
**
** Класс подвижного спрайта - "сердце" моего формата анимации
** 
** При портировании нужно заменить:
**     HGE * - указатель на рендер-контекст, нужен для отрисовки hgeQuad
**  GLTexture2D * - указатель на используемую текстуру
**   hgeQuad - структура для вывода спрайта на экран,
**             имеет 4 вершины, для которых можно задать 
**             текстурные и экранные координаты
**
*/

#ifndef MOVINGPART_H
#define MOVINGPART_H

#include <string>
#include "../Core/rapidxml/rapidxml.hpp"
#include "../Core/MotionValues.h"
#include "../Core/Matrix.h"
#include "../Core/rect.h"
#include "../Core/ogl/vertexbuffer.h"
#include "../Core/ogl/textures.h"

typedef std::vector<Matrix> MatrixStack;

#define fatof(a) static_cast<float>(atof(a))

class Animation;

class MovingPart;

typedef std::vector<MovingPart *> List;

struct PartPosition {
	float angle;
	float sx, sy;
	float x, y;
};

class MovingPart
{
public:

	~MovingPart();

    MovingPart(Animation *animation, rapidxml::xml_node<> *xe, GLTexture2D * hTexture);

	void PreDraw(float p, MatrixStack & stack);
	void PreDrawInLocalPosition(MatrixStack & stack);

	void PreCalcLocalPosition(float p);
	PartPosition precalculatedLocalPosition; 
	PartPosition localPosition; 

	void Draw();

	bool PixelCheck(const FPoint &pos);

	void EncapsulateAllDots(Rect &rect);

   // void Attach(const char *) {}

private:

	std::string boneName;
	
	FPoint _center;
	MotionValues _x;
	MotionValues _y;
	MotionValues _angle;
	MotionValues _scaleX;
	MotionValues _scaleY;
	MotionValues::Motion _movingType;

	List _bones;
	int _order;
    bool _visible;

	void CreateQuad(GLTexture2D * hTexture, const std::string &texture);
	bool _hasTexture;
	VertexBuffer _quad;
	GLTexture2D *_tex;
	FPoint _origin[4];	
	int _width;
	int _height;

    friend bool CmpBoneOrder(MovingPart *one, MovingPart *two);
    friend class Animation;
    friend class AnimationState;
public:
	inline bool HasTexture() const { return _hasTexture; }
};

bool CmpBoneOrder(MovingPart *one, MovingPart *two);

#endif//MOVINGPART_H
