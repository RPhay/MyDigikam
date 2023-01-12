/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-06
 * Description : image editor printing interface.
 *
 * SPDX-FileCopyrightText: 2009      by Angelo Naselli <anaselli at linux dot it>
 * SPDX-FileCopyrightText: 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "printhelper.h"

// C++ includes

#include <memory>

// Qt includes

#include <QWidget>
#include <QCheckBox>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "printoptionspage.h"
#include "iccmanager.h"

namespace DigikamEditorPrintToolPlugin
{

class Q_DECL_HIDDEN PrintHelper::Private
{

public:

    explicit Private()
      : parent(nullptr)
    {
    }

    QWidget* parent;

public:

    QSize adjustSize(PrintOptionsPage* const optionsPage,
                     const DImg& doc,
                     int printerResolution,
                     const QSize& viewportSize)
    {
        QSize size                            = doc.size();
        PrintOptionsPage::ScaleMode scaleMode = optionsPage->scaleMode();

        if      (scaleMode == PrintOptionsPage::ScaleToPage)
        {
            bool imageBiggerThanPaper = (size.width()  > viewportSize.width()) ||
                                        (size.height() > viewportSize.height());

            if (imageBiggerThanPaper || optionsPage->enlargeSmallerImages())
            {
                size.scale(viewportSize, Qt::KeepAspectRatio);
            }

        }
        else if (scaleMode == PrintOptionsPage::ScaleToCustomSize)
        {
            double wImg = optionsPage->scaleWidth();
            double hImg = optionsPage->scaleHeight();
            size.setWidth(int  (wImg * printerResolution));
            size.setHeight(int (hImg * printerResolution));
        }
        else
        {
            // No scale
            const double INCHES_PER_METER = 100. / 2.54;
            QImage img                    = doc.copyQImage();
            int dpmX                      = img.dotsPerMeterX();
            int dpmY                      = img.dotsPerMeterY();

            if ((dpmX > 0) && (dpmY > 0))
            {
                double wImg = double(size.width())  / double(dpmX) * INCHES_PER_METER;
                double hImg = double(size.height()) / double(dpmY) * INCHES_PER_METER;
                size.setWidth(int  (wImg * printerResolution));
                size.setHeight(int (hImg * printerResolution));
            }
        }

        return size;
    }

    QPoint adjustPosition(PrintOptionsPage* const optionsPage,
                          const QSize& imageSize,
                          const QSize& viewportSize)
    {
        Qt::Alignment alignment = optionsPage->alignment();
        int posX;
        int posY;

        if      (alignment & Qt::AlignLeft)
        {
            posX = 0;
        }
        else if (alignment & Qt::AlignHCenter)
        {
            posX = (viewportSize.width() - imageSize.width()) / 2;
        }
        else
        {
            posX = viewportSize.width() - imageSize.width();
        }

        if      (alignment & Qt::AlignTop)
        {
            posY = 0;
        }
        else if (alignment & Qt::AlignVCenter)
        {
            posY = (viewportSize.height() - imageSize.height()) / 2;
        }
        else
        {
            posY = viewportSize.height() - imageSize.height();
        }

        return QPoint(posX, posY);
    }

    void adjustImage(PrintOptionsPage* const optionsPage,
                     const DImg& img)
    {
        if (optionsPage->colorManaged())
        {
            IccManager manager(img);
            manager.transformForOutput(optionsPage->outputProfile());
        }
    }
};

// ------------------------------------------------------------------------------

PrintHelper::PrintHelper(QWidget* const parent)
    : d(new Private)
{
    d->parent = parent;
}

PrintHelper::~PrintHelper()
{
    delete d;
}

void PrintHelper::print(DImg& doc)
{
    QPrinter printer;

    QPrintDialog* const dialog          = new QPrintDialog(&printer, d->parent);
    dialog->setWindowTitle(i18nc("@title:window", "Print Image"));
    PrintOptionsPage* const optionsPage = new PrintOptionsPage(d->parent, doc.size());
    optionsPage->loadConfig();
    dialog->setOptionTabs(QList<QWidget*>() << optionsPage);

    bool wantToPrint = (dialog->exec() == QDialog::Accepted);

    optionsPage->saveConfig();

    if (!wantToPrint)
    {
        return;
    }

    if (optionsPage->autoRotation())
    {
        printer.setPageOrientation(doc.size().width() <= doc.size().height() ? QPageLayout::Portrait
                                                                             : QPageLayout::Landscape);
    }

    QPainter painter(&printer);
    QRect rect   = painter.viewport();
    QSize size   = d->adjustSize(optionsPage, doc, printer.resolution(), rect.size());
    QPoint pos   = d->adjustPosition(optionsPage, size, rect.size());
    d->adjustImage(optionsPage, doc);
    painter.setViewport(pos.x(), pos.y(), size.width(), size.height());

    QImage image = doc.copyQImage();
    painter.setWindow(image.rect());
    painter.drawImage(0, 0, image);
}

} // namespace DigikamEditorPrintToolPlugin
