/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-19
 * Description : Image Quality settings widget
 *
 * SPDX-FileCopyrightText: 2013-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2013-2014 by Gowtham Ashok <gwty93 at gmail dot com>
 * SPDX-FileCopyrightText: 2021-2022 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "imagequalitysettings.h"

// Qt includes

#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QIcon>
#include <QApplication>
#include <QStyle>
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dlayoutbox.h"
#include "picklabelwidget.h"
#include "dnuminput.h"

namespace Digikam
{

class Q_DECL_HIDDEN ImageQualitySettings::Private
{
public:

    enum DetectMethod
    {
        AESTHETIC = 0,
        BASICFACTORS
    };

public:

    Private() = default;

    QCheckBox*    detectBlur            = nullptr;
    QCheckBox*    detectNoise           = nullptr;
    QCheckBox*    detectCompression     = nullptr;
    QCheckBox*    detectExposure        = nullptr;

    QButtonGroup* detectButtonGroup     = nullptr;
    QRadioButton* detectAesthetic       = nullptr;
    QRadioButton* detectBasicFactors    = nullptr;

    QCheckBox*    setRejected           = nullptr;
    QCheckBox*    setPending            = nullptr;
    QCheckBox*    setAccepted           = nullptr;

    QLabel*       lbl2                  = nullptr;
    QLabel*       lbl3                  = nullptr;
    QLabel*       lbl4                  = nullptr;
    QLabel*       lbl5                  = nullptr;
    QLabel*       lbl6                  = nullptr;
    QLabel*       lbl7                  = nullptr;

