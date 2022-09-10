/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef QTAV_WIDGETS_PROPERTY_EDITOR_H
#define QTAV_WIDGETS_PROPERTY_EDITOR_H

// Qt includes

#include <QObject>
#include <QMetaProperty>
#include <QMetaEnum>

// Local includes

#include "QtAVWidgets_Global.h"

class QAction;
class QWidget;

namespace QtAV
{

class DIGIKAM_EXPORT PropertyEditor : public QObject
{
    Q_OBJECT

public:

    explicit PropertyEditor(QObject* const parent = nullptr);

    // call it before others

    void getProperties(QObject* const obj);

    // from config file etc to init properties. call it before buildXXX

    void set(const QVariantHash& hash);
    void set(const QString& conf);

    /*!
     * \brief buildOptions
     * \return command line options
     */
    QString buildOptions();
    QWidget* buildUi(QObject* const obj = nullptr); // obj: read dynamic properties("detail_property")
    QVariantHash exportAsHash();
    QString exportAsConfig();           // json like

private:

    /*!
     * name is property name.
     * 1. add property and value in hash
     * 2. add a widget and set value
     * 3. connect widget value change signal to a slot
     */
    QWidget* createWidgetForFlags(const QString& name, const QVariant& value, QMetaEnum me,
                                  const QString& detail = QString(), QWidget* const parent = nullptr);

    QWidget* createWidgetForEnum(const QString& name, const QVariant& value, QMetaEnum me,
                                 const QString& detail = QString(), QWidget* const parent = nullptr);

    QWidget* createWidgetForInt(const QString& name, int value, const QString& detail = QString(),
                                QWidget* const parent = nullptr);

    QWidget* createWidgetForReal(const QString& name, qreal value, const QString& detail = QString(),
                                 QWidget* const parent = nullptr);

    QWidget* createWidgetForText(const QString& name, const QString& value, bool readOnly,
                                 const QString& detail = QString(), QWidget* const parent = nullptr);

    QWidget* createWidgetForBool(const QString& name, bool value, const QString& detail = QString(),
                                 QWidget* const parent = nullptr);

    // called if value changed by ui (in onXXXChange)

    void updatePropertyValue(const QString& name, const QVariant& value);

private Q_SLOTS:

    // updatePropertyValue

    void onFlagChange(QAction* action);
    void onEnumChange(int value);
    void onIntChange(int value);
    void onRealChange(qreal value);
    void onTextChange(const QString& value);
    void onBoolChange(bool value);

private:

    QList<QMetaProperty> mMetaProperties;
    QVariantHash         mProperties;
    QVariantHash         mPropertyDetails;
};

} // namespace QtAV

#endif // QTAV_WIDGETS_PROPERTY_EDITOR_H
