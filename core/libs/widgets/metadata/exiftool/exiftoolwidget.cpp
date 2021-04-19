/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-04-18
 * Description : ExifTool metadata list view.
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

#include "exiftoolwidget.h"

// Qt includes

#include <QHeaderView>
#include <QFont>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "ditemtooltip.h"

namespace Digikam
{

ExifToolListViewGroup::ExifToolListViewGroup(ExifToolListView* const parent,
                                             const QString& group)
    : QTreeWidgetItem(parent)
{
    setFlags(Qt::ItemIsEnabled);
    setExpanded(true);
    setDisabled(false);
    QFont fn0(font(0));
    fn0.setBold(true);
    fn0.setItalic(false);
    setFont(0, fn0);
    setText(0, group);
}

ExifToolListViewGroup::~ExifToolListViewGroup()
{
}

// ---------------------------------------------------------------------------

ExifToolListViewItem::ExifToolListViewItem(ExifToolListViewGroup* const parent,
                                           const QString& name,
                                           const QString& value,
                                           const QString& desc)
    : QTreeWidgetItem(parent)
{
    setDisabled(false);
    setSelected(false);
    setText(0, name);
    setToolTip(0, !desc.isEmpty() ? desc : name);

    QString tagVal = value.simplified();

    if (tagVal.length() > 512)
    {
        tagVal.truncate(512);
        tagVal.append(QLatin1String("..."));
    }

    setText(1, tagVal);

    DToolTipStyleSheet cnt;
    setToolTip(1, QLatin1String("<qt><p>") + cnt.breakString(tagVal) + QLatin1String("</p></qt>"));
}

ExifToolListViewItem::~ExifToolListViewItem()
{
}

// ---------------------------------------------------------------------------

ExifToolListView::ExifToolListView(QWidget* const parent)
    : QTreeWidget(parent)
{
    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setAllColumnsShowFocus(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setColumnCount(2);
    setHeaderHidden(true);
    header()->setSectionResizeMode(QHeaderView::Stretch);

    m_parser = new ExifToolParser(this);
    m_parser->setTranslations(false);
}

ExifToolListView::~ExifToolListView()
{
}

bool ExifToolListView::loadFromUrl(const QUrl& url)
{
    clear();

    if (!url.isValid())
    {
        return true;
    }

    if (!m_parser->load(url.toLocalFile()))
    {
        return false;
    }

    setMetadata(m_parser->currentParsedTags());

    return true;
}

void ExifToolListView::setMetadata(const ExifToolParser::TagsMap& map)
{
    for (ExifToolParser::TagsMap::const_iterator it = map.constBegin() ;
         it != map.constEnd() ; ++it)
    {
        QString grp                   = it.key().section(QLatin1Char('.'), 0, 0)
                                                .replace(QLatin1Char('_'), QLatin1Char(' '));

        if (grp == QLatin1String("ExifTool"))
        {
            continue;
        }

        QString name                  = it.key().section(QLatin1Char('.'), -1);
        QString value                 = it.value()[1].toString();
        QString desc                  = it.value()[3].toString();
        ExifToolListViewGroup* igroup = findGroup(grp);

        if (!igroup)
        {
            igroup = new ExifToolListViewGroup(this, grp);
        }

        new ExifToolListViewItem(igroup, name, value, desc);
    }
}

ExifToolListViewGroup* ExifToolListView::findGroup(const QString& group)
{
    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        ExifToolListViewGroup* const item = dynamic_cast<ExifToolListViewGroup*>(*it);

        if (item && (item->text(0) == group))
        {
            return item;
        }

        ++it;
    }

    return nullptr;
}

// ---------------------------------------------------------------------------

class Q_DECL_HIDDEN ExifToolWidget::Private
{
public:

    enum ViewMode
    {
        MetadataView = 0,
        ErrorView
    };

public:

    explicit Private()
        : metadataView(nullptr),
          errorView   (nullptr)
    {
    }

    ExifToolListView* metadataView;
    QWidget*          errorView;
};

ExifToolWidget::ExifToolWidget(QWidget* const parent)
    : QStackedWidget(parent),
      d             (new Private)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    d->metadataView         = new ExifToolListView(this);
    d->errorView            = new QWidget(this);
    QGridLayout* const grid = new QGridLayout(d->errorView);

    QLabel* const errorLbl  = new QLabel(d->errorView);
    errorLbl->setAlignment(Qt::AlignCenter);
    errorLbl->setText(i18n("Cannot load data\nwith ExifTool.\nCheck your configuration."));

    QPushButton* const btn  = new QPushButton(d->errorView);
    btn->setText(i18n("Open Setup Dialog..."));

    connect(btn, SIGNAL(clicked()),
            this, SIGNAL(signalSetupExifTool()));

    grid->addWidget(errorLbl, 1, 1, 1, 1);
    grid->addWidget(btn,      2, 1, 1, 1);
    grid->setColumnStretch(0, 10);
    grid->setColumnStretch(2, 10);
    grid->setRowStretch(0, 10);
    grid->setRowStretch(3, 10);

    insertWidget(Private::MetadataView, d->metadataView);
    insertWidget(Private::ErrorView,    d->errorView);

    setCurrentIndex(Private::MetadataView);
}

ExifToolWidget::~ExifToolWidget()
{
    delete d;
}

void ExifToolWidget::loadFromUrl(const QUrl& url)
{
    bool ret = d->metadataView->loadFromUrl(url);

    if (ret)
    {
        setCurrentIndex(Private::MetadataView);
    }
    else
    {
        setCurrentIndex(Private::ErrorView);
    }
}

} // namespace Digikam
