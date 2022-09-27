/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-02-09
 * Description : color label filter
 *
 * SPDX-FileCopyrightText: 2011-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "colorlabelfilter.h"

// Local includes

#include "digikam_debug.h"
#include "albummanager.h"
#include "tagscache.h"

namespace Digikam
{

ColorLabelFilter::ColorLabelFilter(QWidget* const parent)
    : ColorLabelWidget(parent)
{
    setDescriptionBoxVisible(false);
    setButtonsExclusive(false);
    reset();

    connect(this, SIGNAL(signalColorLabelChanged(int)),
            this, SLOT(slotColorLabelSelectionChanged()));
}

ColorLabelFilter::~ColorLabelFilter()
{
}

void ColorLabelFilter::reset()
{
    setColorLabels(QList<ColorLabel>());
    slotColorLabelSelectionChanged();
}

QList<TAlbum*> ColorLabelFilter::getCheckedColorLabelTags()
{
    QList<TAlbum*> list;
    int tagId   = 0;
    TAlbum* tag = nullptr;

    Q_FOREACH (const ColorLabel& cl, colorLabels())
    {
        tagId = TagsCache::instance()->tagForColorLabel(cl);
        tag   = AlbumManager::instance()->findTAlbum(tagId);

        if (tagId)
        {
            list.append(tag);
        }
    }

    return list;
}

void ColorLabelFilter::slotColorLabelSelectionChanged()
{
    Q_EMIT signalColorLabelSelectionChanged(colorLabels());
}

} // namespace Digikam
