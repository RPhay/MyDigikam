/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef QTAV_EXAMPLE_VIDEO_GROUP_H
#define QTAV_EXAMPLE_VIDEO_GROUP_H

// Qt includes

#include <QList>
#include <QPushButton>

// Local includes

#include "QtAV.h"
#include "VideoRenderer.h"

class VideoGroup : public QObject
{
    Q_OBJECT

public:

    explicit VideoGroup(QObject* const parent = nullptr);
    ~VideoGroup();

    void setVideoRendererTypeString(const QString& vt);
    void setRows(int n);
    void setCols(int n);
    int rows() const;
    int cols() const;
    void show();
    void play(const QString& file);
    void updateROI();

public Q_SLOTS:

    void setSingleWindow(bool s);
    void toggleSingleFrame(bool s);
    void toggleFrameless(bool f);
    void openLocalFile();
    void addRenderer();
    void removeRenderer();

private:

    bool                        m1Window    = false;
    bool                        m1Frame     = true;
    bool                        mFrameless  = false;
    int                         r           = 3;
    int                         c           = 3;
    int                         timer_id    = 0;
    QtAV::AVPlayerCore*         mpPlayer    = nullptr;
    QWidget*                    view        = nullptr;
    QWidget*                    mpBar       = nullptr;
    QPushButton*                mpAdd       = nullptr;
    QPushButton*                mpRemove    = nullptr;
    QPushButton*                mpOpen      = nullptr;
    QPushButton*                mpPlay      = nullptr;
    QPushButton*                mpStop      = nullptr;
    QPushButton*                mpPause     = nullptr;
    QPushButton*                mp1Window   = nullptr;
    QPushButton*                mp1Frame    = nullptr;
    QPushButton*                mpFrameless = nullptr;
    QString                     vid         = QString::fromLatin1("qpainter");
    QList<QtAV::VideoRenderer*> mRenderers;
};

#endif // QTAV_EXAMPLE_VIDEO_GROUP_H
