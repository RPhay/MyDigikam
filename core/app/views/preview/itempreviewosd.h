/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2025-12-25
 * Description : On Screen Display preview overlay.
 *
 * SPDX-FileCopyrightText: 2014-2026 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

#include <QWidget>
#include <QPainter>
#include <QString>
#include <QColor>
#include <QUrl>

// Local includes

#include "iteminfo.h"

namespace Digikam
{

class ItemPreviewOsd : public QWidget
{
    Q_OBJECT

public:

    explicit ItemPreviewOsd(QWidget* const parent);
    ~ItemPreviewOsd()                                override;

    void setItemInfo(const ItemInfo& inf);
    void setOsdEnabled(bool val);

private:

    void printInfoText(QPainter& p, int& offset, const QString& str, const QColor& pcol = Qt::white);
    void printComments(QPainter& p, int& offset, const QString& comments);
    void printTags(QPainter& p, int& offset, QStringList& tags);
    void paintEvent(QPaintEvent*)                    override;

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
