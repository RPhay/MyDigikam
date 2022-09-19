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
#include <QRadioButton>
#include <QButtonGroup>
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

    enum DetectMethod
    {
        AESTHETIC = 0,
        BASICFACTORS
    };

public:

    explicit Private()
      : optionsView         (nullptr),
        enableSorter        (nullptr),
        detectBlur          (nullptr),
        detectNoise         (nullptr),
        detectCompression   (nullptr),
        detectExposure      (nullptr),
        detectButtonGroup   (nullptr),
        detectAesthetic     (nullptr),
        detectBasicFactors  (nullptr),
        setRejected         (nullptr),
        setPending          (nullptr),
        setAccepted         (nullptr),
        lbl2                (nullptr),
        lbl3                (nullptr),
        lbl4                (nullptr),
        lbl5                (nullptr),
        lbl6                (nullptr),
        lbl7                (nullptr),
        setRejectedThreshold(nullptr),
        setPendingThreshold (nullptr),
        setAcceptedThreshold(nullptr),
        setBlurWeight       (nullptr),
        setNoiseWeight      (nullptr),
        setCompressionWeight(nullptr)
    {
    }

    QWidget*      optionsView;
    QCheckBox*    enableSorter;
    QCheckBox*    detectBlur;
    QCheckBox*    detectNoise;
    QCheckBox*    detectCompression;
    QCheckBox*    detectExposure;

    QButtonGroup* detectButtonGroup;
    QRadioButton* detectAesthetic;
    QRadioButton* detectBasicFactors;

    QCheckBox*    setRejected;
    QCheckBox*    setPending;
    QCheckBox*    setAccepted;

    QLabel*       lbl2;
    QLabel*       lbl3;
    QLabel*       lbl4;
    QLabel*       lbl5;
    QLabel*       lbl6;
    QLabel*       lbl7;

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
    const int spacing         = qMin(QApplication::style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing),
                                     QApplication::style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing));

    QVBoxLayout* const layout = new QVBoxLayout(this);

    QLabel* const explanation = new QLabel(i18nc("@label", "These settings determines the quality of an image and convert it into a score, "
                                                 "stored in database. This property can be evaluated by two ways: using four basic factors "
                                                 "sabotaging the images (blur, noise, exposure, and compression), or using a deep learning "
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
    d->enableSorter->setToolTip(i18nc("@info:tooltip", "Enable this option to assign automatically "
                                      "Pick Labels based on image quality."));

    d->optionsView            = new QWidget(this);

    layout->addWidget(explanation);
    layout->addWidget(d->enableSorter);
    layout->addWidget(d->optionsView);

    // ------------------------------------------------------------------------------

    DHBox* const hlay1        = new DHBox(d->optionsView);

    d->setRejected            = new QCheckBox(i18nc("@option:check", "Assign 'Rejected' Label to Low Quality Pictures"), hlay1);
    d->setRejected->setToolTip(i18nc("@info:tooltip", "Low quality images detected by blur, noise, and compression analysis will be assigned to Rejected label."));

    QWidget* const hspace1    = new QWidget(hlay1);
    hlay1->setStretchFactor(hspace1, 10);

    QLabel* const workIcon1   = new QLabel(hlay1);
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

    d->detectButtonGroup      = new QButtonGroup(d->optionsView);
    d->detectButtonGroup->setExclusive(true);

    d->detectAesthetic        = new QRadioButton(i18nc("@option:radio", "Detect Aesthetic Image using Deep Learning"),
                                                 d->optionsView);
    d->detectAesthetic->setToolTip(i18nc("@info:tooltip", "Detect if the image has aesthetic value.\n"
                                         "The aesthetic detection engine use deep learning model to classify images"));

    d->detectButtonGroup->addButton(d->detectAesthetic, Private::AESTHETIC);

    d->detectBasicFactors     = new QRadioButton(i18nc("@option:radio", "Detect Quality Using Basic Factors"),
                                                 d->optionsView);
    d->detectBasicFactors->setToolTip(i18nc("@info:tooltip", "Detect if the image is sabotating by four basic factors "
                                            "(blur, noise, exposure, and compression)."));

    d->detectButtonGroup->addButton(d->detectBasicFactors, Private::BASICFACTORS);
    d->detectAesthetic->setChecked(true);

    // ------------------------------------------------------------------------------

    QWidget* const basicView  = new QWidget(d->optionsView);
    QGridLayout* const grid1  = new QGridLayout(basicView);

    d->lbl2                   = new QLabel(i18nc("@label", "Rejected threshold:"), basicView);
    d->lbl2->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->setRejectedThreshold   = new DIntNumInput(basicView);
    d->setRejectedThreshold->setDefaultValue(5);
    d->setRejectedThreshold->setRange(1, 100, 1);
    d->setRejectedThreshold->setToolTip(i18nc("@info:tooltip", "Threshold below which all pictures are assigned Rejected Label"));

    d->lbl3                   = new QLabel(i18nc("@label", "Pending threshold:"), basicView);
    d->lbl3->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->setPendingThreshold    = new DIntNumInput(basicView);
    d->setPendingThreshold->setDefaultValue(5);
    d->setPendingThreshold->setRange(1, 100, 1);
    d->setPendingThreshold->setToolTip(i18nc("@info:tooltip", "Threshold below which all pictures are assigned Pending Label"));

    d->lbl4                   = new QLabel(i18nc("@label", "Accepted threshold:"), basicView);
    d->lbl4->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->setAcceptedThreshold   = new DIntNumInput(basicView);
    d->setAcceptedThreshold->setDefaultValue(5);
    d->setAcceptedThreshold->setRange(1, 100, 1);
    d->setAcceptedThreshold->setToolTip(i18nc("@info:tooltip", "Threshold above which all pictures are assigned Accepted Label"));

    d->detectBlur             = new QCheckBox(i18nc("@option:check", "Detect Blur"), basicView);
    d->detectBlur->setToolTip(i18nc("@info:tooltip", "Detect the amount of blur in the images passed to it"));

    d->lbl5                   = new QLabel(i18nc("@label", "Blur Weight:"), basicView);
    d->lbl5->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->setBlurWeight          = new DIntNumInput(basicView);
    d->setBlurWeight->setDefaultValue(5);
    d->setBlurWeight->setRange(1, 100, 1);
    d->setBlurWeight->setToolTip(i18nc("@info:tooltip", "Weight to assign to Blur Algorithm"));

    d->detectNoise            = new QCheckBox(i18nc("@option:check", "Detect Noise"), basicView);
    d->detectNoise->setToolTip(i18nc("@info:tooltip", "Detect the amount of noise in the images passed to it"));

    d->lbl6                   = new QLabel(i18nc("@label", "Noise Weight:"), basicView);
    d->lbl6->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->setNoiseWeight         = new DIntNumInput(basicView);
    d->setNoiseWeight->setDefaultValue(5);
    d->setNoiseWeight->setRange(1, 100, 1);
    d->setNoiseWeight->setToolTip(i18nc("@info:tooltip", "Weight to assign to Noise Algorithm"));

    d->detectCompression      = new QCheckBox(i18nc("@option:check", "Detect Compression"), basicView);
    d->detectCompression->setToolTip(i18nc("@info:tooltip", "Detect the amount of compression in the images passed to it"));

    d->lbl7                   = new QLabel(i18nc("@label", "Compression Weight:"), basicView);
    d->lbl7->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d->setCompressionWeight   = new DIntNumInput(basicView);
    d->setCompressionWeight->setDefaultValue(5);
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

    grid1->addWidget(d->detectBlur,            3, 0, 1, 3);
    grid1->addWidget(d->lbl5,                  4, 1, 1, 1);
    grid1->addWidget(d->setBlurWeight,         4, 2, 1, 1);

    grid1->addWidget(d->detectNoise,           5, 0, 1, 3);
    grid1->addWidget(d->lbl6,                  6, 1, 1, 1);
    grid1->addWidget(d->setNoiseWeight,        6, 2, 1, 1);

    grid1->addWidget(d->detectCompression,     7, 0, 1, 3);
    grid1->addWidget(d->lbl7,                  8, 1, 1, 1);
    grid1->addWidget(d->setCompressionWeight,  8, 2, 1, 1);

    grid1->addWidget(d->detectExposure,        9, 0, 1, 3);
    grid1->setContentsMargins(2 * spacing, spacing, spacing, spacing);
    grid1->setColumnStretch(0, 1);
    grid1->setColumnStretch(1, 1);
    grid1->setColumnStretch(2, 100);
    grid1->setRowStretch(10, 10);

    // ------------------------------------------------------------------------------

    QGridLayout* const glay = new QGridLayout(d->optionsView);
    glay->addWidget(hlay1,                 0, 1, 1, 1);
    glay->addWidget(hlay2,                 1, 1, 1, 1);
    glay->addWidget(hlay3,                 2, 1, 1, 1);
    glay->addWidget(d->detectAesthetic,    3, 1, 1, 1);
    glay->addWidget(d->detectBasicFactors, 4, 1, 1, 1);
    glay->addWidget(basicView,             5, 1, 1, 1);
    glay->setColumnStretch(1, 10);
    glay->setContentsMargins(2 * spacing, spacing, spacing, spacing);

    // ------------------------------------------------------------------------------

    connect(d->enableSorter, SIGNAL(toggled(bool)),
            d->optionsView, SLOT(setEnabled(bool)));

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

    readSettings();
    slotDisableOptionViews();
}

ImageQualitySettings::~ImageQualitySettings()
{
    delete d;
}

void ImageQualitySettings::applySettings()
{
    ImageQualityContainer imq;

    imq.enableSorter      = d->enableSorter->isChecked();
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

    d->optionsView->setEnabled(imq.enableSorter);
    d->lbl5->setEnabled(imq.detectBlur);
    d->setBlurWeight->setEnabled(imq.detectBlur);
    d->lbl6->setEnabled(imq.detectNoise);
    d->setNoiseWeight->setEnabled(imq.detectNoise);
    d->lbl7->setEnabled(imq.detectCompression);
    d->setCompressionWeight->setEnabled(imq.detectCompression);
}

ImageQualityContainer ImageQualitySettings::getImageQualityContainer() const
{
    ImageQualityContainer imq;

    imq.readFromConfig();

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
}

} // namespace Digikam
