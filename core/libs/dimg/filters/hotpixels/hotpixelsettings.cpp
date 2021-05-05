/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-08-05)
 * Description : HotPixel settings view.
 *
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "hotpixelsettings.h"

// Qt includes

#include <QGridLayout>
#include <QLabel>
#include <QString>
#include <QIcon>
#include <QTextStream>
#include <QPushButton>
#include <QApplication>
#include <QStyle>
#include <QPointer>
#include <QMessageBox>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes
 
#include "digikam_debug.h"
#include "hotpixelfixer.h"
#include "dcombobox.h"
#include "imagedialog.h"
#include "blackframelistview.h"
#include "blackframelistviewitem.h"

namespace Digikam
{

class Q_DECL_HIDDEN HotPixelSettings::Private
{
public:

    explicit Private()
      : blackFrameButton  (nullptr),
        filterMethodCombo (nullptr),
        blackFrameListView(nullptr)
    {
    }

    static const QString configGroupName;
    static const QString configAllBlackFrameFilesEntry;
    static const QString configCurrentBlackFrameFileEntry;
    static const QString configHotPixelsListEntry;
    static const QString configFilterMethodEntry;

    QPushButton*         blackFrameButton;
    DComboBox*           filterMethodCombo;
    BlackFrameListView*  blackFrameListView;

    QList<HotPixelProps> hotPixelsList;
    QUrl                 currentBlackFrameUrl;
    QList<QUrl>          allBlackFrameUrls;
};

const QString HotPixelSettings::Private::configGroupName(QLatin1String("hotpixels Tool"));
const QString HotPixelSettings::Private::configAllBlackFrameFilesEntry(QLatin1String("All Black Frame Files"));
const QString HotPixelSettings::Private::configCurrentBlackFrameFileEntry(QLatin1String("Current Black Frame File"));
const QString HotPixelSettings::Private::configHotPixelsListEntry(QLatin1String("Hot Pixels List"));
const QString HotPixelSettings::Private::configFilterMethodEntry(QLatin1String("Filter Method"));

// --------------------------------------------------------

HotPixelSettings::HotPixelSettings(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    const int spacing               = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);
    QGridLayout* const grid         = new QGridLayout(parent);

    QLabel* const filterMethodLabel = new QLabel(i18n("Filter:"), this);
    d->filterMethodCombo            = new DComboBox(this);
    d->filterMethodCombo->addItem(i18nc("average filter mode",   "Average"));
    d->filterMethodCombo->addItem(i18nc("linear filter mode",    "Linear"));
    d->filterMethodCombo->addItem(i18nc("quadratic filter mode", "Quadratic"));
    d->filterMethodCombo->addItem(i18nc("cubic filter mode",     "Cubic"));
    d->filterMethodCombo->setDefaultIndex(HotPixelContainer::QUADRATIC_INTERPOLATION);

    d->blackFrameButton   = new QPushButton(i18n("Black Frame..."), this);
    d->blackFrameButton->setIcon(QIcon::fromTheme(QLatin1String("document-open")));
    d->blackFrameButton->setWhatsThis(i18n("<p>Use this button to add a new black frame file which will "
                                           "be used by the hot pixels removal filter.</p>"
                                           "<p>The Black Frame subtraction method is the most accurate "
                                           "\"Hot Pixels\" and \"Stuck Pixels\" removal. First you have to "
                                           "create a \"Black Frame\" as a reference. This is easy to do. "
                                           "When you finish taking your long exposure shots, put a lens cap "
                                           "on the camera and take one \"dark\" image with the same exposure "
                                           "time as the images before. This image will be all dark, but with "
                                           "close examination you will see that it has the Hot and Stuck Pixels "
                                           "(colored dots). These are positioned at the same places as on your "
                                           "previous shots. This tool will parse black frame, detect hot pixels, "
                                           "and create a list of positions where image must be corrected to "
                                           "eliminate wrong colors in these pixels.</p>"));

    d->blackFrameListView = new BlackFrameListView(this);

    // -------------------------------------------------------------

    grid->addWidget(filterMethodLabel,     0, 0, 1, 1);
    grid->addWidget(d->filterMethodCombo,  0, 1, 1, 1);
    grid->addWidget(d->blackFrameButton,   0, 2, 1, 1);
    grid->addWidget(d->blackFrameListView, 1, 0, 2, 3);
    grid->setRowStretch(2, 10);
    grid->setContentsMargins(spacing, spacing, spacing, spacing);
    grid->setSpacing(spacing);

    // -------------------------------------------------------------

    connect(d->blackFrameButton, SIGNAL(clicked()),
            this, SLOT(slotAddBlackFrame()));

    connect(d->blackFrameListView, SIGNAL(signalBlackFrameSelected(QList<HotPixelProps>,QUrl)),
            this, SLOT(slotBlackFrameSelected(QList<HotPixelProps>,QUrl)));

    connect(d->blackFrameListView, SIGNAL(signalBlackFrameRemoved(QUrl)),
            this, SLOT(slotBlackFrameRemoved(QUrl)));

