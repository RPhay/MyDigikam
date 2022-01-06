/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-15
 * Description : contextmenu helper class - Services methods.
 *
 * Copyright (C) 2009-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright (C) 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "contextmenuhelper_p.h"

namespace Digikam
{

void ContextMenuHelper::addServicesMenu(const QList<QUrl>& selectedItems)
{
    setSelectedItems(selectedItems);

#ifdef Q_OS_WIN

    if (selectedItems.length() == 1)
    {
        QAction* const openWith = new QAction(i18nc("@action: context menu", "Open With"), this);
        addAction(openWith);

        connect(openWith, SIGNAL(triggered()),
                this, SLOT(slotOpenWith()));
    }

#elif defined Q_OS_MAC

    QList<QUrl> appUrls = DServiceMenu::MacApplicationsForFiles(selectedItems);

    if (!appUrls.isEmpty())
    {
        QMenu* const servicesMenu    = new QMenu(d->parent);
        qDeleteAll(servicesMenu->actions());

        QAction* const serviceAction = servicesMenu->menuAction();
        serviceAction->setText(i18nc("@action: context menu", "Open With"));

        foreach (const QUrl& aurl, appUrls)
        {
            QAction* const action = servicesMenu->addAction(DServiceMenu::MacApplicationBundleName(aurl));
            action->setIcon(DServiceMenu::MacApplicationBundleIcon(aurl));
            action->setData(aurl);
        }

        addAction(serviceAction);

        connect(servicesMenu, SIGNAL(triggered(QAction*)),
                this, SLOT(slotOpenWith(QAction*)));
    }

#else // LINUX

    KService::List offers = DServiceMenu::servicesForOpenWith(selectedItems);

    if (!offers.isEmpty())
    {
        QMenu* const servicesMenu    = new QMenu(d->parent);
        qDeleteAll(servicesMenu->actions());

        QAction* const serviceAction = servicesMenu->menuAction();
        serviceAction->setText(i18nc("@action: context menu", "Open With"));

        foreach (const KService::Ptr& service, offers)
        {
            QString name          = service->name().replace(QLatin1Char('&'), QLatin1String("&&"));
            QAction* const action = servicesMenu->addAction(name);
            action->setIcon(QIcon::fromTheme(service->icon()));
            action->setData(service->name());
            d->servicesMap[name]  = service;
        }

#   ifdef HAVE_KIO

        servicesMenu->addSeparator();
        servicesMenu->addAction(i18nc("@action: open item with other application", "Other..."));

        addAction(serviceAction);

        connect(servicesMenu, SIGNAL(triggered(QAction*)),
                this, SLOT(slotOpenWith(QAction*)));
    }
    else
    {
        QAction* const serviceAction = new QAction(i18nc("@action: context menu", "Open With..."), this);
        addAction(serviceAction);

        connect(serviceAction, SIGNAL(triggered()),
                this, SLOT(slotOpenWith()));

#   endif // HAVE_KIO

    }

#endif // Q_OS_WIN

}

void ContextMenuHelper::slotOpenWith()
{
    // call the slot with an "empty" action

    slotOpenWith(nullptr);
}

void ContextMenuHelper::slotOpenWith(QAction* action)
{

#ifdef Q_OS_WIN

    Q_UNUSED(action);

    // See Bug #380065 for details.

    if (d->selectedItems.length() == 1)
    {
        SHELLEXECUTEINFO sei = {};
        sei.cbSize           = sizeof(sei);
        sei.fMask            = SEE_MASK_INVOKEIDLIST | SEE_MASK_NOASYNC;
        sei.nShow            = SW_SHOWNORMAL;
        sei.lpVerb           = (LPCWSTR)QString::fromLatin1("openas").utf16();
        sei.lpFile           = (LPCWSTR)QDir::toNativeSeparators(d->selectedItems.first().toLocalFile()).utf16();
        ShellExecuteEx(&sei);

        qCDebug(DIGIKAM_GENERAL_LOG) << "ShellExecuteEx::openas return code:" << GetLastError();
    }

#elif defined Q_OS_MAC

    QList<QUrl> list = d->selectedItems;
    QUrl aurl        = action ? action->data().toUrl() : QUrl();

    if (!aurl.isEmpty())
    {
        DServiceMenu::MacOpenFilesWithApplication(list, aurl);
    }

#else // LINUX

    KService::Ptr service;
    QList<QUrl> list = d->selectedItems;
    QString name     = action ? action->data().toString() : QString();

#   ifdef HAVE_KIO

    if (name.isEmpty())
    {
        QPointer<KOpenWithDialog> dlg = new KOpenWithDialog(list);

        if (dlg->exec() != KOpenWithDialog::Accepted)
        {
            delete dlg;
            return;
        }

        service = dlg->service();

        if (!service)
        {
            // User entered a custom command

            if (!dlg->text().isEmpty())
            {
                DServiceMenu::runFiles(dlg->text(), list);
            }

            delete dlg;
            return;
        }

        delete dlg;
    }
    else

#   endif // HAVE_KIO

    {
        service = d->servicesMap[name];
    }

    DServiceMenu::runFiles(service.data(), list);

#endif // Q_OS_WIN

}

void ContextMenuHelper::slotOpenInFileManager()
{
    DFileOperations::openInFileManager(d->selectedItems);
}

} // namespace Digikam
