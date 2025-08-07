/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-07-11
 * Description : shared libraries list dialog common to digiKam and Showfoto
 *
 * SPDX-FileCopyrightText: 2008-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "libsinfodlg_p.h"

namespace Digikam
{

void LibsInfoDlg::populateFeatures()
{
    // NOTE: by default set a list of common components information used by Showfoto and digiKam.

    m_features = new QTreeWidgetItem(listView(), QStringList() << i18nc("@title", "Features"));
    listView()->addTopLevelItem(m_features);

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Parallelized demosaicing") <<        d->checkTriState(DRawDecoder::librawUseGomp()));

#ifdef HAVE_QTXMLPATTERNS

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Rajce support") <<                   Private::SUPPORTED_YES);

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Rajce support") <<                   Private::SUPPORTED_NO);

#endif

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Exiv2 supports XMP metadata") <<     (MetaEngine::supportXmp() ?  Private::SUPPORTED_YES : Private::SUPPORTED_NO));

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Exiv2 supports Base Media") <<       (MetaEngine::supportBmff() ? Private::SUPPORTED_YES : Private::SUPPORTED_NO));

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Exiv2 supports JPEG-XL metadata") << (MetaEngine::supportJpegXL() ? Private::SUPPORTED_YES : Private::SUPPORTED_NO));

#ifdef HAVE_QMULTIMEDIA

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Qt Multimedia support") <<          Private::SUPPORTED_YES);

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Qt Multimedia support") <<          Private::SUPPORTED_YES);

#endif

#ifdef HAVE_SONNET

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Spell-Checking support") <<         Private::SUPPORTED_YES);

#else

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Spell-Checking support") <<         Private::SUPPORTED_NO);

#endif

    int nbcore         = QThread::idealThreadCount();
    new QTreeWidgetItem(m_features, QStringList() <<
                        i18ncp(Private::CONTEXT, "CPU core", "CPU cores", nbcore) << QString::fromLatin1("%1").arg(nbcore));

    KMemoryInfo memInfo;

    if (!memInfo.isNull())
    {
        quint64 available = memInfo.totalPhysical();

        if (available > 0)
        {
            new QTreeWidgetItem(m_features, QStringList() <<
                                i18nc(Private::CONTEXT, "Memory available") << ItemPropertiesTab::humanReadableBytesCount(available));
        }
        else
        {
            new QTreeWidgetItem(m_features, QStringList() <<
                                i18nc(Private::CONTEXT, "Memory available") << i18nc("@item: information about memory", "Unknown"));
        }
    }

    quint64 cacheSize = LoadingCache::cache()->getCacheSize();

    new QTreeWidgetItem(m_features, QStringList() <<
                        i18nc(Private::CONTEXT, "Image cache size") << ItemPropertiesTab::humanReadableBytesCount(cacheSize));
}

} // namespace Digikam
