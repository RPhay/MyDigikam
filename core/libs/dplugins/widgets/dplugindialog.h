/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-12-31
 * Description : digiKam plugin main dialog
 *
 * SPDX-FileCopyrightText: 2018-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DPLUGIN_DIALOG_H
#define DIGIKAM_DPLUGIN_DIALOG_H

// Qt includes

#include <QDialog>
#include <QDialogButtonBox>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DPlugin;

class DIGIKAM_EXPORT DPluginDialog : public QDialog
{
    Q_OBJECT

public:

    explicit DPluginDialog(QWidget* const parent, const QString& objName);
    ~DPluginDialog() override;

    void setPlugin(DPlugin* const tool);

protected:

    void restoreDialogSize();
    void saveDialogSize();

private Q_SLOTS:

    void slotAboutPlugin();
    void slotOnlineHandbook();

protected:

    QDialogButtonBox* m_buttons;

private:

    DPlugin*          m_tool;

    Q_DISABLE_COPY(DPluginDialog)
};

} // namespace Digikam

#endif // DIGIKAM_DPLUGIN_DIALOG_H
