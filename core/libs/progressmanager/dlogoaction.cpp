/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-27-08
 * Description : a tool bar action object to display animated logo
 *
 * SPDX-FileCopyrightText: 2007-2026 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

// Qt includes

#include <QPalette>
#include <QPainter>
#include <QBoxLayout>
#include <QTimer>
#include <QApplication>
#include <QStandardPaths>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dlogoaction.h"
#include "daboutdata.h"
#include "dactivelabel.h"

namespace Digikam
{

class Q_DECL_HIDDEN DLogoAction::Private
{
public:

    Private() = default;

public:

    bool          alignOnRight    = true;
    int           progressCount   = 0;          ///< Animation position (0 à 35).
    QTimer*       progressTimer   = nullptr;
    QImage        image;
    DActiveLabel* urlLabel        = nullptr;
    int           logoHeight      = 32;
};

DLogoAction::DLogoAction(QObject* const parent, bool alignOnRight)
    : QWidgetAction(parent),
      d            (new Private)
{
    QString imgPath;

    if (QApplication::applicationName() == QLatin1String("digikam"))
    {
        setIcon(QIcon::fromTheme(QLatin1String("digikam")));
        imgPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                        QLatin1String("digikam/data/banner-digikam.webp"));
    }
    else
    {
        setIcon(QIcon::fromTheme(QLatin1String("showfoto")));
        imgPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                        QLatin1String("showfoto/data/banner-showfoto.webp"));
    }

    d->alignOnRight  = alignOnRight;
    d->image         = QImage(imgPath);
    d->progressTimer = new QTimer(this);
    d->progressTimer->setSingleShot(true);

    connect(d->progressTimer, &QTimer::timeout,
            this, &DLogoAction::slotProgressTimerDone);
}

DLogoAction::~DLogoAction()
{
    delete d;
}

void DLogoAction::start()
{
    d->progressCount = 0;
    d->progressTimer->start(100); // 100 ms between frame
}

void DLogoAction::stop()
{
    d->progressCount = 0;
    d->progressTimer->stop();

    if (d->urlLabel)
    {
        QPixmap frame(d->logoHeight * 4.5, d->logoHeight);
        frame.fill(Qt::transparent);
        QPainter painter(&frame);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.drawImage(QPoint(0, 0), d->image.scaled(frame.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

        // Reset the display with the static banner

        d->urlLabel->updateData(DAboutData::webProjectUrl(), frame.toImage());
    }
}

bool DLogoAction::running() const
{
    return d->progressTimer->isActive();
}

QPixmap DLogoAction::renderAnimationFrame(int beamPosition)
{
    QPixmap frame(d->logoHeight * 4.5, d->logoHeight);
    frame.fill(Qt::transparent);

    // Draw the banner

    QPainter painter(&frame);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawImage(QPoint(0, 0), d->image.scaled(frame.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Create a mask for the non-transparent pixels.

    QImage frameImage = frame.toImage();
    QImage mask(frame.size(), QImage::Format_Grayscale8);
    mask.fill(0);                                           // NOTE: black = transparent
    {
        QPainter maskPainter(&mask);
        maskPainter.setRenderHint(QPainter::Antialiasing);
        maskPainter.drawImage(QPoint(0, 0), d->image.scaled(frame.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    // Lightening settings.

    int beamWidth     = frame.height(); // Lightening width (ajustable).
    int halfBeamWidth = beamWidth / 2;  // half width for the symetrical gradient.

    // Apply the lightening with a symetrical gradient.

    for (int y = 0; y < frame.height(); ++y)
    {
        for (int x = 0; x < frame.width(); ++x)
        {
            // Checl if the pixel is transparent.

            if (qAlpha(frameImage.pixel(x, y)) > 0)
            {
                // Compute the distance with the center of the lightening.

                int distToCenter = abs(x - beamPosition);

                // If the pixel is inside the lightening.

                if (distToCenter < halfBeamWidth)
                {
                    // Compute the intensity (symetrical gradient).

                    float intensity = 1.0f - (static_cast<float>(distToCenter) / halfBeamWidth);
                    intensity       = qMin(intensity, 1.0f); // Limit to 1.0

                    // Apply the intensity to the pixel.

                    QRgb pixel = frameImage.pixel(x, y);
                    int r      = qMin(qRed(pixel)   + static_cast<int>(intensity * 200), 255);
                    int g      = qMin(qGreen(pixel) + static_cast<int>(intensity * 200), 255);
                    int b      = qMin(qBlue(pixel)  + static_cast<int>(intensity * 200), 255);
                    frameImage.setPixel(x, y, qRgba(r, g, b, qAlpha(pixel)));
                }
            }
        }
    }

    return QPixmap::fromImage(frameImage);
}

void DLogoAction::slotProgressTimerDone()
{
    int beamPosition = (d->progressCount * (d->logoHeight * 4.5 - (d->logoHeight / 2))) / 36;
    QPixmap frame    = renderAnimationFrame(beamPosition);

    if (d->urlLabel)
    {
        d->urlLabel->updateData(DAboutData::webProjectUrl(), frame.toImage());
    }

    d->progressCount = (d->progressCount + 1) % 36;
    d->progressTimer->start(100);
}

QWidget* DLogoAction::createWidget(QWidget* parent)
{
    QWidget* const container  = new QWidget(parent);
    QHBoxLayout* const layout = new QHBoxLayout(container);
    layout->setContentsMargins(QMargins());
    layout->setSpacing(0);

    QPixmap frame(d->logoHeight * 4.5, d->logoHeight);
    frame.fill(Qt::transparent);
    QPainter painter(&frame);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawImage(QPoint(0, 0), d->image.scaled(frame.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    d->urlLabel   = new DActiveLabel(DAboutData::webProjectUrl(), QString(), container);
    d->urlLabel->setToolTip(i18n("Visit digiKam project website"));
    d->urlLabel->updateData(DAboutData::webProjectUrl(), frame.toImage());

    if (d->alignOnRight)
    {
        layout->addStretch();
    }

    layout->addWidget(d->urlLabel);

    return container;
}

void DLogoAction::deleteWidget(QWidget* widget)
{
    stop();
    d->urlLabel = nullptr;
    QWidgetAction::deleteWidget(widget);
}

} // namespace Digikam

#include "moc_dlogoaction.cpp"

