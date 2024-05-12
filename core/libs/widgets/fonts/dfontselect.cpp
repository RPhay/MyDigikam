/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-12-23
 * Description : a widget to select between system font or a custom font.
 *
 * SPDX-FileCopyrightText: 2008-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dfontselect.h"

// Qt includes

#include <QLabel>
#include <QEvent>
#include <QPushButton>
#include <QFontDatabase>
#include <QApplication>
#include <QStyle>
#include <QComboBox>
#include <QFontDialog>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_globals.h"
#include "dexpanderbox.h"

namespace Digikam
{

class Q_DECL_HIDDEN DFontSelect::Private
{
public:

    Private() = default;

    QWidget*              space             = nullptr;
    QLabel*               label             = nullptr;
    DAdjustableLabel*     desc              = nullptr;

    QFont                 font;

    QPushButton*          chooseFontButton  = nullptr;

    QComboBox*            modeCombo         = nullptr;

    DFontSelect::FontMode mode              = DFontSelect::SystemFont;
};

DFontSelect::DFontSelect(const QString& text, QWidget* const parent)
    : DHBox(parent),
      d    (new Private)
{
    d->label     = new QLabel(this);
    d->label->setText(text);
    d->space     = new QWidget(this);

    if (text.isEmpty())
    {
        d->label->hide();
        d->space->hide();
    }

    d->modeCombo = new QComboBox(this);
    d->modeCombo->addItem(i18n("System Font"));
    d->modeCombo->addItem(i18n("Custom Font"));

    d->chooseFontButton = new QPushButton(i18n("Choose..."), this);
    d->desc             = new DAdjustableLabel(this);

    setSpacing(layoutSpacing());

    setContentsMargins(0, 0, 0, 0);
    setStretchFactor(d->space, 10);

    connect(d->modeCombo, SIGNAL(activated(int)),
            this, SLOT(slotChangeMode(int)));

    connect(d->chooseFontButton, SIGNAL(clicked()),
            this, SLOT(slotOpenFontDialog()));

    slotChangeMode(d->modeCombo->currentIndex());
}

DFontSelect::~DFontSelect()
{
    delete d;
}

void DFontSelect::setMode(FontMode mode)
{
    d->mode = mode;
    d->modeCombo->setCurrentIndex(d->mode);
    d->desc->setAdjustedText(QString::fromLatin1("%1 - %2").arg(font().family()).arg(font().pointSize()));
    d->chooseFontButton->setEnabled(d->mode == CustomFont);
}

DFontSelect::FontMode DFontSelect::mode() const
{
    return d->mode;
}

QFont DFontSelect::font() const
{
    return (d->mode == CustomFont) ? d->font
                                   : QFontDatabase::systemFont(QFontDatabase::GeneralFont);
}

void DFontSelect::setFont(const QFont& font)
{
    d->font = font;

    if (d->font == QFontDatabase::systemFont(QFontDatabase::GeneralFont))
    {
        setMode(SystemFont);
    }
    else
    {
        setMode(CustomFont);
    }
}

bool DFontSelect::event(QEvent* e)
{
    if (e->type() == QEvent::Polish)
    {
        d->modeCombo->setFont(font());
    }

    return DHBox::event(e);
}

void DFontSelect::slotOpenFontDialog()
{
    bool ok = false;
    QFont f = QFontDialog::getFont(&ok, font(), this,
                                   i18n("Select a Font"),
                                   QFontDialog::DontUseNativeDialog);

    if (ok)
    {
        setFont(f);

        Q_EMIT signalFontChanged();
    }
}

void DFontSelect::slotChangeMode(int index)
{
    setMode((index == CustomFont) ? CustomFont : SystemFont);

    Q_EMIT signalFontChanged();
}

} // namespace Digikam

#include "moc_dfontselect.cpp"
