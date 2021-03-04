/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-05-17
 * Description : Album Labels Tree View.
 *
 * Copyright (C) 2014-2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * Copyright (C) 2014-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "labelstreeview.h"

// QT includes

#include <QApplication>
#include <QTreeWidget>
#include <QPainter>
#include <QUrl>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "coredbsearchxml.h"
#include "searchtabheader.h"
#include "albummanager.h"
#include "albumtreeview.h"
#include "coredbconstants.h"
#include "itemlister.h"
#include "statesavingobject.h"
#include "coredbaccess.h"
#include "coredb.h"
#include "colorlabelfilter.h"
#include "picklabelfilter.h"
#include "tagscache.h"
#include "applicationsettings.h"
#include "dnotificationwrapper.h"
#include "digikamapp.h"
#include "ratingwidget.h"
#include "dbjobsmanager.h"

namespace Digikam
{

class Q_DECL_HIDDEN LabelsTreeView::Private
{
public:

    explicit Private()
      : ratings             (nullptr),
        picks               (nullptr),
        colors              (nullptr),
        isCheckableTreeView (false),
        isLoadingState      (false),
        iconSizeFromSetting (0)
    {
    }

    QFont                      regularFont;
    QSize                      iconSize;

    QTreeWidgetItem*           ratings;
    QTreeWidgetItem*           picks;
    QTreeWidgetItem*           colors;

    bool                       isCheckableTreeView;
    bool                       isLoadingState;
    int                        iconSizeFromSetting;

    QHash<Labels, QList<int> > selectedLabels;

