/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-21
 * Description : setup tab for slideshow options.
 *
 * Copyright (C) 2005-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2019 by Minh Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#include "setupslideshow.h"

// Qt includes

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QApplication>
#include <QScreen>
#include <QStyle>
#include <QComboBox>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dlayoutbox.h"
#include "dnuminput.h"
#include "dfontselect.h"
#include "digikam_debug.h"
#include "slideshowsettings.h"

namespace Digikam
{

class Q_DECL_HIDDEN SetupSlideShow::Private
{
public:

    explicit Private()
      : startWithCurrent(nullptr),
        loopMode(nullptr),
        suffleMode(nullptr),
        showName(nullptr),
        showDate(nullptr),
        showApertureFocal(nullptr),
        showExpoSensitivity(nullptr),
        showMakeModel(nullptr),
        showLabels(nullptr),
        showRating(nullptr),
        showComment(nullptr),
        showTitle(nullptr),
        showTags(nullptr),
        showCapIfNoTitle(nullptr),
        showProgress(nullptr),
        screenPlacement(nullptr),
        captionFont(nullptr),
        delayInput(nullptr)
    {
    }

    QCheckBox*    startWithCurrent;
    QCheckBox*    loopMode;
    QCheckBox*    suffleMode;
    QCheckBox*    showName;
    QCheckBox*    showDate;
    QCheckBox*    showApertureFocal;
    QCheckBox*    showExpoSensitivity;
    QCheckBox*    showMakeModel;
    QCheckBox*    showLabels;
    QCheckBox*    showRating;
    QCheckBox*    showComment;
    QCheckBox*    showTitle;
    QCheckBox*    showTags;
    QCheckBox*    showCapIfNoTitle;
    QCheckBox*    showProgress;

    QComboBox*    screenPlacement;

