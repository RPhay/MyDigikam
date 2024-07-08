/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QColor>
#include <QImage>
#include <QString>
#include <QSvgRenderer>

class OsmcSymbol
{
public:

    explicit OsmcSymbol(const QString &tag, int size = 20);
    ~OsmcSymbol();

    QImage icon() const;
    QColor wayColor() const;

private:

    bool parseTag(const QString &tag);
    bool parseBackground(const QString &bg);
    QSvgRenderer* parseForeground(const QString &fg);

    void render();

private:

    QColor          m_wayColor;
    QColor          m_backgroundColor;
    QString         m_backgroundType;
    QSvgRenderer *  m_foreground      = nullptr;
    QSvgRenderer *  m_foreground2     = nullptr;
    QString         m_text;
    QColor          m_textColor;

    QImage          m_image;

    QStringList     m_backgroundTypes;
    QStringList     m_foregroundTypes;
    QStringList     m_precoloredForegroundTypes;

    int const       m_side;
/*
    int             m_wayWidth;
*/
};
