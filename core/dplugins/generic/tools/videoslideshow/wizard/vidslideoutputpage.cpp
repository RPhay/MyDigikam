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
#include <QCheckBox>
#include <QSpinBox>
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
#include "dexpanderbox.h"

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
    QTimer*              trigUpdate  = nullptr;
    QCheckBox*           equalize    = nullptr;
    QSpinBox*            strength    = nullptr;
};

VidSlideOutputPage::VidSlideOutputPage(QWizard* const dialog, const QString& title)
    : DWizardPage(dialog, title),
      d          (new Private(dialog))
{
    setObjectName(QLatin1String("OutputPage"));

    QWidget* const main       = new QWidget(this);
    d->trigUpdate             = new QTimer(this);
    d->trigUpdate->setSingleShot(true);

    // --------------------

    QLabel* const formatLabel = new QLabel(main);
    formatLabel->setWordWrap(false);
    formatLabel->setText(i18n("Media Container Format:"));
    d->formatVal              = new QComboBox(main);
    d->formatVal->setEditable(false);
    d->formatVal->setToolTip(i18n("This controls allows to choose the format of the media container to host video stream and soundtrack."));
    formatLabel->setBuddy(d->formatVal);

    // --------------------

    DLineWidget* const line0    = new DLineWidget(Qt::Horizontal, main);
    d->equalize                 = new QCheckBox(i18n("Equalize Video Luminosity"), main);

    QLabel* const strengthLabel = new QLabel(main);
    strengthLabel->setWordWrap(false);
    strengthLabel->setEnabled(false);
    strengthLabel->setText(i18n("Equalization Strength:"));
    d->strength                 = new QSpinBox(main);
    d->strength->setEnabled(false);
    d->strength->setRange(0, 10);
    d->strength->setToolTip(i18n("This controls strength of equalization. Setting this option to zero effectively does nothing."));
    strengthLabel->setBuddy(d->strength);

    QLabel* const equalNote     = new QLabel(main);
    equalNote->setWordWrap(true);
    equalNote->setText(i18n("<i>These settings apply temporal midway video equalization effect while "
                            "encoding frames as video. This adjusts the sequence of frames to have "
                            "the same histograms, while maintaining their dynamics as much as possible. "
                            "It’s useful for e.g. matching exposures from a timelapse sequence.</i>"));

    // --------------------

    DLineWidget* const line1 = new DLineWidget(Qt::Horizontal, main);

    QLabel* const audioLabel = new QLabel(main);
    audioLabel->setWordWrap(false);
    audioLabel->setText(i18n("Soundtrack File:"));

    d->audioUrl              = new DFileSelector(main);
    d->audioUrl->setFileDlgMode(QFileDialog::ExistingFile);
    d->audioUrl->setFileDlgOptions(QFileDialog::ReadOnly);
    d->audioUrl->setFileDlgFilter(QLatin1String("*.mp3 *.ogg *.wav *.flac *.m4a"));
    d->audioUrl->setFileDlgTitle(i18nc("@title:window", "Select Audio Track"));
    audioLabel->setBuddy(d->audioUrl);

    QLabel* const durationLabel = new QLabel(i18n("Soundtrack Duration:"), main);
    d->duration                 = new QLabel(QLatin1String("---"), main);

    QLabel* const audioNote     = new QLabel(main);
    audioNote->setWordWrap(true);
    audioNote->setText(i18n("<i>Notes about soundtrack: if the audio length is smaller than video, it will be "
                            "played in loop. If the audio length is largest than video, it will be trimmed. "
                            "Lets empty this setting if you don't want a sountrack to the media.</i>"));

    DLineWidget* const line2 = new DLineWidget(Qt::Horizontal, main);

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
    grid->addWidget(line0,           1, 0, 1, 2);
    grid->addWidget(d->equalize,     2, 0, 1, 2);
    grid->addWidget(strengthLabel,   3, 0, 1, 1);
    grid->addWidget(d->strength,     3, 1, 1, 1);
    grid->addWidget(equalNote,       4, 0, 1, 2);
    grid->addWidget(line1,           5, 0, 1, 2);
    grid->addWidget(audioLabel,      6, 0, 1, 1);
    grid->addWidget(d->audioUrl,     6, 1, 1, 1);
    grid->addWidget(durationLabel,   7, 0, 1, 1);
    grid->addWidget(d->duration,     7, 1, 1, 1);
    grid->addWidget(audioNote,       8, 0, 1, 2);
    grid->addWidget(line2,           9, 0, 1, 2);
    grid->addWidget(fileLabel,      10, 0, 1, 1);
    grid->addWidget(d->destUrl,     10, 1, 1, 1);
    grid->addWidget(outputLbl,      11, 0, 1, 2);
    grid->addWidget(d->conflictBox, 12, 0, 1, 2);
    grid->addWidget(playerLabel,    13, 0, 1, 1);
    grid->addWidget(d->playerVal,   13, 1, 1, 1);
    grid->setRowStretch(14, 10);

    setPageWidget(main);
    setLeftBottomPix(QIcon::fromTheme(QLatin1String("folder-video")));

    connect(d->trigUpdate, SIGNAL(timeout()),
            this, SIGNAL(completeChanged()));

    connect(d->destUrl->lineEdit(), &QLineEdit::textEdited,
            this, &VidSlideOutputPage::slotTriggerUpdate);

    connect(d->destUrl, SIGNAL(signalUrlSelected(QUrl)),
            this, SLOT(slotTriggerUpdate()));

    connect(d->audioUrl->lineEdit(), &QLineEdit::textEdited,
            this, &VidSlideOutputPage::slotTriggerUpdate);

    connect(d->audioUrl, SIGNAL(signalUrlSelected(QUrl)),
            this, SLOT(slotTriggerUpdate()));

    connect(d->equalize, SIGNAL(toggled(bool)),
            strengthLabel, SLOT(setEnabled(bool)));

    connect(d->equalize, SIGNAL(toggled(bool)),
            d->strength, SLOT(setEnabled(bool)));
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
    d->equalize->setChecked(d->settings->equalize);
    d->strength->setValue(d->settings->strength);
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
    d->settings->equalize     = d->equalize->isChecked();
    d->settings->strength     = d->strength->value();

    return true;
}

void VidSlideOutputPage::slotTriggerUpdate()
{
    d->trigUpdate->start(1000);
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
