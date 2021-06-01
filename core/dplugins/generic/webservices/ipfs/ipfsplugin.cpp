/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a plugin to export to IPFS web-service.
 *
 * Copyright (C) 2018-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "ipfsplugin.h"

// Qt includes

#include <QPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "ipfswindow.h"

namespace DigikamGenericIpfsPlugin
{

IpfsPlugin::IpfsPlugin(QObject* const parent)
    : DPluginGeneric(parent)
{
}

IpfsPlugin::~IpfsPlugin()
{
}

void IpfsPlugin::cleanUp()
{
    delete m_toolDlg;
}

QString IpfsPlugin::name() const
{
    return i18nc("@title", "IPFS");
}

QString IpfsPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon IpfsPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("dk-ipfs"));
}

QString IpfsPlugin::description() const
{
    return i18nc("@info", "A tool to export to IPFS web-service");
}

QString IpfsPlugin::details() const
{
    return i18nc("@info", "This tool allow to export items to IPFS web-service.\n\n"
                 "See IPFS web site for details: %1",
                 QLatin1String("<a href='https://ipfs.io/'>https://ipfs.io/</a>"));
}

QList<DPluginAuthor> IpfsPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Amar Lakshya"),
                             QString::fromUtf8("amar dot lakshya at xaviers dot edu dot in"),
                             QString::fromUtf8("(C) 2018"))
            ;
}

void IpfsPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Export to &Ipfs..."));
    ac->setObjectName(QLatin1String("export_ipfs"));
    ac->setActionCategory(DPluginAction::GenericExport);

    connect(ac, SIGNAL(triggered(bool)),
            this, SLOT(slotIpfs()));

    addAction(ac);
}

void IpfsPlugin::slotIpfs()
{
    if (!reactivateToolDialog(m_toolDlg))
    {
        delete m_toolDlg;
        m_toolDlg = new IpfsWindow(infoIface(sender()), nullptr);
        m_toolDlg->setPlugin(this);
        m_toolDlg->show();
    }
}

} // namespace DigikamGenericIpfsPlugin
