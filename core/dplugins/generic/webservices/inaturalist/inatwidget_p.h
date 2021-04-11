/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-03-20
 * Description : a tool to export images to iNaturalist web service
 *
 * Copyright (C) 2021      by Joerg Lohse <joergmlpts at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_INAT_WIDGET_PRIVATE_H
#define DIGIKAM_INAT_WIDGET_PRIVATE_H

#include "inatwidget.h"

// Qt includes

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QApplication>
#include <QStyle>
#include <QDialog>
#include <QPlainTextEdit>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "wscomboboxdelegate.h"
#include "wscomboboxintermediate.h"
#include "ditemslist.h"
#include "inatsuggest.h"

using namespace Digikam;

namespace DigikamGenericINatPlugin
{

extern const DItemsListView::ColumnType ItemDate;
extern const DItemsListView::ColumnType ItemLocation;

class Q_DECL_HIDDEN INatWidget::Private
{
public:

    explicit Private()
        : removeAccount           (nullptr),
          accountIcon             (nullptr),
          identificationImage     (nullptr),
          identificationLabel     (nullptr),
          closestKnownObservation (nullptr),
          observationDescription  (nullptr),
          identificationEdit      (nullptr),
          taxonPopup              (nullptr),
          photoMaxTimeDiffSpB     (nullptr),
          photoMaxDistanceSpB     (nullptr),
          closestObservationMaxSpB(nullptr),
          placesComboBox          (nullptr),
          moreOptionsButton       (nullptr),
          moreOptionsWidget       (nullptr),
          imglst                  (nullptr)
    {
    }

    QString                 serviceName;
    QPushButton*            removeAccount;
    QLabel*                 accountIcon;
    QLabel*                 identificationImage;
    QLabel*                 identificationLabel;
    QLabel*                 closestKnownObservation;
    QPlainTextEdit*         observationDescription;
    TaxonEdit*              identificationEdit;
    SuggestTaxonCompletion* taxonPopup;
    QSpinBox*               photoMaxTimeDiffSpB;
    QSpinBox*               photoMaxDistanceSpB;
    QSpinBox*               closestObservationMaxSpB;
    QComboBox*              placesComboBox;
    QPushButton*            moreOptionsButton;
    QWidget*                moreOptionsWidget;
    DItemsList*             imglst;
};

} // namespace DigikamGenericINatPlugin

#endif // DIGIKAM_INAT_WIDGET_PRIVATE_H
