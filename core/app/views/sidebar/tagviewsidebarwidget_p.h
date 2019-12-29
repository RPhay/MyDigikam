/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-05
 * Description : Side Bar Widget for the tag view - private container.
 *
 * Copyright (C) 2009-2010 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2010-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright (C) 2014      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * Copyright (C) 2010      by Aditya Bhatt <adityabhatt1991 at gmail dot com>
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

#ifndef DIGIKAM_TAG_VIEW_SIDE_BAR_WIDGET_P_H
#define DIGIKAM_TAG_VIEW_SIDE_BAR_WIDGET_P_H

#include "tagviewsidebarwidget.h"

// Qt includes

#include <QButtonGroup>
#include <QLabel>
#include <QScrollBar>
#include <QTimer>
#include <QToolButton>
#include <QRadioButton>
#include <QApplication>
#include <QStyle>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QIcon>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "albummanager.h"
#include "albummodificationhelper.h"
#include "albumselectiontreeview.h"
#include "applicationsettings.h"
#include "datefolderview.h"
#include "editablesearchtreeview.h"
#include "fuzzysearchview.h"
#include "searchfolderview.h"
#include "searchtabheader.h"
#include "searchtextbar.h"
#include "searchtreeview.h"
#include "coredbsearchxml.h"
#include "tagfolderview.h"
#include "timelinewidget.h"
#include "facescanwidget.h"
#include "facesdetector.h"
#include "tagsmanager.h"
#include "coredb.h"
#include "dexpanderbox.h"
#include "dnotificationwidget.h"

namespace Digikam
{

class Q_DECL_HIDDEN TagViewSideBarWidget::Private
{
public:

    enum TagsSource
    {
        NoTags = 0,
        ExistingTags
    };

public:

    explicit Private()
      : openTagMngr(nullptr),
        tagSearchBar(nullptr),
        tagFolderView(nullptr),
        btnGroup(nullptr),
        noTagsBtn(nullptr),
        tagsBtn(nullptr),
        noTagsWasChecked(false),
        ExistingTagsWasChecked(false),
        configTagsSourceEntry(QLatin1String("TagsSource"))
    {
    }

public:

    QPushButton*         openTagMngr;
    SearchTextBar*       tagSearchBar;
    TagFolderView*       tagFolderView;
    QButtonGroup*        btnGroup;
    QRadioButton*        noTagsBtn;
    QRadioButton*        tagsBtn;

    bool                 noTagsWasChecked;
    bool                 ExistingTagsWasChecked;

    QString              noTagsSearchXml;

    const QString configTagsSourceEntry;
};

} // namespace Digikam

#endif // DIGIKAM_TAG_VIEW_SIDE_BAR_WIDGET_P_H
