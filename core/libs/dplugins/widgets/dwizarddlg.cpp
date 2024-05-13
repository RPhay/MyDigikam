/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a template to create wizard dialog.
 *
 * SPDX-FileCopyrightText: 2009-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dwizarddlg.h"

// Qt includes

#include <QAbstractButton>
#include <QApplication>
#include <QPointer>
#include <QScreen>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QIcon>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "digikam_globals.h"
#include "dxmlguiwindow.h"
#include "dpluginaboutdlg.h"

namespace Digikam
{

DWizardDlg::DWizardDlg(QWidget* const parent, const QString& objName)
    : QWizard(parent)
{
    setWizardStyle(QWizard::ClassicStyle);
    setObjectName(objName);
    restoreDialogSize();
}

DWizardDlg::~DWizardDlg()
{
    saveDialogSize();
}

void DWizardDlg::setPlugin(DPlugin* const tool)
{
    m_tool = tool;

    if (m_tool)
    {
        setOption(QWizard::HaveHelpButton);
        setButtonText(QWizard::HelpButton, i18nc("@action:button", "Help"));

        QPushButton* const help       = reinterpret_cast<QPushButton*>(button(QWizard::HelpButton));
        help->setIcon(QIcon::fromTheme(QLatin1String("help-browser")));
        QMenu* const menu             = new QMenu(help);
        QAction* const handbookAction = menu->addAction(QIcon::fromTheme(QLatin1String("globe")), i18n("Online Handbook..."));
        QAction* const aboutAction    = menu->addAction(QIcon::fromTheme(QLatin1String("help-about")), i18n("About..."));
        help->setMenu(menu);

        connect(handbookAction, SIGNAL(triggered()),
                this, SLOT(slotOnlineHandbook()));

        connect(aboutAction, SIGNAL(triggered()),
                this, SLOT(slotAboutPlugin()));
    }
}

void DWizardDlg::slotAboutPlugin()
{
    QPointer<DPluginAboutDlg> dlg = new DPluginAboutDlg(m_tool);
    dlg->exec();
    delete dlg;
}

void DWizardDlg::slotOnlineHandbook()
{
    openOnlineDocumentation(m_tool->handbookSection(), m_tool->handbookChapter(), m_tool->handbookReference());
}

void DWizardDlg::restoreDialogSize()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group      = config->group(objectName());

    if (group.exists())
    {
        winId();
        DXmlGuiWindow::restoreWindowSize(windowHandle(), group);
        resize(windowHandle()->size());
    }
    else
    {
        QScreen* screen = qApp->primaryScreen();

        if (QWidget* const widget = qApp->activeWindow())
        {
            if (QWindow* const window = widget->windowHandle())
            {
                screen = window->screen();
            }
        }

        QRect srect = screen->availableGeometry();

        resize(
               (800 <= srect.width())  ? 800 : srect.width(),
               (750 <= srect.height()) ? 750 : srect.height()
              );
    }
}

void DWizardDlg::saveDialogSize()
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup group      = config->group(objectName());
    DXmlGuiWindow::saveWindowSize(windowHandle(), group);
    config->sync();
}

} // namespace Digikam

#include "moc_dwizarddlg.cpp"
