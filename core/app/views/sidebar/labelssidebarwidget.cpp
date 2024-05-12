/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-05
 * Description : Labels sidebar widgets
 *
 * SPDX-FileCopyrightText: 2009-2010 by Johannes Wienke <languitar at semipol dot de>
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2012      by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2014      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * SPDX-FileCopyrightText: 2010      by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "labelssidebarwidget.h"

// Qt includes

#include <QVBoxLayout>
#include <QApplication>
#include <QStyle>
#include <QIcon>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_globals.h"
#include "digikam_debug.h"
#include "albummanager.h"
#include "applicationsettings.h"

namespace Digikam
{

class Q_DECL_HIDDEN LabelsSideBarWidget::Private
{

public:

    Private() = default;

    LabelsTreeView* labelsTree = nullptr;
};

LabelsSideBarWidget::LabelsSideBarWidget(QWidget* const parent)
    : SidebarWidget(parent),
      d            (new Private)
{
    setObjectName(QLatin1String("Labels Sidebar"));
    setProperty("Shortcut", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F3));

    const int spacing         = layoutSpacing();

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

#include "moc_labelssidebarwidget.cpp"
