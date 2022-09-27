/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-07-07
 * Description : a tool to export images to Flickr web service
 *
 * SPDX-FileCopyrightText: 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * SPDX-FileCopyrightText: 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009      by Luka Renko <lure at kubuntu dot org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FLICKR_WIDGET_PRIVATE_H
#define DIGIKAM_FLICKR_WIDGET_PRIVATE_H

#include "flickrwidget.h"

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

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "wscomboboxdelegate.h"
#include "wscomboboxintermediate.h"
#include "flickrlist.h"
#include "dtextedit.h"

using namespace Digikam;

namespace DigikamGenericFlickrPlugin
{

class Q_DECL_HIDDEN FlickrWidget::Private
{
public:

    explicit Private()
      : removeAccount               (nullptr),
        extendedTagsButton          (nullptr),
        extendedPublicationButton   (nullptr),
        exportHostTagsCheckBox      (nullptr),
        stripSpaceTagsCheckBox      (nullptr),
        addExtraTagsCheckBox        (nullptr),
        familyCheckBox              (nullptr),
        friendsCheckBox             (nullptr),
        publicCheckBox              (nullptr),
        extendedTagsBox             (nullptr),
        extendedPublicationBox      (nullptr),
        tagsLineEdit                (nullptr),
        contentTypeComboBox         (nullptr),
        safetyLevelComboBox         (nullptr),
        imglst                      (nullptr)
    {
    }

    QString                 serviceName;
    QPushButton*            removeAccount;
    QPushButton*            extendedTagsButton;
    QPushButton*            extendedPublicationButton;

    QCheckBox*              exportHostTagsCheckBox;
    QCheckBox*              stripSpaceTagsCheckBox;
    QCheckBox*              addExtraTagsCheckBox;
    QCheckBox*              familyCheckBox;
    QCheckBox*              friendsCheckBox;
    QCheckBox*              publicCheckBox;

    QGroupBox*              extendedTagsBox;
    QGroupBox*              extendedPublicationBox;

    DTextEdit*              tagsLineEdit;

    WSComboBoxIntermediate* contentTypeComboBox;
    WSComboBoxIntermediate* safetyLevelComboBox;

    FlickrList*             imglst;
};

} // namespace DigikamGenericFlickrPlugin

#endif // DIGIKAM_FLICKR_WIDGET_PRIVATE_H
