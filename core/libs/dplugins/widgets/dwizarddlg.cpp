/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a template to create wizard dialog.
 *
 * SPDX-FileCopyrightText: 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "dxmlguiwindow.h"
#include "dpluginaboutdlg.h"

namespace Digikam
{

DWizardDlg::DWizardDlg(QWidget* const parent, const QString& objName)
    : QWizard(parent),
      m_tool (nullptr)
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
        setButtonText(QWizard::HelpButton, i18nc("@action: button", "About..."));

        connect(button(QWizard::HelpButton), SIGNAL(clicked()),
                this, SLOT(slotAboutPlugin()));
    }
}

void DWizardDlg::slotAboutPlugin()
{
    QPointer<DPluginAboutDlg> dlg = new DPluginAboutDlg(m_tool);
    dlg->exec();
    delete dlg;
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
        resize(800 <= srect.width()  ? 800 : srect.width(),
               750 <= srect.height() ? 750 : srect.height());
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