    DIntNumInput* setRejectedThreshold  = nullptr;
    DIntNumInput* setPendingThreshold   = nullptr;
    DIntNumInput* setAcceptedThreshold  = nullptr;
    DIntNumInput* setBlurWeight         = nullptr;
    DIntNumInput* setNoiseWeight        = nullptr;
    DIntNumInput* setCompressionWeight  = nullptr;
};

// --------------------------------------------------------

ImageQualitySettings::ImageQualitySettings(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    const int spacing         = layoutSpacing();


    DHBox* const hlay1        = new DHBox(this);

    d->setRejected            = new QCheckBox(i18nc("@option:check", "Assign 'Rejected' Label to Low Quality Pictures"), hlay1);
    d->setRejected->setToolTip(i18nc("@info:tooltip", "Low quality images detected by blur, noise, and compression analysis will be assigned to Rejected label."));

    QWidget* const hspace1    = new QWidget(hlay1);
    hlay1->setStretchFactor(hspace1, 10);

    QLabel* const workIcon1   = new QLabel(hlay1);
    workIcon1->setPixmap(QIcon::fromTheme(QLatin1String("flag-red")).pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize)));

    // ------------------------------------------------------------------------------

    DHBox* const hlay2        = new DHBox(this);

    d->setPending             = new QCheckBox(i18nc("@option:check", "Assign 'Pending' Label to Medium Quality Pictures"), hlay2);
    d->setPending->setToolTip(i18nc("@info:tooltip", "Medium quality images detected by blur, noise, and compression analysis will be assigned to Pending label."));

    QWidget* const hspace2    = new QWidget(hlay2);
    hlay2->setStretchFactor(hspace2, 10);

    QLabel* const workIcon2   = new QLabel(hlay2);
    workIcon2->setPixmap(QIcon::fromTheme(QLatin1String("flag-yellow")).pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize)));

    // ------------------------------------------------------------------------------

    DHBox* const hlay3        = new DHBox(this);

    d->setAccepted            = new QCheckBox(i18nc("@option:check", "Assign 'Accepted' Label to High Quality Pictures"), hlay3);
    d->setAccepted->setToolTip(i18nc("@info:tooltip", "High quality images detected by blur, noise, and compression analysis will be assigned to Accepted label."));

    QWidget* const hspace3    = new QWidget(hlay3);
    hlay3->setStretchFactor(hspace3, 10);

    QLabel* const workIcon3   = new QLabel(hlay3);
    workIcon3->setPixmap(QIcon::fromTheme(QLatin1String("flag-green")).pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize)));

    // ------------------------------------------------------------------------------

    d->detectButtonGroup      = new QButtonGroup(this);
    d->detectButtonGroup->setExclusive(true);

    d->detectAesthetic        = new QRadioButton(i18nc("@option:radio", "Detect Aesthetic Image using Deep Learning"),
                                                 this);
    d->detectAesthetic->setToolTip(i18nc("@info:tooltip", "Detect if the image has aesthetic value.\n"
                                         "The aesthetic detection engine use deep learning model to classify images"));

    d->detectButtonGroup->addButton(d->detectAesthetic, Private::AESTHETIC);

    d->detectBasicFactors     = new QRadioButton(i18nc("@option:radio", "Detect Quality Using Basic Factors"),
                                                 this);
    d->detectBasicFactors->setToolTip(i18nc("@info:tooltip", "Detect if the image is sabotaging by four basic factors "
                                            "(blur, noise, exposure, and compression)."));

    d->detectButtonGroup->addButton(d->detectBasicFactors, Private::BASICFACTORS);
    d->detectAesthetic->setChecked(true);

    // ------------------------------------------------------------------------------

    QWidget* const basicView  = new QWidget(this);
    QGridLayout* const grid1  = new QGridLayout(basicView);

    d->lbl2                   = new QLabel(i18nc("@label", "Rejected threshold:"), basicView);
    d->lbl2->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    d->setRejectedThreshold   = new DIntNumInput(basicView);
    d->setRejectedThreshold->setDefaultValue(10);
    d->setRejectedThreshold->setRange(1, 100, 1);
    d->setRejectedThreshold->setToolTip(i18nc("@info:tooltip", "Threshold below which all pictures are assigned Rejected Label"));

    d->lbl3                   = new QLabel(i18nc("@label", "Pending threshold:"), basicView);
    d->lbl3->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    d->setPendingThreshold    = new DIntNumInput(basicView);
    d->setPendingThreshold->setDefaultValue(40);
    d->setPendingThreshold->setRange(1, 100, 1);
    d->setPendingThreshold->setToolTip(i18nc("@info:tooltip", "Threshold below which all pictures are assigned Pending Label"));

    d->lbl4                   = new QLabel(i18nc("@label", "Accepted threshold:"), basicView);
    d->lbl4->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    d->setAcceptedThreshold   = new DIntNumInput(basicView);
    d->setAcceptedThreshold->setDefaultValue(60);
    d->setAcceptedThreshold->setRange(1, 100, 1);
    d->setAcceptedThreshold->setToolTip(i18nc("@info:tooltip", "Threshold above which all pictures are assigned Accepted Label"));

    d->detectBlur             = new QCheckBox(i18nc("@option:check", "Detect Blur"), basicView);
    d->detectBlur->setToolTip(i18nc("@info:tooltip", "Detect the amount of blur in the images passed to it"));

    d->lbl5                   = new QLabel(i18nc("@label", "Weight:"), basicView);
    d->lbl5->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    d->setBlurWeight          = new DIntNumInput(basicView);
    d->setBlurWeight->setDefaultValue(100);
    d->setBlurWeight->setRange(1, 100, 1);
    d->setBlurWeight->setToolTip(i18nc("@info:tooltip", "Weight to assign to Blur Algorithm"));

    d->detectNoise            = new QCheckBox(i18nc("@option:check", "Detect Noise"), basicView);
    d->detectNoise->setToolTip(i18nc("@info:tooltip", "Detect the amount of noise in the images passed to it"));

    d->lbl6                   = new QLabel(i18nc("@label", "Weight:"), basicView);
    d->lbl6->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    d->setNoiseWeight         = new DIntNumInput(basicView);
    d->setNoiseWeight->setDefaultValue(100);
    d->setNoiseWeight->setRange(1, 100, 1);
    d->setNoiseWeight->setToolTip(i18nc("@info:tooltip", "Weight to assign to Noise Algorithm"));

    d->detectCompression      = new QCheckBox(i18nc("@option:check", "Detect Compression"), basicView);
    d->detectCompression->setToolTip(i18nc("@info:tooltip", "Detect the amount of compression in the images passed to it"));

    d->lbl7                   = new QLabel(i18nc("@label", "Weight:"), basicView);
    d->lbl7->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    d->setCompressionWeight   = new DIntNumInput(basicView);
    d->setCompressionWeight->setDefaultValue(100);
    d->setCompressionWeight->setRange(1, 100, 1);
    d->setCompressionWeight->setToolTip(i18nc("@info:tooltip", "Weight to assign to Compression Algorithm"));

    d->detectExposure         = new QCheckBox(i18nc("@option:check", "Detect Under and Over Exposure"), basicView);
    d->detectExposure->setToolTip(i18nc("@info:tooltip", "Detect if the images are under-exposed or over-exposed"));

    grid1->addWidget(d->lbl2,                  0, 0, 1, 2);
    grid1->addWidget(d->setRejectedThreshold,  0, 2, 1, 1);
    grid1->addWidget(d->lbl3,                  1, 0, 1, 2);
    grid1->addWidget(d->setPendingThreshold,   1, 2, 1, 1);
    grid1->addWidget(d->lbl4,                  2, 0, 1, 2);
    grid1->addWidget(d->setAcceptedThreshold,  2, 2, 1, 1);

    grid1->addWidget(d->detectBlur,            3, 0, 1, 1);
    grid1->addWidget(d->lbl5,                  3, 1, 1, 1);
    grid1->addWidget(d->setBlurWeight,         3, 2, 1, 1);

    grid1->addWidget(d->detectNoise,           4, 0, 1, 1);
    grid1->addWidget(d->lbl6,                  4, 1, 1, 1);
    grid1->addWidget(d->setNoiseWeight,        4, 2, 1, 1);

    grid1->addWidget(d->detectCompression,     5, 0, 1, 1);
    grid1->addWidget(d->lbl7,                  5, 1, 1, 1);
    grid1->addWidget(d->setCompressionWeight,  5, 2, 1, 1);

    grid1->addWidget(d->detectExposure,        6, 0, 1, 3);
    grid1->setContentsMargins(2 * spacing, spacing, spacing, spacing);
    grid1->setColumnStretch(0, 1);
    grid1->setColumnStretch(1, 1);
    grid1->setColumnStretch(2, 100);
    grid1->setRowStretch(7, 10);

    // ------------------------------------------------------------------------------

    QGridLayout* const glay = new QGridLayout(this);
    glay->addWidget(hlay1,                 0, 1, 1, 1);
    glay->addWidget(hlay2,                 1, 1, 1, 1);
    glay->addWidget(hlay3,                 2, 1, 1, 1);
    glay->addWidget(d->detectAesthetic,    3, 1, 1, 1);
    glay->addWidget(d->detectBasicFactors, 4, 1, 1, 1);
    glay->addWidget(basicView,             5, 1, 1, 1);
    glay->setColumnStretch(1, 10);
    glay->setContentsMargins(2 * spacing, spacing, spacing, spacing);

    // ------------------------------------------------------------------------------

    connect(d->detectBlur, SIGNAL(toggled(bool)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->detectNoise, SIGNAL(toggled(bool)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->detectCompression, SIGNAL(toggled(bool)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->detectExposure, SIGNAL(toggled(bool)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->setRejected, SIGNAL(toggled(bool)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->setPending, SIGNAL(toggled(bool)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->setAccepted, SIGNAL(toggled(bool)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->setRejectedThreshold, SIGNAL(valueChanged(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->setPendingThreshold, SIGNAL(valueChanged(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->setAcceptedThreshold, SIGNAL(valueChanged(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->setBlurWeight, SIGNAL(valueChanged(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->setNoiseWeight, SIGNAL(valueChanged(int)),
            this, SIGNAL(signalSettingsChanged()));

    connect(d->setCompressionWeight, SIGNAL(valueChanged(int)),
            this, SIGNAL(signalSettingsChanged()));

    // ------------------------------------------------------------------------------

    connect(d->detectBlur, SIGNAL(toggled(bool)),
            d->lbl5, SLOT(setEnabled(bool)));

    connect(d->detectBlur, SIGNAL(toggled(bool)),
            d->setBlurWeight, SLOT(setEnabled(bool)));

    connect(d->detectNoise, SIGNAL(toggled(bool)),
            d->lbl6, SLOT(setEnabled(bool)));

    connect(d->detectNoise, SIGNAL(toggled(bool)),
            d->setNoiseWeight, SLOT(setEnabled(bool)));

    connect(d->detectCompression, SIGNAL(toggled(bool)),
            d->lbl7, SLOT(setEnabled(bool)));

    connect(d->detectCompression, SIGNAL(toggled(bool)),
            d->setCompressionWeight, SLOT(setEnabled(bool)));

#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))

    connect(d->detectButtonGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::idClicked),
            this, &ImageQualitySettings::slotDisableOptionViews);

#else

    connect(d->detectButtonGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
            this, &ImageQualitySettings::slotDisableOptionViews);

#endif

    slotDisableOptionViews();
}

ImageQualitySettings::~ImageQualitySettings()
{
    delete d;
}

void ImageQualitySettings::applySettings()
{
    ImageQualityContainer imq = getImageQualityContainer();
    imq.writeToConfig();
}

void ImageQualitySettings::applySettings(KConfigGroup& group)
{
    ImageQualityContainer imq = getImageQualityContainer();
    imq.writeToConfig(group);
}

void ImageQualitySettings::readSettings()
{
    ImageQualityContainer imq;
    imq.readFromConfig();
    setImageQualityContainer(imq);
}

void ImageQualitySettings::readSettings(const KConfigGroup& group)
{
    ImageQualityContainer imq;
    imq.readFromConfig(group);
    setImageQualityContainer(imq);
}

void ImageQualitySettings::setImageQualityContainer(const ImageQualityContainer& imq)
{
    d->detectBlur->setChecked(imq.detectBlur);
    d->detectNoise->setChecked(imq.detectNoise);
    d->detectCompression->setChecked(imq.detectCompression);
    d->detectExposure->setChecked(imq.detectExposure);

    if (imq.detectAesthetic)
    {
        d->detectAesthetic->setChecked(true);
    }
    else
    {
        d->detectBasicFactors->setChecked(true);
    }

    d->setRejected->setChecked(imq.lowQRejected);
    d->setPending->setChecked(imq.mediumQPending);
    d->setAccepted->setChecked(imq.highQAccepted);
    d->setRejectedThreshold->setValue(imq.rejectedThreshold);
    d->setPendingThreshold->setValue(imq.pendingThreshold);
    d->setAcceptedThreshold->setValue(imq.acceptedThreshold);
    d->setBlurWeight->setValue(imq.blurWeight);
    d->setNoiseWeight->setValue(imq.noiseWeight);
    d->setCompressionWeight->setValue(imq.compressionWeight);

    d->lbl5->setEnabled(imq.detectBlur);
    d->setBlurWeight->setEnabled(imq.detectBlur);
    d->lbl6->setEnabled(imq.detectNoise);
    d->setNoiseWeight->setEnabled(imq.detectNoise);
    d->lbl7->setEnabled(imq.detectCompression);
    d->setCompressionWeight->setEnabled(imq.detectCompression);

    slotDisableOptionViews();
}

ImageQualityContainer ImageQualitySettings::getImageQualityContainer() const
{
    ImageQualityContainer imq;

    imq.detectBlur        = d->detectBlur->isChecked();
    imq.detectNoise       = d->detectNoise->isChecked();
    imq.detectCompression = d->detectCompression->isChecked();
    imq.detectExposure    = d->detectExposure->isChecked();
    imq.detectAesthetic   = d->detectAesthetic->isChecked();
    imq.lowQRejected      = d->setRejected->isChecked();
    imq.mediumQPending    = d->setPending->isChecked();
    imq.highQAccepted     = d->setAccepted->isChecked();
    imq.rejectedThreshold = d->setRejectedThreshold->value();
    imq.pendingThreshold  = d->setPendingThreshold->value();
    imq.acceptedThreshold = d->setAcceptedThreshold->value();
    imq.blurWeight        = d->setBlurWeight->value();
    imq.noiseWeight       = d->setNoiseWeight->value();
    imq.compressionWeight = d->setCompressionWeight->value();

    return imq;
}

void ImageQualitySettings::slotDisableOptionViews()
{
    bool b = d->detectBasicFactors->isChecked();

    d->lbl2->setEnabled(b);
    d->lbl3->setEnabled(b);
    d->lbl4->setEnabled(b);
    d->detectBlur->setEnabled(b);
    d->detectNoise->setEnabled(b);
    d->detectCompression->setEnabled(b);
    d->detectExposure->setEnabled(b);
    d->setRejectedThreshold->setEnabled(b);
    d->setPendingThreshold->setEnabled(b);
    d->setAcceptedThreshold->setEnabled(b);

    d->lbl5->setEnabled(b && d->detectBlur->isChecked());
    d->setBlurWeight->setEnabled(b && d->detectBlur->isChecked());
    d->lbl6->setEnabled(b && d->detectNoise->isChecked());
    d->setNoiseWeight->setEnabled(b && d->detectNoise->isChecked());
    d->lbl7->setEnabled(b && d->detectCompression->isChecked());
    d->setCompressionWeight->setEnabled(b && d->detectCompression->isChecked());

    Q_EMIT signalSettingsChanged();
}

void ImageQualitySettings::resetToDefault()
{
    blockSignals(true);

    ImageQualityContainer prm;
    setImageQualityContainer(prm);

    blockSignals(false);
}

ImageQualityContainer ImageQualitySettings::defaultSettings() const
{
    ImageQualityContainer prm;

    return prm;
}

} // namespace Digikam

#include "moc_imagequalitysettings.cpp"