    connect(d->blackFrameListView, SIGNAL(signalClearBlackFrameList()),
            this, SLOT(slotClearBlackFrameList()));
}

HotPixelSettings::~HotPixelSettings()
{
    delete d;
}

QString HotPixelSettings::configGroupName() const
{
    return d->configGroupName;
}

HotPixelContainer HotPixelSettings::settings() const
{
    HotPixelContainer prm;
    prm.blackFrameUrl = d->currentBlackFrameUrl;
    prm.hotPixelsList = d->hotPixelsList;
    prm.filterMethod  = (HotPixelContainer::InterpolationMethod)d->filterMethodCombo->currentIndex();

    return prm;
}

void HotPixelSettings::setSettings(const HotPixelContainer& settings)
{
    blockSignals(true);
    d->currentBlackFrameUrl = settings.blackFrameUrl;
    d->hotPixelsList        = settings.hotPixelsList;
    d->filterMethodCombo->setCurrentIndex(settings.filterMethod);
    blockSignals(false);
}

void HotPixelSettings::resetToDefault()
{
    blockSignals(true);
    d->currentBlackFrameUrl = QUrl();
    d->hotPixelsList        = QList<HotPixelProps>();
    d->filterMethodCombo->slotReset();
    blockSignals(false);
}

HotPixelContainer HotPixelSettings::defaultSettings() const
{
    return HotPixelContainer();
}

void HotPixelSettings::readSettings(KConfigGroup& group)
{
    d->allBlackFrameUrls         = group.readEntry(d->configAllBlackFrameFilesEntry, QList<QUrl>());

    HotPixelContainer prm;
    HotPixelContainer defaultPrm = defaultSettings();

    prm.blackFrameUrl            = group.readEntry(d->configCurrentBlackFrameFileEntry,
                                                   defaultPrm.blackFrameUrl);

    QStringList hplst            = group.readEntry(d->configHotPixelsListEntry, QStringList());
    prm.hotPixelsList            = HotPixelProps::fromStringList(hplst);

    prm.filterMethod             = (HotPixelContainer::InterpolationMethod)group.readEntry(d->configFilterMethodEntry, (int)defaultPrm.filterMethod);

    setSettings(prm);

    foreach (const QUrl& url, d->allBlackFrameUrls)
    {
        if (url.isValid() && !d->blackFrameListView->contains(url))
        {
            loadBlackFrame(url, (url == d->currentBlackFrameUrl));
        }
    }
}

void HotPixelSettings::writeSettings(KConfigGroup& group)
{
    group.writeEntry(d->configAllBlackFrameFilesEntry,    d->allBlackFrameUrls);

    HotPixelContainer prm = settings();

    group.writeEntry(d->configCurrentBlackFrameFileEntry, prm.blackFrameUrl);

    QStringList hplst     = HotPixelProps::toStringList(prm.hotPixelsList);
    group.writeEntry(d->configHotPixelsListEntry,         hplst);

    group.writeEntry(d->configFilterMethodEntry,          (int)prm.filterMethod);
}

void HotPixelSettings::slotAddBlackFrame()
{
    QUrl url = ImageDialog::getImageURL(qApp->activeWindow(), d->currentBlackFrameUrl, i18n("Select Black Frame Image"));

    if (!url.isEmpty())
    {
        if (d->blackFrameListView->contains(url))
        {
            QMessageBox::information(this, i18n("Black Frame Parser"),
                                     i18n("This black frame image is already present in the list of parsed items."));

            return;
        }

        // Load the selected file and insert into the list.

        d->allBlackFrameUrls << url;
        d->currentBlackFrameUrl = url;
        loadBlackFrame(url, true);
    }
}

void HotPixelSettings::loadBlackFrame(const QUrl& url, bool selected)
{
    QPointer<BlackFrameListViewItem> item = new BlackFrameListViewItem(d->blackFrameListView, url);

    if (selected)
    {
        d->blackFrameListView->setCurrentItem(item);
    }
}

void HotPixelSettings::slotBlackFrameSelected(const QList<HotPixelProps>& hpList, const QUrl& url)
{
    if (d->blackFrameListView->isSelected(url))
    {
        d->currentBlackFrameUrl = url;
        d->hotPixelsList        = hpList;
        int i                   = 0;

        QPolygon pointList(d->hotPixelsList.size());
        QList <HotPixelProps>::const_iterator it;

        for (it = d->hotPixelsList.constBegin() ; it != d->hotPixelsList.constEnd() ; ++it, ++i)
        {
            pointList.setPoint(i, (*it).rect.center());
        }

        emit signalHotPixels(pointList);
    }
}

void HotPixelSettings::slotBlackFrameRemoved(const QUrl& url)
{
    d->allBlackFrameUrls.removeAll(url);
    d->currentBlackFrameUrl = d->blackFrameListView->currentUrl();
}

void HotPixelSettings::slotClearBlackFrameList()
{
    d->allBlackFrameUrls.clear();
    d->currentBlackFrameUrl = QUrl();
}

} // namespace Digikam
