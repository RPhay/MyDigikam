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

#include "ConfigDialog.h"

// Qt includes

#include <QFile>
#include <QLayout>
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "CaptureConfigPage.h"
#include "DecoderConfigPage.h"
#include "AVFormatConfigPage.h"
#include "AVFilterConfigPage.h"
#include "MiscPage.h"
#include "ShaderPage.h"
#include "AVPlayerConfigMngr.h"
#include "digikam_debug.h"

using namespace QtAV;

namespace AVPlayer
{

void ConfigDialog::display()
{
    static ConfigDialog* const dialog = new ConfigDialog();
    dialog->show();
}

ConfigDialog::ConfigDialog(QWidget* const parent)
    : QDialog(parent)
{
    QVBoxLayout* const vbl = new QVBoxLayout();
    setLayout(vbl);
    vbl->setSizeConstraint(QLayout::SetFixedSize);

    mpContent              = new QTabWidget();
    mpContent->setTabPosition(QTabWidget::North);

    mpButtonBox            = new QDialogButtonBox(Qt::Horizontal);
    mpButtonBox->addButton(i18n("Reset"),  QDialogButtonBox::ResetRole);
    mpButtonBox->addButton(i18n("Ok"),     QDialogButtonBox::AcceptRole);
    mpButtonBox->addButton(i18n("Cancel"), QDialogButtonBox::RejectRole);
    mpButtonBox->addButton(i18n("Apply"),  QDialogButtonBox::ApplyRole);

    connect(mpButtonBox, SIGNAL(accepted()),
            this, SLOT(accept()));

    connect(mpButtonBox, SIGNAL(rejected()),
            this, SLOT(reject()));

    connect(mpButtonBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(onButtonClicked(QAbstractButton*)));

    vbl->addWidget(mpContent);
    vbl->addWidget(mpButtonBox);

    mPages << new MiscPage()
           << new CaptureConfigPage()
           << new DecoderConfigPage()
           << new AVFormatConfigPage()
           << new AVFilterConfigPage()
           << new ShaderPage()
    ;

    Q_FOREACH (ConfigPageBase* const page, mPages)
    {
        page->applyToUi();
        page->applyOnUiChange(false);
        mpContent->addTab(page, page->name());
    }
}

void ConfigDialog::onButtonClicked(QAbstractButton* btn)
{
    qCDebug(DIGIKAM_AVPLAYER_LOG).noquote()
        << QString::asprintf("QDialogButtonBox clicked role=%d",
            mpButtonBox->buttonRole(btn));

    switch (mpButtonBox->buttonRole(btn))
    {
        case QDialogButtonBox::ResetRole:
        {
            qCDebug(DIGIKAM_AVPLAYER_LOG).noquote()
                << QString::asprintf("QDialogButtonBox ResetRole clicked");

            onReset();

            break;
        }

        case QDialogButtonBox::AcceptRole:
        case QDialogButtonBox::ApplyRole:
        {
            qCDebug(DIGIKAM_AVPLAYER_LOG).noquote()
                << QString::asprintf("QDialogButtonBox ApplyRole clicked");

            onApply();

            break;
        }

        case QDialogButtonBox::DestructiveRole:
        case QDialogButtonBox::RejectRole:
        {
            onCancel();

            break;
        }

        default:
        {
            break;
        }
    }
}

void ConfigDialog::onReset()
{
    AVPlayerConfigMngr::instance().reset();

    // TODO: check change

    Q_FOREACH (ConfigPageBase* const page, mPages)
    {
        page->reset();
    }
}

void ConfigDialog::onApply()
{
    // TODO: check change

    Q_FOREACH (ConfigPageBase* const page, mPages)
    {
        page->apply();
    }

    AVPlayerConfigMngr::instance().save();
}

void ConfigDialog::onCancel()
{
    // TODO: check change

    Q_FOREACH (ConfigPageBase* const page, mPages)
    {
        page->cancel();
    }
}

} // namespace AVPlayer
