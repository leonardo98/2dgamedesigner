#include "pngopt/pngopt.h"
#include "AtlasWin.h"

CSurfaceWin::CSurfaceWin(int width, int height) 
: m_Width(width)
, m_Height(height)
{
    data = new DWORD[m_Width * m_Height];
    memset(data, 0, m_Width * m_Height * sizeof(DWORD));
}

CSurfaceWin::~CSurfaceWin()
{
    delete [] data;
}

void CSurfaceWin::SetPixel(int x, int y, DWORD d) {
    assert(y * m_Width + x < m_Width * m_Height);
    data[y * m_Width + x] = d;
}

void CSurfaceWin::Save(const char *filepath)
{    
    struct color
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };

    GLTexture2D * tex = new GLTexture2D(m_Width, m_Height);
    int width, height;
    color *data = (color *)this->data;
    color *buf;
    
    int i,j,k,l;
    int r,g,b;
    int count;

    FILE *fp=0;
        
    width  = this->m_Width;
    height = this->m_Height;

    buf = new color[width * height];
    if(!buf) { printf("Can't lock texture.\n"); return; }    



    for(i = 0; i < height; i++) {
        for(j = 0; j < width; j++) {
            if(!data[i * width + j].a)
            {
                count = 0;
                r=g=b = 0;

                for(k = -1; k <= 1; k++)
                    for(l = -1; l <= 1; l++)
                        if(i + k >= 0 && i + k < height &&
                           j + l >= 0 && j + l < width &&
                           data[(i + k) * width + (j + l)].a)
                        {
                            r += data[(i + k) * width + (j + l)].r;
                            g += data[(i + k) * width + (j + l)].g;
                            b += data[(i + k) * width + (j + l)].b;
                            count++;
                        }

                if(count)
                {
                    data[i * width + j].r = static_cast<unsigned char>(r / count);
                    data[i * width + j].g = static_cast<unsigned char>(g / count);
                    data[i * width + j].b = static_cast<unsigned char>(b / count);
                }
            }
            buf[i * width + j] = data[i * width + j];
        }
    }

    fp=fopen(filepath,"wb");
    if(!fp)
    {
        delete tex;
        delete [] buf;
        printf("Can't write to file.\n");
        return;
    }

    if(!Write32BitPNGWithPitch(fp, buf, true, width, height, width))
    {
        printf("Error writing data.\n");
    }
    else
    {
        printf("Ok\n");
    }

    fclose(fp);
    delete tex;
    delete [] buf;
}
