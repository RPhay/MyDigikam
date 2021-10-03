/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-09-27
 * Description : Showfoto stack view favorite edit dialog
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "showfotostackviewfavoritedlg.h"

// Qt includes

#include <QGridLayout>
#include <QLabel>
#include <QPointer>
#include <QLineEdit>
#include <QApplication>
#include <QStyle>
#include <QDir>
#include <QUrl>
#include <QStandardPaths>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QListWidget>

// KDE includes

#include <klocalizedstring.h>

#ifdef HAVE_KICONTHEMES
#   include <kicondialog.h>
#endif

// Local includes

#include "digikam_debug.h"
#include "dlayoutbox.h"
#include "showfotostackviewfavorites.h"

using namespace Digikam;

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoStackViewFavoriteDlg::Private
{
public:

    explicit Private()
      : create         (false),
        topLabel       (nullptr),
        icon           (QLatin1String("folder-favorites")),
        iconButton     (nullptr),
        resetIconButton(nullptr),
        buttons        (nullptr),
        nameEdit       (nullptr),
        urlsEdit       (nullptr),
        list           (nullptr)
    {
    }

    bool                         create;

    QLabel*                      topLabel;

    QString                      icon;

    QPushButton*                 iconButton;
    QPushButton*                 resetIconButton;

    QDialogButtonBox*            buttons;

    QLineEdit*                   nameEdit;
    QListWidget*                 urlsEdit;
    ShowfotoStackViewFavorites*  list;
};

ShowfotoStackViewFavoriteDlg::ShowfotoStackViewFavoriteDlg(ShowfotoStackViewFavorites* const parent,
                                                           bool create)
    : QDialog(parent),
      d      (new Private)
{
    setModal(true);

    d->create  = create;
    d->buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    d->buttons->button(QDialogButtonBox::Ok)->setDefault(true);

    if (d->create)
    {
        setWindowTitle(i18nc("@title", "New Favorite"));
    }
    else
    {
        setWindowTitle(i18nc("@title", "Edit Favorite"));
    }

    QWidget* const page     = new QWidget(this);

    // --------------------------------------------------------

    QGridLayout* const grid = new QGridLayout(page);

    d->topLabel             = new QLabel(page);
    d->topLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    d->topLabel->setWordWrap(false);

    // --------------------------------------------------------

    QLabel* const nameLabel = new QLabel(page);
    nameLabel->setText(i18nc("@label: favorite properties", "&Name:"));

    d->nameEdit             = new QLineEdit(page);
    d->nameEdit->setPlaceholderText(i18n("Enter favorite name here..."));
    nameLabel->setBuddy(d->nameEdit);

    // --------------------------------------------------------

    QLabel* const iconTextLabel = new QLabel(page);
    iconTextLabel->setText(i18nc("@label", "&Icon:"));

    d->iconButton               = new QPushButton(page);
    d->iconButton->setFixedSize(40, 40);
    d->iconButton->setIcon(QIcon::fromTheme(d->icon));
    iconTextLabel->setBuddy(d->iconButton);

    d->resetIconButton          = new QPushButton(QIcon::fromTheme(QLatin1String("view-refresh")),
                                                  i18nc("@button", "Reset"), page);

#ifndef HAVE_KICONTHEMES

    iconTextLabel->hide();
    d->iconButton->hide();
    d->resetIconButton->hide();

#endif

    // --------------------------------------------------------

    QLabel* const urlsLabel = new QLabel(page);
    urlsLabel->setText(i18nc("@label: favorites properties", "&Items:"));

    d->urlsEdit             = new QListWidget(page);
    urlsLabel->setBuddy(d->urlsEdit);

    // --------------------------------------------------------

    const int spacing = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);
    const int cmargin = QApplication::style()->pixelMetric(QStyle::PM_DefaultChildMargin);

    grid->addWidget(nameLabel,          0, 0, 1, 1);
    grid->addWidget(d->nameEdit,        0, 1, 1, 3);
    grid->addWidget(iconTextLabel,      1, 0, 1, 1);
    grid->addWidget(d->iconButton,      1, 1, 1, 1);
    grid->addWidget(d->resetIconButton, 1, 2, 1, 1);
    grid->addWidget(urlsLabel,          2, 0, 1, 1);
    grid->addWidget(d->urlsEdit,        2, 1, 1, 3);
    grid->setRowStretch(3, 10);
    grid->setColumnStretch(3, 10);
    grid->setContentsMargins(cmargin, cmargin, cmargin, cmargin);
    grid->setSpacing(spacing);

    QVBoxLayout* const vbx = new QVBoxLayout(this);
    vbx->addWidget(page);
    vbx->addWidget(d->buttons);
    setLayout(vbx);

    // --------------------------------------------------------

    connect(d->iconButton, SIGNAL(clicked()),
            this, SLOT(slotIconChanged()));

    connect(d->resetIconButton, SIGNAL(clicked()),
            this, SLOT(slotIconResetClicked()));

    connect(d->buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(accept()));

    connect(d->buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
            this, SLOT(reject()));

    // --------------------------------------------------------

    d->nameEdit->setFocus();
    adjustSize();
}

