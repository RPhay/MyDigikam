/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
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

#ifndef QTAV_PLAYER_PROPERTYEDITOR_H
#define QTAV_PLAYER_PROPERTYEDITOR_H

#include <QObject>
#include <QMetaProperty>
#include <QMetaEnum>

QT_BEGIN_NAMESPACE
class QAction;
class QWidget;
QT_END_NAMESPACE
class PropertyEditor : public QObject
{
    Q_OBJECT
public:
    explicit PropertyEditor(QObject *parent = 0);
    // call it before others
    void getProperties(QObject *obj);

    // from config file etc to init properties. call it before buildXXX
    void set(const QVariantHash& hash);
    void set(const QString& conf);
    /*!
     * \brief buildOptions
     * \return command line options
     */
    QString buildOptions();
    QWidget* buildUi(QObject* obj = 0); //obj: read dynamic properties("detail_property")
    QVariantHash exportAsHash();
    QString exportAsConfig(); //json like

private:
    /*!
     * name is property name.
     * 1. add property and value in hash
     * 2. add a widget and set value
     * 3. connect widget value change signal to a slot
     */
    QWidget* createWidgetForFlags(const QString& name, const QVariant& value, QMetaEnum me, const QString& detail = QString(), QWidget* parent = 0);
    QWidget* createWidgetForEnum(const QString& name, const QVariant& value, QMetaEnum me, const QString& detail = QString(), QWidget* parent = 0);
    QWidget* createWidgetForInt(const QString& name, int value, const QString& detail = QString(), QWidget* parent = 0);
    QWidget* createWidgetForReal(const QString& name, qreal value, const QString& detail = QString(), QWidget* parent = 0);
    QWidget* createWidgetForText(const QString& name, const QString& value, bool readOnly, const QString& detail = QString(), QWidget* parent = 0);
    QWidget* createWidgetForBool(const QString& name, bool value, const QString& detail = QString(), QWidget* parent = 0);

    // called if value changed by ui (in onXXXChange)
    void updatePropertyValue(const QString& name, const QVariant& value);
private Q_SLOTS:
    // updatePropertyValue
    void onFlagChange(QAction *action);
    void onEnumChange(int value);
    void onIntChange(int value);
    void onRealChange(qreal value);
    void onTextChange(const QString& value);
    void onBoolChange(bool value);

private:
    QList<QMetaProperty> mMetaProperties;
    QVariantHash mProperties;
    QVariantHash mPropertyDetails;
};

#endif // QTAV_PLAYER_PROPERTYEDITOR_H
