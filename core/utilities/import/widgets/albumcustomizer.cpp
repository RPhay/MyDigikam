/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-08-11
 * Description : a widget to customize album name created by
 *               camera interface.
 *
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "albumcustomizer.h"

// Qt includes

#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QToolButton>
#include <QApplication>
#include <QStyle>
#include <QComboBox>
#include <QLineEdit>
#include <QIcon>

// KDE includes

#include <klocalizedstring.h>
#include <kconfiggroup.h>

// Local includes

#include "dlayoutbox.h"
#include "tooltipdialog.h"
#include "digikam_debug.h"
#include "dexpanderbox.h"

namespace Digikam
{

class Q_DECL_HIDDEN AlbumCustomizer::Private
{
public:

    explicit Private()
        : autoAlbumDateCheck (nullptr),
          autoAlbumExtCheck  (nullptr),
          folderDateLabel    (nullptr),
          customizer         (nullptr),
          tooltipToggleButton(nullptr),
          customExample      (nullptr),
          folderDateFormat   (nullptr),
          tooltipDialog      (nullptr)
    {
    }

    QCheckBox*          autoAlbumDateCheck;
    QCheckBox*          autoAlbumExtCheck;

    QLabel*             folderDateLabel;

    QLineEdit*          customizer;

    QToolButton*        tooltipToggleButton;

    DAdjustableLabel*   customExample;

    QComboBox*          folderDateFormat;

