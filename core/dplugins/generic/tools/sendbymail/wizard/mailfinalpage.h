/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-06-27
 * Description : a tool to export items by email.
 *
 * SPDX-FileCopyrightText: 2017-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_MAIL_FINAL_PAGE_H
#define DIGIKAM_MAIL_FINAL_PAGE_H

// Qt includes

#include <QString>

// Local includes

#include "dwizardpage.h"

using namespace Digikam;

namespace DigikamGenericSendByMailPlugin
{

class MailFinalPage : public DWizardPage
{
    Q_OBJECT

public:

    explicit MailFinalPage(QWizard* const dialog, const QString& title);
    ~MailFinalPage()        override;

    void initializePage()   override;
    bool isComplete() const override;
    void cleanupPage()      override;

private Q_SLOTS:

    void slotDone();
    void slotProcess();
    void slotMessage(const QString&, bool);

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace DigikamGenericSendByMailPlugin

#endif // DIGIKAM_MAIL_FINAL_PAGE_H
