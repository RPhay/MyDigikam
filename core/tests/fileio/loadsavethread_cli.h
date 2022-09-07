/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-23
 * Description : a command line tool to test DImg image loader
 *
 * Copyright (C) 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_LOAD_SAVE_THREAD_CLI_H
#define DIGIKAM_LOAD_SAVE_THREAD_CLI_H

// Qt includes

#include <QString>
#include <QApplication>

// Local includes

#include "dimg.h"
#include "loadingdescription.h"
#include "loadsavethread.h"

using namespace Digikam;

class LoadSaveThreadTest : public QApplication
{
    Q_OBJECT

public:

    explicit LoadSaveThreadTest(int& argc, char** argv);

private Q_SLOTS:

    void slotImageLoaded(const LoadingDescription&, const DImg&);
    void slotImageSaved(const QString&, bool);
    void slotLoadingProgress(const LoadingDescription&, float);
    void slotSavingProgress(const QString&, float);

private:

    LoadSaveThread* m_thread;
};

#endif // DIGIKAM_LOAD_SAVE_THREAD_CLI_H
