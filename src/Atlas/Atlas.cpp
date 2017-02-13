#include <vector>
#include <algorithm>
#include "Atlas.h"

// Квадрат длинны вектора (для вычисления длинны диаганали)
#define __QUADLENGTH(x,y) (x*x+y*y)

Atlas::Rect::Rect(int x, int y, int w, int h) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
}

Atlas::Rect::Rect() {
}

void Atlas::Rect::Set(int x, int y, int w, int h) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
}

Atlas::Atlas(int SurfaceMaxWidth, int SurfaceMaxHeight) 
{
    m_AreaRoots.push_back(new Rect(1, 1, SurfaceMaxWidth - 1, SurfaceMaxHeight - 1));
};

Atlas::~Atlas() {
    for (List::iterator i = m_AreaRoots.begin(), e = m_AreaRoots.end(); i != e; ++i) {
        delete (*i);
    }
    for (List::iterator i = m_Busy.begin(), e = m_Busy.end(); i != e; ++i) {
        delete (*i);
    }
}

void Atlas::Split(Rect *parent, int w, int h) {
    // Выбрать тот вариант разбиения, 
    // при котором разница между объемами новых контейнеров наибольшая
    // при котором образуется контейнер наиобольшего объема
    Rect rects[4];
    // NB
    // AA
    // NA
    // BA
    rects[0].Set(parent->x,parent->y+h,w,parent->h-h);
    rects[1].Set(parent->x+w,parent->y,parent->w-w,parent->h);
    rects[2].Set(parent->x+w,parent->y,parent->w-w,h);
    rects[3].Set(parent->x,parent->y+h,parent->w,parent->h-h);
    // Найти контейнер с минимальной диаганалью
    int _indMin = 0;
    for(int i = 1; i < 4; i++)
    {
        if( __QUADLENGTH(rects[i].w,rects[i].h)<__QUADLENGTH(rects[_indMin].w,rects[_indMin].h) ) {
            _indMin = i;
        }
    }
    // Возьмем тот вариант, при котором ПОЛУЧАЕТСЯ контейнер с минимальной диаганалью
    int index = 0;
    if(_indMin>1) {
        index = 2;
    }
    
    // Создать дочерние контейнеры
    m_AreaRoots.push_back(new Rect(rects[index].x, rects[index].y, rects[index].w, rects[index].h));
    index++;
    m_AreaRoots.push_back(new Rect(rects[index].x, rects[index].y, rects[index].w, rects[index].h));
};

//! Функция сравнения областей для сортировки
bool CmpArea(const Atlas::Rect& rect1, const Atlas::Rect& rect2) {
    if (rect1.h != rect2.h) {
        return (rect1.h > rect2.h);
    }
    return (rect1.w > rect2.w);
}

Atlas::Rect * Atlas::Alloc(int w,int h)
{    
    List::iterator minimum = m_AreaRoots.end();
    for (List::iterator i = m_AreaRoots.begin(), e = m_AreaRoots.end(); i != e; ++i) {
        if (w <= (*i)->w && h < (*i)->h) {
            if (minimum == m_AreaRoots.end() || (__QUADLENGTH((*minimum)->w, (*minimum)->h) > __QUADLENGTH((*i)->w, (*i)->h))) {
                minimum = i;
            }
        }
    }

    // Проверим: а может на существующих поверхностях невозможно выделить место под область требуемых размеров
    if(minimum != m_AreaRoots.end()) {
        Rect *base = (*minimum);
        m_AreaRoots.erase(minimum);
        Rect *rect = new Rect(base->x, base->y, w, h);
        Split(base, w, h);
        delete base;
        m_Busy.push_back(rect);
        return rect;
    }
    //
    return NULL;        
};        
