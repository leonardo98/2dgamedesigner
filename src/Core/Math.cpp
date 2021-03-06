#include "Math.h"
#include "ogl/render.h"
#include "../Atlas/DataContainer.h"
#include <QDir>
#include <QFile>

int Math::round(float a) {
    int b = static_cast<int>(a);
    return (a - b) >= 0.5f ? b + 1 : b;
} 

int Math::sign(float a) {
    return (a > 0 ? 1 : (a < 0 ? -1 : 0));
}

float Math::random(float a, float b) {
    return a + (b - a) * rand() / RAND_MAX;
} 
float Math::Read(rapidxml::xml_node<> *xe, const char *name, const float defaultValue) {
    rapidxml::xml_attribute<> *tmp = xe->first_attribute(name);
    return (tmp ? fatof(tmp->value()) : defaultValue);
}

void Math::Write(rapidxml::xml_node<> *xe, const char *name, const float value) {
    char buff[20];
    sprintf(buff, "%g", value);
    char *copyBuff = xe->document()->allocate_string(buff);
    xe->append_attribute(xe->document()->allocate_attribute(name, copyBuff));
}

void Math::Write(rapidxml::xml_node<> *xe, const char *name, const int value) {
    char buff[20];
    sprintf(buff, "%i", value);
    char *copyBuff = xe->document()->allocate_string(buff);
    xe->append_attribute(xe->document()->allocate_attribute(name, copyBuff));
}

void Math::Write(rapidxml::xml_node<> *xe, const char *name, const unsigned int value) {
    char buff[20];
    sprintf(buff, "%i", value);
    char *copyBuff = xe->document()->allocate_string(buff);
    xe->append_attribute(xe->document()->allocate_attribute(name, copyBuff));
}

void Math::Write(rapidxml::xml_node<> *xe, const char *name, const char *value) {
    char *copyBuff = xe->document()->allocate_string(value);
    xe->append_attribute(xe->document()->allocate_attribute(name, copyBuff));
}

bool Math::Read(rapidxml::xml_node<> *xe, const char *name, const bool defaultValue) {
    rapidxml::xml_attribute<> *tmp = xe->first_attribute(name);
    if (tmp) {
        std::string s(tmp->value());
        if (s == "true" || s == "1")
        {
            return true;
        }
        return  false;
    }
    return defaultValue;
}

bool Math::GetCirclesIntersect(float X1, float Y1, float R1, float X2, float Y2, float R2, FPoint &P1, FPoint &P2)
{

    if (FPoint(X1 - X2, Y1 - Y2).Length() >= R1 + R2) {
        return false;
    }
    //      Near1.
    float  C1 = R1 * R1 - R2 * R2 + Y2 * Y2 - (X2 - X1) * (X2 - X1) - Y1 * Y1,  //  .
        C2 = X2 - X1,                                                        //  .
        C3 = Y2 - Y1,                                                        //  .
        a  = -4 * C2 * C2 - 4 * C3 * C3,
        b  = (8 * C2 * C2 * Y2 + 4 * C1 * C3) / a,
        c  = (4 * C2 * C2 * R2 * R2 - 4 * C2 * C2 * Y2 * Y2 - C1 * C1) / a,
        X_1_1 = 0, X_1_2 = 0, Y_1 = 0, X_2_1 = 0, X_2_2 = 0, Y_2 = 0;
    a = 1;
    float  sqrtVal = b * b - 4 * a * c;
    if (fabs(sqrtVal) < 0.5f) {
        sqrtVal = 0.f;
    }
    if ( sqrtVal < 0  ||  a == 0 )
    {
        assert(false);
        if ( X1 == X2  &&  Y1 == Y2  &&  R1 == R2 )  //   ,   ,   :
        {
            P1.x = X1 - R1;
            P1.y = Y1;
            P2.x = X1 + R1;
            P2.y = Y1;
            return true;
        }
        return false;
    }

    Y_1 = (-b + sqrt(sqrtVal)) / (2 * a);
    Y_2 = (-b - sqrt(sqrtVal)) / (2 * a);
    float sqrtVal2 = R1 * R1 - (Y_1 - Y1) * (Y_1 - Y1);
    if (fabs(sqrtVal2) < 0.5f) {
        sqrtVal2 = 0.f;
    }
    X_1_1 = X1 + sqrt(sqrtVal2);
    X_1_2 = X1 - sqrt(sqrtVal2);
    float sqrtVal3 = R2 * R2 - (Y_2 - Y2) * (Y_2 - Y2);
    if (fabs(sqrtVal3) < 0.5f) {
        sqrtVal3 = 0.f;
    }
    X_2_1 = X2 + sqrt(sqrtVal3);
    X_2_2 = X2 - sqrt(sqrtVal3);

    //P1.y = Y_1;
    //P2.y = Y_2;
    int counter = 0;
    if (fabs(FPoint(X2 - X_1_1, Y2 - Y_1).Length() - R2) < 1) {
        P1.x = X_1_1;
        P1.y = Y_1;
        counter = 1;
    }
    if (fabs(FPoint(X2 - X_1_2, Y2 - Y_1).Length() - R2) < 1) {
        if (counter == 0) {
            P1.x = X_1_2;
            P1.y = Y_1;
            counter = 1;
        } else {
            P2.x = X_1_2;
            P2.y = Y_1;
            return true;
        }
    }
    if (fabs(FPoint(X1 - X_2_1, Y1 - Y_2).Length() - R1) < 1) {
        if (counter == 0) {
            P1.x = X_2_1;
            P1.y = Y_2;
            counter = 1;
        } else {
            P2.x = X_2_1;
            P2.y = Y_2;
            return true;
        }
    }
    if (fabs(FPoint(X1 - X_2_2, Y1 - Y_2).Length() - R1) < 1) {
        if (counter == 0) {
            P1.x = X_2_2;
            P1.y = Y_2;
            counter = 1;
        } else {
            P2.x = X_2_2;
            P2.y = Y_2;
            return true;
        }
    }
    assert(false);
    return true;
}

