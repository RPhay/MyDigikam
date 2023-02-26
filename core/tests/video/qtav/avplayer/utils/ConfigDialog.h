/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef AV_PLAYER_CONFIG_DIALOG_H
#define AV_PLAYER_CONFIG_DIALOG_H

// Qt includes

#include <QDialog>
#include <QDialogButtonBox>
#include <QTabWidget>
#include <QList>

namespace QtAV
{
class ConfigPageBase;
};

namespace AVPlayer
{

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:

    static void display();

private Q_SLOTS:

    void onButtonClicked(QAbstractButton* btn);
    void onApply();
    void onCancel();
    void onReset();

private:

    explicit ConfigDialog(QWidget* const parent = nullptr);

private:

    QTabWidget*                  mpContent;
    QDialogButtonBox*            mpButtonBox;
    QList<QtAV::ConfigPageBase*> mPages;
};

} // namespace AVPlayer

#endif // AV_PLAYER_CONFIG_DIALOG_H
