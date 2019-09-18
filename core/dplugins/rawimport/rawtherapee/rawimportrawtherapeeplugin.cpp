/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-18
 * Description : RawTherapee raw import plugin.
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

#include "rawimportrawtherapeeplugin.h"

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

namespace DigikamRawImportRawTherapeePlugin
{

RawTherapeeRawImportPlugin::RawTherapeeRawImportPlugin(QObject* const parent)
    : DPluginRawImport(parent),
      m_rawtherapee(nullptr),
      m_tempFile(nullptr)
{
}

RawTherapeeRawImportPlugin::~RawTherapeeRawImportPlugin()
{
}

QString RawTherapeeRawImportPlugin::name() const
{
    return QString::fromUtf8("Raw Import using RawTherapee");
}

QString RawTherapeeRawImportPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon RawTherapeeRawImportPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("image-x-adobe-dng"));
}

QString RawTherapeeRawImportPlugin::description() const
{
    return QString::fromUtf8("A RAW import plugin based on RawTherapee");
}

QString RawTherapeeRawImportPlugin::details() const
{
    return QString::fromUtf8("<p>This RAW Import plugin use RawTherapee tool to pre-process file in Image Editor.</p>");
}

QList<DPluginAuthor> RawTherapeeRawImportPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2019"))
            ;
}

void RawTherapeeRawImportPlugin::setup(QObject* const /*parent*/)
{
    // Nothing to do
}

bool RawTherapeeRawImportPlugin::run(const QString& filePath, const DRawDecoding& /*def*/)
{
    m_fileInfo = QFileInfo(filePath);
    m_props    = LoadingDescription(m_fileInfo.filePath(), LoadingDescription::ConvertForEditor);
    m_decoded  = DImg();

    delete m_tempFile;

    m_tempFile = new QTemporaryFile();
    m_tempFile->open();

    m_rawtherapee    = new QProcess(this);
    m_rawtherapee->setProcessChannelMode(QProcess::MergedChannels);
    m_rawtherapee->setWorkingDirectory(m_fileInfo.path());

    connect(m_rawtherapee, SIGNAL(errorOccurred(QProcess::ProcessError)),
            this, SLOT(slotErrorOccurred(QProcess::ProcessError)));

    connect(m_rawtherapee, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(slotProcessFinished(int,QProcess::ExitStatus)));

    connect(m_rawtherapee, SIGNAL(readyRead()),
            this, SLOT(slotProcessReadyRead()));

    // --------

    m_fileInfo = QFileInfo(filePath);

    m_rawtherapee->setProgram(QLatin1String("rawtherapee"));
    m_rawtherapee->setArguments(QStringList() << QLatin1String("-gimp")   // Special mode used initialy as Gimp plugin
                                              << filePath                 // Input file
                                              << m_tempFile->fileName()); // Output file

    qCDebug(DIGIKAM_GENERAL_LOG) << "RawTherapee arguments:" << m_rawtherapee->arguments();

    m_rawtherapee->start();

    return true;
}

void RawTherapeeRawImportPlugin::slotErrorOccurred(QProcess::ProcessError error)
{
    switch (error)
    {
        case QProcess::FailedToStart:
            qCDebug(DIGIKAM_GENERAL_LOG) << "RawTherapee :: Process has failed to start";
            break;
        case QProcess::Crashed:
            qCDebug(DIGIKAM_GENERAL_LOG) << "RawTherapee :: Process has crashed";
            break;
        case QProcess::Timedout:
            qCDebug(DIGIKAM_GENERAL_LOG) << "RawTherapee :: Process time-out";
            break;
        case QProcess::WriteError:
            qCDebug(DIGIKAM_GENERAL_LOG) << "RawTherapee :: Process write error";
            break;
        case QProcess::ReadError:
            qCDebug(DIGIKAM_GENERAL_LOG) << "RawTherapee :: Process read error";
            break;
        default:
            qCDebug(DIGIKAM_GENERAL_LOG) << "RawTherapee :: Process error unknown";
            break;
    }
}

void RawTherapeeRawImportPlugin::slotProcessFinished(int code, QProcess::ExitStatus status)
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "RawTherapee :: return code:" << code << ":: Exit status:" << status;

    m_decoded = DImg(m_tempFile->fileName());

    if (m_decoded.isNull())
    {
        QString message = i18n("Error to import RAW image with RawTherapee\nClose this dialog to load RAW image with native import tool");
        QMessageBox::information(0, qApp->applicationName(), message);

        qCDebug(DIGIKAM_GENERAL_LOG) << "Decoded image is null! Load with Native tool...";
        qCDebug(DIGIKAM_GENERAL_LOG) << m_props.filePath;
        emit signalLoadRaw(m_props);
    }
    else
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Decoded image is not null...";
        qCDebug(DIGIKAM_GENERAL_LOG) << m_props.filePath;
        m_props = LoadingDescription(m_tempFile->fileName(), LoadingDescription::ConvertForEditor);
        emit signalDecodedImage(m_props, m_decoded);
    }

    delete m_tempFile;
    m_tempFile = nullptr;
}

void RawTherapeeRawImportPlugin::slotProcessReadyRead()
{
    QByteArray data   = m_rawtherapee->readAllStandardError();
    QStringList lines = QString::fromUtf8(data).split(QLatin1Char('\n'), QString::SkipEmptyParts);

    foreach (const QString& one, lines)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "RawTherapee ::" << one;
    }
}

} // namespace DigikamRawImportRawTherapeePlugin