float Math::VMul(const FPoint &one, const FPoint &two) {
    return (one.x * two.y - one.y * two.x);
}

float Math::SMul(const FPoint &one, const FPoint &two) {
    return (one.x * two.x + one.y * two.y) / (one.Length() * two.Length());
}

bool Math::STrinagle(float a, float b, float c, float &s) {
    float p = (a + b + c) / 2;
    float underRoot = p * (p - a) * (p - b) * (p - c);
    if (underRoot > 0.f) {
        s = sqrt(underRoot);
        return true;
    }
    s = -1.f;
    return false;
}

bool Math::GetCirclesIntersect2(float x1, float y1, float r1, float x2, float y2, float r2, FPoint &q1, FPoint &q2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float d = sqrt(dx * dx + dy * dy);
    float sTriangle;
    if (!STrinagle(d, r1, r2, sTriangle)) {
        return false;
    }
    float h = 2 * sTriangle / d;
    float t = sqrt(r1 * r1 - h * h);
    FPoint o(dx / d * t, dy / d * t);
    FPoint po(o.y, -o.x);
    o.x += x1;
    o.y += y1;
    po = po / po.Length() * h;
    q1 = o + po;
    q2 = o - po;
    return true;
}

std::string Math::IntToStr(int i) {
    char buff[50];
    sprintf(buff, "%i", i);
    return buff;
}

FPoint Math::lerp(const FPoint &one, const FPoint &two, float p) {
    return FPoint((two.x - one.x) * p + one.x, (two.y - one.y) * p + one.y);
}

float Math::lerp(float one, float two, float p) {
    return (two - one) * p + one;
}

char __buff[20];

const char * Math::FloatToChar(float f, char *s)
{
    if (s)
    {
        sprintf(s, "%0.2f", f);
        return s;
    }
    else
    {
        sprintf(__buff, "%0.2f", f);
        return __buff;
    }
//    sprintf(s, "%g", f);
//    unsigned int i = 0;
//    unsigned int len = strlen(s);
//    while (i < len && (s[i] != '.' && s[i] != ',')) {
//        ++i;
//    }
//    if (i == len) {
//        s[i++] = '.';
//        s[i++] = '0';
//        s[i] = 0;
//    }
}

bool Math::Intersection(const FPoint &line1Start, const FPoint &line1End, 
                            const FPoint &line2Start, const FPoint &line2End, FPoint *result)
{
    float a1 = (line1Start.y - line1End.y);//x coeff
    float b1 = -(line1Start.x - line1End.x);//y coeff
    float c1 = - (a1 * line1Start.x + b1 * line1Start.y);

    float a2 = (line2Start.y - line2End.y);//x coeff
    float b2 = -(line2Start.x - line2End.x);//y coeff
    float c2 = - (a2 * line2Start.x + b2 * line2Start.y);

    //   
    // a1 * x + b1 * y + c1 = 0
    // a2 * x + b2 * y + c2 = 0

    float underline = a2 * b1 - a1 * b2;
    if (fabs(underline) < 1e-3) {
        return false;
    }
    FPoint r((b2 * c1 - b1 * c2) / underline, (a1 * c2 - a2 * c1) / underline);
    if (result) {
        *result = r;
    }
    float l1 = (line1Start - line1End).Length() - 1e-3f;
    float l2 = (line2Start - line2End).Length() - 1e-3f;
    if ((line1Start - r).Length() < l1 && (line1End - r).Length() < l1
        && (line2Start - r).Length() < l2 && (line2End - r).Length() < l2) {
        return true;
    }
    return false;
}

