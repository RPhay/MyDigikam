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

// C++ includes

#include <sstream>
#include <iostream>

// Qt includes

#include <QStringList>
#include <QString>
#include <QLabel>
#include <QLayout>
#include <QGridLayout>
#include <QTreeWidget>
#include <QHeaderView>
#include <QMetaEnum>
#include <QTextStream>

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
      : header   (nullptr),
        searchBar(nullptr)
    {
    }

    QLabel*        header;
    SearchTextBar* searchBar;
};

std::ostream& operator<<(std::ostream& out, const QString& msg)
{
    return (out << msg.toLocal8Bit().constData());
}

std::ostream& operator<<(std::ostream& out, const QVariant& value)
{
    switch (value.type())
    {
        case QVariant::StringList:
        {
            out << "{";

            const QStringList list         = value.toStringList();
            QStringList::ConstIterator it  = list.constBegin();
            QStringList::ConstIterator end = list.constEnd();

            for ( ; it != end ; ++it)
            {
                out << "'" << *it << "'";

                if (it + 1 != end)
                {
                    out << ", ";
                }
            }

            out << "}  (string list)";

            break;
        }

        case QVariant::Bool:
        {
            out << (value.toBool()?"true":"false") << "  (bool)";
            break;
        }

        case QVariant::Int:
        case QVariant::LongLong:
        {
            out << value.toString() << "  (0x" << QString::number(value.toLongLong(), 16) << ")  (" << QVariant::typeToName(value.type()) << ")";
            break;
        }

        case QVariant::UInt:
        case QVariant::ULongLong:
        {
            out << value.toString() << "  (0x" << QString::number(value.toULongLong(), 16) << ")  (" << QVariant::typeToName(value.type()) << ")";
            break;
        }

        case QVariant::Double:
        {
            out << value.toString() << " (double)";
            break;
        }

        case QVariant::UserType:
        {
            //qDebug() << "got variant type:" << value.typeName();
            if (value.canConvert<QList<int> >())
            {
                const QList<int> intlist = value.value<QList<int>>();
                QStringList tmp;
                for (const int val : intlist)
                {
                    tmp.append(QString::number(val));
                }

                out << "{" << tmp.join(QLatin1String(",")) << "} (int list)";
            }

            break;
        }

        default:
        {
            out << "'" << value.toString() << "'  (string)";
            break;
        }
    }

    return out;
}

std::ostream& operator<<(std::ostream& out, const Solid::Device& device)
{
    out << "  parent = "      << QVariant(device.parentUdi())   << endl;
    out << "  vendor = "      << QVariant(device.vendor())      << endl;
    out << "  product = "     << QVariant(device.product())     << endl;
    out << "  description = " << QVariant(device.description()) << endl;

    int index          = Solid::DeviceInterface::staticMetaObject.indexOfEnumerator("Type");
    QMetaEnum typeEnum = Solid::DeviceInterface::staticMetaObject.enumerator(index);

    for (int i = 0 ; i < typeEnum.keyCount() ; ++i)
    {
        Solid::DeviceInterface::Type type       = (Solid::DeviceInterface::Type)typeEnum.value(i);
        const Solid::DeviceInterface* interface = device.asDeviceInterface(type);

        if (interface)
        {
            const QMetaObject* meta = interface->metaObject();

            for (int i = meta->propertyOffset() ; i < meta->propertyCount() ; ++i)
            {
                QMetaProperty property = meta->property(i);
                out << "  " << QString::fromLatin1(meta->className()).mid(7) << "." << property.name() << " = ";

                QVariant value = property.read(interface);

                if (property.isEnumType())
                {
                    QMetaEnum metaEnum = property.enumerator();

                    if (metaEnum.isFlag())
                    {
                        out << "'" << metaEnum.valueToKeys(value.toInt()).constData() << "'"
                            << "  (0x" << QString::number(value.toInt(), 16) << ")  (flag)";
                    }
                    else
                    {
                        out << "'" << metaEnum.valueToKey(value.toInt()) << "'"
                            << "  (0x" << QString::number(value.toInt(), 16) << ")  (enum)";
                    }

                    out << endl;

                    
                }
                else
                {
                    out << value << endl;
                }
            }
        }
    }

    return out;
}

std::ostream& operator<<(std::ostream& out, const QMap<QString, QVariant>& properties)
{
    for (auto it = properties.cbegin() ; it != properties.cend( ); ++it)
    {
        out << "  " << it.key() << " = " << it.value() << endl;
    }

    return out;
}

SolidHardwareDlg::SolidHardwareDlg(QWidget* const parent)
    : InfoDlg(parent),
      d      (new Private)
{
    setWindowTitle(i18n("List of detected hardware"));

    QStringList list;

    const QList<Solid::Device> all = Solid::Device::allDevices();

    for (const Solid::Device& device : all)
    {
        QString           temp;
        QTextStream       stream(&temp);
        std::stringstream out;
        stream << QLatin1String("udi = '") << device.udi() << QLatin1String("'\n");
        out    << device;
        stream << QString::fromStdString(out.str());
        stream << QLatin1String("\n");

        QStringList strlst = temp.split(QLatin1Char('\n'));

        for (const QString& str : strlst)
        {
            if (!str.isEmpty())
            {
                list << str;
            }
        }

        list << QString();
    }

    // --------------------------------------------------------

    d->header    = new QLabel(this);
    d->header->setText(i18n("Using Solid version %1", QLatin1String(SOLID_VERSION_STRING)));
    d->searchBar = new SearchTextBar(this, QLatin1String("SolidHardwareDlgSearchBar"));

    listView()->setColumnCount(1);
    listView()->setHeaderLabels(QStringList() << QLatin1String("UDI")); // Header is hidden. No i18n here.
    listView()->header()->hide();

    for (QStringList::const_iterator it = list.constBegin() ; it != list.constEnd() ; ++it)
    {
        new QTreeWidgetItem(listView(), QStringList() << *it);
    }

    // --------------------------------------------------------

    QGridLayout* const  grid = dynamic_cast<QGridLayout*>(mainWidget()->layout());
    grid->addWidget(d->header,    1, 0, 1, -1);
    grid->addWidget(d->searchBar, 3, 0, 1, -1);

    // --------------------------------------------------------

    connect(d->searchBar, SIGNAL(signalSearchTextSettings(SearchTextSettings)),
            this, SLOT(slotSearchTextChanged(SearchTextSettings)));
}

SolidHardwareDlg::~SolidHardwareDlg()
{
    delete d;
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

        if (item->text(0).toLower().contains(search, settings.caseSensitive))
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

} // namespace Digikam
