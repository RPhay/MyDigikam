// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Torsten Rahn <rahn@kde.org>
// SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>
//

#include "MarblePluginSettingsWidget.h"
#include "ui_MarblePluginSettingsWidget.h"

// Qt
#include <QPointer>
#include <QDialog>

// Marble
#include "DialogConfigurationInterface.h"
#include "PluginItemDelegate.h"
#include "RenderPluginModel.h"

namespace Marble
{

class MarblePluginSettingsWidgetPrivate : public Ui::MarblePluginSettingsWidget
{
public:

    explicit MarblePluginSettingsWidgetPrivate(Marble::MarblePluginSettingsWidget* const parent)
        : q             (parent),
          m_itemDelegate(nullptr)
    {
    }

    /**
     * Shows the configuration dialog for the plugin with the corresponding @p nameId.
     */
    void showPluginConfigDialog(const QModelIndex& index);

public:

    Marble::MarblePluginSettingsWidget* const q;
    PluginItemDelegate*                       m_itemDelegate;
    QPointer<RenderPluginModel>               m_pluginModel;
};

void MarblePluginSettingsWidgetPrivate::showPluginConfigDialog(const QModelIndex& index)
{
    if (m_pluginModel.isNull())
    {
        return;
    }

    DialogConfigurationInterface* const configInterface = m_pluginModel->pluginDialogConfigurationInterface(index);
    QDialog* const configDialog                         = configInterface ? configInterface->configDialog() : nullptr;

    if (configDialog)
    {
        configDialog->exec();
    }
}

// ---

MarblePluginSettingsWidget::MarblePluginSettingsWidget( QWidget *parent )
    : QWidget( parent ),
      d( new MarblePluginSettingsWidgetPrivate( this ) )
{
    d->setupUi( this );

    d->m_itemDelegate = new PluginItemDelegate( d->m_pluginListView, this );
    d->m_pluginListView->setItemDelegate( d->m_itemDelegate );

    connect( d->m_itemDelegate, SIGNAL(configPluginClicked(QModelIndex)),
             this, SLOT(showPluginConfigDialog(QModelIndex)) );
}

MarblePluginSettingsWidget::~MarblePluginSettingsWidget()
{
    delete d;
}

void MarblePluginSettingsWidget::setConfigIcon( const QIcon& icon )
{
    d->m_itemDelegate->setConfigIcon( icon );
}

void MarblePluginSettingsWidget::setModel( RenderPluginModel* pluginModel )
{
    if ( !d->m_pluginModel.isNull() )
    {
        disconnect( d->m_pluginModel.data(), nullptr, this, nullptr );
    }

    d->m_pluginModel = pluginModel;
    d->m_pluginListView->setModel( pluginModel );

    if ( !d->m_pluginModel.isNull() )
    {
        connect( d->m_pluginModel.data(), SIGNAL(itemChanged(QStandardItem*)),
                 this, SIGNAL(pluginListViewClicked()) );
    }
}

} // namespace Marble

#include "moc_MarblePluginSettingsWidget.cpp"
