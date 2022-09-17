/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-19
 * Description : Image Quality settings widget
 *
 * SPDX-FileCopyrightText: 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2013-2014 by Gowtham Ashok <gwty93 at gmail dot com>
 * SPDX-FileCopyrightText: 2021-2022 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "imagequalitysettings.h"

// Qt includes

#include <QCheckBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QIcon>
#include <QApplication>
#include <QStyle>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dlayoutbox.h"
#include "picklabelwidget.h"
#include "dnuminput.h"
#include "imagequalitycontainer.h"

namespace Digikam
{

class Q_DECL_HIDDEN ImageQualitySettings::Private
{
public:

    explicit Private()
      : optionsView         (nullptr),
        enableSorter        (nullptr),
        useFullImage        (nullptr),
        detectBlur          (nullptr),
        detectNoise         (nullptr),
        detectCompression   (nullptr),
        detectExposure      (nullptr),
        detectAesthetic     (nullptr),
        setRejected         (nullptr),
        setPending          (nullptr),
        setAccepted         (nullptr),
        setSpeed            (nullptr),
        setRejectedThreshold(nullptr),
        setPendingThreshold (nullptr),
        setAcceptedThreshold(nullptr),
        setBlurWeight       (nullptr),
        setNoiseWeight      (nullptr),
        setCompressionWeight(nullptr)
    {
    }

    DVBox*        optionsView;
    QCheckBox*    enableSorter;
    QCheckBox*    useFullImage;
    QCheckBox*    detectBlur;
    QCheckBox*    detectNoise;
    QCheckBox*    detectCompression;
    QCheckBox*    detectExposure;
    QCheckBox*    detectAesthetic;
    QCheckBox*    setRejected;
    QCheckBox*    setPending;
    QCheckBox*    setAccepted;

