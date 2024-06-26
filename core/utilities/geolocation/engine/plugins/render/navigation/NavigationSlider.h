// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#pragma once

#include <QAbstractSlider>
#include <QPixmap>

namespace Marble
{

class NavigationSlider : public QAbstractSlider
{
    Q_OBJECT
public:
    explicit NavigationSlider( QWidget *parent = nullptr );
    ~NavigationSlider() override;

Q_SIGNALS:
    void repaintNeeded();

protected:

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

    void enterEvent( QEnterEvent * ) override;

#else

    void enterEvent( QEvent * ) override;

#endif

    void mouseMoveEvent( QMouseEvent * mouseEvent) override;
    void mousePressEvent( QMouseEvent * ) override;
    void mouseReleaseEvent( QMouseEvent * ) override;
    void leaveEvent( QEvent * ) override;
    void paintEvent( QPaintEvent * ) override;
    void repaint();

private:
    static QPixmap pixmap(const QString &id );
    QString m_handleImagePath;
};

}
