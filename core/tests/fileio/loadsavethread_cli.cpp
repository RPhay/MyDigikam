/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-23
 * Description : a command line tool to test DImg image loader
 *
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "loadsavethread_cli.h"

// Qt includes

#include <QFileInfo>

// Local includes

#include "digikam_debug.h"
#include "metaengine.h"
#include "dimg.h"
#include "drawdecoding.h"

LoadSaveThreadTest::LoadSaveThreadTest(int& argc, char** argv)
    : QApplication(argc, argv)
{
    qRegisterMetaType<LoadingDescription>("LoadingDescription");
    qRegisterMetaType<DImg>("DImg");

    m_thread = new LoadSaveThread;

    connect( m_thread, SIGNAL(signalImageLoaded(LoadingDescription,DImg)),
             this, SLOT(slotImageLoaded(LoadingDescription,DImg)) );

    connect( m_thread, SIGNAL(signalImageSaved(QString,bool)),
             this, SLOT(slotImageSaved(QString,bool)) );

    connect( m_thread, SIGNAL(signalLoadingProgress(LoadingDescription,float)),
             this, SLOT(slotLoadingProgress(LoadingDescription,float)) );

    connect( m_thread, SIGNAL(signalSavingProgress(QString,float)),
             this, SLOT(slotSavingProgress(QString,float)) );

    DRawDecoderSettings settings;
    settings.halfSizeColorImage    = false;
    settings.sixteenBitsImage      = false;
    settings.RGBInterpolate4Colors = false;
    settings.RAWQuality            = DRawDecoderSettings::BILINEAR;

    LoadingDescription desc(QString::fromUtf8(argv[1]), DRawDecoding(settings));

    m_thread->load(desc);
}

void LoadSaveThreadTest::slotLoadingProgress(const LoadingDescription& desc, float p)
{
    QFileInfo fi(desc.filePath);
    qCDebug(DIGIKAM_TESTS_LOG) << "Loading " << fi.baseName() << " : " << p << " %";
}

void LoadSaveThreadTest::slotImageLoaded(const LoadingDescription& desc, const DImg& img)
{
    QFileInfo fi(desc.filePath);
    qCDebug(DIGIKAM_TESTS_LOG) << "Image " << fi.baseName() << " loaded";

    QString outFilePath(fi.baseName() + QString::fromUtf8(".out.png"));
    DImg image = img;
    m_thread->save(image, outFilePath, QLatin1String("PNG"));
}

void LoadSaveThreadTest::slotSavingProgress(const QString& filePath, float p)
{
    QFileInfo fi(filePath);
    qCDebug(DIGIKAM_TESTS_LOG) << "Saving " << fi.baseName() << " : " << p << " %";
}

void LoadSaveThreadTest::slotImageSaved(const QString& filePath, bool b)
{
    QFileInfo fi(filePath);
    qCDebug(DIGIKAM_TESTS_LOG) << fi.baseName() << " saved : " << (b ? "ok" : "pb");

    exit();
}

// ------------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "loadsavethreadtest - test DImg image loader with multithreading";
        qCDebug(DIGIKAM_TESTS_LOG) << "Usage: <image>";
        return -1;
    }

    MetaEngine::initializeExiv2();

    LoadSaveThreadTest app(argc, argv);
    int ret = app.exec();

    return ret;
}
