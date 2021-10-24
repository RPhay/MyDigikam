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

#include "showfotostackviewfavoritefolderdlg.h"

// Qt includes

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QApplication>
#include <QStyle>
#include <QFont>
#include <QLocale>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "dlayoutbox.h"
#include "dexpanderbox.h"
#include "showfotostackviewfavoritelist.h"
#include "showfotostackviewfavoriteitem.h"
#include "showfotostackviewlist.h"

using namespace Digikam;

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoStackViewFavoriteFolderDlg::Private
{
public:

    explicit Private()
      : create         (false),
        hierarchyLabel (nullptr),
        buttons        (nullptr),
        nameEdit       (nullptr),
        helpLabel      (nullptr),
        list           (nullptr),
        pitem          (nullptr)
    {
    }

    bool                                        create;

    DAdjustableLabel*                           hierarchyLabel;

    QString                                     icon;
    QString                                     originalName;

    QDialogButtonBox*                           buttons;

    QLineEdit*                                  nameEdit;
    DAdjustableLabel*                           helpLabel;
    ShowfotoStackViewFavoriteList*              list;
    ShowfotoStackViewFavoriteBase*              pitem;
};

ShowfotoStackViewFavoriteFolderDlg::ShowfotoStackViewFavoriteFolderDlg(ShowfotoStackViewFavoriteList* const list,
                                                           bool create)
    : QDialog(list),
      d      (new Private)
{
    setModal(true);

    d->create  = create;
    d->list    = list;
    d->buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    d->buttons->button(QDialogButtonBox::Ok)->setDefault(true);

    if (d->create)
    {
        setWindowTitle(i18nc("@title", "New Favorite Folder"));
    }
    else
    {
        setWindowTitle(i18nc("@title", "Edit Favorite Folder"));
    }

    QWidget* const page     = new QWidget(this);
    QGridLayout* const grid = new QGridLayout(page);

    // --------------------------------------------------------

    QLabel* const nameLabel = new QLabel(page);
    nameLabel->setText(i18nc("@label: favorite folder title properties", "&Name:"));

    d->nameEdit             = new QLineEdit(page);
    d->nameEdit->setPlaceholderText(i18nc("@info", "Enter favorite folder name here..."));
    d->nameEdit->setToolTip(i18nc("@info", "The favorite folder name which must be unique and not empty"));
    nameLabel->setBuddy(d->nameEdit);

    // --------------------------------------------------------

    QLabel* const hierLabel = new QLabel(page);
    hierLabel->setText(i18nc("@label: favorite folder hierarchy properties", "Hierarchy:"));

    d->hierarchyLabel       = new DAdjustableLabel(page);
    d->hierarchyLabel->setToolTip(i18nc("@info", "The favorite folder hierarchy which must be unique in tree-view"));

    // --------------------------------------------------------

    d->helpLabel = new DAdjustableLabel(page);
    QPalette pal = d->helpLabel->palette();
    pal.setColor(QPalette::WindowText, Qt::red);
    d->helpLabel->setPalette(pal);

    // --------------------------------------------------------

    grid->addWidget(nameLabel,          0, 0, 1, 1);
    grid->addWidget(d->nameEdit,        0, 1, 1, 3);
    grid->addWidget(hierLabel,          1, 0, 1, 1);
    grid->addWidget(d->hierarchyLabel,  1, 1, 1, 3);
    grid->addWidget(d->helpLabel,       2, 0, 1, 3);
    grid->setRowStretch(3, 10);
    grid->setColumnStretch(3, 10);

    QVBoxLayout* const vbx = new QVBoxLayout(this);
    vbx->addWidget(page);
    vbx->addWidget(d->buttons);
    setLayout(vbx);

    // --------------------------------------------------------

    connect(d->nameEdit, SIGNAL(textChanged(QString)),
            this, SLOT(slotModified()));

    connect(d->buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(accept()));

    connect(d->buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
            this, SLOT(reject()));

    // --------------------------------------------------------

    d->nameEdit->setFocus();
    adjustSize();
}

ShowfotoStackViewFavoriteFolderDlg::~ShowfotoStackViewFavoriteFolderDlg()
{
    delete d;
}

bool ShowfotoStackViewFavoriteFolderDlg::canAccept() const
{
    bool b1 = name().isEmpty();
    bool b3 = false;     // If dialog in edit mode, the original name can be accepted.

    if (
        d->create ||
        (!d->create && (name() != d->originalName))
       )
    {
        b3 = d->list->findFavoriteByHierarchy(ShowfotoStackViewFavoriteBase::hierarchyFromParent(name(), d->pitem));
    }

    bool b4 = (!b1 && !b3);

    if (b4)
    {
        d->helpLabel->setAdjustedText(QString());
    }
    else
    {
        if      (b1)
        {
            d->helpLabel->setAdjustedText(i18nc("@label", "Note: name cannot be empty!"));
        }
        else if (b3)
        {
            d->helpLabel->setAdjustedText(i18nc("@label", "Note: name already exists in favorites list!"));
        }
    }

    return (
            !b1 &&
            !b3
           );
}

void ShowfotoStackViewFavoriteFolderDlg::slotAccept()
{
    if (canAccept())
    {
        accept();
    }
}

void ShowfotoStackViewFavoriteFolderDlg::slotModified()
{
    d->buttons->button(QDialogButtonBox::Ok)->setEnabled(canAccept());
    d->hierarchyLabel->setAdjustedText(ShowfotoStackViewFavoriteBase::hierarchyFromParent(name(), d->pitem));
}

QString ShowfotoStackViewFavoriteFolderDlg::name() const
{
    return d->nameEdit->text();
}

void ShowfotoStackViewFavoriteFolderDlg::setName(const QString& name)
{
    d->nameEdit->setText(name);
    d->originalName = name;
}

void ShowfotoStackViewFavoriteFolderDlg::setParentItem(ShowfotoStackViewFavoriteBase* const pitem)
{
    d->pitem = pitem;
}

bool ShowfotoStackViewFavoriteFolderDlg::favoriteFolderDialog(ShowfotoStackViewFavoriteList* const list,
                                                            QString& name,
                                                            ShowfotoStackViewFavoriteBase* const pitem,
                                                            bool create)
{
    QPointer<ShowfotoStackViewFavoriteFolderDlg> dlg = new ShowfotoStackViewFavoriteFolderDlg(list, create);
    dlg->setName(name);
    dlg->setParentItem(pitem);

    bool valRet = dlg->exec();

    if (valRet == QDialog::Accepted)
    {
        name    = dlg->name();
    }

    delete dlg;

    return valRet;
}

} // namespace ShowFoto
