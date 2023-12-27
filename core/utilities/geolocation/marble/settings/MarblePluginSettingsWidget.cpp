/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-12-21
 * Description : Marble Settings View.
 *
 * SPDX-FileCopyrightText: 2007      by Torsten Rahn <rahn at kde dot org>
 * SPDX-FileCopyrightText: 2008      by Jens-Michael Hoffmann <jensmh at gmx dot de>
 * SPDX-FileCopyrightText: 2022-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "MarblePluginSettingsWidget.h"

// Qt includes

#include <QPointer>
#include <QMessageBox>
#include <QListView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "DialogConfigurationInterface.h"
#include "PluginItemDelegate.h"
#include "RenderPluginModel.h"

namespace Marble
{

class MarblePluginSettingsWidget::Private
{
public:

    Private() = default;

public:

    QListView*                  pluginListView = nullptr;
    PluginItemDelegate*         itemDelegate   = nullptr;
    QPointer<RenderPluginModel> pluginModel;
};

// ---

MarblePluginSettingsWidget::MarblePluginSettingsWidget(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    d->pluginListView       = new QListView(this);
    QVBoxLayout* const vlay = new QVBoxLayout(this);
    vlay->addWidget(d->pluginListView);

    d->itemDelegate         = new PluginItemDelegate(d->pluginListView, this);
    d->pluginListView->setItemDelegate(d->itemDelegate);

    connect(d->itemDelegate, SIGNAL(aboutPluginClicked(QModelIndex)),
            this, SLOT(slotPluginAboutDialog(QModelIndex)));

    connect(d->itemDelegate, SIGNAL(configPluginClicked(QModelIndex)),
            this, SLOT(slotPluginConfigDialog(QModelIndex)));
}

MarblePluginSettingsWidget::~MarblePluginSettingsWidget()
{
    delete d;
}

void MarblePluginSettingsWidget::setAboutIcon(const QIcon& icon)
{
    d->itemDelegate->setAboutIcon(icon);
}

void MarblePluginSettingsWidget::setConfigIcon(const QIcon& icon)
{
    d->itemDelegate->setConfigIcon(icon);
}

void MarblePluginSettingsWidget::setModel(RenderPluginModel* const pluginModel)
{
    if (!d->pluginModel.isNull())
    {
        disconnect(d->pluginModel.data(), nullptr, this, nullptr);
    }

    d->pluginModel = pluginModel;
    d->pluginListView->setModel(pluginModel);

    if (!d->pluginModel.isNull())
    {
        connect(d->pluginModel.data(), SIGNAL(itemChanged(QStandardItem*)),
                this, SIGNAL(pluginListViewClicked()));
    }
}

void MarblePluginSettingsWidget::slotPluginAboutDialog(const QModelIndex& index)
{
    if (d->pluginModel.isNull())
    {
        return;
    }

    QDialog* const dlg = new QDialog(this);
    dlg->setWindowFlags((dlg->windowFlags() & ~Qt::Dialog) |
                   Qt::Window                              |
                   Qt::WindowCloseButtonHint               |
                   Qt::WindowMinMaxButtonsHint);

    QString years = d->pluginModel->data(index, RenderPluginModel::CopyrightYears).toString();
    QString auth  = years + i18n(" by ");

    for (const PluginAuthor& author: d->pluginModel->pluginAuthors(index))
    {
        auth += author.name + QLatin1String(", ");
    }

    auth.truncate(auth.size() - 2);

    dlg->setWindowTitle(i18n("About Plugin"));
    QLabel* const icon      = new QLabel(dlg);
    icon->setPixmap(qvariant_cast<QIcon>(d->pluginModel->data(index, RenderPluginModel::Icon)).pixmap(48, 48));
    QLabel* const text      = new QLabel(dlg);
    text->setWordWrap(true);
    text->setText(i18n(
                       "<p><u>Name:</u> %1</p>"
                       "<p><u>Version:</u> %2</p>"
                       "<p><u>Copyrights:</u> %3</p>"
                       "<p><u>Description:</u> %4</p>",
                       d->pluginModel->data(index, RenderPluginModel::Name).toString(),
                       d->pluginModel->data(index, RenderPluginModel::Version).toString(),
                       auth,
                       d->pluginModel->data(index, RenderPluginModel::Description).toString()
                      )
                 );

    QDialogButtonBox* const btn = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    btn->button(QDialogButtonBox::Ok)->setDefault(true);

    QVBoxLayout* const vlay = new QVBoxLayout(dlg);
    QHBoxLayout* const hlay = new QHBoxLayout;
    hlay->addWidget(icon);
    hlay->addWidget(text);
    vlay->addLayout(hlay);
    vlay->addWidget(btn);

    connect(btn->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            dlg, SLOT(accept()));

    dlg->adjustSize();
    dlg->exec();
}

void MarblePluginSettingsWidget::slotPluginConfigDialog(const QModelIndex& index)
{
    if (d->pluginModel.isNull())
    {
        return;
    }

    DialogConfigurationInterface* const configInterface = d->pluginModel->pluginDialogConfigurationInterface(index);
    QDialog* const configDialog                         = configInterface ? configInterface->configDialog() : nullptr;

    if (configDialog)
    {
        configDialog->exec();
    }
}

} // namespace Marble

#include "moc_MarblePluginSettingsWidget.cpp"
