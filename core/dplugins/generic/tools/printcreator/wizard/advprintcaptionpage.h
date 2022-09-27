/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-25
 * Description : a tool to print images
 *
 * SPDX-FileCopyrightText: 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ADV_PRINT_CAPTION_PAGE_H
#define DIGIKAM_ADV_PRINT_CAPTION_PAGE_H

// Qt includes

#include <QString>
#include <QPrinter>
#include <QList>
#include <QUrl>

// Local includes

#include "dwizardpage.h"
#include "ditemslist.h"
#include "ui_advprintcaptionpage.h"

using namespace Digikam;

namespace DigikamGenericPrintCreatorPlugin
{

class AdvPrintPhoto;

class AdvPrintCaptionPage : public DWizardPage
{
    Q_OBJECT

public:

    explicit AdvPrintCaptionPage(QWizard* const wizard, const QString& title);
    ~AdvPrintCaptionPage() override;

    DItemsList* imagesList() const;

    void blockCaptionButtons(bool block=true);

    void initializePage()  override;
    bool validatePage()    override;

    static QString captionFormatter(AdvPrintPhoto* const photo);

public Q_SLOTS:

    void slotCaptionChanged(int);
    void slotUpdateImagesList();
    void slotUpdateCaptions();

private:

    /**
     * Fix caption group layout according to captions combobox text.
     */
    void enableCaptionGroup(int);

    void updateCaption(AdvPrintPhoto* const);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericPrintCreatorPlugin

#endif // DIGIKAM_ADV_PRINT_CAPTION_PAGE_H
