/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a plugin to export to jAlbum gallery generator
 *
 * SPDX-FileCopyrightText: 2018-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "jalbumplugin.h"

// Qt includes

#include <QPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "jalbumwizard.h"

namespace DigikamGenericJAlbumPlugin
{

JAlbumPlugin::JAlbumPlugin(QObject* const parent)
    : DPluginGeneric(parent)
{
}

QString JAlbumPlugin::name() const
{
    return i18n("jAlbum Export");
}

QString JAlbumPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon JAlbumPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("text-html"));
}

QString JAlbumPlugin::description() const
{
    return i18n("A tool to export images to jAlbum gallery generator");
}

QString JAlbumPlugin::details() const
{
    return i18n("<p>This tool allows users to export items to jAlbum html gallery generator.</p>"
                "<p>Items to process can be selected one by one or by group through a selection of albums.</p>"
                "<p>jAlbum is themable with different templates and layout. See the jAlbum web-site for details: https://jalbum.net/.</p>");
}

QString JAlbumPlugin::handbookSection() const
{
    return QLatin1String("post_processing");
}

QString JAlbumPlugin::handbookChapter() const
{
    return QLatin1String("jalbum_tool");
}

QList<DPluginAuthor> JAlbumPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Andrew Goodbody"),
                             QString::fromUtf8("ajg zero two at elfringham dot co dot uk"),
                             QString::fromUtf8("(c) 2013-2020"),
                             i18n("Author and Maintainer"))
            ;
}

void JAlbumPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Create jAlbum Gallery..."));
    ac->setObjectName(QLatin1String("jalbum"));
    ac->setActionCategory(DPluginAction::GenericTool);

    connect(ac, SIGNAL(triggered(bool)),
            this, SLOT(slotJAlbum()));

    addAction(ac);
}

void JAlbumPlugin::slotJAlbum()
{
    QPointer<JAlbumWizard> wzrd = new JAlbumWizard(nullptr, infoIface(sender()));
    wzrd->setPlugin(this);
    wzrd->exec();
    delete wzrd;
}

} // namespace DigikamGenericJAlbumPlugin

#include "moc_jalbumplugin.cpp"
