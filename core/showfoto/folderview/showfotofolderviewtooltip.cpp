/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : Tool tip for Showfoto folder-view item.
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "showfotofolderviewtooltip.h"

// Qt includes

#include <QDateTime>
#include <QPainter>
#include <QScopedPointer>
#include <QPixmap>
#include <QFileInfo>
#include <QTextDocument>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "dmetadata.h"
#include "showfotosettings.h"
#include "showfotoiteminfo.h"
#include "showfotofolderviewlist.h"
#include "showfotofolderviewmodel.h"
#include "showfototooltipfiller.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoFolderViewToolTip::Private
{
public:

    explicit Private()
      : view        (nullptr)
    {
    }

    ShowfotoFolderViewList* view;
    QModelIndex             index;
};

ShowfotoFolderViewToolTip::ShowfotoFolderViewToolTip(ShowfotoFolderViewList* const view)
    : DItemToolTip(),
      d           (new Private)
{
    d->view = view;
}

ShowfotoFolderViewToolTip::~ShowfotoFolderViewToolTip()
{
    delete d;
}

void ShowfotoFolderViewToolTip::setIndex(const QModelIndex& index)
{
    d->index = index;

    if (!d->index.isValid())
//     || !ShowfotoSettings::instance()->showToolTipsIsValid())
    {
        hide();
    }
    else
    {
        updateToolTip();
        reposition();

        if (isHidden() && !toolTipIsEmpty())
        {
            show();
        }
    }
}

QRect ShowfotoFolderViewToolTip::repositionRect()
{
    if (!d->index.isValid())
    {
        return QRect();
    }

    QRect rect = d->view->visualRect(d->index);
    rect.moveTopLeft(d->view->viewport()->mapToGlobal(rect.topLeft()));

    return rect;
}

QString ShowfotoFolderViewToolTip::tipContents()
{
    if (!d->index.isValid())
    {
        return QString();
    }

    ShowfotoFolderViewModel* const model = dynamic_cast<ShowfotoFolderViewModel*>(d->view->model());

    if (!model)
    {
        return QString();
    }

    QString path                         = model->filePath(d->index);

    ShowfotoItemInfo iteminfo;
    QScopedPointer<DMetadata> meta(new DMetadata);

    QFileInfo fi(path);
    iteminfo.name      = fi.fileName();
    iteminfo.mime      = fi.suffix();
    iteminfo.size      = fi.size();
    iteminfo.folder    = fi.path();
    iteminfo.url       = QUrl::fromLocalFile(fi.filePath());

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))

    iteminfo.dtime     = fi.birthTime();

#else

    iteminfo.dtime     = fi.created();

#endif

    meta->load(fi.filePath());
    iteminfo.ctime     = meta->getItemDateTime();
    iteminfo.width     = meta->getItemDimensions().width();
    iteminfo.height    = meta->getItemDimensions().height();
    iteminfo.photoInfo = meta->getPhotographInformation();

    return ShowfotoToolTipFiller::ShowfotoItemInfoTipContents(iteminfo);
}

} // namespace ShowFoto
