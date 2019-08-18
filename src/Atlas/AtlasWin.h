#ifndef  AtlasWin_H
#define  AtlasWin_H

#include "Core/ogl/render.h"

class CSurfaceWin
{
private:
    DWORD *data;

public:
    //! ������ �����������
    const int m_Width;
    //! ������ �����������
    const int m_Height;

    //! �����������
    CSurfaceWin(int width, int height);
    //! ����������
    ~CSurfaceWin();

    void SetPixel(int x, int y, DWORD d);

    //! ���������� ����������� � ����
    void Save(const char *filepath);

};

#endif
