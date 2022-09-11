/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-06-29
 * Description : Pressable Button class using QGraphicsItem
 *               based on Frederico Duarte implementation.
 *
 * SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FACE_ENGINE_DEMO_BUTTON_H
#define DIGIKAM_FACE_ENGINE_DEMO_BUTTON_H

// Qt includes

#include <QObject>
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

namespace FaceEngineDemo
{

class Button : public QObject,
               public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:

    explicit Button(QGraphicsItem* const parent = nullptr);
    explicit Button(const QString& normal, const QString& pressed = QString(), QGraphicsItem* const parent = nullptr);
    Button(const QPixmap& normal, const QPixmap& pressed, QGraphicsItem* const parent = nullptr);
    ~Button() override;

public:

    QRectF boundingRect() const                                                                      override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    void setPixmap(const QString& normal, const QString& pressed = QString());
    void setPixmap(const QPixmap& normal, const QPixmap& pressed);

Q_SIGNALS:

    void clicked();

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent*)   override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent*)    override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*) override;

private:

    // Disable
    explicit Button(QObject*);

    class Private;
    Private* const d;
};

} // namespace FaceEngineDemo

#endif // DIGIKAM_FACE_ENGINE_DEMO_BUTTON_H
