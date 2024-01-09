/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-25
 * Description : a tool to generate video slideshow from images.
 *
 * SPDX-FileCopyrightText: 2017-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "vidslideoutputpage.h"

// Qt includes

#include <QIcon>
#include <QLabel>
#include <QUrl>
#include <QTimer>
#include <QWidget>
#include <QApplication>
#include <QStyle>
#include <QComboBox>
#include <QGridLayout>
#include <QStandardItemModel>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_config.h"
#include "digikam_debug.h"
#include "vidslidewizard.h"
#include "dfileselector.h"
#include "filesaveconflictbox.h"
#include "ffmpeglauncher.h"

namespace DigikamGenericVideoSlideShowPlugin
{

class Q_DECL_HIDDEN VidSlideOutputPage::Private
{
public:

    explicit Private(QWizard* const dialog)
    {
        wizard = dynamic_cast<VidSlideWizard*>(dialog);

        if (wizard)
        {
            settings = wizard->settings();
        }
    }

    DFileSelector*       audioUrl    = nullptr;
    DFileSelector*       destUrl     = nullptr;
    FileSaveConflictBox* conflictBox = nullptr;
    QComboBox*           playerVal   = nullptr;
    QComboBox*           formatVal   = nullptr;
    VidSlideWizard*      wizard      = nullptr;
    VidSlideSettings*    settings    = nullptr;
    QLabel*              duration    = nullptr;
};

VidSlideOutputPage::VidSlideOutputPage(QWizard* const dialog, const QString& title)
    : DWizardPage(dialog, title),
      d          (new Private(dialog))
{
    setObjectName(QLatin1String("OutputPage"));

    QWidget* const main       = new QWidget(this);

    // --------------------

    QLabel* const formatLabel = new QLabel(main);
    formatLabel->setWordWrap(false);
    formatLabel->setText(i18n("Media Container Format:"));
    d->formatVal              = new QComboBox(main);
    d->formatVal->setEditable(false);
    formatLabel->setBuddy(d->formatVal);

    // --------------------

    QLabel* const audioLabel = new QLabel(main);
    audioLabel->setWordWrap(false);
    audioLabel->setText(i18n("Soundtrack File:"));

    d->audioUrl              = new DFileSelector(main);
    d->audioUrl->setFileDlgMode(QFileDialog::ExistingFile);
    d->audioUrl->setFileDlgOptions(QFileDialog::ReadOnly);
    d->audioUrl->setFileDlgFilter(QLatin1String("*.mp3 *.ogg *.wav"));
    d->audioUrl->setFileDlgTitle(i18nc("@title:window", "Select Audio Track"));
    d->audioUrl->lineEdit()->setPlaceholderText(i18n("Video soundtrack (lets empty if none)"));
    audioLabel->setBuddy(d->audioUrl);

    QLabel* const durationLabel = new QLabel(i18n("Soundtrack Duration:"), main);
    d->duration                 = new QLabel(QLatin1String("---"), main);

    // --------------------

    QLabel* const fileLabel = new QLabel(main);
    fileLabel->setWordWrap(false);
    fileLabel->setText(i18n("Destination folder:"));

    d->destUrl              = new DFileSelector(main);
    d->destUrl->setFileDlgMode(QFileDialog::Directory);
    d->destUrl->setFileDlgOptions(QFileDialog::ShowDirsOnly);
    d->destUrl->setFileDlgTitle(i18nc("@title:window", "Destination Folder"));
    d->destUrl->lineEdit()->setPlaceholderText(i18n("Output Destination Path"));
    fileLabel->setBuddy(d->destUrl);

    // --------------------

    QLabel* const outputLbl = new QLabel(main);
    outputLbl->setText(i18n("The video output file name will be generated automatically."));
    d->conflictBox          = new FileSaveConflictBox(main);

    // --------------------

    QLabel* const playerLabel = new QLabel(main);
    playerLabel->setWordWrap(false);
    playerLabel->setText(i18n("Open in Player:"));
    d->playerVal              = new QComboBox(main);
    d->playerVal->setEditable(false);

    QMap<VidSlideSettings::VidPlayer, QString> map2                = VidSlideSettings::videoPlayerNames();
    QMap<VidSlideSettings::VidPlayer, QString>::const_iterator it2 = map2.constBegin();

    while (it2 != map2.constEnd())
    {
        d->playerVal->addItem(it2.value(), (int)it2.key());
        ++it2;
    }

#ifndef HAVE_MEDIAPLAYER

    auto* const model = qobject_cast<QStandardItemModel*>(d->playerVal->model());

    if (model)
    {
        auto* const item = model->item(VidSlideSettings::INTERNAL);

        if (item)
        {
            item->setEnabled(false);
        }
    }

#endif

    playerLabel->setBuddy(d->playerVal);

    // --------------------

    QGridLayout* const grid = new QGridLayout(main);
    grid->setSpacing(qMin(QApplication::style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing),
                             QApplication::style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing)));
    grid->addWidget(formatLabel,     0, 0, 1, 1);
    grid->addWidget(d->formatVal,    0, 1, 1, 1);
    grid->addWidget(audioLabel,      1, 0, 1, 1);
    grid->addWidget(d->audioUrl,     1, 1, 1, 1);
    grid->addWidget(durationLabel,   2, 0, 1, 1);
    grid->addWidget(d->duration,     2, 1, 1, 1);
    grid->addWidget(fileLabel,       3, 0, 1, 1);
    grid->addWidget(d->destUrl,      3, 1, 1, 1);
    grid->addWidget(outputLbl,       4, 0, 1, 2);
    grid->addWidget(d->conflictBox,  5, 0, 1, 2);
    grid->addWidget(playerLabel,     6, 0, 1, 1);
    grid->addWidget(d->playerVal,    6, 1, 1, 1);
    grid->setRowStretch(7, 10);

    setPageWidget(main);
    setLeftBottomPix(QIcon::fromTheme(QLatin1String("folder-video")));

    connect(d->destUrl->lineEdit(), &QLineEdit::textEdited,
            this, &DWizardPage::completeChanged);

    connect(d->destUrl, SIGNAL(signalUrlSelected(QUrl)),
            this, SIGNAL(completeChanged()));

    connect(d->audioUrl->lineEdit(), &QLineEdit::textEdited,
            this, &DWizardPage::completeChanged);

    connect(d->audioUrl, SIGNAL(signalUrlSelected(QUrl)),
            this, SIGNAL(completeChanged()));
}