ShowfotoStackViewFavoriteDlg::~ShowfotoStackViewFavoriteDlg()
{
    delete d;
}

QString ShowfotoStackViewFavoriteDlg::name() const
{
    return d->nameEdit->text();
}

QString ShowfotoStackViewFavoriteDlg::icon() const
{
    return d->icon;
}

QList<QUrl> ShowfotoStackViewFavoriteDlg::urls() const
{
    QList<QUrl> urls;

    for (int i = 0 ; i < d->urlsEdit->count() ; ++i)
    {
        urls << QUrl::fromLocalFile(d->urlsEdit->item(i)->text());
    }

    return urls;
}

void ShowfotoStackViewFavoriteDlg::setName(const QString& name)
{
    d->nameEdit->setText(name);
}

void ShowfotoStackViewFavoriteDlg::setIcon(const QString& icon)
{
    d->icon = icon;
}

void ShowfotoStackViewFavoriteDlg::setUrls(const QList<QUrl>& urls)
{
    foreach (const QUrl& url, urls)
    {
        d->urlsEdit->addItem(url.toLocalFile());
    }
}

void ShowfotoStackViewFavoriteDlg::slotIconResetClicked()
{
    d->icon = QLatin1String("folder-favorites");
    d->iconButton->setIcon(QIcon::fromTheme(d->icon));
}

void ShowfotoStackViewFavoriteDlg::slotIconChanged()
{

#ifdef HAVE_KICONTHEMES

    QPointer<KIconDialog> dlg = new KIconDialog(this);
    dlg->setup(KIconLoader::NoGroup, KIconLoader::Application, false, 20, false, false, false);
    QString icon              = dlg->openDialog();
    delete dlg;

    if (icon.isEmpty() || (icon == d->icon))
    {
        return;
    }

    d->icon                   = icon;
    d->iconButton->setIcon(QIcon::fromTheme(d->icon));

#endif

}

bool ShowfotoStackViewFavoriteDlg::favoriteEdit(ShowfotoStackViewFavorites* const parent,
                                                QString& name, QString& icon, QList<QUrl>& urls)
{
    QPointer<ShowfotoStackViewFavoriteDlg> dlg = new ShowfotoStackViewFavoriteDlg(parent);
    dlg->setName(name);
    dlg->setIcon(icon);
    dlg->setUrls(urls);

    bool valRet = dlg->exec();

    if (valRet == QDialog::Accepted)
    {
        name = dlg->name();
        icon = dlg->icon();
        urls = dlg->urls();
    }

    delete dlg;

    return valRet;
}

bool ShowfotoStackViewFavoriteDlg::favoriteCreate(ShowfotoStackViewFavorites* const parent,
                                                  QString& name, QString& icon, QList<QUrl>& urls)
{
    QPointer<ShowfotoStackViewFavoriteDlg> dlg = new ShowfotoStackViewFavoriteDlg(parent, true);
    dlg->setName(name);
    dlg->setIcon(icon);
    dlg->setUrls(urls);

    bool valRet = dlg->exec();

    if (valRet == QDialog::Accepted)
    {
        name = dlg->name();
        icon = dlg->icon();
        urls = dlg->urls();
    }

    delete dlg;

    return valRet;
}

} // namespace ShowFoto
