/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-18
 * Description : UFRaw raw import plugin.
 *
 * Copyright (C) 2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "rawimportufrawplugin.h"

// Qt includes

#include <QMessageBox>
#include <QApplication>
#include <QPointer>
#include <QDebug>
#include <QByteArray>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"

namespace DigikamRawImportUFRawPlugin
{

UFRawRawImportPlugin::UFRawRawImportPlugin(QObject* const parent)
    : DPluginRawImport(parent),
      m_ufraw(nullptr),
      m_tempFile(nullptr)
{
}

UFRawRawImportPlugin::~UFRawRawImportPlugin()
{
}

QString UFRawRawImportPlugin::name() const
{
    return QString::fromUtf8("Raw Import using UFRaw");
}

QString UFRawRawImportPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon UFRawRawImportPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("image-x-adobe-dng"));
}

QString UFRawRawImportPlugin::description() const
{
    return QString::fromUtf8("A RAW import plugin based on UFRaw");
}

QString UFRawRawImportPlugin::details() const
{
    return QString::fromUtf8("<p>This RAW Import plugin use UFRaw tool to pre-process file in Image Editor.</p>");
}

QList<DPluginAuthor> UFRawRawImportPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2019"))
            ;
}

void UFRawRawImportPlugin::setup(QObject* const /*parent*/)
{
    // Nothing to do
}

bool UFRawRawImportPlugin::run(const QString& filePath, const DRawDecoding& /*def*/)
{
    m_fileInfo = QFileInfo(filePath);
    m_props    = LoadingDescription(m_fileInfo.filePath(), LoadingDescription::ConvertForEditor);
    m_decoded  = DImg();

    delete m_tempFile;

    m_tempFile = new QTemporaryFile();
    m_tempFile->open();

    m_ufraw    = new QProcess(this);
    m_ufraw->setProcessChannelMode(QProcess::MergedChannels);
    m_ufraw->setWorkingDirectory(m_fileInfo.path());
    m_ufraw->setStandardOutputFile(m_tempFile->fileName());

    connect(m_ufraw, SIGNAL(errorOccurred(QProcess::ProcessError)),
            this, SLOT(slotErrorOccurred(QProcess::ProcessError)));

    connect(m_ufraw, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(slotProcessFinished(int,QProcess::ExitStatus)));

    connect(m_ufraw, SIGNAL(readyRead()),
            this, SLOT(slotProcessReadyRead()));

    // --------

    m_fileInfo = QFileInfo(filePath);

    m_ufraw->setProgram(QLatin1String("ufraw"));
    m_ufraw->setArguments(QStringList() << QLatin1String("--out-depth=16")                               // 16 bits per color per pixels
                                        << QLatin1String("--out-type=tif")                               // TIFF output
                                        << QLatin1String("--overwrite")                                  // Overwrite target temporay file
                                        << QString::fromUtf8("--output=%1").arg(m_tempFile->fileName())  // output file
                                        << filePath);

    qCDebug(DIGIKAM_GENERAL_LOG) << "UFRaw arguments:" << m_ufraw->arguments();

    m_ufraw->start();

    return true;
}

void UFRawRawImportPlugin::slotErrorOccurred(QProcess::ProcessError error)
{
    switch (error)
    {
        case QProcess::FailedToStart:
            qCDebug(DIGIKAM_GENERAL_LOG) << "UFRaw :: Process has failed to start";
            break;
        case QProcess::Crashed:
            qCDebug(DIGIKAM_GENERAL_LOG) << "UFRaw :: Process has crashed";
            break;
        case QProcess::Timedout:
            qCDebug(DIGIKAM_GENERAL_LOG) << "UFRaw :: Process time-out";
            break;
        case QProcess::WriteError:
            qCDebug(DIGIKAM_GENERAL_LOG) << "UFRaw :: Process write error";
            break;
        case QProcess::ReadError:
            qCDebug(DIGIKAM_GENERAL_LOG) << "UFRaw :: Process read error";
            break;
        default:
            qCDebug(DIGIKAM_GENERAL_LOG) << "UFRaw :: Process error unknown";
            break;
    }
}

void UFRawRawImportPlugin::slotProcessFinished(int code, QProcess::ExitStatus status)
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "UFRaw :: return code:" << code << ":: Exit status:" << status;

    m_props   = LoadingDescription(m_tempFile->fileName(), LoadingDescription::ConvertForEditor);
    m_decoded = DImg(m_props.filePath);

    if (m_decoded.isNull())
    {
        QString message = i18n("Error to import RAW image with UFRaw\nClose this dialog to load RAW image with native import tool");
        QMessageBox::information(0, qApp->applicationName(), message);

        qCDebug(DIGIKAM_GENERAL_LOG) << "Decoded image is null! Load with Native tool...";
        qCDebug(DIGIKAM_GENERAL_LOG) << m_props.filePath;
        emit signalLoadRaw(m_props);
    }
    else
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Decoded image is not null...";
        qCDebug(DIGIKAM_GENERAL_LOG) << m_props.filePath;
        emit signalDecodedImage(m_props, m_decoded);
    }

    delete m_tempFile;
    m_tempFile = nullptr;
}

void UFRawRawImportPlugin::slotProcessReadyRead()
{
    QByteArray data   = m_ufraw->readAllStandardError();
    QStringList lines = QString::fromUtf8(data).split(QLatin1Char('\n'), QString::SkipEmptyParts);

    foreach (const QString& one, lines)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "UFRaw ::" << one;
    }
}

} // namespace DigikamRawImportUFRawPlugin