VidSlideOutputPage::~VidSlideOutputPage()
{
    delete d;
}

void VidSlideOutputPage::initializePage()
{
    // Populate Formats List

    QMap<VidSlideSettings::VidFormat, QString> map                = VidSlideSettings::videoFormatNames();
    QMap<VidSlideSettings::VidFormat, QString>::const_iterator it = map.constBegin();

    QStringList formats = d->settings->ffmpegFormats.keys();
    int currentFormat   = d->settings->vFormat;

    while (it != map.constEnd())
    {
        d->formatVal->addItem(it.value(), (int)it.key());

        // Disable entry if FFmpeg format is not available.

        VidSlideSettings tmp;
        tmp.vFormat = (VidSlideSettings::VidFormat)it.key();

        if (!formats.contains(tmp.videoFormat()))
        {
            d->formatVal->setItemData((int)it.key(), false, Qt::UserRole-1);
        }
        else
        {
            if ((int)it.key() == currentFormat)
            {
                d->formatVal->setCurrentIndex(currentFormat);
            }
        }

        ++it;
    }

    d->audioUrl->setFileDlgPath(d->settings->audioTrack);
    d->destUrl->setFileDlgPath(d->settings->outputDir);
    d->conflictBox->setConflictRule(d->settings->conflictRule);
    d->playerVal->setCurrentIndex(d->settings->outputPlayer);
}

bool VidSlideOutputPage::validatePage()
{
    if (d->destUrl->fileDlgPath().isEmpty())
    {
        return false;
    }

    d->settings->vFormat      = (VidSlideSettings::VidFormat)d->formatVal->currentIndex();
    d->settings->outputDir    = d->destUrl->fileDlgPath();
    d->settings->audioTrack   = d->audioUrl->fileDlgPath();
    d->settings->conflictRule = d->conflictBox->conflictRule();
    d->settings->outputPlayer = (VidSlideSettings::VidPlayer)d->playerVal->currentIndex();

    return true;
}

bool VidSlideOutputPage::isComplete() const
{
    d->duration->setText(QLatin1String("---"));

    QString apath = d->audioUrl->fileDlgPath();

    if (!apath.isEmpty())
    {
        FFmpegLauncher ffmpeg;
        ffmpeg.setSettings(d->wizard->settings());
        d->wizard->settings()->soundtrackLength = ffmpeg.soundTrackLength(apath);

        if (d->wizard->settings()->soundtrackLength.isValid())
        {
            d->duration->setText(d->wizard->settings()->soundtrackLength.toString());
        }
    }

    return (!d->destUrl->fileDlgPath().isEmpty());
}

} // namespace DigikamGenericVideoSlideShowPlugin

#include "moc_vidslideoutputpage.cpp"
