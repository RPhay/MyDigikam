/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-10-08
 * Description : a widget to edit Application2 ObjectAttribute
 *               Iptc tag.
 *
 * SPDX-FileCopyrightText: 2007-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_OBJECT_ATTRIBUTES_EDIT_H
#define DIGIKAM_OBJECT_ATTRIBUTES_EDIT_H

// Qt includes

#include <QWidget>

namespace DigikamGenericMetadataEditPlugin
{

class ObjectAttributesEdit : public QWidget
{
    Q_OBJECT

public:

    explicit ObjectAttributesEdit(QWidget* const parent, int size=-1);
    ~ObjectAttributesEdit()       override;

    void setValid(bool v);
    bool isValid()          const;

    void setValues(const QStringList& values);
    bool getValues(QStringList& oldValues, QStringList& newValues);

Q_SIGNALS:

    void signalModified();

private Q_SLOTS:

    void slotSelectionChanged();
    void slotAddValue();
    void slotDeleteValue();
    void slotReplaceValue();
    void slotLineEditModified();

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace DigikamGenericMetadataEditPlugin

#endif // DIGIKAM_OBJECT_ATTRIBUTES_EDIT_H
