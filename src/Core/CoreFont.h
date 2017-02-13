#ifndef COREFONT_H
#define COREFONT_H

#include "ogl/sprite.h"

#define MAX_WORD 256

class CoreFont
{
public:

    void        Render(float x, float y, int align, const char *string);

//    void        SetProportion(float prop) { fProportion = prop; }
    void        SetTracking(float tracking) { fTracking = tracking; }
    void        SetSpacing(float spacing) { lineSpacingScale = spacing; }

//    float        GetProportion() const { return fProportion; }
    float        GetTracking() const {return fTracking;}
    float        GetSpacing() const {return lineSpacingScale;}

//    Sprite*     GetSprite(char chr);
//    float        GetPreWidth(char chr);
//    float        GetPostWidth(char chr);
    float        GetStringWidth(const char *string, bool bMultiline = true);
    float        GetStringHeight(const char *string);

//    Sprite *    GetLetterSpt( int nIndex );


private:

    CoreFont(const char *filename);
    ~CoreFont();
    CoreFont(const CoreFont &) {} // no permission
    CoreFont&    operator= (const CoreFont &) {} // no permission
    Sprite*     letters[MAX_WORD];

    float        pre[MAX_WORD];
    float        post[MAX_WORD];
    float       yOffset[MAX_WORD];

    float        fTracking;
    float        lineSpacingScale;
    int         lineSpacing;

    float       fHeight;

    friend class FontManager;
};

#endif // COREFONT_H
