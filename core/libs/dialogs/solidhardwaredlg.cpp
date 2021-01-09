/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-04-07
 * Description : Solid hardware list dialog
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "solidhardwaredlg.h"

// Qt includes

#include <QStringList>
#include <QString>
#include <QLabel>
#include <QLayout>
#include <QGridLayout>
#include <QTreeWidget>
#include <QHeaderView>
#include <QMimeData>
#include <QClipboard>
#include <QApplication>
#include <QPushButton>
#include <QDialogButtonBox>

// KDE includes

#include <klocalizedstring.h>

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wnonportable-include-path"
#endif

#include <solid_version.h>
#include <solid/solidnamespace.h>
#include <solid/camera.h>
#include <solid/device.h>
#include <solid/deviceinterface.h>
#include <solid/genericinterface.h>

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic pop
#endif

using namespace std;

namespace Digikam
{

class Q_DECL_HIDDEN SolidHardwareDlg::Private
{
public:

    explicit Private()
      : header    (nullptr),
        searchBar (nullptr)
    {
    }

    QLabel*        header;
    SearchTextBar* searchBar;
};

SolidHardwareDlg::SolidHardwareDlg(QWidget* const parent)
    : InfoDlg(parent),
      d      (new Private)
{
    setWindowTitle(i18n("List of detected hardware"));

    d->header     = new QLabel(this);
    d->header->setText(i18n("%1 use Solid framework version %2\n"
                            "to detect and manage devices from your computer.\n"
                            "Press \"Refresh\" to update list if you plug a removable device.",
                       QApplication::applicationName(),
                       QLatin1String(SOLID_VERSION_STRING)));
    d->searchBar  = new SearchTextBar(this, QLatin1String("SolidHardwareDlgSearchBar"));

    listView()->setHeaderLabels(QStringList() << QLatin1String("Properties") << QLatin1String("Value")); // Hidden header -> no i18n
    listView()->header()->show();
    listView()->setSortingEnabled(true);
    listView()->setRootIsDecorated(true);
    listView()->setSelectionMode(QAbstractItemView::SingleSelection);
    listView()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    listView()->setAllColumnsShowFocus(true);
    listView()->setColumnCount(2);
    listView()->header()->setSectionResizeMode(QHeaderView::Stretch);
    listView()->header()->hide();

    buttonBox()->addButton(QDialogButtonBox::Reset);
    buttonBox()->button(QDialogButtonBox::Reset)->setText(i18n("Refresh"));


    // --------------------------------------------------------

    QGridLayout* const  grid = dynamic_cast<QGridLayout*>(mainWidget()->layout());
    grid->addWidget(d->header,     1, 0, 1, -1);
    grid->addWidget(d->searchBar,  3, 0, 1, -1);

    // --------------------------------------------------------

    connect(d->searchBar, SIGNAL(signalSearchTextSettings(SearchTextSettings)),
            this, SLOT(slotSearchTextChanged(SearchTextSettings)));

    connect(buttonBox()->button(QDialogButtonBox::Reset), SIGNAL(clicked()),
            this, SLOT(slotPopulateDevices()));

    // --------------------------------------------------------

    slotPopulateDevices();
}

SolidHardwareDlg::~SolidHardwareDlg()
{
    delete d;
}

void SolidHardwareDlg::slotPopulateDevices()
{
    listView()->clear();

    const QList<Solid::Device> all = Solid::Device::allDevices();

    for (const Solid::Device& device : all)
    {
        QString typeStr;

        if      (device.isDeviceInterface(Solid::DeviceInterface::StorageDrive))
        {
            typeStr = Solid::DeviceInterface::typeToString(Solid::DeviceInterface::StorageDrive);
        }
        else if (device.isDeviceInterface(Solid::DeviceInterface::StorageAccess))
        {
            typeStr = Solid::DeviceInterface::typeToString(Solid::DeviceInterface::StorageAccess);
        }
        else if (device.isDeviceInterface(Solid::DeviceInterface::StorageVolume))
        {
            typeStr = Solid::DeviceInterface::typeToString(Solid::DeviceInterface::StorageVolume);
        }
        else if (device.isDeviceInterface(Solid::DeviceInterface::OpticalDrive))
        {
            typeStr = Solid::DeviceInterface::typeToString(Solid::DeviceInterface::OpticalDrive);
        }
        else if (device.isDeviceInterface(Solid::DeviceInterface::OpticalDisc))
        {
            typeStr = Solid::DeviceInterface::typeToString(Solid::DeviceInterface::OpticalDisc);
        }
        else if (device.isDeviceInterface(Solid::DeviceInterface::Camera))
        {
            typeStr = Solid::DeviceInterface::typeToString(Solid::DeviceInterface::Camera);
        }
        else if (device.isDeviceInterface(Solid::DeviceInterface::Processor))
        {
            typeStr = Solid::DeviceInterface::typeToString(Solid::DeviceInterface::Processor);
        }
        else if (device.isDeviceInterface(Solid::DeviceInterface::Block))
        {
            typeStr = Solid::DeviceInterface::typeToString(Solid::DeviceInterface::Block);
        }
        else if (device.isDeviceInterface(Solid::DeviceInterface::PortableMediaPlayer))
        {
            typeStr = Solid::DeviceInterface::typeToString(Solid::DeviceInterface::PortableMediaPlayer);
        }
        else if (device.isDeviceInterface(Solid::DeviceInterface::NetworkShare))
        {
            typeStr = Solid::DeviceInterface::typeToString(Solid::DeviceInterface::NetworkShare);
        }
        else if (device.isDeviceInterface(Solid::DeviceInterface::Unknown))
        {
            typeStr = Solid::DeviceInterface::typeToString(Solid::DeviceInterface::Unknown);
        }

        if (!typeStr.isEmpty())
        {
            QList<QTreeWidgetItem*> lst = listView()->findItems(typeStr, Qt::MatchExactly);
            QTreeWidgetItem* hitem      = nullptr;

            if (!lst.isEmpty())
            {
                hitem = lst[0];
            }
            else
            {
                hitem = new QTreeWidgetItem(listView(), QStringList() << typeStr);
                hitem->setData(0, Qt::UserRole, 0);
                listView()->addTopLevelItem(hitem);
            }

            QTreeWidgetItem* vitem       = nullptr;
            QTreeWidgetItem* const titem = new QTreeWidgetItem(hitem, QStringList() << 

#if (SOLID_VERSION >= QT_VERSION_CHECK(5, 71, 0))

                device.displayName()

#else

                device.udi().section(QLatin1Char('/'), -1)

#endif

            );
            titem->setData(0, Qt::UserRole, 1);

            vitem = new QTreeWidgetItem(titem, QStringList() << i18n("Udi")         << (device.udi().isEmpty()         ? i18n("empty") : device.udi()));
            vitem->setData(0, Qt::UserRole, 2);

            vitem = new QTreeWidgetItem(titem, QStringList() << i18n("Parent Udi")  << (device.parentUdi().isEmpty()   ? i18n("none")  : device.parentUdi()));
            vitem->setData(0, Qt::UserRole, 2);

            vitem = new QTreeWidgetItem(titem, QStringList() << i18n("Vendor")      << (device.vendor().isEmpty()      ? i18n("empty") : device.vendor()));
            vitem->setData(0, Qt::UserRole, 2);

            vitem = new QTreeWidgetItem(titem, QStringList() << i18n("Product")     << (device.product().isEmpty()     ? i18n("empty") : device.product()));
            vitem->setData(0, Qt::UserRole, 2);

            vitem = new QTreeWidgetItem(titem, QStringList() << i18n("Description") << (device.description().isEmpty() ? i18n("empty") : device.description()));
            vitem->setData(0, Qt::UserRole, 2);

            vitem = new QTreeWidgetItem(titem, QStringList() << i18n("States")      << (device.emblems().isEmpty()     ? i18n("none")  : device.emblems().join(QLatin1String(", "))));
            vitem->setData(0, Qt::UserRole, 2);

            if (device.is<Solid::GenericInterface>())
            {
                QTreeWidgetItem* const vitem = new QTreeWidgetItem(titem, QStringList() << i18n("Properties"));
                vitem->setData(0, Qt::UserRole, 2);

                QMap<QString, QVariant> properties = device.as<Solid::GenericInterface>()->allProperties();

                for (auto it = properties.constBegin() ; it != properties.constEnd() ; ++it)
                {
                    QTreeWidgetItem* const pitem = new QTreeWidgetItem(vitem, QStringList() << it.key() << it.value().toString());
                    pitem->setData(0, Qt::UserRole, 3);
                }
            }
        }
    }
}

void SolidHardwareDlg::slotSearchTextChanged(const SearchTextSettings& settings)
{
    bool query     = false;
    int  results   = 0;
    QString search = settings.text.toLower();

    QTreeWidgetItemIterator it(listView());

    while (*it)
    {
        QTreeWidgetItem* const item  = *it;

        if (item->text(0).toLower().contains(search, settings.caseSensitive) ||
            item->text(1).toLower().contains(search, settings.caseSensitive))
        {
            ++results;
            query = true;
            item->setHidden(false);
        }
        else
        {
            item->setHidden(true);
        }

        ++it;
    }

    d->searchBar->slotSearchResult(query);
}

void SolidHardwareDlg::slotCopy2ClipBoard()
{
    QString textInfo;

    textInfo.append(QApplication::applicationName());
    textInfo.append(QLatin1String(" version "));
    textInfo.append(QApplication::applicationVersion());
    textInfo.append(QLatin1Char('\n'));
    textInfo.append(QLatin1String("Solid framework version "));
    textInfo.append(QLatin1String(SOLID_VERSION_STRING));
    textInfo.append(QLatin1Char('\n'));

    QTreeWidgetItemIterator it(listView());

    while (*it)
    {
        int id = (*it)->data(0, Qt::UserRole).toInt();
        textInfo.append(QString().fill(QLatin1Char(' '), id*3));
        textInfo.append((*it)->text(0));
        textInfo.append(QLatin1String(": "));
        textInfo.append((*it)->text(1));
        textInfo.append(QLatin1Char('\n'));
        ++it;
    }

    QMimeData* const mimeData = new QMimeData();
    mimeData->setText(textInfo);
    QApplication::clipboard()->setMimeData(mimeData, QClipboard::Clipboard);
}


} // namespace Digikam
