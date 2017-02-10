/*
** 2D Game Animation api 
** by Pakholkov Leonid am98pln@gmail.com
**
** 
** GLTexture2D
**
*/

#ifndef ANIMATION_H
#define ANIMATION_H

#include "../Core/rapidxml/rapidxml.hpp"
#include <string>
#include <map>
#include "../Core/Matrix.h"
#include "MovingPart.h"
#include "../Core/ogl/vertexbuffer.h"
#include "../Core/rect.h"

class Animation
{
public:

    static std::string errMessage;

    Animation(rapidxml::xml_node<> *xe, GLTexture2D * hTexture);

	virtual ~Animation();

	void Draw(float position);

	void PreDraw(float position);

    void DrawMixed(float position, const Animation &anim, float animPosition, float mix);

    //void Attach(const std::string &boneName, const char *fileName);

	// progress += deltaTime / anim->Time();
	// if(progress >= 1.f) progress = 0.f;
	float Time() const;

	bool PixelCheck(const FPoint &pos);

	void EncapsulateAllDots(float p, Rect &rect);

    void SetTexturePath(const std::string &path) { _texturePath = path; }
    const std::string &GetTetxurePath() { return _texturePath; }

private:
    std::string _texturePath;
    void AddBone(MovingPart *bone);
	float _time;
	FPoint _pivotPos;
    List _bones;
    List _renderList;
	Matrix _subPosition;
	
	MatrixStack _matrixsStack;
    friend class MovingPart;
    friend class AnimationState;
};

class AnimationState {
private:
	std::vector<PartPosition> _partPositions;
	VertexBuffer _quad;
    PartPosition _mixed;
public:
	// делаем "слепок" анимации без учета установленой матрицы
	void InitWithAnimation(Animation *anim, float position);
	void InitWithAnimations(Animation *animOne, float positionOne, Animation *animTwo, float positionTwo, float mix);
	void DrawMixed(Animation *anim, float position, float mix);
	// смешиваем с новой анимацией без учета матрицы, но вместо рисовния
	// делаем ее новым слепком
	void MixWith(Animation *anim, float position, float mix);

	void DrawMixed(Animation *one, Animation *two, float oneTime, float twoTime, float mix);
};

class AnimationManager {
	typedef std::map<std::string, Animation *> AnimationMap;
    static QVector<std::string> _files;
	static AnimationMap _animations;
    static QVector<GLTexture2D *> _textures;
public:

	static bool Load(const std::string &fileName);
    static void UnloadAll();
	static Animation *getAnimation(const std::string &animationId);

};

#endif//ANIMATION_H

