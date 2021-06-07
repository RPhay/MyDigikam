/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-12
 * Description : Widget for assignment and confirmation of names for faces
 *
 * Copyright (C) 2010      by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_ASSIGN_NAME_WIDGET_P_H
#define DIGIKAM_ASSIGN_NAME_WIDGET_P_H

#include "assignnamewidget.h"

// Qt includes

#include <QGridLayout>
#include <QKeyEvent>
#include <QToolButton>
#include <QApplication>
#include <QIcon>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dexpanderbox.h"
#include "digikam_debug.h"
#include "addtagscombobox.h"
#include "addtagslineedit.h"
#include "album.h"
#include "albummanager.h"
#include "albumtreeview.h"
#include "facetagsiface.h"
#include "facetags.h"
#include "dimg.h"
#include "iteminfo.h"
#include "thememanager.h"
#include "applicationsettings.h"

namespace Digikam
{

class Q_DECL_HIDDEN AssignNameWidget::Private
{
public:

    explicit Private(AssignNameWidget* const q);

    bool         isValid()                                        const;

    void         updateModes();
    void         updateContents();

    /**
     * Reject button shows different Tooltips for
     * Unconfirmed and Unknown faces, however both
     * are of type UnconfirmedEditMode.
     * This method is responsible for the Tooltip updates.
     */
    void         updateRejectButtonTooltip();

private:

    void         clearWidgets();
    void         checkWidgets();
    void         updateLayout();
    void         updateVisualStyle();

    QToolButton* createToolButton(const QIcon& icon,
                                  const QString& text,
                                  const QString& tip = QString()) const;

    QWidget* addTagsWidget()                                      const;

    template <class T> void setupAddTagsWidget(T* const widget);
    template <class T> void setAddTagsWidgetContents(T* const widget);

    void layoutAddTagsWidget(bool exceedBounds, int minimumContentsLength);
    void setSizePolicies(QSizePolicy::Policy h, QSizePolicy::Policy v);
    void setToolButtonStyles(Qt::ToolButtonStyle style);
    QString styleSheetFontDescriptor(const QFont& font)           const;

public:

    ItemInfo                   info;
    QVariant                   faceIdentifier;
    AlbumPointer<TAlbum>       currentTag;

    Mode                       mode;
    LayoutMode                 layoutMode;
    VisualStyle                visualStyle;
    TagEntryWidgetMode         widgetMode;

    AddTagsComboBox*           comboBox;
    AddTagsLineEdit*           lineEdit;
    QToolButton*               confirmButton;
    QToolButton*               rejectButton;
    DClickLabel*               clickLabel;

    bool                       modelsGiven;
    TagModel*                  tagModel;
    CheckableAlbumFilterModel* tagFilterModel;
    TagPropertiesFilterModel*  tagFilteredModel;
    AlbumPointer<TAlbum>       parentTag;

    QGridLayout*               layout;

    AssignNameWidget* const    q;
};

} // namespace Digikam

#endif // DIGIKAM_ASSIGN_NAME_WIDGET_P_H
