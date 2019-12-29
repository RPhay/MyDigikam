/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-05
 * Description : Labels sidebar widgets
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

#include "labelssidebarwidgets.h"

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
#include <QMessageBox>

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
#include "labelstreeview.h"
#include "coredb.h"
#include "dexpanderbox.h"
#include "dnotificationwidget.h"

namespace Digikam
{

class Q_DECL_HIDDEN LabelsSideBarWidget::Private
{

public:

    explicit Private()
      : labelsTree(nullptr)
    {
    }

    LabelsTreeView* labelsTree;
};

LabelsSideBarWidget::LabelsSideBarWidget(QWidget* const parent)
    : SidebarWidget(parent),
      d(new Private)
{
    setObjectName(QLatin1String("Labels Sidebar"));
    setProperty("Shortcut", Qt::CTRL + Qt::SHIFT + Qt::Key_F3);

    const int spacing         = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    QVBoxLayout* const layout = new QVBoxLayout(this);

    d->labelsTree = new LabelsTreeView(this);
    d->labelsTree->setConfigGroup(getConfigGroup());

    layout->addWidget(d->labelsTree);
    layout->setContentsMargins(0, 0, spacing, 0);
}

LabelsSideBarWidget::~LabelsSideBarWidget()
{
    delete d;
}

LabelsTreeView *LabelsSideBarWidget::labelsTree()
{
    return d->labelsTree;
}

void LabelsSideBarWidget::setActive(bool active)
{
    if (active)
    {
        d->labelsTree->setCurrentAlbum();
    }
}

void LabelsSideBarWidget::applySettings()
{
}

void LabelsSideBarWidget::changeAlbumFromHistory(const QList<Album*>& album)
{
    Q_UNUSED(album);
}

void LabelsSideBarWidget::doLoadState()
{
    d->labelsTree->doLoadState();
}

void LabelsSideBarWidget::doSaveState()
{
    d->labelsTree->doSaveState();
}

const QIcon LabelsSideBarWidget::getIcon()
{
    return QIcon::fromTheme(QLatin1String("folder-favorites"));
}

const QString LabelsSideBarWidget::getCaption()
{
    return i18n("Labels");
}

QHash<LabelsTreeView::Labels, QList<int> > LabelsSideBarWidget::selectedLabels()
{
    return d->labelsTree->selectedLabels();
}

} // namespace Digikam
