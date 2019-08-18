#ifndef  AtlasWin_H
#define  AtlasWin_H

#include "Core/ogl/render.h"

class CSurfaceWin
{
private:
    DWORD *data;

public:
    //! Ширина поверхности
    const int m_Width;
    //! Высота поверхности
    const int m_Height;

    //! Конструктор
    CSurfaceWin(int width, int height);
    //! Деструктор
    ~CSurfaceWin();

    void SetPixel(int x, int y, DWORD d);

    //! Сохранение поверхности в файл
    void Save(const char *filepath);

};

#endif
