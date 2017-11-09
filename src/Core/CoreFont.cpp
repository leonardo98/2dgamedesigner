#include "CoreFont.h"
#include "Core.h"

CoreFont::~CoreFont()
{
    for (int i = 0; i < MAX_WORD; ++i)
    {
        if (letters[i])
            delete letters[i];
    }
}

#ifdef Q_WS_WIN
void ZeroMemory(void *ptr, unsigned int size)
{
    memset(ptr, 0, size);
}
#endif

CoreFont::CoreFont(const char *filename)
{
    memset( &letters, 0, sizeof(Sprite*)*MAX_WORD );
    memset( &pre, 0, sizeof(float)*MAX_WORD );
    memset( &post, 0, sizeof(float)*MAX_WORD );
    memset( &yOffset, 0, sizeof(float)*MAX_WORD );

    // Load font description
    std::ifstream myfile(filename);
    std::vector<char> buffer;
    buffer = std::vector<char>(std::istreambuf_iterator<char>(myfile), std::istreambuf_iterator<char>());
    if (buffer.size() == 0)
    {
        LOG("Error! Font not found! " + filename);
        return;
    }
    buffer.push_back('\0');
    rapidxml::xml_document<> doc;
    if (!Parse(doc, buffer, filename))
    {
        return;
    }

    // Parse font description
    rapidxml::xml_node<> *root = doc.first_node();
    if (!root)
    {
        return;
    }
    rapidxml::xml_node<> *textures = root->first_node("textures");
    if (!textures)
    {
        return;
    }

    fTracking = atof(root->first_attribute("spacing")->value());
    lineSpacing = atoi(root->first_attribute("lineSpacing")->value());
    fHeight = 0;//atoi(root->first_attribute("size")->value());
    lineSpacingScale = 1.f;

    std::vector<GLTexture2D *> texPtrs;
    rapidxml::xml_node<> *texture = textures->first_node("tex");
    std::string path(filename);
    std::string::size_type coma = path.rfind("/");
    if (coma != std::string::npos)
    {
        path = path.substr(0, coma + 1);// todo: check both ways!
    }
    while (texture)
    {
        std::string texFileName(texture->first_attribute("name")->value());
        texPtrs.push_back(Core::getTexture(path + texFileName));
        texture = texture->next_sibling("tex");
    }
    if (texPtrs.size() == 0)
    {
       return;
    }

    rapidxml::xml_node<> *symbols = root->first_node("symbols");
    if (!symbols)
    {
        return;
    }
    rapidxml::xml_node<> *sym = symbols->first_node("sym");
    while (sym)
    {
        unsigned int code = (unsigned int)atoi(sym->first_attribute("code")->value());

        if (code < MAX_WORD)
        {
            unsigned int tex = (unsigned int)atoi(sym->first_attribute("tex")->value());
            int x = atoi(sym->first_attribute("x")->value());
            int y = atoi(sym->first_attribute("y")->value());
            int w = atoi(sym->first_attribute("width")->value());
            int h = atoi(sym->first_attribute("height")->value());

            if (fHeight < h)
                fHeight = h;

            pre[code] = atoi(sym->first_attribute("left")->value());
            post[code] = atoi(sym->first_attribute("right")->value());
            yOffset[code] = atoi(sym->first_attribute("yOffset")->value());

            letters[code] = new Sprite(texPtrs[tex], x, y, w, h);
        }

        sym = sym->next_sibling("sym");
    }
}

void CoreFont::Render(float x, float y, int align, const char *string)
{
    unsigned int i;
    float    fx = x;
    y -= fHeight / 2;

    if (align == 1)
        fx -= GetStringWidth(string, false);
    else if (align == 0)
        fx -= int(GetStringWidth(string, false) / 2.0f);

    int nItS = 0;

    while (*string)
    {
        if (*string == '\n')
        {
            y += int(lineSpacingScale * lineSpacing);
            fx = x;
            if (align == 1)
                fx -= GetStringWidth(string + 1, false);
            else if (align == 0)
                fx -= int(GetStringWidth(string + 1, false) / 2.0f);
        }
        else
        {
            i = (unsigned int)*string;
            if (i < MAX_WORD)
            {
                Sprite * pSpt = letters[i];
                if (!pSpt)
                {
                    i='?';
                }
                if(pSpt)
                {
                    fx += pre[i];
                    pSpt->Render(fx, y);// + yOffset[i]);
                    fx += (pSpt->Width() + post[i] + fTracking);
                }
            }
        }
        string++;
        nItS++;
    }
}

float CoreFont::GetStringWidth(const char *string, bool bMultiline)
{
    unsigned int i;
    float linew, w = 0;

    while(*string)
    {
        linew = 0;

        while (*string && *string != '\n')
        {
            i = (unsigned int)*string;

            if (i < MAX_WORD)
            {
                Sprite* iter1 = letters[ i ];
                if (iter1)
                {
                    linew += iter1->Width() + pre[ i ] + post[ i ] + fTracking;
                }
            }

            string++;
        }

        if (!bMultiline)
            return linew;

        if (linew > w)
            w = linew;

        while (*string == '\n' || *string == '\r') string++;
    }

    return w;
}

float CoreFont::GetStringHeight(const char *string)
{
    if (!string)
        return fHeight;

    float h = fHeight;

    while(*string)
    {
        while (*string && *string != '\n')
        {
            string++;
        }
        h += lineSpacing * lineSpacingScale;
        if (*string && *string == '\n')
        {
            string++;
        }
    }

    h -= (lineSpacing * lineSpacingScale);

    return h;
}
