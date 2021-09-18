/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : Showfoto folder view bookmark edit dialog
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

#include "showfotofolderviewbookmarkdlg.h"

// Qt includes

#include <QGridLayout>
#include <QLabel>
#include <QPointer>
#include <QLineEdit>
#include <QApplication>
#include <QStyle>
#include <QStandardPaths>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>

#ifdef HAVE_KICONTHEMES
#   include <kicondialog.h>
#endif

// Local includes

#include "digikam_debug.h"
#include "dfileselector.h"
#include "dlayoutbox.h"

using namespace Digikam;

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoFolderViewBookmarkDlg::Private
{
public:

    explicit Private()
      : create         (false),
        topLabel       (nullptr),
        iconButton     (nullptr),
        resetIconButton(nullptr),
        buttons        (nullptr),
        titleEdit      (nullptr),
        pathEdit       (nullptr)
    {
    }

    bool                create;

    QLabel*             topLabel;

    QString             icon;

    QPushButton*        iconButton;
    QPushButton*        resetIconButton;

    QDialogButtonBox*   buttons;

    QLineEdit*          titleEdit;
    DFileSelector*      pathEdit;
};

ShowfotoFolderViewBookmarkDlg::ShowfotoFolderViewBookmarkDlg(QWidget* const parent, bool create)
    : QDialog(parent),
      d      (new Private)
{
    setModal(true);

    d->buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    d->buttons->button(QDialogButtonBox::Ok)->setDefault(true);

    if (create)
    {
        setWindowTitle(i18n("New Bookmark"));
    }
    else
    {
        setWindowTitle(i18n("Edit Bookmark"));
    }

    d->create           = create;
    QWidget* const page = new QWidget(this);

    // --------------------------------------------------------

    QGridLayout* const grid = new QGridLayout(page);

    d->topLabel             = new QLabel(page);
    d->topLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    d->topLabel->setWordWrap(false);

    // --------------------------------------------------------

    QLabel* const titleLabel = new QLabel(page);
    titleLabel->setText(i18nc("@label: bookmark properties", "&Title:"));

    d->titleEdit             = new QLineEdit(page);
    d->titleEdit->setPlaceholderText(i18n("Enter bookmark name here..."));
    titleLabel->setBuddy(d->titleEdit);

    // --------------------------------------------------------

    QLabel* const iconTextLabel = new QLabel(page);
    iconTextLabel->setText(i18n("&Icon:"));

    d->iconButton               = new QPushButton(page);
    d->iconButton->setFixedSize(40, 40);
    d->iconButton->setIcon(QIcon::fromTheme(QLatin1String("folder")));
    iconTextLabel->setBuddy(d->iconButton);

    // In create mode, by default assign the icon of the parent (if not root) to this new tag.

    d->icon = QLatin1String("folder");

    d->resetIconButton = new QPushButton(QIcon::fromTheme(QLatin1String("view-refresh")), i18n("Reset"), page);

#ifndef HAVE_KICONTHEMES

    iconTextLabel->hide();
    d->iconButton->hide();
    d->resetIconButton->hide();

#endif

    // --------------------------------------------------------

    QLabel* const pathLabel = new QLabel(page);
    pathLabel->setText(i18nc("@label: bookmark properties", "&Path:"));

    d->pathEdit             = new DFileSelector(page);
    d->pathEdit->setFileDlgMode(QFileDialog::DirectoryOnly);
    d->pathEdit->setFileDlgOptions(QFileDialog::ShowDirsOnly);
    d->pathEdit->lineEdit()->setPlaceholderText(i18n("Enter bookmark path here..."));
    pathLabel->setBuddy(d->pathEdit);

    // --------------------------------------------------------

    const int spacing = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);
    const int cmargin = QApplication::style()->pixelMetric(QStyle::PM_DefaultChildMargin);

    grid->addWidget(titleLabel,         0, 0, 1, 1);
    grid->addWidget(d->titleEdit,       0, 1, 1, 3);
    grid->addWidget(iconTextLabel,      1, 0, 1, 1);
    grid->addWidget(d->iconButton,      1, 1, 1, 1);
    grid->addWidget(d->resetIconButton, 1, 2, 1, 1);
    grid->addWidget(pathLabel,          2, 0, 1, 1);
    grid->addWidget(d->pathEdit,        2, 1, 1, 3);
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

    connect(d->buttons->button(QDialogButtonBox::Help), SIGNAL(clicked()),
            this, SLOT(slotHelp()));

    // --------------------------------------------------------

    d->titleEdit->setFocus();
    adjustSize();
}

ShowfotoFolderViewBookmarkDlg::~ShowfotoFolderViewBookmarkDlg()
{
    delete d;
}

QString ShowfotoFolderViewBookmarkDlg::title() const
{
    return d->titleEdit->text();
}

QString ShowfotoFolderViewBookmarkDlg::icon() const
{
    return d->icon;
}

QString ShowfotoFolderViewBookmarkDlg::path() const
{
    return d->pathEdit->fileDlgPath();
}

void ShowfotoFolderViewBookmarkDlg::setTitle(const QString& title)
{
    d->titleEdit->setText(title);
}

void ShowfotoFolderViewBookmarkDlg::setIcon(const QString& icon)
{
    d->icon = icon;
}

void ShowfotoFolderViewBookmarkDlg::setPath(const QString& path)
{
    d->pathEdit->setFileDlgPath(path);
}

void ShowfotoFolderViewBookmarkDlg::slotIconResetClicked()
{
    d->icon = QLatin1String("folder");
    d->iconButton->setIcon(QIcon::fromTheme(d->icon));
}

void ShowfotoFolderViewBookmarkDlg::slotIconChanged()
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

bool ShowfotoFolderViewBookmarkDlg::bookmarkEdit(QWidget* const parent, QString& title, QString& icon, QString& path)
{
    QPointer<ShowfotoFolderViewBookmarkDlg> dlg = new ShowfotoFolderViewBookmarkDlg(parent);
    dlg->setTitle(title);
    dlg->setIcon(icon);
    dlg->setPath(path);

    bool valRet = dlg->exec();

    if (valRet == QDialog::Accepted)
    {
        title = dlg->title();
        icon  = dlg->icon();
        path  = dlg->path();
    }

    delete dlg;

    return valRet;
}

bool ShowfotoFolderViewBookmarkDlg::bookmarkCreate(QWidget* const parent, QString& title, QString& icon, QString& path)
{
    QPointer<ShowfotoFolderViewBookmarkDlg> dlg = new ShowfotoFolderViewBookmarkDlg(parent, true);
    dlg->setTitle(title);
    dlg->setIcon(icon);
    dlg->setPath(path);

    bool valRet = dlg->exec();

    if (valRet == QDialog::Accepted)
    {
        title = dlg->title();
        icon  = dlg->icon();
        path  = dlg->path();
    }

    delete dlg;

    return valRet;
}

} // namespace ShowFoto