    DIntNumInput* setSpeed;
    DIntNumInput* setRejectedThreshold;
    DIntNumInput* setPendingThreshold;
    DIntNumInput* setAcceptedThreshold;
    DIntNumInput* setBlurWeight;
    DIntNumInput* setNoiseWeight;
    DIntNumInput* setCompressionWeight;
};

// --------------------------------------------------------

ImageQualitySettings::ImageQualitySettings(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    QVBoxLayout* const layout = new QVBoxLayout(this);

    QLabel* const explanation = new QLabel(i18nc("@label", "The goal of this settings is to determine "
                                                 "the quality of an image and convert it into a score, stored in database. "
                                                 "This property can be evaluated by two ways: using four basic factors sabotaging "
                                                 "the images (blur, noise, exposure, and compression), or using a deep learning "
                                                 "neural network engine. The first one helps to determine whether images are distorted "
                                                 "by the basic factors, however it demands some drawbacks as fine-tuning from the user’s "
                                                 "side and it cannot work along the aesthetic image processing. The second one uses an IA "
                                                 "approach based on %1 model to predict the score. As deep learning is an end-to-end "
                                                 "solution, it doesn’t require hyper-parameter settings, and make this feature easier to use.",
                                           QString::fromUtf8("<a href='https://expertphotography.com/aesthetic-photography/'>%1</a>")
                                                 .arg(i18nc("@label", "aesthetic image quality"))), this);
    explanation->setOpenExternalLinks(true);
    explanation->setWordWrap(true);
    explanation->setTextFormat(Qt::RichText);

    d->enableSorter           = new QCheckBox(i18nc("@option:check", "Enable Image Quality Sorting"), this);
    d->enableSorter->setToolTip(i18nc("@info:tooltip", "Enable this option to assign automatically Pick Labels based on image quality."));

    d->optionsView            = new DVBox(this);

    layout->addWidget(explanation);
    layout->addWidget(d->enableSorter);
    layout->addWidget(d->optionsView);

    // ------------------------------------------------------------------------------

    d->detectAesthetic         = new QCheckBox(i18nc("@option:check", "Detect Aesthetic Image using Deep Learning"), d->optionsView);
    d->detectAesthetic->setToolTip(i18nc("@info:tooltip", "Detect if the image has aesthetic value.\n"
                                         "The aesthetic detection engine use deep learning model to classify images"));

    d->detectBlur             = new QCheckBox(i18nc("@option:check", "Detect Blur"), d->optionsView);
    d->detectBlur->setToolTip(i18nc("@info:tooltip", "Detect the amount of blur in the images passed to it"));

    d->detectNoise            = new QCheckBox(i18nc("@option:check", "Detect Noise"), d->optionsView);
    d->detectNoise->setToolTip(i18nc("@info:tooltip", "Detect the amount of noise in the images passed to it"));

    d->detectCompression      = new QCheckBox(i18nc("@option:check", "Detect Compression"), d->optionsView);
    d->detectCompression->setToolTip(i18nc("@info:tooltip", "Detect the amount of compression in the images passed to it"));

    d->detectExposure         = new QCheckBox(i18nc("@option:check", "Detect Under and Over Exposure"), d->optionsView);
    d->detectExposure->setToolTip(i18nc("@info:tooltip", "Detect if the images are under-exposed or over-exposed"));

    // ------------------------------------------------------------------------------

    DHBox* const hlay1        = new DHBox(d->optionsView);

    d->setRejected            = new QCheckBox(i18nc("@option:check", "Assign 'Rejected' Label to Low Quality Pictures"), hlay1);
    d->setRejected->setToolTip(i18nc("@info:tooltip", "Low quality images detected by blur, noise, and compression analysis will be assigned to Rejected label."));

    QWidget* const hspace1    = new QWidget(hlay1);
    hlay1->setStretchFactor(hspace1, 10);

    QLabel* const workIcon1 = new QLabel(hlay1);
    workIcon1->setPixmap(QIcon::fromTheme(QLatin1String("flag-red")).pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize)));

    // ------------------------------------------------------------------------------

    DHBox* const hlay2        = new DHBox(d->optionsView);

    d->setPending             = new QCheckBox(i18nc("@option:check", "Assign 'Pending' Label to Medium Quality Pictures"), hlay2);
    d->setPending->setToolTip(i18nc("@info:tooltip", "Medium quality images detected by blur, noise, and compression analysis will be assigned to Pending label."));

    QWidget* const hspace2    = new QWidget(hlay2);
    hlay2->setStretchFactor(hspace2, 10);

    QLabel* const workIcon2   = new QLabel(hlay2);
    workIcon2->setPixmap(QIcon::fromTheme(QLatin1String("flag-yellow")).pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize)));

    // ------------------------------------------------------------------------------

    DHBox* const hlay3        = new DHBox(d->optionsView);

    d->setAccepted            = new QCheckBox(i18nc("@option:check", "Assign 'Accepted' Label to High Quality Pictures"), hlay3);
    d->setAccepted->setToolTip(i18nc("@info:tooltip", "High quality images detected by blur, noise, and compression analysis will be assigned to Accepted label."));

    QWidget* const hspace3    = new QWidget(hlay3);
    hlay3->setStretchFactor(hspace3, 10);

    QLabel* const workIcon3   = new QLabel(hlay3);
    workIcon3->setPixmap(QIcon::fromTheme(QLatin1String("flag-green")).pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize)));

    // ------------------------------------------------------------------------------

    QWidget* const settings   = new QWidget(d->optionsView);
    QGridLayout* const glay   = new QGridLayout(settings);

    QLabel* const lbl1        = new QLabel(i18n("Speed:"), settings);
    lbl1->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->setSpeed               = new DIntNumInput(settings);
    d->setSpeed->setDefaultValue(5);
    d->setSpeed->setRange(1, 3, 1);
    d->setSpeed->setToolTip(i18nc("@info:tooltip", "Tradeoff between speed and accuracy of sorting algorithm"));

    QLabel* const lbl2        = new QLabel(i18nc("@label", "Rejected threshold:"), settings);
    lbl2->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->setRejectedThreshold   = new DIntNumInput(settings);
    d->setRejectedThreshold->setDefaultValue(5);
    d->setRejectedThreshold->setRange(1, 100, 1);
    d->setRejectedThreshold->setToolTip(i18nc("@info:tooltip", "Threshold below which all pictures are assigned Rejected Label"));

    QLabel* const lbl3        = new QLabel(i18nc("@label", "Pending threshold:"), settings);
    lbl3->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->setPendingThreshold    = new DIntNumInput(settings);
    d->setPendingThreshold->setDefaultValue(5);
    d->setPendingThreshold->setRange(1, 100, 1);
    d->setPendingThreshold->setToolTip(i18nc("@info:tooltip", "Threshold below which all pictures are assigned Pending Label"));

    QLabel* const lbl4        = new QLabel(i18nc("@label", "Accepted threshold:"), settings);
    lbl4->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->setAcceptedThreshold   = new DIntNumInput(settings);
    d->setAcceptedThreshold->setDefaultValue(5);
    d->setAcceptedThreshold->setRange(1, 100, 1);
    d->setAcceptedThreshold->setToolTip(i18nc("@info:tooltip", "Threshold above which all pictures are assigned Accepted Label"));

    QLabel* const lbl5        = new QLabel(i18nc("@label", "Blur Weight:"), settings);
    lbl5->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->setBlurWeight          = new DIntNumInput(settings);
    d->setBlurWeight->setDefaultValue(5);
    d->setBlurWeight->setRange(1, 100, 1);
    d->setBlurWeight->setToolTip(i18nc("@info:tooltip", "Weight to assign to Blur Algorithm"));

    QLabel* const lbl6        = new QLabel(i18nc("@label", "Noise Weight:"), settings);
    lbl6->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->setNoiseWeight         = new DIntNumInput(settings);
    d->setNoiseWeight->setDefaultValue(5);
    d->setNoiseWeight->setRange(1, 100, 1);
    d->setNoiseWeight->setToolTip(i18nc("@info:tooltip", "Weight to assign to Noise Algorithm"));

    QLabel* const lbl7        = new QLabel(i18nc("@label", "Compression Weight:"), settings);
    lbl7->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->setCompressionWeight   = new DIntNumInput(settings);
    d->setCompressionWeight->setDefaultValue(5);
    d->setCompressionWeight->setRange(1, 100, 1);
    d->setCompressionWeight->setToolTip(i18nc("@info:tooltip", "Weight to assign to Compression Algorithm"));

    glay->addWidget(lbl1,                     0, 0, 1, 1);
    glay->addWidget(d->setSpeed,              0, 1, 1, 1);
    glay->addWidget(lbl2,                     1, 0, 1, 1);
    glay->addWidget(d->setRejectedThreshold,  1, 1, 1, 1);
    glay->addWidget(lbl3,                     2, 0, 1, 1);
    glay->addWidget(d->setPendingThreshold,   2, 1, 1, 1);
    glay->addWidget(lbl4,                     3, 0, 1, 1);
    glay->addWidget(d->setAcceptedThreshold,  3, 1, 1, 1);
    glay->addWidget(lbl5,                     4, 0, 1, 1);
    glay->addWidget(d->setBlurWeight,         4, 1, 1, 1);
    glay->addWidget(lbl6,                     5, 0, 1, 1);
    glay->addWidget(d->setNoiseWeight,        5, 1, 1, 1);
    glay->addWidget(lbl7,                     6, 0, 1, 1);
    glay->addWidget(d->setCompressionWeight,  6, 1, 1, 1);
    glay->setRowStretch(7, 10);

    connect(d->enableSorter, SIGNAL(toggled(bool)),
            d->optionsView, SLOT(setEnabled(bool)));

    connect(d->detectAesthetic, SIGNAL(toggled(bool)),
            this, SLOT(setDisableOptionViews(bool)));

    readSettings();
}

