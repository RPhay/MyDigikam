/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-01-29
 * Description : Camera settings container.
 *
 * SPDX-FileCopyrightText: 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QString>
#include <QPointer>

class QAction;

namespace Digikam
{

class ImportUI;

class CameraType
{
public:

    CameraType();
    CameraType(const QString& title, const QString& model,
               const QString& port, const QString& path,
               int startingNumber, QAction* const action = nullptr);
    ~CameraType();

    CameraType(const CameraType& ctype);
    CameraType& operator=(const CameraType& type);

    void setTitle(const QString& title);
    void setModel(const QString& model);
    void setPort(const QString& port);
    void setPath(const QString& path);
    void setStartingNumber(int sn);
    void setAction(QAction* const action);
    void setValid(bool valid);
    void setCurrentImportUI(ImportUI* const importui);

    QString   title()           const;
    QString   model()           const;
    QString   port()            const;
    QString   path()            const;
    int       startingNumber()  const;
    QAction*  action()          const;
    bool      valid()           const;
    ImportUI* currentImportUI() const;

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
