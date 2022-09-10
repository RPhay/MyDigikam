/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-05-28
 * Description : a dialog for showing the advancedrename tooltip
 *
 * SPDX-FileCopyrightText: 2010-2012 by Andi Clemens <andi dot clemens at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "tooltipdialog.h"

// Qt includes

#include <QTextBrowser>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "tooltipcreator.h"

namespace Digikam
{

class Q_DECL_HIDDEN TooltipDialog::Private
{
public:

    explicit Private()
      : buttons(nullptr),
        textBrowser(nullptr)
    {
    }

    QDialogButtonBox* buttons;
    QTextBrowser*     textBrowser;
};

TooltipDialog::TooltipDialog(QWidget* const parent)
    : QDialog(parent),
      d(new Private)
{
    setWindowTitle(i18n("Information"));

    d->buttons     = new QDialogButtonBox(QDialogButtonBox::Close, this);
    d->buttons->button(QDialogButtonBox::Close)->setDefault(true);

    d->textBrowser = new QTextBrowser(this);
    d->textBrowser->setFrameStyle(QFrame::NoFrame);
    d->textBrowser->setOpenLinks(true);
    d->textBrowser->setOpenExternalLinks(true);

    QVBoxLayout* const vbx = new QVBoxLayout(this);
    vbx->addWidget(d->textBrowser);
    vbx->addWidget(d->buttons);
    setLayout(vbx);

    connect(d->buttons->button(QDialogButtonBox::Close), SIGNAL(clicked()),
            this, SLOT(accept()));
}

TooltipDialog::~TooltipDialog()
{
    delete d;
}

void TooltipDialog::setTooltip(const QString& tooltip)
{
    clearTooltip();

    // set image resources

    d->textBrowser->document()->addResource(QTextDocument::ImageResource,
                                            QUrl::fromLocalFile(TooltipCreator::getInstance().getInfoIconResourceName()),
                                            TooltipCreator::getInstance().getInfoIcon());
    d->textBrowser->setHtml(tooltip);
}

void TooltipDialog::clearTooltip()
{
    d->textBrowser->clear();
}

} // namespace Digikam
