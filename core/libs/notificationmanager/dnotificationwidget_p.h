/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-07-03
 * Description : A widget to provide feedback or propose opportunistic interactions
 *
 * SPDX-FileCopyrightText: 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2011      by Aurelien Gateau <agateau at kde dot org>
 * SPDX-FileCopyrightText: 2014      by Dominik Haumann <dhaumann at kde dot org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DNOTIFICATION_WIDGET_PRIVATE_H
#define DIGIKAM_DNOTIFICATION_WIDGET_PRIVATE_H

// Qt includes

#include <QFrame>
#include <QObject>
#include <QLabel>
#include <QToolButton>
#include <QTimeLine>
#include <QTimer>
#include <QPixmap>

// Local includes

#include "dnotificationwidget.h"

namespace Digikam
{

class Q_DECL_HIDDEN DNotificationWidget::Private : public QObject
{
    Q_OBJECT

public:

    explicit Private(DNotificationWidget* const);
    ~Private() override;

    void init();
    void createLayout();
    void updateSnapShot();
    void updateLayout();
    int  bestContentHeight() const;

public:

    DNotificationWidget*             q;
    QFrame*                          content;
    QLabel*                          iconLabel;
    QLabel*                          textLabel;
    QToolButton*                     closeButton;
    QTimeLine*                       timeLine;
    QTimer*                          timer;
    QIcon                            icon;
    QString                          text;

    DNotificationWidget::MessageType messageType;
    bool                             wordWrap;
    QList<QToolButton*>              buttons;
    QPixmap                          contentSnapShot;
    int                              delay;

private Q_SLOTS:

    void slotTimeLineChanged(qreal);
    void slotTimeLineFinished();
};

} // namespace Digikam

#endif // DIGIKAM_DNOTIFICATION_WIDGET_PRIVATE_H
