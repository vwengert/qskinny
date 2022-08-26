/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSubcontrolLayoutEngine.h"
#include "QskLayoutElement.h"
#include "QskLayoutChain.h"
#include "QskTextRenderer.h"
#include "QskSkinnable.h"
#include "QskMargins.h"
#include "QskTextOptions.h"

#include <qfont.h>
#include <qfontmetrics.h>
#include <qmath.h>

QskSubcontrolLayoutEngine::LayoutElement::LayoutElement(
        const QskSkinnable* skinnable, const QskAspect::Subcontrol subControl )
    : m_skinnable( skinnable )
    , m_subControl( subControl )
{
}

Qt::Alignment QskSubcontrolLayoutEngine::LayoutElement::alignment() const
{
    return m_skinnable->alignmentHint( m_subControl );
}

void QskSubcontrolLayoutEngine::LayoutElement::setMaximumSize( const QSizeF& size )
{
    setExplicitSizeHint( Qt::MaximumSize, size );
}

void QskSubcontrolLayoutEngine::LayoutElement::setMinimumSize( const QSizeF& size )
{
    setExplicitSizeHint( Qt::MinimumSize, size );
}

void QskSubcontrolLayoutEngine::LayoutElement::setPreferredSize( const QSizeF& size )
{
    setExplicitSizeHint( Qt::PreferredSize, size );
}

void QskSubcontrolLayoutEngine::LayoutElement::setFixedSize( const QSizeF& size )
{
    setSizePolicy( QskSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed ) );

    const auto newSize = size.expandedTo( QSizeF( 0, 0 ) );
    setExplicitSizeHint( Qt::PreferredSize, newSize );
}

void QskSubcontrolLayoutEngine::LayoutElement::setExplicitSizeHint(
    Qt::SizeHint which, const QSizeF& size )
{
    if ( which >= Qt::MinimumSize && which <= Qt::MaximumSize )
    {
        const QSizeF newSize( ( size.width() < 0 ) ? -1.0 : size.width(),
            ( size.height() < 0 ) ? -1.0 : size.height() );

        m_explicitSizeHints[ which ] = size;
    }
}

QSizeF QskSubcontrolLayoutEngine::LayoutElement::sizeHint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which < Qt::MinimumSize || which > Qt::MaximumSize )
        return QSizeF( 0, 0 );

    if ( constraint.isValid() )
        return constraint;

    const bool isConstrained =
        constraint.width() >= 0 || constraint.height() >= 0;

    QSizeF hint;

    if ( !isConstrained )
    {
        // explicit size hints are never constrained
        hint = m_explicitSizeHints[ which ];
    }

    if ( which == Qt::PreferredSize )
    {
        if ( isConstrained )
        {
            const QskMargins padding = m_skinnable->paddingHint( m_subControl );

            auto innerConstraint = constraint;

            if ( constraint.width() > 0 )
            {
                const auto w = constraint.width() - padding.width();
                if ( w <= 0 )
                    return QSizeF();

                innerConstraint.setWidth( w );
            }
            else if ( constraint.height() > 0 )
            {
                const auto h = constraint.height() - padding.height();
                if ( h <= 0 )
                    return QSizeF();

                innerConstraint.setHeight( h );
            }

            hint = implicitSize( innerConstraint );

            if ( hint.width() >= 0 )
                hint.setWidth( hint.width() + padding.width() );

            if ( hint.height() >= 0 )
                hint.setHeight( hint.height() + padding.height() );
        }
        else
        {
            if ( !hint.isValid() )
            {
                const auto sz = implicitSize( constraint );

                if ( hint.width() < 0 )
                    hint.setWidth( sz.width() );

                if ( hint.height() < 0 )
                    hint.setHeight( sz.height() );
            }
        }
    }
    else
    {
        if ( isConstrained )
            hint = QSizeF(); // not implemented
    }

    return hint;
}

QSizeF QskSubcontrolLayoutEngine::TextElement::implicitSize( const QSizeF& constraint ) const
{
    const auto font = skinnable()->effectiveFont( subControl() );
    const auto textOptions = skinnable()->textOptionsHint( subControl() );

    QSizeF hint;

    const qreal lineHeight = QFontMetricsF( font ).height();

    if ( m_text.isEmpty() )
    {
        if ( constraint.height() < 0.0 )
            hint.setHeight( qCeil( lineHeight ) );
    }
    else if ( constraint.width() >= 0.0 )
    {
        if ( textOptions.effectiveElideMode() != Qt::ElideNone )
        {
            hint.setHeight( qCeil( lineHeight ) );
        }
        else
        {
            /*
                In case of QskTextOptions::NoWrap we could count
                the line numbers and calculate the height from
                lineHeight. TODO ...
             */

            qreal maxHeight = std::numeric_limits< qreal >::max();
            if ( maxHeight / lineHeight > textOptions.maximumLineCount() )
            {
                // be careful with overflows
                maxHeight = textOptions.maximumLineCount() * lineHeight;
            }

            QSizeF size( constraint.width(), maxHeight );
            size = QskTextRenderer::textSize( m_text, font, textOptions, size );

            hint.setHeight( qCeil( size.height() ) );
        }
    }
    else if ( constraint.height() >= 0.0 )
    {
        const qreal maxWidth = std::numeric_limits< qreal >::max();

        QSizeF size( maxWidth, constraint.height() );
        size = QskTextRenderer::textSize( m_text, font, textOptions, size );

        hint.setWidth( qCeil( size.width() ) );
    }
    else
    {
        hint = QskTextRenderer::textSize( m_text, font, textOptions );
    }

    return hint;
}