    DFontSelect*  captionFont;
    DIntNumInput* delayInput;
};

// --------------------------------------------------------

SetupSlideShow::SetupSlideShow(QWidget* const parent)
    : QScrollArea(parent),
      d(new Private)
{
    QWidget* const panel      = new QWidget(viewport());
    setWidget(panel);
    setWidgetResizable(true);

    const int spacing         = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    DHBox* const hbox1        = new DHBox(panel);
    new QLabel(i18n("Delay between images:"), hbox1);
    d->delayInput             = new DIntNumInput(hbox1);
    d->delayInput->setDefaultValue(5);
    d->delayInput->setRange(1, 3600, 1);
    d->delayInput->setWhatsThis(i18n("The delay, in seconds, between images."));

    d->startWithCurrent       = new QCheckBox(i18n("Start with current image"), panel);
    d->startWithCurrent->setWhatsThis(i18n("If this option is enabled, the Slideshow will be started "
                                           "with the current image selected in the images list."));

    d->loopMode               = new QCheckBox(i18n("Slideshow runs in a loop"), panel);
    d->loopMode->setWhatsThis(i18n("Run the slideshow in a loop."));

    d->suffleMode             = new QCheckBox(i18n("Shuffle images"), panel);
    d->suffleMode->setWhatsThis(i18n("If this option is enabled, the Slideshow will run in random order"));

    d->showProgress           = new QCheckBox(i18n("Show progress indicator"), panel);
    d->showProgress->setWhatsThis(i18n("Show a progress indicator with pending items to show and time progression."));

    d->showName               = new QCheckBox(i18n("Show image file name"), panel);
    d->showName->setWhatsThis(i18n("Show the image file name at the bottom of the screen."));

    d->showDate               = new QCheckBox(i18n("Show image creation date"), panel);
    d->showDate->setWhatsThis(i18n("Show the image creation time/date at the bottom of the screen."));

    d->showApertureFocal      = new QCheckBox(i18n("Show camera aperture and focal length"), panel);
    d->showApertureFocal->setWhatsThis(i18n("Show the camera aperture and focal length at the bottom of the screen."));

    d->showExpoSensitivity    = new QCheckBox(i18n("Show camera exposure and sensitivity"), panel);
    d->showExpoSensitivity->setWhatsThis(i18n("Show the camera exposure and sensitivity at the bottom of the screen."));

    d->showMakeModel          = new QCheckBox(i18n("Show camera make and model"), panel);
    d->showMakeModel->setWhatsThis(i18n("Show the camera make and model at the bottom of the screen."));

    d->showComment            = new QCheckBox(i18n("Show image caption"), panel);
    d->showComment->setWhatsThis(i18n("Show the image caption at the bottom of the screen."));

    d->showTitle              = new QCheckBox(i18n("Show image title"), panel);
    d->showTitle->setWhatsThis(i18n("Show the image title at the bottom of the screen."));

    d->showCapIfNoTitle       = new QCheckBox(i18n("Show image caption if it has not title"), panel);
    d->showCapIfNoTitle->setWhatsThis(i18n("Show the image caption at the bottom of the screen if no titles existed."));

    d->showTags               = new QCheckBox(i18n("Show image tags"), panel);
    d->showTags->setWhatsThis(i18n("Show the digiKam image tag names at the bottom of the screen."));

    d->showLabels             = new QCheckBox(i18n("Show image labels"), panel);
    d->showLabels->setWhatsThis(i18n("Show the digiKam image color label and pick label at the bottom of the screen."));

    d->showRating             = new QCheckBox(i18n("Show image rating"), panel);
    d->showRating->setWhatsThis(i18n("Show the digiKam image rating at the bottom of the screen."));

    d->captionFont            = new DFontSelect(i18n("Caption font:"), panel);
    d->captionFont->setToolTip(i18n("Select here the font used to display text in the slideshow."));

    DHBox* const screenSelectBox = new DHBox(panel);
    new QLabel(i18n("Screen placement:"), screenSelectBox);
    d->screenPlacement           = new QComboBox(screenSelectBox);
    d->screenPlacement->setToolTip(i18n("In case of multi-screen computer, select here the monitor to slide contents."));

    QStringList choices;
    choices.append(i18nc("@label:listbox The current screen, for the presentation mode", "Current Screen"));
    choices.append(i18nc("@label:listbox The default screen for the presentation mode",  "Default Screen"));

    for (int i = 0 ; i < qApp->screens().count() ; ++i)
    {
        QString model = qApp->screens().at(i)->model();
        choices.append(i18nc("@label:listbox %1 is the screen number (0, 1, ...)", "Screen %1", i) +
                             QString::fromUtf8(" (%1)").arg(model.left(model.length() - 1)));
    }

    d->screenPlacement->addItems(choices);

    // Disable and uncheck the "Show captions if no title" checkbox if the "Show comment" checkbox enabled

    connect(d->showComment, SIGNAL(stateChanged(int)),
            this, SLOT(slotSetUnchecked(int)));

    connect(d->showComment, SIGNAL(toggled(bool)),
            d->showCapIfNoTitle, SLOT(setDisabled(bool)));

    // Only digiKam support this feature, showFoto do not support digiKam database information.

    if (qApp->applicationName() == QLatin1String("showfoto"))
    {
        d->showCapIfNoTitle->hide();
        d->showLabels->hide();
        d->showRating->hide();
        d->showTitle->hide();
        d->showTags->hide();
    }

    QGridLayout* const grid   = new QGridLayout(panel);
    grid->addWidget(hbox1,                   0, 0, 1, 2);
    grid->addWidget(d->startWithCurrent,     1, 0, 1, 1);
    grid->addWidget(d->loopMode,             1, 1, 1, 1);
    grid->addWidget(d->suffleMode,           2, 0, 1, 1);
    grid->addWidget(d->showProgress,         2, 1, 1, 1);
    grid->addWidget(d->showName,             3, 0, 1, 1);
    grid->addWidget(d->showDate,             3, 1, 1, 1);
    grid->addWidget(d->showApertureFocal,    4, 0, 1, 1);
    grid->addWidget(d->showExpoSensitivity,  4, 1, 1, 1);
    grid->addWidget(d->showMakeModel,        5, 0, 1, 1);
    grid->addWidget(d->showComment,          5, 1, 1, 1);
    grid->addWidget(d->showTitle,            6, 0, 1, 1);
    grid->addWidget(d->showCapIfNoTitle,     6, 1, 1, 1);
    grid->addWidget(d->showTags,             7, 0, 1, 1);
    grid->addWidget(d->showLabels,           7, 1, 1, 1);
    grid->addWidget(d->showRating,           8, 0, 1, 1);
    grid->addWidget(d->captionFont,          9, 0, 1, 2);
    grid->addWidget(screenSelectBox,        10, 0, 1, 2);
    grid->setRowStretch(11, 10);
    grid->setContentsMargins(spacing, spacing, spacing, spacing);
    grid->setSpacing(spacing);

    readSettings();
}

SetupSlideShow::~SetupSlideShow()
{
    delete d;
}

void SetupSlideShow::slotSetUnchecked(int)
{
    d->showCapIfNoTitle->setCheckState(Qt::Unchecked);
}

void SetupSlideShow::applySettings()
{
    SlideShowSettings settings;

    settings.delay                 = d->delayInput->value();
    settings.startWithCurrent      = d->startWithCurrent->isChecked();
    settings.loop                  = d->loopMode->isChecked();
    settings.suffle                = d->suffleMode->isChecked();
    settings.printName             = d->showName->isChecked();
    settings.printDate             = d->showDate->isChecked();
    settings.printApertureFocal    = d->showApertureFocal->isChecked();
    settings.printExpoSensitivity  = d->showExpoSensitivity->isChecked();
    settings.printMakeModel        = d->showMakeModel->isChecked();
    settings.printComment          = d->showComment->isChecked();
    settings.printTitle            = d->showTitle->isChecked();
    settings.printCapIfNoTitle     = d->showCapIfNoTitle->isChecked();
    settings.printTags             = d->showTags->isChecked();
    settings.printLabels           = d->showLabels->isChecked();
    settings.printRating           = d->showRating->isChecked();
    settings.showProgressIndicator = d->showProgress->isChecked();
    settings.captionFont           = d->captionFont->font();
    settings.slideScreen           = d->screenPlacement->currentIndex() - 2;
    settings.writeToConfig();
}

void SetupSlideShow::readSettings()
{
    SlideShowSettings settings;
    settings.readFromConfig();

    d->delayInput->setValue(settings.delay);
    d->startWithCurrent->setChecked(settings.startWithCurrent);
    d->loopMode->setChecked(settings.loop);
    d->suffleMode->setChecked(settings.suffle);
    d->showName->setChecked(settings.printName);
    d->showDate->setChecked(settings.printDate);
    d->showApertureFocal->setChecked(settings.printApertureFocal);
    d->showExpoSensitivity->setChecked(settings.printExpoSensitivity);
    d->showMakeModel->setChecked(settings.printMakeModel);
    d->showComment->setChecked(settings.printComment);
    d->showTitle->setChecked(settings.printTitle);
    d->showCapIfNoTitle->setChecked(settings.printCapIfNoTitle);
    d->showTags->setChecked(settings.printTags);
    d->showLabels->setChecked(settings.printLabels);
    d->showRating->setChecked(settings.printRating);
    d->showProgress->setChecked(settings.showProgressIndicator);
    d->captionFont->setFont(settings.captionFont);

    const int screen = settings.slideScreen;

    if (screen >= -2 && screen < qApp->screens().count())
    {
        d->screenPlacement->setCurrentIndex(screen + 2);
    }
    else
    {
        d->screenPlacement->setCurrentIndex(0);
        settings.slideScreen = -2;
        settings.writeToConfig();
    }
}

} // namespace Digikam
