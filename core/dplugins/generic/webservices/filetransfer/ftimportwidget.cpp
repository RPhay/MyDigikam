/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 04-10-2009
 * Description : main widget of the import dialog
 *
 * SPDX-FileCopyrightText: 2009      by Johannes Wienke <languitar at semipol dot de>
 * SPDX-FileCopyrightText: 2011-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "ftimportwidget.h"

// Qt includes

#include <QApplication>
#include <QPushButton>
#include <QBoxLayout>
#include <QPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dfiledialog.h"

namespace DigikamGenericFileTransferPlugin
{

class Q_DECL_HIDDEN FTImportWidget::Private
{
public:

    Private() = default;

    DItemsList*  imageList          = nullptr;
    QWidget*     uploadWidget       = nullptr;
    QPushButton* importSearchBtn    = nullptr;
};

FTImportWidget::FTImportWidget(QWidget* const parent, DInfoInterface* const iface)
    : QWidget(parent),
      d      (new Private)
{
    d->importSearchBtn = new QPushButton(i18n("Select import location..."), this);
    d->importSearchBtn->setIcon(QIcon::fromTheme(QLatin1String("folder-remote")));

    // setup image list

    d->imageList = new DItemsList(this);
    d->imageList->setObjectName(QLatin1String("FTImport ImagesList"));
    d->imageList->setAllowRAW(true);
    d->imageList->setIface(iface);
    d->imageList->listView()->setColumnEnabled(DItemsListView::Thumbnail, false);
    d->imageList->setControlButtons(DItemsList::Remove | DItemsList::MoveUp | DItemsList::MoveDown | DItemsList::Clear);
    d->imageList->listView()->setWhatsThis(i18n("This is the list of images to import "
                                                "into the current album."));

    // setup upload widget

    d->uploadWidget = iface->uploadWidget(this);

    // layout dialog

    QVBoxLayout* const layout = new QVBoxLayout(this);
    layout->addWidget(d->importSearchBtn);
    layout->addWidget(d->imageList);
    layout->addWidget(d->uploadWidget);
    layout->setContentsMargins(QMargins());
    layout->setSpacing(layoutSpacing());


    connect(d->importSearchBtn, SIGNAL(clicked(bool)),
            this, SLOT(slotShowImportDialogClicked(bool)));
}

FTImportWidget::~FTImportWidget()
{
    delete d;
}

void FTImportWidget::slotShowImportDialogClicked(bool checked)
{
    Q_UNUSED(checked);

    // TODO : store and restore previous session url from rc file

    QPointer<DFileDialog> importDlg = new DFileDialog(this, i18nc("@title:window", "Select Items to Import..."),
                                                      QString(),
                                                      i18n("All Files (*)"));
    importDlg->setAcceptMode(QFileDialog::AcceptOpen);
    importDlg->setFileMode(QFileDialog::ExistingFiles);
    importDlg->exec();

    if (importDlg->hasAcceptedUrls())
    {
        d->imageList->slotAddImages(importDlg->selectedUrls());
    }

    delete importDlg;
}

DItemsList* FTImportWidget::imagesList() const
{
    return d->imageList;
}

QWidget* FTImportWidget::uploadWidget() const
{
    return d->uploadWidget;
}

QList<QUrl> FTImportWidget::sourceUrls() const
{
    return d->imageList->imageUrls();
}

} // namespace DigikamGenericFileTransferPlugin

#include "moc_ftimportwidget.cpp"
