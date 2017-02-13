// Object.h: interface for the Object class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MYENGINE_OBJECT_H
#define MYENGINE_OBJECT_H

#include "types.h"

//
// базовый класс, наследуемся от него если нам нужны только два 
// метода и не нужно обрабатывать ввод, иначе см. InputSystem.h(удален за ненадобностью)
//

class Object  
{
public:
    virtual ~Object();
    virtual void Draw();
    virtual void Update(float);
};

#endif//OBJECT_H
