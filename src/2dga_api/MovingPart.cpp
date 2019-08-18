#include "MovingPart.h"
#include "Animation.h"
#include "Core/Math.h"
#include "Core/ogl/render.h"
#include "Core/rapidxml/rapidxml.hpp"


//
//     XML 
//    -    float,
//   -  defaultValue
//
float Read(rapidxml::xml_node<> *xe, const char *name, const float defaultValue) {
    rapidxml::xml_attribute<> *tmp = xe->first_attribute(name);
    return (tmp ? fatof(tmp->value()) : defaultValue);
}

// 
//  hgeQuad
//      ,
//         "texture"
//
//
void MovingPart::CreateQuad(GLTexture2D * hTexture, const std::string &texture) {

    //   -      
    float width = (float)(_width = hTexture->Width());
    float height = (float)(_height = hTexture->Height());
    //  

    std::string::size_type start = 0;
    std::string::size_type end = texture.find(':', start);
    float x = fatof(texture.substr(start, end - start).c_str());
    start = end + 1;
    end = texture.find(':', start);
    float y = fatof(texture.substr(start, end - start).c_str());
    start = end + 1;
    end = texture.find(':', start);
    float w = fatof(texture.substr(start, end - start).c_str());
    start = end + 1;
    end = texture.find(':', start);
    float h = fatof(texture.substr(start, end - start).c_str());
    start = end + 1;
    end = texture.find(':', start);
    float offX = fatof(texture.substr(start, end - start).c_str());
    start = end + 1;
    float offY = fatof(texture.substr(start).c_str());

    //   -      
    _tex = hTexture;

    _quad.Resize(4, 6);
    _quad.VertUV(0).x = x / width; _quad.VertUV(0).y = y / height;
    _quad.VertUV(1).x = (x + w) / width; _quad.VertUV(1).y = y / height;
    _quad.VertUV(2).x = (x + w) / width; _quad.VertUV(2).y = (y + h) / height;
    _quad.VertUV(3).x = x / width; _quad.VertUV(3).y = (y + h) / height;
    //  

    x = offX;
    y = offY;
    _origin[0].x = x;     _origin[0].y = y;
    _origin[1].x = x + w;     _origin[1].y = y;
    _origin[2].x = x + w; _origin[2].y = y + h;
    _origin[3].x = x; _origin[3].y = y + h;

}


//
//  -   
//
MovingPart::~MovingPart() {
    for (unsigned int i = 0; i < _bones.size(); ++i) 
        delete _bones[i];
}


//
//  -     XML
//  ,    
//
MovingPart::MovingPart(Animation *animation, rapidxml::xml_node<> *xe, GLTexture2D * hTexture)
{
    animation->AddBone(this);

    boneName =  xe->first_attribute("name")->value();

    rapidxml::xml_attribute<> *tmp = xe->first_attribute("moving_type");
    if (tmp == NULL || strcmp(tmp->value(), "spline") == 0) {
        _movingType = MotionValues::m_spline;
    } else if (strcmp(tmp->value(), "line") == 0) {
        _movingType = MotionValues::m_line;
    } else {
        _movingType = MotionValues::m_discontinuous;
    }
    _order = atoi(xe->first_attribute("order")->value());
    _center.x = Read(xe, "centerX", 0.f);
    _center.y = Read(xe, "centerY", 0.f);
    tmp = xe->first_attribute("texture");
    if ((_hasTexture = (tmp != NULL))) {
        CreateQuad(hTexture, tmp->value());
    }

    //
    //      -    
    //          
    //  
    //
    rapidxml::xml_node<> *pos = xe->first_node("pos");
    if (pos == NULL) {
        _x.AddKey(0.f, 0.f);
        _y.AddKey(0.f, 0.f);
        _scaleX.AddKey(0.f, 1.f);
        _scaleY.AddKey(0.f, 1.f);
        _angle.AddKey(0.f, 0.f);
    }
    while (pos) {
        float time = Read(pos, "time", 0.f);
        _x.AddKey(time, Read(pos, "x", 0.f));
        _y.AddKey(time, Read(pos, "y", 0.f));    
        _scaleX.AddKey(time, Read(pos, "scaleX", 1.f));
        _scaleY.AddKey(time, Read(pos, "scaleY", 1.f));
        _angle.AddKey(time, Read(pos, "angle", 0.f));

        pos = pos->next_sibling("pos");
    }
    _x.SetType(_movingType);
    _y.SetType(_movingType);    
    _scaleX.SetType(_movingType);
    _scaleY.SetType(_movingType);
    _angle.SetType(_movingType);

    // 
    //   -    
    //
    rapidxml::xml_node<> *element = xe->first_node("movingPart");
    while (element) {
        _bones.push_back( new MovingPart(animation, element, hTexture) );
        element = element->next_sibling("movingPart");
    }
}


