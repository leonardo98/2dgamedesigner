#include "../Core/Math.h"
#include "ColoredButton.h"

//GWEN_CONTROL_CONSTRUCTOR(ColoredButton)
//{
//}

//void ColoredButton::SetColor(DWORD inColor, bool callEvent)
//{
//    std::string color(Math::IntToStr((inColor & 0xFF0000) >> 16) + " " + Math::IntToStr((inColor & 0xFF00) >> 8) + " " + Math::IntToStr(inColor & 0xFF));
//    if (color == GetPropertyValue().c_str())
//    {
//        return;
//    }

//    SetPropertyValue(color, false);
        
//    float col[3];
//    Gwen::Utility::Strings::To::Floats( m_TextBox->GetText().Get(), col, 3);
//    m_Button->SetColor( Gwen::Color( col[0], col[1], col[2] ) );
//}

//Gwen::Color ColoredButton::GetColor() {
//    int r, g, b;
//    sscanf(GetPropertyValue().c_str(), "%i %i %i", &r, &g, &b);
//    return Gwen::Color(r, g, b);
//}
