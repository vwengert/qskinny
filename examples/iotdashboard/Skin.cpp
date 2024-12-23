/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "Skin.h"

#include "Box.h"
#include "BoxWithButtons.h"
#include "DashboardPage.h"
#include "Diagram.h"
#include "DiagramSkinlet.h"
#include "GridBox.h"
#include "LightDisplay.h"
#include "LightDisplaySkinlet.h"
#include "MenuBar.h"
#include "RoomsPage.h"
#include "RoundButton.h"
#include "RoundedIcon.h"
#include "StorageBar.h"
#include "StorageBarSkinlet.h"
#include "StoragePage.h"
#include "TopBar.h"
#include "UsageBox.h"
#include "UsageDiagram.h"

#include <QskArcMetrics.h>
#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskColorFilter.h>
#include <QskFunctions.h>
#include <QskShadowMetrics.h>
#include <QskSkinHintTableEditor.h>
#include <QskStateCombination.h>
#include <QskTextLabel.h>
#include <QskProgressRing.h>
#include <QskGraphicLabel.h>
#include <QskFontRole.h>

#include <QFontDatabase>

namespace
{
    inline QFont createFont( qreal pointSize,
        QFont::Weight weight = QFont::Normal )
    {
        return QFont( QStringLiteral( "Proxima Nova" ), pointSize, weight );
    }
}

Skin::Skin( QObject* parent )
    : QskSkin( parent )
{
    setObjectName( "iot" );

    declareSkinlet< Diagram, DiagramSkinlet >();
    declareSkinlet< LightDisplay, LightDisplaySkinlet >();
    declareSkinlet< StorageBar, StorageBarSkinlet >();

    setColorScheme( LightScheme );
}

Skin::~Skin()
{
}

