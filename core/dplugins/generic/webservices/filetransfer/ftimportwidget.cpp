/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 04-10-2009
 * Description : a tool to import items from a KIO accessible location
 *
 * SPDX-FileCopyrightText: 2009      by Johannes Wienke <languitar at semipol dot de>
 * SPDX-FileCopyrightText: 2011-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "ftimportwidget.h"

// Qt includes

#include <QApplication>
#include <QPushButton>
#include <QGridLayout>
#include <QPointer>
#include <QLabel>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dfiledialog.h"
#include "dexpanderbox.h"

namespace DigikamGenericFileTransferPlugin
{

class Q_DECL_HIDDEN FTImportWidget::Private
{
public:

    Private() = default;

public:

    QPushButton*      importBtn    = nullptr;
    DAdjustableLabel* srcLabel     = nullptr;
    DItemsList*       imageList    = nullptr;
    QWidget*          uploadWidget = nullptr;
};

FTImportWidget::FTImportWidget(QWidget* const parent, DInfoInterface* const iface)
    : QWidget(parent),
      d      (new Private)
{
    // Setup Source Selector

    d->importBtn = new QPushButton(i18n("Select Source Location..."), this);
    d->importBtn->setIcon(QIcon::fromTheme(QLatin1String("folder-remote")));
    d->srcLabel  = new DAdjustableLabel(this);
    d->srcLabel->setAdjustedText(i18n("no source selected"));
    QFont fnt;
    fnt.setItalic(true);
    d->srcLabel->setFont(fnt);

    // Setup image list

    d->imageList = new DItemsList(this);
    d->imageList->setObjectName(QLatin1String("FTImport ImagesList"));
    d->imageList->setAllowRAW(true);
    d->imageList->setIface(iface);
    d->imageList->listView()->setColumnEnabled(DItemsListView::Thumbnail, false);
    d->imageList->setControlButtons(DItemsList::Remove | DItemsList::MoveUp | DItemsList::MoveDown | DItemsList::Clear);
    d->imageList->listView()->setWhatsThis(i18n("This is the list of source items to import "
                                                "into the selected destination."));

    // Separator and Arrow

    DLineWidget* const vline1 = new DLineWidget(Qt::Vertical, this);
    QLabel* const arrow       = new QLabel(QChar(0x21D2));
    arrow->setAlignment(Qt::AlignCenter);
    DLineWidget* const vline2 = new DLineWidget(Qt::Vertical, this);

    // Setup Destination Selector

    QLabel* const uploadLabel = new QLabel(i18n("Select Destination Location:"), this);
    d->uploadWidget           = iface->uploadWidget(this);

    // Layout Widget

    QGridLayout* const grid = new QGridLayout(this);

    grid->addWidget(d->importBtn,       0, 0, 1, 1);
    grid->addWidget(d->srcLabel,        1, 0, 1, 1);
    grid->addWidget(d->imageList,       2, 0, 4, 1);

    grid->addWidget(vline1,             0, 1, 3, 1);
    grid->addWidget(arrow,              3, 1, 1, 1);
    grid->addWidget(vline2,             4, 1, 2, 1);

    grid->addWidget(uploadLabel,        0, 2, 1, 1);
    grid->addWidget(d->uploadWidget,    1, 2, 5, 1);

    grid->setSpacing(layoutSpacing());
    grid->setContentsMargins(QMargins(0, 0, 0, 0));
    grid->setRowStretch(2, 10);
    grid->setRowStretch(4, 10);
    grid->setColumnStretch(0, 10);
    grid->setColumnStretch(2, 10);

    connect(d->importBtn, SIGNAL(clicked(bool)),
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
        d->srcLabel->setAdjustedText(importDlg->selectedUrls().first().adjusted(QUrl::RemoveFilename        |
                                                                                QUrl::NormalizePathSegments |
                                                                                QUrl::RemoveFragment        |
                                                                                QUrl::RemoveUserInfo
                                                                               ).toString());
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