ImageQualitySettings::~ImageQualitySettings()
{
    delete d;
}

void ImageQualitySettings::applySettings()
{
    ImageQualityContainer imq;

    imq.enableSorter      = d->enableSorter->isChecked();
    imq.speed             = d->setSpeed->value();
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

    imq.writeToConfig();
}

void ImageQualitySettings::readSettings()
{
    ImageQualityContainer imq;
    imq.readFromConfig();

    d->enableSorter->setChecked(imq.enableSorter);
    d->setSpeed->setValue(imq.speed);
    d->detectBlur->setChecked(imq.detectBlur);
    d->detectNoise->setChecked(imq.detectNoise);
    d->detectCompression->setChecked(imq.detectCompression);
    d->detectExposure->setChecked(imq.detectExposure);
    d->detectAesthetic->setChecked(imq.detectAesthetic);
    d->setRejected->setChecked(imq.lowQRejected);
    d->setPending->setChecked(imq.mediumQPending);
    d->setAccepted->setChecked(imq.highQAccepted);
    d->setRejectedThreshold->setValue(imq.rejectedThreshold);
    d->setPendingThreshold->setValue(imq.pendingThreshold);
    d->setAcceptedThreshold->setValue(imq.acceptedThreshold);
    d->setBlurWeight->setValue(imq.blurWeight);
    d->setNoiseWeight->setValue(imq.noiseWeight);
    d->setCompressionWeight->setValue(imq.compressionWeight);

    d->optionsView->setEnabled(imq.enableSorter);
}

ImageQualityContainer ImageQualitySettings::getImageQualityContainer() const
{
    ImageQualityContainer imq;

    imq.readFromConfig();

    return imq;
}

void ImageQualitySettings::setDisableOptionViews(bool b)
{
    d->detectBlur->setEnabled(!b);
    d->detectNoise->setEnabled(!b);
    d->detectCompression->setEnabled(!b);
    d->detectExposure->setEnabled(!b);
    d->setRejectedThreshold->setEnabled(!b);
    d->setPendingThreshold->setEnabled(!b);
    d->setAcceptedThreshold->setEnabled(!b);
    d->setBlurWeight->setEnabled(!b);
    d->setNoiseWeight->setEnabled(!b);
    d->setCompressionWeight->setEnabled(!b);
}

} // namespace Digikam
