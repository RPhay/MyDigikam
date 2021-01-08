/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-05
 * Description : Labels sidebar widgets
 *
 * Copyright (C) 2009-2010 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_LABELS_SIDE_BAR_WIDGET_H
#define DIGIKAM_LABELS_SIDE_BAR_WIDGET_H

// Local includes

#include "album.h"
#include "sidebarwidget.h"
#include "labelstreeview.h"

namespace Digikam
{

class LabelsSideBarWidget : public SidebarWidget
{
    Q_OBJECT

public:

    explicit LabelsSideBarWidget(QWidget* const parent);
    ~LabelsSideBarWidget() override;

    LabelsTreeView* labelsTree();

    void          setActive(bool active)                             override;
    void          applySettings()                                    override;
    void          changeAlbumFromHistory(const QList<Album*>& album) override;
    void          doLoadState()                                      override;
    void          doSaveState()                                      override;
    const QIcon   getIcon()                                          override;
    const QString getCaption()                                       override;

    QHash<LabelsTreeView::Labels, QList<int> > selectedLabels();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_LABELS_SIDE_BAR_WIDGET_H
