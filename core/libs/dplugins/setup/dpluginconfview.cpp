/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-12-31
 * Description : configuration view for external plugin
 *
 * SPDX-FileCopyrightText: 2018-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dpluginconfview.h"

// Qt include

#include <QList>
#include <QHeaderView>

// KDE includes

#include <ksharedconfig.h>
#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "dplugin.h"

namespace Digikam
{

class Q_DECL_HIDDEN DPluginCB : public QTreeWidgetItem
{
public:

    explicit DPluginCB(DPlugin* const plugin, QTreeWidget* const parent)
        : QTreeWidgetItem(parent),
          m_plugin       (plugin)
    {
        setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
        setDisabled(false);

        // Name + Icon + Selector

        setText(0, m_plugin->name());
        setIcon(0, m_plugin->icon());

        if (m_plugin->hasVisibilityProperty())
        {
            setCheckState(0, m_plugin->shouldLoaded() ? Qt::Checked : Qt::Unchecked);
        }

        setToolTip(0, m_plugin->description());

        // Categories

        QStringList list = m_plugin->categories();
        setText(1, list.join(QString::fromLatin1(", ")));

        // Number of tools

        setText(2, QString::number(m_plugin->count()));

        // Description

        setText(3, m_plugin->description());

        // Authors

        QStringList auth = m_plugin->pluginAuthors();
        setText(4, auth.join(QString::fromLatin1(", ")));
    };

    ~DPluginCB() override = default;

    bool contains(const QString& txt, Qt::CaseSensitivity cs) const
    {
        return (
                text(0).contains(txt, cs) ||
                text(1).contains(txt, cs) ||
                text(2).contains(txt, cs) ||
                text(3).contains(txt, cs) ||
                text(4).contains(txt, cs)
               );
    };

public:

    DPlugin* m_plugin = nullptr;

private:

    Q_DISABLE_COPY(DPluginCB)
};

// ---------------------------------------------------------------------

class Q_DECL_HIDDEN DPluginConfView::Private
{
public:

    Private() = default;

    QString           filter;
    QList<DPluginCB*> plugBoxes;
};

DPluginConfView::DPluginConfView(QWidget* const parent)
    : QTreeWidget(parent),
      d          (new Private)
{
    setRootIsDecorated(false);
    setUniformRowHeights(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAllColumnsShowFocus(true);
    setSortingEnabled(true);
    setColumnCount(5);

    header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    header()->setSectionResizeMode(3, QHeaderView::Stretch);
    header()->setSectionResizeMode(4, QHeaderView::Interactive);
    header()->setSortIndicatorShown(true);

    QStringList labels;
    labels.append(i18nc("@title: Dplugin property", "Name"));
    labels.append(i18nc("@title: Dplugin property", "Categories"));
    labels.append(i18nc("@title: Dplugin property", "Tools"));
    labels.append(i18nc("@title: Dplugin property", "Description"));
    labels.append(i18nc("@title: Dplugin property", "Authors"));
    setHeaderLabels(labels);

    setAutoFillBackground(false);
    viewport()->setAutoFillBackground(false);
}

DPluginConfView::~DPluginConfView()
{
    delete d;
}

QTreeWidgetItem* DPluginConfView::appendPlugin(DPlugin* const plugin)
{
    DPluginCB* const cb = new DPluginCB(plugin, this);
    d->plugBoxes.append(cb);

    return cb;
}

DPlugin* DPluginConfView::plugin(QTreeWidgetItem* const item) const
{
    if (item)
    {
        DPluginCB* const cb = dynamic_cast<DPluginCB*>(item);

        if (cb)
        {
            return cb->m_plugin;
        }
    }

    return nullptr;
}

void DPluginConfView::apply()
{
    DPluginLoader* const loader = DPluginLoader::instance();

    if (loader)
    {
        KSharedConfigPtr config = KSharedConfig::openConfig();
        KConfigGroup group      = config->group(loader->configGroupName());

        for (DPluginCB* const item : std::as_const(d->plugBoxes))
        {
            if (item->m_plugin->hasVisibilityProperty())
            {
                bool load = (item->checkState(0) == Qt::Checked);
                group.writeEntry(item->m_plugin->iid(), load);
                item->m_plugin->setVisible(load);
                item->m_plugin->setShouldLoaded(load);
            }
        }

        config->sync();
    }
}

void DPluginConfView::selectAll()
{
    for (DPluginCB* const item : std::as_const(d->plugBoxes))
    {
        item->setCheckState(0, Qt::Checked);
    }
}

void DPluginConfView::clearAll()
{
    for (DPluginCB* const item : std::as_const(d->plugBoxes))
    {
        item->setCheckState(0, Qt::Unchecked);
    }
}

int DPluginConfView::count() const
{
    return d->plugBoxes.count();
}

int DPluginConfView::actived() const
{
    int actived = 0;

    for (DPluginCB* const item : std::as_const(d->plugBoxes))
    {
        if (item->checkState(0) == Qt::Checked)
        {
            ++actived; // cppcheck-suppress useStlAlgorithm
        }
    }

    return actived;
}

int DPluginConfView::itemsVisible() const
{
    int visible = 0;

    for (DPluginCB* const item : std::as_const(d->plugBoxes))
    {
        if (!item->isHidden())
        {
            ++visible; // cppcheck-suppress useStlAlgorithm
        }
    }

    return visible;
}

int DPluginConfView::itemsWithVisiblyProperty() const
{
    int vp = 0;

    for (DPluginCB* const item : std::as_const(d->plugBoxes))
    {
        if (!item->isHidden() && item->m_plugin->hasVisibilityProperty())
        {
            ++vp; // cppcheck-suppress useStlAlgorithm
        }
    }

    return vp;
}

void DPluginConfView::setFilter(const QString& filter, Qt::CaseSensitivity cs)
{
    d->filter = filter;
    int found = 0;

    for (DPluginCB* const item : std::as_const(d->plugBoxes))
    {
        if (item->contains(filter, cs))
        {
            found++;
            item->setHidden(false);
        }
        else
        {
            item->setHidden(true);
        }
    }

    Q_EMIT signalSearchResult(found);
}

QString DPluginConfView::filter() const
{
    return d->filter;
}

} // namespace Digikam

#include "moc_dpluginconfview.cpp"
