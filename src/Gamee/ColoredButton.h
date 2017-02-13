#ifndef COLORED_BUTTON_H
#define COLORED_BUTTON_H

#include "../Core/types.h"



//class ColourButton : public Gwen::Controls::Button
//{
//    GWEN_CONTROL_INLINE ( ColourButton, Gwen::Controls::Button )
//    {
//        m_Color = Gwen::Colors::Black;
//        SetText( "" );
//    }

//    void Render( Gwen::Skin::Base* skin )
//    {
//        skin->GetRender()->SetDrawColor( m_Color );
//        skin->GetRender()->DrawFilledRect( GetRenderBounds() );
//        skin->GetRender()->SetDrawColor( Gwen::Colors::Grey );
//        skin->GetRender()->DrawLinedRect( GetRenderBounds() );
//    }

//    void SetColor( const Gwen::Color& col ){ m_Color = col; }

//    Gwen::Color    m_Color;
//};

/*
class ColoredButton : public Gwen::Controls::Property::Text
{
    GWEN_CONTROL_INLINE( ColoredButton, Gwen::Controls::Property::Text )
    {
        m_Button = new ColourButton( m_TextBox );
        m_Button->Dock( Gwen::Pos::Right );
        m_Button->SetWidth( 20 );
        m_Button->onPress.Add( this, &ThisClass::OnButtonPress );
        m_Button->SetMargin( Gwen::Margin( 1, 1, 1, 2 ) );
    }

    void OnButtonPress( Gwen::Controls::Base* control )
    {
        Gwen::Controls::Menu* pMenu = new Gwen::Controls::Menu( GetCanvas() );
        pMenu->SetSize( 256 + 110, 256 );
        pMenu->SetDeleteOnClose( true );
        pMenu->onClose.Add( this, &ThisClass::EndColorChanging);
        pMenu->SetDisableIconMargin( true );

        Gwen::Controls::HSVColorPicker* picker = new Gwen::Controls::HSVColorPicker( pMenu );
        picker->Dock( Gwen::Pos::Fill );
        picker->SetSize( 256 + 110, 256 );

        float defaultColor[3];
        Gwen::Utility::Strings::To::Floats( m_TextBox->GetText().Get(), defaultColor, 3);

        picker->SetColor( Gwen::Color( defaultColor[0], defaultColor[1], defaultColor[2], 255 ), false, true );
        picker->onColorChanged.Add( this, &ThisClass::ColorChanged );

        pMenu->Open( Gwen::Pos::Right | Gwen::Pos::Top );
    }

    void ColorChanged( Gwen::Controls::Base* control )
    {
        Gwen::Controls::HSVColorPicker* picker = gwen_cast<Gwen::Controls::HSVColorPicker>( control );

        Gwen::String colorStr;
        colorStr += Gwen::Utility::ToString( ( int )picker->GetColor().r ) + " ";
        colorStr += Gwen::Utility::ToString( ( int )picker->GetColor().g ) + " ";
        colorStr += Gwen::Utility::ToString( ( int )picker->GetColor().b );

        m_TextBox->SetText( colorStr );
        DoChanged();
    }

    virtual Gwen::TextObject GetPropertyValue()
    {
        return m_TextBox->GetText();
    }

    virtual void SetPropertyValue( const Gwen::TextObject& v, bool bFireChangeEvents )
    {
        m_TextBox->SetText( v, bFireChangeEvents );
    }

    virtual bool IsEditing()
    {
        return m_TextBox == Gwen::KeyboardFocus;
    }

    virtual void DoChanged()
    {
        BaseClass::DoChanged();

        float col[3];
        Gwen::Utility::Strings::To::Floats( m_TextBox->GetText().Get(), col, 3);

        m_Button->SetColor( Gwen::Color( col[0], col[1], col[2] ) );
    }

    void SetColor(DWORD inColor, bool callEvent = true);
    Gwen::Color GetColor();    

    Gwen::Event::Caller onEndColorChanging;

    void EndColorChanging()
    {
        onEndColorChanging.Call(this);
    }

    ColourButton*        m_Button;
};
*/




#endif//COLORED_BUTTON_H
