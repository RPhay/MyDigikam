// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Torsten Rahn <rahn@kde.org>
// SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>
//

#include "MarblePluginSettingsWidget.h"
#include "ui_MarblePluginSettingsWidget.h"

// Qt includes

#include <QPointer>
#include <QMessageBox>

// KDE includes

#include <klocalizedstring.h>

// Local includes

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
     * Shows the about dialog for the plugin with the corresponding @p nameId.
     */
    void showPluginAboutDialog(const QModelIndex& index);

    /**
     * Shows the configuration dialog for the plugin with the corresponding @p nameId.
     */
    void showPluginConfigDialog(const QModelIndex& index);

public:

    Marble::MarblePluginSettingsWidget* const q;
    PluginItemDelegate*                       m_itemDelegate;
    QPointer<RenderPluginModel>               m_pluginModel;
};

void MarblePluginSettingsWidgetPrivate::showPluginAboutDialog(const QModelIndex& index)
{
    if (m_pluginModel.isNull())
    {
        return;
    }

    QMessageBox* const dlg = new QMessageBox(q);
    dlg->setWindowFlags((dlg->windowFlags() & ~Qt::Dialog) |
                   Qt::Window                    |
                   Qt::WindowCloseButtonHint     |
                   Qt::WindowMinMaxButtonsHint);

    dlg->setWindowTitle(i18n("About Plugin %1", m_pluginModel->data(index, RenderPluginModel::Name).toString()));
    dlg->setIconPixmap(qvariant_cast<QIcon>(m_pluginModel->data(index, RenderPluginModel::Icon)).pixmap(48, 48));
    dlg->addButton(QMessageBox::Ok);
    dlg->setText(i18n(
                      "<p>Version: %1</p>"
                      "<p>Description: %2</p>",
                      m_pluginModel->data(index, RenderPluginModel::Version).toString(),
                      m_pluginModel->data(index, RenderPluginModel::AboutDataText).toString()
                     )
                );

    dlg->exec();
}

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

MarblePluginSettingsWidget::MarblePluginSettingsWidget(QWidget* parent)
    : QWidget(parent),
      d      (new MarblePluginSettingsWidgetPrivate(this))
{
    d->setupUi(this);

    d->m_itemDelegate = new PluginItemDelegate(d->m_pluginListView, this);
    d->m_pluginListView->setItemDelegate(d->m_itemDelegate);

    connect(d->m_itemDelegate, SIGNAL(aboutPluginClicked(QModelIndex)),
            this, SLOT(showPluginAboutDialog(QModelIndex)));

    connect(d->m_itemDelegate, SIGNAL(configPluginClicked(QModelIndex)),
            this, SLOT(showPluginConfigDialog(QModelIndex)));
}

MarblePluginSettingsWidget::~MarblePluginSettingsWidget()
{
    delete d;
}

void MarblePluginSettingsWidget::setAboutIcon(const QIcon& icon)
{
    d->m_itemDelegate->setAboutIcon(icon);
}

void MarblePluginSettingsWidget::setConfigIcon(const QIcon& icon)
{
    d->m_itemDelegate->setConfigIcon(icon);
}

void MarblePluginSettingsWidget::setModel(RenderPluginModel* pluginModel)
{
    if (!d->m_pluginModel.isNull())
    {
        disconnect(d->m_pluginModel.data(), nullptr, this, nullptr);
    }

    d->m_pluginModel = pluginModel;
    d->m_pluginListView->setModel(pluginModel);

    if (!d->m_pluginModel.isNull())
    {
        connect(d->m_pluginModel.data(), SIGNAL(itemChanged(QStandardItem*)),
                this, SIGNAL(pluginListViewClicked()));
    }
}

} // namespace Marble

#include "moc_MarblePluginSettingsWidget.cpp"