QSizeF QskSubcontrolLayoutEngine::GraphicElement::effectiveStrutSize() const
{
    auto size = skinnable()->strutSizeHint( subControl() );

    if ( size.isEmpty() )
    {
        const qreal aspectRatio = m_sourceSize.width() / m_sourceSize.height();

        if ( size.width() > 0 )
        {
            size.setHeight( size.width() / aspectRatio );
        }
        else if ( size.height() > 0 )
        {
            size.setWidth( size.height() * aspectRatio );
        }
    }

    return size;
}


QSizeF QskSubcontrolLayoutEngine::GraphicElement::implicitSize( const QSizeF& constraint ) const
{
    auto hint = m_sourceSize;

    if ( !hint.isEmpty() )
    {
        const qreal aspectRatio = hint.width() / hint.height();

        if ( constraint.width() >= 0.0 )
        {
            hint.setHeight( constraint.width() / aspectRatio );
            hint.setWidth( -1.0 );
        }
        else if ( constraint.height() > 0.0 )
        {
            hint.setWidth( constraint.height() * aspectRatio );
            hint.setHeight( -1.0 );
        }
    }

    return hint;
}


static QskLayoutChain::CellData qskCell( const QskSubcontrolLayoutEngine::LayoutElement* element,
    Qt::Orientation orientation, bool isLayoutOrientation, qreal constraint )
{
    QskLayoutChain::CellData cell;
    cell.isValid = true;

    const auto policy = element->sizePolicy().policy( orientation );

    if ( isLayoutOrientation )
    {
        const auto stretch = element->stretch();

        if ( stretch < 0 )
            cell.stretch = ( policy & QskSizePolicy::ExpandFlag ) ? 1 : 0;
        else
            cell.stretch = stretch;
    }

    cell.canGrow = policy & QskSizePolicy::GrowFlag;
    cell.metrics = element->metrics( orientation, constraint );

    return cell;
}

class QskSubcontrolLayoutEngine::PrivateData
{
  public:
    PrivateData( Qt::Orientation orientation )
        : orientation( orientation )
    {
    }

    Qt::Orientation orientation;
    LayoutElement* elements[2] = {};
};

QskSubcontrolLayoutEngine::QskSubcontrolLayoutEngine( Qt::Orientation orientation )
    : m_data( new PrivateData( orientation ) )
{
    setExtraSpacingAt( Qt::TopEdge | Qt::BottomEdge | Qt::LeftEdge | Qt::RightEdge );
}

QskSubcontrolLayoutEngine::~QskSubcontrolLayoutEngine()
{
    for ( auto element : m_data->elements )
        delete element;
}

bool QskSubcontrolLayoutEngine::setOrientation( Qt::Orientation orientation )
{
    if ( m_data->orientation != orientation )
    {
        m_data->orientation = orientation;
        invalidate( LayoutCache );

        return true;
    }

    return false;
}

Qt::Orientation QskSubcontrolLayoutEngine::orientation() const
{
    return m_data->orientation;
}

void QskSubcontrolLayoutEngine::setElementAt( int index, LayoutElement* element )
{
    if ( index >= 0 && index < count() )
    {
        delete m_data->elements[ index ];
        m_data->elements[ index ] = element;
    }
}

QskSubcontrolLayoutEngine::LayoutElement* QskSubcontrolLayoutEngine::elementAt( int index ) const
{
    if ( index >= 0 && index < count() )
        return m_data->elements[ index ];

    return nullptr;
}

QskSizePolicy QskSubcontrolLayoutEngine::sizePolicyAt( int index ) const
{
    if ( index >= 0 && index < count() )
    {
        if ( auto element = m_data->elements[ index ] )
            return element->sizePolicy();
    }

    return QskSizePolicy();
}

int QskSubcontrolLayoutEngine::count() const
{
    return sizeof( m_data->elements ) / sizeof( m_data->elements[0] );
}

void QskSubcontrolLayoutEngine::layoutItems()
{
    int row = 0;
    int col = 0;

    for ( auto element : m_data->elements )
    {
        if ( element && !element->isIgnored() )
        {
            const auto rect = geometryAt( element, QRect( col, row, 1, 1 ) );
            element->setGeometry( rect );

            if ( m_data->orientation == Qt::Horizontal )
                col++;
            else
                row++;
        }
    }
}

int QskSubcontrolLayoutEngine::effectiveCount( Qt::Orientation orientation ) const
{
    return ( orientation == m_data->orientation ) ? 2 : 1;
}

void QskSubcontrolLayoutEngine::invalidateElementCache()
{
}

void QskSubcontrolLayoutEngine::setupChain( Qt::Orientation orientation,
    const QskLayoutChain::Segments& constraints, QskLayoutChain& chain ) const
{
    uint index1 = 0;
    uint index2 = 0;

    const bool isLayoutOrientation = ( orientation == m_data->orientation );

    for ( auto element : m_data->elements )
    {
        if ( element == nullptr || element->isIgnored() )
            continue;

        qreal constraint = -1.0;
        if ( !constraints.isEmpty() )
            constraint = constraints[index1].length;

        const auto cell = qskCell( element, orientation, isLayoutOrientation, constraint );
        chain.expandCell( index2, cell );

        if ( isLayoutOrientation )
            index2++;
        else
            index1++;
    }
}