//
//    
//     - p, [ 0<= p <= 1 ]
//    - stack
//
void MovingPart::PreDraw(float p, MatrixStack &stack) {
    
    float localT;
    int index = _x.Value(p, localT);

    //      "p" - 
    //     "_x" - ..        
    if ((_visible = (index >= 0))) {
        stack.push_back(stack.back());
        stack.back().Move(_x.GetFrame(index, localT), _y.GetFrame(index, localT));
        stack.back().Rotate(_angle.GetFrame(index, localT));
        stack.back().Scale(_scaleX.GetFrame(index, localT), _scaleY.GetFrame(index, localT));

        stack.back().Move(-_center.x, -_center.y);

        if (_hasTexture) {
            stack.back().Mul(_origin[0].x, _origin[0].y, _quad.VertXY(0).x, _quad.VertXY(0).y);
            stack.back().Mul(_origin[1].x, _origin[1].y, _quad.VertXY(1).x, _quad.VertXY(1).y);
            stack.back().Mul(_origin[2].x, _origin[2].y, _quad.VertXY(2).x, _quad.VertXY(2).y);
            stack.back().Mul(_origin[3].x, _origin[3].y, _quad.VertXY(3).x, _quad.VertXY(3).y);
        }        
        //  

        for (unsigned int i = 0; i < _bones.size(); ++i)
            _bones[i]->PreDraw(p, stack);
        
        stack.pop_back();
    }
}

void MovingPart::PreDrawInLocalPosition(MatrixStack &stack) {
    
    //      "p" - 
    //     "_x" - ..        
    if (_visible) {
        stack.push_back(stack.back());
        stack.back().Move(localPosition.x, localPosition.y);
        stack.back().Rotate(localPosition.angle);
        stack.back().Scale(localPosition.sx, localPosition.sy);

        stack.back().Move(-_center.x, -_center.y);

        if (_hasTexture) {
            stack.back().Mul(_origin[0].x, _origin[0].y, _quad.VertXY(0).x, _quad.VertXY(0).y);
            stack.back().Mul(_origin[1].x, _origin[1].y, _quad.VertXY(1).x, _quad.VertXY(1).y);
            stack.back().Mul(_origin[2].x, _origin[2].y, _quad.VertXY(2).x, _quad.VertXY(2).y);
            stack.back().Mul(_origin[3].x, _origin[3].y, _quad.VertXY(3).x, _quad.VertXY(3).y);
        }        
        //  

        for (unsigned int i = 0; i < _bones.size(); ++i)
            _bones[i]->PreDrawInLocalPosition(stack);
        
        stack.pop_back();
    }
}

void MovingPart::PreCalcLocalPosition(float p) {
    float localT;
    int index = _x.Value(p, localT);

    //      "p" - 
    //     "_x" - ..        
    if (_visible = (index >= 0)) {
        precalculatedLocalPosition.x = _x.GetFrame(index, localT);
        precalculatedLocalPosition.y = _y.GetFrame(index, localT);
        precalculatedLocalPosition.angle = _angle.GetFrame(index, localT);
        precalculatedLocalPosition.sx = _scaleX.GetFrame(index, localT);
        precalculatedLocalPosition.sy = _scaleY.GetFrame(index, localT);
    }
}

void MovingPart::Draw() {
    if (_visible && _hasTexture) {
        //      
        _tex->bind();
        _quad.Draw();
        _tex->unbind();
    }
}

void MovingPart::EncapsulateAllDots(Rect &rect) {
    if (_visible && _hasTexture) {
        for (int i = 0; i < 4; ++i) {
            rect.Encapsulate(_quad.VertXY(i).x, _quad.VertXY(i).y);
        }    
    }
}

bool CmpBoneOrder(MovingPart *one, MovingPart *two) {
    return one->_order < two->_order;
}

bool MovingPart::PixelCheck(const FPoint &pos) {
    if (_visible && _hasTexture) {
        FPoint m(pos.x - _quad.VertXY(0).x, pos.y - _quad.VertXY(0).y);
        FPoint a(_quad.VertXY(1).x - _quad.VertXY(0).x, _quad.VertXY(1).y - _quad.VertXY(0).y);
        FPoint b(_quad.VertXY(3).x - _quad.VertXY(0).x, _quad.VertXY(3).y - _quad.VertXY(0).y);

        float k1 = (m.x * b.y - m.y * b.x) / (a.x * b.y - a.y * b.x);
        float k2 = (fabs(b.y) > 1e-5) ? (m.y - k1 * a.y) / b.y : (m.x - k1 * a.x) / b.x;

        if (k1 < 0 || k1 > 1 || k2 < 0 || k2 > 1) {
            return false;
        }

        return true;
        //int i = (k1 * (_quad.VertUV(1).x - _quad.VertUV(0).x) + _quad.VertUV(0).x) * _width;
        //int j = (k2 * (_quad.VertUV(3).y - _quad.VertUV(0).y) + _quad.VertUV(0).y) * _height;

        //DWORD *dw;
        //dw = _hge->Texture_Lock(_quad.tex, true, i, j, 1, 1);
        //bool result = ((*dw) >> 24) > 0x7F;
        //_hge->Texture_Unlock(_quad.tex);
        //return result;
    }
    return false;
}