bool Math::Inside(const FPoint &m, const QVector<FPoint> &dots) {
    //       
    int counter = 0;
    int n = dots.size();
    for (int j = 0; j < n; ++j) {
        const FPoint *a2 = &dots[j];
        const FPoint *b2 = &dots[(j + 1) % dots.size()];
        if ((a2->x < b2->x && a2->x < m.x && m.x <= b2->x)
            || (a2->x > b2->x && a2->x >= m.x && m.x > b2->x)
            ) {//         a2b2
            float k = (a2->y - b2->y) / (a2->x - b2->x);
            float b = a2->y - a2->x * k;
            float y = k * m.x + b;
            if (y > m.y) {
                ++counter;
            }
        }
    }
    return (counter % 2 == 1);
}

bool Math::Inside(const FPoint &m, const FPoint &a, const FPoint &b, const FPoint &c) {
    QVector<FPoint> d;
    d.push_back(a);
    d.push_back(b);
    d.push_back(c);
    return Inside(m, d);
}


float Math::Distance(const FPoint &one, const FPoint &two, const FPoint &point) {
    float d = 0;
    float ab = (one - two).Length();
    STrinagle(ab, (two - point).Length(), (point - one).Length(), d);
    return d * 2 / ab; 
}

bool Math::DotNearLine(const FPoint &one, const FPoint &two, const FPoint &p) {
    float a = (one - p).Length();
    float b = (p - two).Length();
    float c = (one - two).Length();
    if (c > a && c > b) {
        float s;
        Math::STrinagle(a, b, c, s);
        return (s / c < 4);
    }
    return false;
}

DWORD Math::ReadColor(std::string color) {
    assert(color.substr(0, 2) == "0x" && color.size() == 10);
    DWORD result = 0;
    unsigned char c;
    for (unsigned int i = 2; i < color.size(); ++i) {
        if (color[i] >= '0' && color[i] <= '9') {
            c = color[i] - '0';
        } else if (color[i] >= 'A' && color[i] <= 'F') {
            c = 10 + (color[i] - 'A');
        } else if (color[i] >= 'a' && color[i] <= 'f') {
            c = 10 + (color[i] - 'a');
        } else {
            assert(false);
        }
        result = (result << 4) + static_cast<DWORD>(c);
    }
    return result;
}

std::string Math::ColorToString(DWORD color) {
    static std::string hex("0123456789ABCDEF");
    std::string result;
    for (;color > 0; color = color >> 4) {
        result = hex[color & 0xF] + result;
    }
    return "0x" + result;
}

std::string Math::GetOnlyFileName(const std::string &filePath) {
    std::string result = filePath.substr(0, filePath.size() - 4);// cut of file extenstion
    std::string::size_type r = filePath.rfind("\\");
    return result.substr(r + 1);
}

std::string Math::ProcessTexture(const char *filePath, const std::string &currentWorkDir)
{
    GLTexture2D *hTex = new GLTexture2D(filePath);
    if (hTex->failed()) {
        return "";
    }
    delete hTex;
    std::string lastOpenedDir = CutFileName(filePath);
    std::string name(std::string(filePath).substr(lastOpenedDir.length() + 1));
    std::string dir = "";

    std::string tmp(CutFileName(lastOpenedDir));
    if (tmp.size() != 0) {
        dir = lastOpenedDir.substr(tmp.length() + 1);
        QDir().mkdir((currentWorkDir + "/" + dir).c_str());
        dir += "/";
    }
    std::string newFileName = currentWorkDir + "/" + dir + name;
    bool result = Agregator::TrimAndSave(filePath, newFileName);
    if (!result)
    {
        if (QFile::exists(newFileName.c_str()))
        {
            //GMessageBoxShow((std::string("can't create new beauty with: ") + filePath).c_str());
            QFile::remove(newFileName.c_str());
        }

        bool result = QFile::copy(filePath, newFileName.c_str());
        if (!result)
        {
            std::string s("can't copy \"");
            s.append(filePath);
            s.append("\" to \"");
            s.append(newFileName);
            s.append("\"");
            //GMessageBoxShow(s.c_str());
        }
    }

    return dir + name;
}

std::string Math::CutFileName(const std::string &filePath) {
    std::string result;
    std::string::size_type index = filePath.find("/");
    std::string::size_type last = std::string::npos;
    while (index != std::string::npos) {
        last = index;
        index = filePath.find("/", index + 1);
    }
    if (last == std::string::npos) {
        return "";
    }
    return filePath.substr(0, last);
} 

