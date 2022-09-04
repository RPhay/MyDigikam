/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * https://www.digikam.org
 *
 * Date        : 2022-08-26
 * Description : Text converter batch dialog
 *
 * Copyright (C) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2022      by Quoc Hung Tran <quochungtran1999 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_TEXT_CONVERTER_DIALOG_H
#define DIGIKAM_TEXT_CONVERTER_DIALOG_H

// Qt includes

#include <QUrl>
#include <QTreeWidgetItem>

// Local includes

#include "dplugindialog.h"
#include "dinfointerface.h"

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

class TextConverterActionData;

class TextConverterDialog : public DPluginDialog
{

    Q_OBJECT

public:

    explicit TextConverterDialog(QWidget* const parent, DInfoInterface* const iface);
    ~TextConverterDialog() override;

   void addItems(const QList<QUrl>& itemList);

protected:

    void closeEvent(QCloseEvent* e);

private:

    void readSettings();
    void saveSettings();

    void busy(bool busy);

    void processAll();
    void processingFailed(const QUrl& url, int result);
    void processed(const QUrl& url, const QString& outputFile, const QString& ocrResult);

    int  calculateNumberOfWords(const QString& text);

Q_SIGNALS:

    void singalMetadataChangedForUrl(const QUrl&);

private Q_SLOTS:

    void slotDefault();
    void slotClose();
    void slotStartStop();
    void slotAborted();
    void slotThreadFinished();

    void slotTextConverterAction(const DigikamGenericTextConverterPlugin::TextConverterActionData&);
    void slotDoubleClick(QTreeWidgetItem* element);
    void slotUpdateText();

    void slotSetDisable();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericTextConverterPlugin

#endif // DIGIKAM_TEXT_CONVERTER_DIALOG_H