    TooltipDialog*      tooltipDialog;
};

AlbumCustomizer::AlbumCustomizer(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    const int spacing = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    d->tooltipDialog = new TooltipDialog(this);
    d->tooltipDialog->setTooltip(i18nc("@info",
                                       "These expressions may be used to customize date format:\n"
                                       "\"d\": The day as a number without a leading zero (1 to 31)\n"
                                       "\"dd\": The day as a number with a leading zero (01 to 31)\n"
                                       "\"ddd\": The abbreviated localized day name (e.g. 'Mon' to 'Sun')\n"
                                       "\"dddd\": The long localized day name (e.g. 'Monday' to 'Sunday').\n"
                                       "\"M\": The month as a number without a leading zero (1 to 12)\n"
                                       "\"MM\": The month as a number with a leading zero (01 to 12)\n"
                                       "\"MMM\": The abbreviated localized month name (e.g. 'Jan' to 'Dec')\n"
                                       "\"MMMM\": The long localized month name (e.g. 'January' to 'December')\n"
                                       "\"yy\": The year as two digit number (eg. 00 to 99)\n"
                                       "\"yyyy\": The year as four digit number (eg. 2012)\n"
                                       "All other input characters will be treated as text. Any sequence of characters "
                                       "that are enclosed in singlequotes will be treated as text and not be used as an "
                                       "expression. Examples, if date is 20 July 1969:\n"
                                       "\"dd.MM.yyyy\" : 20.07.1969\n"
                                       "\"ddd MMMM d yy\" : Sun July 20 69\n"
                                       "\"'Photo shot on ' dddd\" : Photo shot on Sunday\n"
                                     ));
    d->tooltipDialog->resize(650, 530);

    QVBoxLayout* const albumVlay = new QVBoxLayout(this);
    d->autoAlbumExtCheck         = new QCheckBox(i18nc("@option:check", "Extension-based sub-albums"), this);
    d->autoAlbumDateCheck        = new QCheckBox(i18nc("@option:check", "Date-based sub-albums"), this);
    DHBox* const hbox1           = new DHBox(this);
    d->folderDateLabel           = new QLabel(i18nc("@label:listbox", "Date format:"), hbox1);
    d->folderDateFormat          = new QComboBox(hbox1);
    d->folderDateFormat->insertItem(IsoDateFormat,    i18nc("@item:inlistbox folder date format", "ISO"));
    d->folderDateFormat->insertItem(TextDateFormat,   i18nc("@item:inlistbox folder date format", "Full Text"));
    d->folderDateFormat->insertItem(LocalDateFormat,  i18nc("@item:inlistbox folder date format", "Local Settings"));
    d->folderDateFormat->insertItem(CustomDateFormat, i18nc("@item:inlistbox folder date format", "Custom"));

    DHBox* const hbox2     = new DHBox(this);
    d->customizer          = new QLineEdit(hbox2);
    d->tooltipToggleButton = new QToolButton(hbox2);
    d->tooltipToggleButton->setIcon(QIcon::fromTheme(QLatin1String("dialog-information")));
    d->tooltipToggleButton->setToolTip(i18nc("@info", "Show a list of all available options"));

    d->customExample       = new DAdjustableLabel(this);

    albumVlay->addWidget(d->autoAlbumExtCheck);
    albumVlay->addWidget(d->autoAlbumDateCheck);
    albumVlay->addWidget(hbox1);
    albumVlay->addWidget(hbox2);
    albumVlay->addWidget(d->customExample);
    albumVlay->addStretch();
    albumVlay->setContentsMargins(spacing, spacing, spacing, spacing);
    albumVlay->setSpacing(spacing);

    setWhatsThis(i18nc("@info", "Set how digiKam creates albums automatically when downloading."));
    d->autoAlbumExtCheck->setWhatsThis(i18nc("@info", "Enable this option if you want to download your "
                                            "pictures into automatically created file extension-based sub-albums of the destination "
                                            "album. This way, you can separate JPEG and RAW files as they are downloaded from your camera."));
    d->autoAlbumDateCheck->setWhatsThis(i18nc("@info", "Enable this option if you want to "
                                             "download your pictures into automatically created file date-based sub-albums "
                                             "of the destination album."));
    d->folderDateFormat->setWhatsThis(i18nc("@info", "Select your preferred date format used to\n"
                                           "create new albums. The options available are:\n"
                                           "\"ISO\": the date format is in accordance with ISO 8601\n"
                                           "(YYYY-MM-DD). E.g.: 2006-08-24\n"
                                           "\"Full Text\": the date format is in a user-readable string.\n"
                                           "E.g.: Thu Aug 24 2006\n"
                                           "\"Local Settings\": the date format depending on KDE control panel settings.\n"
                                           "\"Custom\": use a customized format for date."));
    d->customExample->setWhatsThis(i18nc("@info", "Show the result of converted date 1968-12-26 using your customized format."));

    // --------------------------------------------------------------------------------------

    connect(d->autoAlbumDateCheck, SIGNAL(toggled(bool)),
            d->folderDateFormat, SLOT(setEnabled(bool)));

    connect(d->autoAlbumDateCheck, SIGNAL(toggled(bool)),
            d->folderDateLabel, SLOT(setEnabled(bool)));

    connect(d->tooltipToggleButton, SIGNAL(clicked(bool)),
            this, SLOT(slotToolTipButtonToggled(bool)));

    connect(d->folderDateFormat, SIGNAL(activated(int)),
            this, SLOT(slotFolderDateFormatChanged(int)));

    connect(d->customizer, SIGNAL(textChanged(QString)),
            this, SLOT(slotCustomizerChanged()));
}

AlbumCustomizer::~AlbumCustomizer()
{
    delete d;
}

void AlbumCustomizer::readSettings(KConfigGroup& group)
{
    d->autoAlbumDateCheck->setChecked(group.readEntry("AutoAlbumDate",       false));
    d->autoAlbumExtCheck->setChecked(group.readEntry("AutoAlbumExt",         false));
    d->folderDateFormat->setCurrentIndex(group.readEntry("FolderDateFormat", (int)IsoDateFormat));
    d->customizer->setText(group.readEntry("CustomDateFormat",               QString()));

    d->folderDateFormat->setEnabled(d->autoAlbumDateCheck->isChecked());
    d->folderDateLabel->setEnabled(d->autoAlbumDateCheck->isChecked());
    slotFolderDateFormatChanged(d->folderDateFormat->currentIndex());
}

void AlbumCustomizer::saveSettings(KConfigGroup& group)
{
    group.writeEntry("AutoAlbumDate",    d->autoAlbumDateCheck->isChecked());
    group.writeEntry("AutoAlbumExt",     d->autoAlbumExtCheck->isChecked());
    group.writeEntry("FolderDateFormat", d->folderDateFormat->currentIndex());
    group.writeEntry("CustomDateFormat", d->customizer->text());
}

bool AlbumCustomizer::autoAlbumDateEnabled() const
{
    return d->autoAlbumDateCheck->isChecked();
}

bool AlbumCustomizer::autoAlbumExtEnabled() const
{
    return d->autoAlbumExtCheck->isChecked();
}

int AlbumCustomizer::folderDateFormat() const
{
    return d->folderDateFormat->currentIndex();
}

QString AlbumCustomizer::customDateFormat() const
{
    return d->customizer->text();
}

bool AlbumCustomizer::customDateFormatIsValid() const
{
    QDate date(1968, 12, 26);

    return !date.toString(customDateFormat()).isEmpty();
}

void AlbumCustomizer::slotToolTipButtonToggled(bool /*checked*/)
{
    if (!d->tooltipDialog->isVisible())
    {
        d->tooltipDialog->show();
    }

    d->tooltipDialog->raise();
}

void AlbumCustomizer::slotFolderDateFormatChanged(int index)
{
    bool b = (index == CustomDateFormat);
    d->customizer->setEnabled(b);
    d->tooltipToggleButton->setEnabled(b);
    d->customExample->setEnabled(b);
    slotCustomizerChanged();
}

void AlbumCustomizer::slotCustomizerChanged()
{
    if (folderDateFormat() == CustomDateFormat)
    {
        QDate date(1968, 12, 26);

        if (customDateFormatIsValid())
        {
            d->customExample->setAdjustedText(i18nc("@info Example of custom date format for album naming", "Ex.: %1", date.toString(customDateFormat())));
        }
        else
        {
            d->customExample->setAdjustedText(i18nc("@info Custom date format", "Format is not valid..."));
        }
    }
    else
    {
        d->customExample->setAdjustedText();
    }
}

} // namespace Digikam
