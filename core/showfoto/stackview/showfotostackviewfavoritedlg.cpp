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
#include "ditemslist.h"
#include "showfotostackviewfavoritelist.h"
#include "showfotostackviewlist.h"

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
        descEdit       (nullptr),
        urlsEdit       (nullptr),
        list           (nullptr)
    {
    }

    bool                            create;

    QLabel*                         topLabel;

    QString                         icon;

    QPushButton*                    iconButton;
    QPushButton*                    resetIconButton;

    QDialogButtonBox*               buttons;

    QLineEdit*                      nameEdit;
    QLineEdit*                      descEdit;
    DItemsList*                     urlsEdit;
    ShowfotoStackViewFavoriteList*  list;
};

ShowfotoStackViewFavoriteDlg::ShowfotoStackViewFavoriteDlg(ShowfotoStackViewFavoriteList* const parent,
                                                           bool create)
    : QDialog(parent),
      d      (new Private)
{
    setModal(true);

    d->create  = create;
    d->list    = parent;
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
    d->nameEdit->setPlaceholderText(i18nc("#info", "Enter favorite name here..."));
    d->nameEdit->setToolTip(i18nc("#info", "The favorite name which must be unique and not empty"));
    nameLabel->setBuddy(d->nameEdit);

    // --------------------------------------------------------

    QLabel* const descLabel = new QLabel(page);
    descLabel->setText(i18nc("@label: favorite properties", "&Description:"));

    d->descEdit             = new QLineEdit(page);
    d->descEdit->setPlaceholderText(i18nc("#info", "Enter favorite description here..."));
    descLabel->setBuddy(d->descEdit);

    // --------------------------------------------------------

    QLabel* const iconTextLabel = new QLabel(page);
    iconTextLabel->setText(i18nc("@label", "&Icon:"));

    d->iconButton               = new QPushButton(page);
    d->iconButton->setFixedSize(40, 40);
    d->iconButton->setIcon(QIcon::fromTheme(d->icon));
    iconTextLabel->setBuddy(d->iconButton);

    d->resetIconButton          = new QPushButton(QIcon::fromTheme(QLatin1String("view-refresh")),
                                                  i18nc("@action:button", "Reset"), page);

#ifndef HAVE_KICONTHEMES

    iconTextLabel->hide();
    d->iconButton->hide();
    d->resetIconButton->hide();

#endif

    // --------------------------------------------------------

    QLabel* const urlsLabel = new QLabel(page);
    urlsLabel->setText(i18nc("@label: favorites properties", "&Items:"));

    d->urlsEdit             = new DItemsList(page, ShowfotoStackViewList::SizeSmall);
    d->urlsEdit->setAllowRAW(true);
    d->urlsEdit->setAllowDuplicate(false);
    d->urlsEdit->setControlButtonsPlacement(DItemsList::NoControlButtons);
    urlsLabel->setBuddy(d->urlsEdit);

    // --------------------------------------------------------

    grid->addWidget(nameLabel,          0, 0, 1, 1);
    grid->addWidget(d->nameEdit,        0, 1, 1, 3);
    grid->addWidget(descLabel,          1, 0, 1, 1);
    grid->addWidget(d->descEdit,        1, 1, 1, 3);
    grid->addWidget(iconTextLabel,      2, 0, 1, 1);
    grid->addWidget(d->iconButton,      2, 1, 1, 1);
    grid->addWidget(d->resetIconButton, 2, 2, 1, 1);
    grid->addWidget(urlsLabel,          3, 0, 1, 1);
    grid->addWidget(d->urlsEdit,        3, 1, 1, 3);
    grid->setRowStretch(4, 10);
    grid->setColumnStretch(1, 10);

    QVBoxLayout* const vbx = new QVBoxLayout(this);
    vbx->addWidget(page);
    vbx->addWidget(d->buttons);
    setLayout(vbx);

    // --------------------------------------------------------

    connect(d->nameEdit, SIGNAL(textChanged(QString)),
            this, SLOT(slotModified()));

    connect(d->urlsEdit, SIGNAL(signalImageListChanged()),
            this, SLOT(slotModified()));

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

bool ShowfotoStackViewFavoriteDlg::canAccept() const
{
    bool b = true;

    if (d->create)
    {
        b = !d->list->favoriteExists(name());
    }

    return (
            !name().isEmpty() &&
            !urls().isEmpty() &&
            b
           );
}

void ShowfotoStackViewFavoriteDlg::slotAccept()
{
    if (canAccept())
    {
        accept();
    }
}

void ShowfotoStackViewFavoriteDlg::slotModified()
{
    d->buttons->button(QDialogButtonBox::Ok)->setEnabled(canAccept());
}

QString ShowfotoStackViewFavoriteDlg::name() const
{
    return d->nameEdit->text();
}

QString ShowfotoStackViewFavoriteDlg::description() const
{
    return d->descEdit->text();
}

QString ShowfotoStackViewFavoriteDlg::icon() const
{
    return d->icon;
}

QList<QUrl> ShowfotoStackViewFavoriteDlg::urls() const
{
    return d->urlsEdit->imageUrls();
}

void ShowfotoStackViewFavoriteDlg::setName(const QString& name)
{
    d->nameEdit->setText(name);
}

void ShowfotoStackViewFavoriteDlg::setDescription(const QString& desc)
{
    d->descEdit->setText(desc);
}

void ShowfotoStackViewFavoriteDlg::setIcon(const QString& icon)
{
    d->icon = icon;
}

void ShowfotoStackViewFavoriteDlg::setUrls(const QList<QUrl>& urls)
{
    d->urlsEdit->slotAddImages(urls);
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

bool ShowfotoStackViewFavoriteDlg::favoriteEdit(ShowfotoStackViewFavoriteList* const parent,
                                                  QString& name,
                                                  QString& desc,
                                                  QString& icon,
                                                  QList<QUrl>& urls)
{
    QPointer<ShowfotoStackViewFavoriteDlg> dlg = new ShowfotoStackViewFavoriteDlg(parent);
    dlg->setName(name);
    dlg->setDescription(desc);
    dlg->setIcon(icon);
    dlg->setUrls(urls);

    bool valRet = dlg->exec();

    if (valRet == QDialog::Accepted)
    {
        name = dlg->name();
        desc = dlg->description();
        icon = dlg->icon();
        urls = dlg->urls();
    }

    delete dlg;

    return valRet;
}

bool ShowfotoStackViewFavoriteDlg::favoriteCreate(ShowfotoStackViewFavoriteList* const parent,
                                                  QString& name,
                                                  QString& desc,
                                                  QString& icon,
                                                  QList<QUrl>& urls)
{
    QPointer<ShowfotoStackViewFavoriteDlg> dlg = new ShowfotoStackViewFavoriteDlg(parent, true);
    dlg->setName(name);
    dlg->setDescription(desc);
    dlg->setIcon(icon);
    dlg->setUrls(urls);

    bool valRet = dlg->exec();

    if (valRet == QDialog::Accepted)
    {
        name = dlg->name();
        desc = dlg->description();
        icon = dlg->icon();
        urls = dlg->urls();
    }

    delete dlg;

    return valRet;
}

} // namespace ShowFoto
