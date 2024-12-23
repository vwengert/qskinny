/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskFluent2TextFieldSkinlet.h"
#include "QskTextField.h"

using Q = QskTextField;

QskFluent2TextFieldSkinlet::QskFluent2TextFieldSkinlet( QskSkin* skin )
    : Inherited( skin )
{
}

QskFluent2TextFieldSkinlet::~QskFluent2TextFieldSkinlet()
{
}

QRectF QskFluent2TextFieldSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSizeF QskFluent2TextFieldSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    return Inherited::sizeHint( skinnable, which, constraint );
}

#include "moc_QskFluent2TextFieldSkinlet.cpp"
