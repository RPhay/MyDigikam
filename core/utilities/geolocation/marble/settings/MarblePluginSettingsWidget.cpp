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

#include <QList>
#include <QPointer>
#include <QMessageBox>
#include <QGroupBox>
#include <QListView>
#include <QHeaderView>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "DialogConfigurationInterface.h"
#include "ParseRunnerPlugin.h"
#include "PluginItemDelegate.h"
#include "RenderPluginModel.h"

#include "geopluginaboutdlg.h"

using namespace Digikam;

namespace Marble
{

class MarblePluginSettingsWidget::Private
{
public:

    Private() = default;

public:

    QListView*                      renderListView = nullptr;
    QTreeWidget*                    runnerListView = nullptr;
    PluginItemDelegate*             itemDelegate   = nullptr;
    QPointer<RenderPluginModel>     renderPluginModel;
    QList<const ParseRunnerPlugin*> runnerPluginList;
};

// ---

MarblePluginSettingsWidget::MarblePluginSettingsWidget(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    QGroupBox* const grpRender    = new QGroupBox(i18n("Render Tools"), this);
    QVBoxLayout* const vlayRender = new QVBoxLayout(grpRender);
    d->renderListView             = new QListView(grpRender);
    d->itemDelegate               = new PluginItemDelegate(d->renderListView, this);
    d->renderListView->setItemDelegate(d->itemDelegate);
    vlayRender->addWidget(d->renderListView);

    QGroupBox* const grpRunner    = new QGroupBox(i18n("Runner Tools"), this);
    QVBoxLayout* const vlayRunner = new QVBoxLayout(grpRunner);
    d->runnerListView             = new QTreeWidget(grpRunner);
    d->runnerListView->setColumnCount(2);
    d->runnerListView->setRootIsDecorated(false);
    d->runnerListView->setSelectionMode(QAbstractItemView::SingleSelection);
    d->runnerListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->runnerListView->setAllColumnsShowFocus(true);
    d->runnerListView->setSortingEnabled(true);
    d->runnerListView->sortByColumn(0, Qt::AscendingOrder);
    d->runnerListView->setIconSize(QSize(16, 16));
    d->runnerListView->setHeaderHidden(true);
    d->runnerListView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    vlayRunner->addWidget(d->runnerListView);

    QVBoxLayout* const vlay = new QVBoxLayout(this);
    vlay->addWidget(grpRender);
    vlay->addWidget(grpRunner);

    connect(d->itemDelegate, SIGNAL(aboutPluginClicked(QModelIndex)),
            this, SLOT(slotAboutRenderPlugin(QModelIndex)));

    connect(d->itemDelegate, SIGNAL(configPluginClicked(QModelIndex)),
            this, SLOT(slotRenderPluginConfigDialog(QModelIndex)));

    connect(d->runnerListView, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(slotAboutRunnerPlugin(QTreeWidgetItem*,int)));
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
    if (!d->renderPluginModel.isNull())
    {
        disconnect(d->renderPluginModel.data(), nullptr, this, nullptr);
    }

    d->renderPluginModel = pluginModel;
    d->renderListView->setModel(d->renderPluginModel);

    if (!d->renderPluginModel.isNull())
    {
        connect(d->renderPluginModel.data(), SIGNAL(itemChanged(QStandardItem*)),
                this, SIGNAL(pluginListViewClicked()));
    }
}

void MarblePluginSettingsWidget::setRunnerPlugins(const QList<const ParseRunnerPlugin*>& list)
{
    d->runnerPluginList = list;

    Q_FOREACH (const ParseRunnerPlugin* plug, d->runnerPluginList)
    {
        QTreeWidgetItem* const item = new QTreeWidgetItem();
        item->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
        item->setText(0, plug->name());
        item->setIcon(0, plug->icon());
        item->setToolTip(0, plug->description());
        d->runnerListView->addTopLevelItem(item);
    }
}

void MarblePluginSettingsWidget::slotAboutRenderPlugin(const QModelIndex& index)
{
    if (d->renderPluginModel.isNull())
    {
        return;
    }

    QPointer<GeoPluginAboutDlg> dlg = new GeoPluginAboutDlg(d->renderPluginModel->pluginIface(index), this);
    dlg->exec();
    delete dlg;
}

void MarblePluginSettingsWidget::slotAboutRunnerPlugin(QTreeWidgetItem* item, int)
{
    if (!item)
    {
        return;
    }

    Q_FOREACH (const ParseRunnerPlugin* plug, d->runnerPluginList)
    {
        if (plug->name() == item->text(0))
        {
            QPointer<GeoPluginAboutDlg> dlg = new GeoPluginAboutDlg((PluginInterface*)(plug), this);
            dlg->exec();
            delete dlg;

            break;
        }
    }
}

void MarblePluginSettingsWidget::slotRenderPluginConfigDialog(const QModelIndex& index)
{
    if (d->renderPluginModel.isNull())
    {
        return;
    }

    DialogConfigurationInterface* const configInterface = d->renderPluginModel->pluginDialogConfigurationInterface(index);
    QDialog* const configDialog                         = configInterface ? configInterface->configDialog() : nullptr;

    if (configDialog)
    {
        configDialog->exec();
    }
}

} // namespace Marble

#include "moc_MarblePluginSettingsWidget.cpp"
