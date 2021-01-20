/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-04-02
 * Description : plugin to export image as wallpaper.
 *
 * Copyright (C) 2019      by Igor Antropov <antropovi at yahoo dot com>
 * Copyright (C) 2019-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "digikam_config.h"
#include "wallpaperplugin.h"

// Qt includes

#include <QString>
#include <QMessageBox>
#include <QProcess>

#ifdef HAVE_DBUS
#   include <QDBusMessage>
#   include <QDBusConnection>
#endif

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"

// Windows includes

#ifdef Q_OS_WIN
#   include <windows.h>
#   include <shlobj.h>
#endif

namespace DigikamGenericWallpaperPlugin
{

WallpaperPlugin::WallpaperPlugin(QObject* const parent)
    : DPluginGeneric(parent)
{
}

WallpaperPlugin::~WallpaperPlugin()
{
}

QString WallpaperPlugin::name() const
{
    return i18n("Export as wallpaper");
}

QString WallpaperPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon WallpaperPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("preferences-desktop-wallpaper"));
}

QString WallpaperPlugin::description() const
{
    return i18n("A tool to set image as wallpaper");
}

QString WallpaperPlugin::details() const
{
    return i18n("<p>This tool changes background wallpaper to selected image for all desktops.</p>"
                "<p>If many images are selected, the first one will be used.</p>"
                "<p>If no image is selected, the first one from current album will be used.</p>");
}

QList<DPluginAuthor> WallpaperPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Igor Antropov"),
                             QString::fromUtf8("antropovi at yahoo dot com"),
                             QString::fromUtf8("(C) 2019"));
}

void WallpaperPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Set as wallpaper"));
    ac->setObjectName(QLatin1String("Wallpaper"));
    ac->setActionCategory(DPluginAction::GenericTool);

    connect(ac, SIGNAL(triggered(bool)),
            this, SLOT(slotWallpaper()));

    addAction(ac);
}

void WallpaperPlugin::slotWallpaper()
{
    DInfoInterface* const iface = infoIface(sender());
    QList<QUrl> images          = iface->currentSelectedItems();

    if (images.isEmpty())
    {
        images = iface->currentAlbumItems();
    }

    if (!images.isEmpty())
    {

#if defined Q_OS_MACOS

        QStringList args;
        args << QLatin1String("-e");
        args << QLatin1String("tell application \"System Events\"");
        args << QLatin1String("-e");
        args << QLatin1String("tell current desktop");
        args << QLatin1String("-e");
        args << QString::fromUtf8("set picture to POSIX file \"%1\"").arg(images[0].toString());
        args << QLatin1String("-e");
        args << QLatin1String("end tell");
        args << QLatin1String("-e");
        args << QLatin1String("end tell");
        args << QLatin1String("-e");
        args << QLatin1String("return");

        if (QProcess::execute(QLatin1String("/usr/bin/osascript"), args) == 0)
        {
            QMessageBox::warning(nullptr,
                                 i18nc("@title:window",
                                       "Error while to set image as wallpaper"),
                                 i18n("Cannot change wallpaper image from current desktop with\n%1",
                                      images[0].toString()));
        }

#elif defined Q_OS_WIN

        // NOTE: IDesktopWallpaper is only defined with Windows >= 8.
        //       To be compatible with Windows 7, we needs to use IActiveDesktop instead.

        wchar_t path[MAX_PATH];
        QString pathStr           = images[0].toString().replace(L'/', L'\\');

        if (pathStr.size() > MAX_PATH - 1)
        {
            qCWarning(DIGIKAM_GENERAL_LOG) << "Image path to set as wall paper is too long...";
            return;
        }

        int pathLen               = pathStr.toWCharArray(path);
        path[pathLen]             = L'\0'; // toWCharArray doesn't add NULL terminator

        int    nStyle             = 0;

        CoInitializeEx(0, COINIT_APARTMENTTHREADED);

        IActiveDesktop* iADesktop = nullptr;
        HRESULT status            = CoCreateInstance(CLSID_ActiveDesktop, NULL, CLSCTX_INPROC_SERVER, IID_IActiveDesktop, (void**)&iADesktop);
        WALLPAPEROPT wOption;
        ZeroMemory(&wOption, sizeof(WALLPAPEROPT));
        wOption.dwSize            = sizeof(WALLPAPEROPT);

        switch (nStyle)
        {
            case 1:
            {
                wOption.dwStyle = WPSTYLE_TILE;
                break;
            }

            case 2:
            {
                wOption.dwStyle = WPSTYLE_CENTER;
                break;
            }

            default:
            {
                wOption.dwStyle = WPSTYLE_STRETCH;
                break;
            }
        }

        status = iADesktop->SetWallpaper(path, 0);
        status = iADesktop->SetWallpaperOptions(&wOption, 0);
        status = iADesktop->ApplyChanges(AD_APPLY_ALL);

        iADesktop->Release();
        CoUninitialize();

/*
        HRESULT hr                           = CoInitialize(nullptr);
        IDesktopWallpaper* pDesktopWallpaper = nullptr;
        hr                                   = CoCreateInstance(__uuidof(DesktopWallpaper),
                                                                nullptr,
                                                                CLSCTX_ALL,
                                                                IID_PPV_ARGS(&pDesktopWallpaper));
        if (FAILED(hr))
        {
            QMessageBox::warning(nullptr,
                                 i18nc("@title:window",
                                       "Error while to set image as wallpaper"),
                                 i18n("Cannot change wallpaper image from current desktop\n%1",
                                      images[0].toString()));
        }
        else
        {
            pDesktopWallpaper->SetWallpaper(nullptr,
                                            images[0].toString().toStdWString().c_str());
        }

        CoUninitialize();
*/

#elif defined HAVE_DBUS

        QDBusMessage message = QDBusMessage::createMethodCall(
            QLatin1String("org.kde.plasmashell"),
            QLatin1String("/PlasmaShell"),
            QLatin1String("org.kde.PlasmaShell"),
            QLatin1String("evaluateScript"));

        message << QString::fromUtf8
        (
            "var allDesktops = desktops();"
            "for (i=0;i<allDesktops.length;i++)"
            "{"
                "d = allDesktops[i];"
                "d.wallpaperPlugin = \"org.kde.image\";"
                "d.currentConfigGroup = Array(\"Wallpaper\", \"org.kde.image\", \"General\");"
                "d.writeConfig(\"Image\", \"%1\")"
            "}"
        ).arg(images[0].toString());

        QDBusMessage reply = QDBusConnection::sessionBus().call(message);

        if (reply.type() == QDBusMessage::ErrorMessage)
        {
            QMessageBox::warning(nullptr,
                                 i18nc("@title:window",
                                       "Error while to set image as wallpaper"),
                                 i18n("Cannot change wallpaper image from current desktop\n%1\n\n%2",
                                      images[0].toString(),
                                      reply.errorMessage()));
        }

#endif

    }
}

} // namespace DigikamGenericWallpaperPlugin