void Skin::initHints()
{
    const auto palette = Skin::palette( colorScheme() );

    QFontDatabase::addApplicationFont( ":/fonts/ProximaNova-Regular.otf" );

    setFont( { QskFontRole::Caption, QskFontRole::Low }, createFont( 9 ) );
    setFont( { QskFontRole::Caption, QskFontRole::Normal }, createFont( 10 ) );
    setFont( { QskFontRole::Caption, QskFontRole::High }, createFont( 10, QFont::Bold ) );

    setFont( { QskFontRole::Body, QskFontRole::Normal }, createFont( 12 ) );
    setFont( { QskFontRole::Subtitle, QskFontRole::Normal }, createFont( 13 ) );
    setFont( { QskFontRole::Headline, QskFontRole::Normal }, createFont( 20 ) );
    setFont( { QskFontRole::Display, QskFontRole::Normal }, createFont( 27, QFont::Bold ) );

    QskSkinHintTableEditor ed( &hintTable() );

    ed.setPadding( MainContentGridBox::Panel, { 19, 0, 27, 24 } );

    {
        // menu bar:

        using Q = QskPushButton;
        using A = QskAspect;

        ed.setStrutSize( Q::Panel | A::Header, -1, 40 );
        ed.setPadding( Q::Panel | A::Padding, 10 );

        QColor color( Qt::white );
        color.setAlphaF( 0.09 );
        ed.setGradient( Q::Panel | A::Header | Q::Hovered, color );

        color.setAlphaF( 0.14 );
        ed.setGradient( Q::Panel | A::Header | Q::Checked, color );
        ed.setSpacing( Q::Panel | A::Header, 10 );

        ed.setColor( Q::Text | A::Header, Qt::white );
        ed.setFontRole( Q::Text | A::Header, QskFontRole::Caption );
        ed.setAlignment( Q::Text | A::Header, Qt::AlignLeft | Qt::AlignVCenter );

        ed.setPadding( Q::Icon | A::Header, { 30, 0, 0, 0 } );
        ed.setStrutSize( Q::Icon | A::Header, { 14, -1 } );
        ed.setAlignment( Q::Icon | A::Header, Qt::AlignCenter );
    }

    {
        // top bar:

        ed.setPadding( TopBar::Panel, { 25, 35, 25, 0 } );

        ed.setColor( TopBarItem::Item1 | QskAspect::TextColor, 0xffff3122 );
        ed.setColor( TopBarItem::Item2 | QskAspect::TextColor, 0xff6776ff );
        ed.setColor( TopBarItem::Item3 | QskAspect::TextColor, 0xfff99055 );
        ed.setColor( TopBarItem::Item4 | QskAspect::TextColor, 0xff6776ff );

        ed.setGradient( TopBarItem::Item1, 0xffff5c00, 0xffff3122 );
        ed.setGradient( TopBarItem::Item2, 0xff6776ff, 0xff6100ff );
        ed.setGradient( TopBarItem::Item3, 0xffffce50, 0xffff3122 );
        ed.setGradient( TopBarItem::Item4, 0xff6776ff, 0xff6100ff );
    }

    // the bar gradient is defined through the top bar items above
    ed.setArcMetrics( QskProgressRing::Groove, 90, -360, 8.53 );
    // the span angle will be set in the progress bar, we just give a dummy
    // value here:
    ed.setArcMetrics( QskProgressRing::Fill, ed.arcMetrics( QskProgressRing::Groove ) );

    ed.setFontRole( TimeTitleLabel::Text, { QskFontRole::Caption, QskFontRole::High } );

    ed.setFontRole( TimeLabel::Text, QskFontRole::Display );
    ed.setColor( TimeLabel::Text, 0xff6776ff );

    // boxes:
    ed.setPadding( Box::Panel, 8 );

    // content in boxes (indoor temperature, humidity etc.):
    ed.setFontRole( UsageBox::Separator, QskFontRole::Caption );
    ed.setColor( UsageBox::Separator, 0xffdddddd );

    ed.setPadding( BoxWithButtons::Panel, 8 );

    for ( auto subControl : { RoundedIcon::Panel, RoundedIcon::PalePanel } )
    {
        ed.setBoxShape( subControl, 6 );

        QskGradient normal( 0xff6776ff, 0xff6100ff );
        normal.setLinearDirection( Qt::Vertical );

        QskGradient bright( 0xffff7d34, 0xffff3122 );
        bright.setLinearDirection( Qt::Vertical );

        if ( subControl == RoundedIcon::PalePanel ) // My Devices section
        {
            const uint alpha = 38;
            normal.setAlpha( alpha );
            bright.setAlpha( alpha );

            auto pressedNormal = normal;
            pressedNormal.setAlpha( 10 );
            auto pressedBright = bright;
            pressedBright.setAlpha( 10 );

            const int duration = 300;

            ed.setGradient( RoundedIcon::PalePanel | QskAbstractButton::Checked, pressedNormal );
            ed.setGradient(
                RoundedIcon::PalePanel | RoundedIcon::Bright | QskAbstractButton::Checked,
                pressedBright );
            ed.setAnimation( RoundedIcon::PalePanel | QskAspect::Color, duration );

            ed.setGraphicRole( RoundedIcon::Graphic, RoundedIcon::NormalRole );
            ed.setGraphicRole( RoundedIcon::Graphic | QskAbstractButton::Checked,
                RoundedIcon::CheckedRole,
                { QskStateCombination::CombinationNoState, RoundedIcon::Bright } );
            ed.setAnimation( RoundedIcon::Graphic, duration );

            QskColorFilter filter;
            filter.addColorSubstitution(
                0xff606675, palette.deviceGraphic ); // color comes from the SVG
            setGraphicFilter( RoundedIcon::CheckedRole, filter );
        }

        ed.setGradient( subControl, normal );
        ed.setGradient( subControl | RoundedIcon::Bright, bright );
    }

    ed.setFontRole( BoxWithButtons::ValueText, QskFontRole::Display );
    ed.setColor( BoxWithButtons::ValueText, 0xff929cb2 );

    ed.setPadding( BoxWithButtons::ValuePanel, 0, 10, 0, 0 );

    ed.setStrutSize( RoundButton::Panel, 27, 38 );
    ed.setBoxShape( RoundButton::Panel, 0, 0, 30, 30 );
    ed.setBoxShape( RoundButton::Panel | RoundButton::Top, 30, 30, 0, 0 );

    // diagram:
    ed.setBoxBorderMetrics( UsageDiagramBox::DaysBox, 0, 0, 3, 3 );
    ed.setFontRole( UsageDiagramBox::DayText, { QskFontRole::Caption, QskFontRole::Low } );

    ed.setStrutSize( UsageDiagramLegend::Symbol, 8, 8 );
    ed.setBoxShape( UsageDiagramLegend::Symbol, 100, Qt::RelativeSize ); // a circle
    ed.setGradient( UsageDiagramLegend::Symbol | UsageDiagramLegend::Water, { 0xff6776ff } );
    ed.setGradient( UsageDiagramLegend::Symbol | UsageDiagramLegend::Electricity, { 0xffff3122 } );
    ed.setGradient( UsageDiagramLegend::Symbol | UsageDiagramLegend::Gas, { 0xffff7d34 } );

    ed.setPadding( UsageDiagramBox::Panel, 0 );

    // new diagram:
    ed.setColor( Diagram::ChartArea1, 0x666776ff );
    ed.setColor( Diagram::ChartArea2, 0x66ff3122 );
    ed.setColor( Diagram::ChartArea3, 0x66ff7d34 );

    ed.setColor( Diagram::ChartBar1, 0xff6776ff );
    ed.setColor( Diagram::ChartBar2, 0xffff3122 );
    ed.setColor( Diagram::ChartBar3, 0xffff7d34 );

    ed.setStrutSize( Diagram::ChartBar1, { 6, -1 } );
    ed.setStrutSize( Diagram::ChartBar2, { 6, -1 } );
    ed.setStrutSize( Diagram::ChartBar3, { 6, -1 } );

    ed.setBoxShape( Diagram::ChartBar1, { 3, 3, 0, 0 } );
    ed.setBoxShape( Diagram::ChartBar2, { 3, 3, 0, 0 } );
    ed.setBoxShape( Diagram::ChartBar3, { 3, 3, 0, 0 } );

    // light intensity:
    ed.setBoxShape( LightDisplay::Panel, 100, Qt::RelativeSize );

    ed.setArcMetrics( LightDisplay::ColdAndWarmArc, 0, 180, 8.785 );

    ed.setMetric( LightDisplay::Tickmarks, 1 );
    ed.setArcMetrics( LightDisplay::Tickmarks, 0, 180, 4.69 );
    ed.setColor( LightDisplay::Tickmarks, 0x55929cb2 );

    ed.setFontRole( LightDisplay::ValueText, QskFontRole::Headline );
    ed.setColor( LightDisplay::ValueText, 0xff929cb2 );

    ed.setStrutSize( LightDisplay::Handle, { 20, 20 } );
    ed.setBoxBorderMetrics( LightDisplay::Handle, 1 );
    ed.setBoxShape( LightDisplay::Handle, 100, Qt::RelativeSize );

    // palette dependent skin hints:
    ed.setGradient( MenuBar::Panel, palette.menuBar );
    ed.setGradient( DashboardPage::Panel, palette.mainContent );
    ed.setGradient( RoomsPage::Panel, palette.mainContent );
    ed.setGradient( StoragePage::Panel, palette.mainContent );

    ed.setColor( Box::Panel, palette.box );
    QskShadowMetrics shadowMetrics( 0, 10 );
    ed.setShadowMetrics( Box::Panel, shadowMetrics );
    ed.setShadowColor( Box::Panel, palette.shadow );

    ed.setGradient( BoxWithButtons::Panel, palette.box );
    ed.setShadowMetrics( BoxWithButtons::Panel, shadowMetrics );
    ed.setShadowColor( BoxWithButtons::Panel, palette.shadow );

    ed.setGradient( UsageDiagramBox::Panel, palette.box );
    ed.setShadowMetrics( UsageDiagramBox::Panel, shadowMetrics );
    ed.setShadowColor( UsageDiagramBox::Panel, palette.shadow );

    ed.setGradient( LightDisplay::Panel, palette.box );
    ed.setGradient( LightDisplay::Handle, palette.box );
    ed.setGradient( LightDisplay::ColdAndWarmArc, palette.lightDisplayColdAndWarmArc );
    ed.setBoxBorderColors( LightDisplay::Handle, palette.lightDisplayKnobBorder );
    ed.setShadowMetrics( LightDisplay::Groove, { 0, 20 } );
    ed.setShadowColor( LightDisplay::Groove, palette.shadow );
    ed.setGradient( LightDisplay::Groove, palette.box );
    ed.setBoxShape( LightDisplay::Groove, 100, Qt::RelativeSize );

    ed.setGradient( RoundButton::Panel, palette.roundButton );
    ed.setGradient( RoundButton::Panel | QskAbstractButton::Pressed, palette.roundButtonPressed,
        { QskStateCombination::CombinationNoState, RoundButton::Top } );
    ed.setAnimation( RoundButton::Panel | QskAspect::Color, 100 );

    ed.setBoxBorderColors( UsageDiagramBox::DaysBox, palette.weekdayBox );
    ed.setColor( QskTextLabel::Text, palette.text );
    ed.setColor( UsageDiagramBox::DayText, palette.text );

    ed.setMetric( QskProgressRing::Groove | QskAspect::Border, 2 );
    ed.setColor( QskProgressRing::Groove | QskAspect::Border,
        palette.circularProgressBarGroove );

    // storage bar
    {
        const auto make_gradient = []( const QColor color ) -> QskGradient
            { return { color.lighter(), color }; };

        ed.setGradient( StorageBar::Pictures, make_gradient( "#FFBE0B" ) );
        ed.setGradient( StorageBar::Music, make_gradient( "#FB5607" ) );
        ed.setGradient( StorageBar::Videos, make_gradient( "#FF006E" ) );
        ed.setGradient( StorageBar::Documents, make_gradient( "#8338EC" ) );
        ed.setGradient( StorageBar::Others, make_gradient( "#3A86FF" ) );
        ed.setGradient( StorageBar::Free, make_gradient( "lightgray" ) );
    }

    // storage meter
    {
        ed.setGradient( StoragePage::Status,
            { {
                { 0.00, "#00ff00" }, { 0.33, "#00ff00" },
                { 0.33, "#ffaf00" }, { 0.66, "#ffaf00" },
                { 0.66, "#ff0000" }, { 1.00, "#ff0000" }
            } }
        );
    }
}

Skin::Palette Skin::palette( QskSkin::ColorScheme colorScheme ) const
{
    if ( colorScheme == DarkScheme )
    {
        return {
            0xff2937A7,
            0xff040404,
            Qt::black,
            0xff0a0a0a,
            0xff1a1a1a,
            0xff0c0c0c,
            Qt::white,
            0xff4a4a4a,
            0xff555555,
            { { { 0.0, 0xff991100 }, { 0.4, 0xff9a7a57 }, { 1.0, 0xff3726af } } },
            0x10ffffff,
            0xff222222
        };
    }
    else
    {
        return {
            0xff6d7bfb,
            0xfffbfbfb,
            Qt::white,
            0xfff7f7f7,
            0xffe5e5e5,
            0xfff4f4f4,
            Qt::black,
            0xffe5e5e5,
            0xffc4c4c4,
            { { { 0.0, 0xffff3122 }, { 0.4, 0xfffeeeb7 }, { 0.6, 0xffa7b0ff }, { 1.0, 0xff6776ff } } },
            0x10000000,
            0xffdddddd
        };
    }
}
