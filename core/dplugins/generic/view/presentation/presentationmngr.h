/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-01-31
 * Description : a presentation tool.
 *
 * SPDX-FileCopyrightText: 2006-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * SPDX-FileCopyrightText: 2009      by Andi Clemens <andi dot clemens at googlemail dot com>
 * SPDX-FileCopyrightText: 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_PRESENTATION_MNGR_H
#define DIGIKAM_PRESENTATION_MNGR_H

// Qt includes

#include <QObject>
#include <QList>
#include <QUrl>

// Local includes

#include "dplugingeneric.h"
#include "dinfointerface.h"
#include "presentationdlg.h"

using namespace Digikam;

namespace DigikamGenericPresentationPlugin
{

class PresentationContainer;

class PresentationMngr : public QObject
{
    Q_OBJECT

public:

    explicit PresentationMngr(QObject* const parent, DInfoInterface* const iface);
    ~PresentationMngr() override;

    void addFiles(const QList<QUrl>& urls);
    void showConfigDialog();

    void setPlugin(DPlugin* const plugin);

private Q_SLOTS:

    void slotSlideShow();

private:

    DPlugin*               m_plugin;
    PresentationDlg*       m_dialog;
    PresentationContainer* m_sharedData;
};

} // namespace DigikamGenericPresentationPlugin

#endif // DIGIKAM_PRESENTATION_MNGR_H