bool Math::GenerateTriangles(QVector<FPoint> inputDots, Sprite &sprite, DWORD color, GLTexture2D * texture, const Matrix *transform)
{
    if (inputDots.size() < 3)
    {
        return false;
    }
    QVector<unsigned short> dots;
    dots.resize(inputDots.size());
    for (unsigned short i = 0; i < dots.size(); ++i)
    {
        dots[i] = i;
    }
    sprite.Set(texture);
    VertexBuffer &vb = sprite.GetVB();
    vb.Resize(dots.size(), 3 * (dots.size() - 2));
    if (texture && transform != NULL) 
    {
        for (unsigned int i = 0; i < vb.Size(); ++i) {
            vb.VertXY(i).x = inputDots[i].x;
            vb.VertXY(i).y = inputDots[i].y;
            transform->Mul(vb.VertXY(i).x, vb.VertXY(i).y, vb.VertUV(i).x, vb.VertUV(i).y);
        }
    }
    else
    {
        for (unsigned int i = 0; i < vb.Size(); ++i) {
            vb.VertXY(i).x = inputDots[i].x;
            vb.VertXY(i).y = inputDots[i].y;
            vb.VertUV(i).x = inputDots[i].x / 512.f;
            vb.VertUV(i).y = inputDots[i].y / 512.f;
        }
    }
    float sign = 0.f;
    {
        //       
        const FPoint *a;
        const FPoint *b;
        const FPoint *c;
        int index = 0;
        float minAngle = 180.f;
        for (unsigned int i = 0; i < inputDots.size(); ++i) {
            a = &inputDots[i];
            b = &inputDots[(i + 1) % inputDots.size()];
            c = &inputDots[(i + 2) % inputDots.size()];
            FPoint m((*a + *b + *c) / 3);
            if (Math::Inside(m, inputDots)) {
                FPoint tmpPoint((*c - *b));
                float angle = (*a - *b).Angle(&tmpPoint);
                if (angle <= 0.f) {
                    assert(false);
                }
                if (angle < minAngle) {
                    minAngle = angle;
                    index = i;
                }
            }
        }
        a = &inputDots[index];
        b = &inputDots[(index + 1) % inputDots.size()];
        c = &inputDots[(index + 2) % inputDots.size()];
        sign = Math::VMul(*b - *a, *c - *b);
    }
    int counter = 0;
    int dsize = 0;
    int triCounter = 0;
    while (dots.size() > 0) {
        assert(dots.size() > 2);
        if (dots.size() != dsize) {
            dsize = dots.size();
            counter = 0;
        } else {
            ++counter;
            if (counter > 1) {
                for (unsigned int i = 2; i < dots.size(); ++i) {
                    FillTriangle(triCounter++, dots[0], dots[i - 1], dots[i], vb);
                }
                return false;
            }
        }
        if (dots.size() == 3) {
            FillTriangle(triCounter++, dots[0], dots[1], dots[2], vb);
            dots.clear();
        } else {
            const FPoint *a;
            const FPoint *b;
            const FPoint *c;
            for (unsigned int i = 0; i < dots.size(); ++i) {
                a = &inputDots[dots[i]];
                b = &inputDots[dots[(i + 1) % dots.size()]];
                c = &inputDots[dots[(i + 2) % dots.size()]];
                
                bool intersection = false;
                for (unsigned int j = 0; j < dots.size() && !intersection; ++j) {
                    const FPoint *a2 = &inputDots[dots[j]];
                    const FPoint *b2 = &inputDots[dots[(j + 1) % dots.size()]];
                    intersection = (Math::Intersection(*a, *c, *a2, *b2, NULL) 
                                    || Math::Intersection(*a, *b, *a2, *b2, NULL)
                                    || Math::Intersection(*b, *c, *a2, *b2, NULL));
                }

                for (unsigned int j = 0; j < dots.size() && !intersection; ++j) {
                    const FPoint *a2 = &inputDots[dots[j]];
                    intersection = (a2 != a && a2 != b && a2 != c && Math::Inside(*a2, *a, *b, *c));
                }

                if (!intersection && Math::VMul(*b - *a, *c - *b) * sign > 0.f) {//       
                    FillTriangle(triCounter++, dots[i], dots[(i + 1) % dots.size()], dots[(i + 2) % dots.size()], vb);
                    if (i < dots.size() - 1) {
                        dots.erase(dots.begin() + i + 1);
                    } else {
                        dots.erase(dots.begin());
                    }
                    break;
                }
            }
        }        
    }
    return true;
}


void Math::FillTriangle(int index, int a, int b, int c, VertexBuffer &vb) 
{
    vb.Index(index * 3) = a;
    vb.Index(index * 3 + 1) = b;
    vb.Index(index * 3 + 2) = c;
}