    static const QString       configRatingSelectionEntry;
    static const QString       configPickSelectionEntry;
    static const QString       configColorSelectionEntry;
    static const QString       configExpansionEntry;
};

const QString LabelsTreeView::Private::configRatingSelectionEntry(QLatin1String("RatingSelection"));
const QString LabelsTreeView::Private::configPickSelectionEntry(QLatin1String("PickSelection"));
const QString LabelsTreeView::Private::configColorSelectionEntry(QLatin1String("ColorSelection"));
const QString LabelsTreeView::Private::configExpansionEntry(QLatin1String("Expansion"));

LabelsTreeView::LabelsTreeView(QWidget* const parent, bool setCheckable)
    : QTreeWidget      (parent),
      StateSavingObject(this),
      d                (new Private)
{
    d->regularFont         = ApplicationSettings::instance()->getTreeViewFont();
    d->iconSizeFromSetting = ApplicationSettings::instance()->getTreeViewIconSize();
    d->iconSize            = QSize(d->iconSizeFromSetting, d->iconSizeFromSetting);
    d->isCheckableTreeView = setCheckable;

    setHeaderLabel(i18nc("@title", "Labels"));
    setUniformRowHeights(false);
    initTreeView();

    if (d->isCheckableTreeView)
    {
        QTreeWidgetItemIterator it(this);

        while (*it)
        {
            if ((*it)->parent())
            {
                (*it)->setFlags((*it)->flags()|Qt::ItemIsUserCheckable);
                (*it)->setCheckState(0, Qt::Unchecked);
            }

            ++it;
        }
    }
    else
    {
        setSelectionMode(QAbstractItemView::ExtendedSelection);
    }

    connect(ApplicationSettings::instance(), SIGNAL(setupChanged()),
            this, SLOT(slotSettingsChanged()));
}

LabelsTreeView::~LabelsTreeView()
{
    delete d;
}

bool LabelsTreeView::isCheckable() const
{
    return d->isCheckableTreeView;
}

bool LabelsTreeView::isLoadingState() const
{
    return d->isLoadingState;
}

QPixmap LabelsTreeView::goldenStarPixmap(bool fillin) const
{
    QPixmap pixmap = QPixmap(60, 60);
    pixmap.fill(Qt::transparent);

    QPainter p1(&pixmap);
    p1.setRenderHint(QPainter::Antialiasing, true);

    if (fillin)
    {
        p1.setBrush(qApp->palette().color(QPalette::Link));
    }

    QPen pen(palette().color(QPalette::Active, foregroundRole()));
    p1.setPen(pen);

    QTransform transform;
    transform.scale(4, 4);     // 60px/15px (RatingWidget::starPolygon() size is 15*15px)
    p1.setTransform(transform);

    p1.drawPolygon(RatingWidget::starPolygon(), Qt::WindingFill);
    p1.end();

    return pixmap;
}

QPixmap LabelsTreeView::colorRectPixmap(const QColor& color) const
{
    QRect rect(8, 8, 48, 48);
    QPixmap pixmap = QPixmap(60, 60);
    pixmap.fill(Qt::transparent);

    QPainter p1(&pixmap);
    p1.setRenderHint(QPainter::Antialiasing, true);
    p1.setBrush(color);
    p1.setPen(palette().color(QPalette::Active, foregroundRole()));
    p1.drawRect(rect);
    p1.end();

    return pixmap;
}

QHash<LabelsTreeView::Labels, QList<int> > LabelsTreeView::selectedLabels()
{
    QHash<Labels, QList<int> > selectedLabelsHash;
    QList<int> selectedRatings;
    QList<int> selectedPicks;
    QList<int> selectedColors;

    if (d->isCheckableTreeView)
    {
        QTreeWidgetItemIterator it(this, QTreeWidgetItemIterator::Checked);

        while (*it)
        {
            QTreeWidgetItem* const item = (*it);

            if      (item->parent() == d->ratings)
            {
                selectedRatings << indexFromItem(item).row();
            }
            else if (item->parent() == d->picks)
            {
                selectedPicks << indexFromItem(item).row();
            }
            else
            {
                selectedColors << indexFromItem(item).row();
            }

            ++it;
        }
    }
    else
    {
        foreach (QTreeWidgetItem* const item, selectedItems())
        {
            if      (item->parent() == d->ratings)
            {
                selectedRatings << indexFromItem(item).row();
            }
            else if (item->parent() == d->picks)
            {
                selectedPicks << indexFromItem(item).row();
            }
            else
            {
                selectedColors << indexFromItem(item).row();
            }
        }
    }

    selectedLabelsHash[Ratings] = selectedRatings;
    selectedLabelsHash[Picks]   = selectedPicks;
    selectedLabelsHash[Colors]  = selectedColors;

    return selectedLabelsHash;
}

void LabelsTreeView::doLoadState()
{
    d->isLoadingState                = true;
    KConfigGroup configGroup         = getConfigGroup();
    const QList<int> expansion       = configGroup.readEntry(entryName(d->configExpansionEntry),       QList<int>());
    const QList<int> selectedRatings = configGroup.readEntry(entryName(d->configRatingSelectionEntry), QList<int>());
    const QList<int> selectedPicks   = configGroup.readEntry(entryName(d->configPickSelectionEntry),   QList<int>());
    const QList<int> selectedColors  = configGroup.readEntry(entryName(d->configColorSelectionEntry),  QList<int>());

    d->ratings->setExpanded(true);
    d->picks->setExpanded(true);
    d->colors->setExpanded(true);

    foreach (int parent, expansion)
    {
        switch (parent)
        {
            case 1:
                d->ratings->setExpanded(false);
                break;

            case 2:
                d->picks->setExpanded(false);
                break;

            case 3:
                d->colors->setExpanded(false);

            default:
                break;
        }
    }

    foreach (int rating, selectedRatings)
    {
        if (d->isCheckableTreeView)
        {
            d->ratings->child(rating)->setCheckState(0, Qt::Checked);
        }
        else
        {
            d->ratings->child(rating)->setSelected(true);
        }
    }

    foreach (int pick, selectedPicks)
    {
        if (d->isCheckableTreeView)
        {
            d->picks->child(pick)->setCheckState(0, Qt::Checked);
        }
        else
        {
            d->picks->child(pick)->setSelected(true);
        }
    }

    foreach (int color, selectedColors)
    {
        if (d->isCheckableTreeView)
        {
            d->colors->child(color)->setCheckState(0, Qt::Checked);
        }
        else
        {
            d->colors->child(color)->setSelected(true);
        }
    }

    d->isLoadingState = false;
}

void LabelsTreeView::doSaveState()
{
    KConfigGroup configGroup = getConfigGroup();
    QList<int> expansion;

    if (!d->ratings->isExpanded())
    {
        expansion << 1;
    }

    if (!d->picks->isExpanded())
    {
        expansion << 2;
    }

    if (!d->colors->isExpanded())
    {
        expansion << 3;
    }

    QHash<Labels, QList<int> > labels = selectedLabels();

    configGroup.writeEntry(entryName(d->configExpansionEntry),       expansion);
    configGroup.writeEntry(entryName(d->configRatingSelectionEntry), labels[Ratings]);
    configGroup.writeEntry(entryName(d->configPickSelectionEntry),   labels[Picks]);
    configGroup.writeEntry(entryName(d->configColorSelectionEntry),  labels[Colors]);
}

void LabelsTreeView::setCurrentAlbum()
{
    emit signalSetCurrentAlbum();
}

void LabelsTreeView::initTreeView()
{
    setIconSize(QSize(d->iconSizeFromSetting*5,d->iconSizeFromSetting));
    initRatingsTree();
    initPicksTree();
    initColorsTree();
    expandAll();
    setRootIsDecorated(false);
}

void LabelsTreeView::initRatingsTree()
{
    d->ratings = new QTreeWidgetItem(this);
    d->ratings->setText(0, i18nc("@item: rating tree", "Rating"));
    d->ratings->setFont(0, d->regularFont);
    d->ratings->setFlags(Qt::ItemIsEnabled);

    QTreeWidgetItem* const noRate = new QTreeWidgetItem(d->ratings);
    noRate->setText(0, i18nc("@item: rating tree", "No Rating"));
    noRate->setFont(0, d->regularFont);
    QPixmap pix2(goldenStarPixmap().size());
    pix2.fill(Qt::transparent);
    QPainter p2(&pix2);
    p2.setRenderHint(QPainter::Antialiasing, true);
    p2.setPen(palette().color(QPalette::Active, foregroundRole()));
    p2.drawPixmap(0, 0, goldenStarPixmap(false));
    noRate->setIcon(0, QIcon(pix2));
    noRate->setSizeHint(0, d->iconSize);

    for (int rate = 1 ; rate <= 5 ; ++rate)
    {
        QTreeWidgetItem* const rateWidget = new QTreeWidgetItem(d->ratings);

        QPixmap pix(goldenStarPixmap().width()*rate, goldenStarPixmap().height());
        pix.fill(Qt::transparent);
        QPainter p(&pix);
        int offset = 0;
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setPen(palette().color(QPalette::Active, foregroundRole()));

        for (int i = 0 ; i < rate ; ++i)
        {
            p.drawPixmap(offset, 0, goldenStarPixmap());
            offset += goldenStarPixmap().width();
        }

        rateWidget->setIcon(0, QIcon(pix));
        rateWidget->setSizeHint(0, d->iconSize);
    }
}

void LabelsTreeView::initPicksTree()
{
    d->picks = new QTreeWidgetItem(this);
    d->picks->setText(0, i18nc("@title: pick tree", "Pick"));
    d->picks->setFont(0, d->regularFont);
    d->picks->setFlags(Qt::ItemIsEnabled);

    QStringList pickSetNames;
    pickSetNames << i18nc("@item: pick tree", "No Pick")
                 << i18nc("@item: pick tree", "Rejected Item")
                 << i18nc("@item: pick tree", "Pending Item")
                 << i18nc("@item: pick tree", "Accepted Item");

    QStringList pickSetIcons;
    pickSetIcons << QLatin1String("flag-black")
                 << QLatin1String("flag-red")
                 << QLatin1String("flag-yellow")
                 << QLatin1String("flag-green");

    foreach (const QString& pick, pickSetNames)
    {
        QTreeWidgetItem* const pickWidgetItem = new QTreeWidgetItem(d->picks);
        pickWidgetItem->setText(0, pick);
        pickWidgetItem->setFont(0, d->regularFont);
        pickWidgetItem->setIcon(0, QIcon::fromTheme(pickSetIcons.at(pickSetNames.indexOf(pick))));
    }
}

void LabelsTreeView::initColorsTree()
{
    d->colors                      = new QTreeWidgetItem(this);
    d->colors->setText(0, i18nc("@item: color tree", "Color"));
    d->colors->setFont(0, d->regularFont);
    d->colors->setFlags(Qt::ItemIsEnabled);

    QTreeWidgetItem* const noColor = new QTreeWidgetItem(d->colors);
    noColor->setText(0, i18nc("@item: color tree", "No Color"));
    noColor->setFont(0, d->regularFont);
    noColor->setIcon(0, QIcon::fromTheme(QLatin1String("emblem-unmounted")));

    QStringList colorSet;
    colorSet << QLatin1String("red")      << QLatin1String("orange")
             << QLatin1String("yellow")   << QLatin1String("darkgreen")
             << QLatin1String("darkblue") << QLatin1String("magenta")
             << QLatin1String("darkgray") << QLatin1String("black")
             << QLatin1String("white");

    QStringList colorSetNames;
    colorSetNames << i18nc("@item: color tree", "Red")    << i18nc("@item: color tree", "Orange")
                  << i18nc("@item: color tree", "Yellow") << i18nc("@item: color tree", "Green")
                  << i18nc("@item: color tree", "Blue")   << i18nc("@item: color tree", "Magenta")
                  << i18nc("@item: color tree", "Gray")   << i18nc("@item: color tree", "Black")
                  << i18nc("@item: color tree", "White");

    foreach (const QString& color, colorSet)
    {
        QTreeWidgetItem* const colorWidgetItem = new QTreeWidgetItem(d->colors);
        colorWidgetItem->setText(0, colorSetNames.at(colorSet.indexOf(color)));
        colorWidgetItem->setFont(0, d->regularFont);
        QPixmap colorIcon                      = colorRectPixmap(QColor(color));
        colorWidgetItem->setIcon(0, QIcon(colorIcon));
        colorWidgetItem->setSizeHint(0, d->iconSize);
    }
}

void LabelsTreeView::slotSettingsChanged()
{
    if (d->iconSizeFromSetting != ApplicationSettings::instance()->getTreeViewIconSize())
    {
        d->iconSizeFromSetting = ApplicationSettings::instance()->getTreeViewIconSize();
        setIconSize(QSize(d->iconSizeFromSetting*5, d->iconSizeFromSetting));
        d->iconSize            = QSize(d->iconSizeFromSetting, d->iconSizeFromSetting);
        QTreeWidgetItemIterator it(this);

        while (*it)
        {
            (*it)->setSizeHint(0, d->iconSize);
            ++it;
        }
    }

    if (d->regularFont != ApplicationSettings::instance()->getTreeViewFont())
    {
        d->regularFont = ApplicationSettings::instance()->getTreeViewFont();
        QTreeWidgetItemIterator it(this);

        while (*it)
        {
            (*it)->setFont(0, d->regularFont);
            ++it;
        }
    }
}

void LabelsTreeView::restoreSelectionFromHistory(QHash<Labels, QList<int> > neededLabels)
{
    QTreeWidgetItemIterator it(this, QTreeWidgetItemIterator::Selected);

    while (*it)
    {
        (*it)->setSelected(false);
        ++it;
    }

    foreach (int rateItemIndex, neededLabels[Ratings])
    {
        d->ratings->child(rateItemIndex)->setSelected(true);
    }

    foreach (int pickItemIndex, neededLabels[Picks])
    {
        d->picks->child(pickItemIndex)->setSelected(true);
    }

    foreach (int colorItemIndex, neededLabels[Colors])
    {
        d->colors->child(colorItemIndex)->setSelected(true);
    }
}

} // namespace Digikam
