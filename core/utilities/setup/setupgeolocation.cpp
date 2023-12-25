/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-12-24
 * Description : geo-location setup page.
 *
 * SPDX-FileCopyrightText: 2022-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "setupgeolocation.h"

// Qt includes

#include <QApplication>
#include <QLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QList>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "MarbleConfigView.h"
#include "marblesettings.h"
#include "digikam_debug.h"

using namespace Marble;

namespace Digikam
{

class Q_DECL_HIDDEN SetupGeolocation::Private
{
public:

    Private() = default;

    MarbleConfigView* tab = nullptr;
};

SetupGeolocation::SetupGeolocation(QWidget* const parent)
    : QScrollArea(parent),
      d          (new Private)
{
    QWidget* const panel   = new QWidget(viewport());
    setWidget(panel);
    setWidgetResizable(true);

    QVBoxLayout* const vbl = new QVBoxLayout();
    panel->setLayout(vbl);

    d->tab                 = new MarbleConfigView(MarbleSettings::instance()->mainMarbleWidget(), this);

    vbl->addWidget(d->tab);

    readSettings();
}

SetupGeolocation::~SetupGeolocation()
{
    delete d;
}

void SetupGeolocation::setActiveTab(GeolocationTab tab)
{
    d->tab->setCurrentIndex(tab);
}

SetupGeolocation::GeolocationTab SetupGeolocation::activeTab() const
{
    return (GeolocationTab)d->tab->currentIndex();
}

void SetupGeolocation::applySettings()
{
    d->tab->applySettings();
}

void SetupGeolocation::readSettings()
{
    d->tab->readSettings();
}

} // namespace Digikam

#include "moc_setupgeolocation.cpp"
