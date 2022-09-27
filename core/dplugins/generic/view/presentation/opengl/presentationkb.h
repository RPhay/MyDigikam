/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-11-14
 * Description : a presentation tool.
 *
 * SPDX-FileCopyrightText: 2007-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * Parts of this code are based on
 * smoothslidesaver by Carsten Weinhold <carsten dot weinhold at gmx dot de>
 * and slideshowgl by Renchi Raju <renchi dot raju at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_PRESENTATION_KB_H
#define DIGIKAM_PRESENTATION_KB_H

// C++ includes

#ifndef Q_CC_MSVC
#   include <cstdlib>
#else
#   include <winsock2.h>
#endif

// Qt includes

#include <QList>
#include <QKeyEvent>
#include <QMap>
#include <QMouseEvent>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QOpenGLWidget>
#include <QOpenGLTexture>
#include <QRandomGenerator>

namespace DigikamGenericPresentationPlugin
{

class PresentationContainer;

class KBViewTrans
{

public:

    KBViewTrans(bool zoomIn, float relAspect);
    KBViewTrans();
    ~KBViewTrans();

    float transX(float pos) const;
    float transY(float pos) const;
    float scale(float pos)  const;
    float xScaleCorrect()   const;
    float yScaleCorrect()   const;

private:

    double rnd()            const;
    double rndSign()        const;

private:

    /// delta and scale values (begin to end) and the needed offsets
    double m_deltaX;
    double m_deltaY;
    double m_deltaScale;
    double m_baseScale;
    double m_baseX;
    double m_baseY;
    float  m_xScale;
    float  m_yScale;
};

// -------------------------------------------------------------------------

class KBImage
{

public:

    explicit KBImage(KBViewTrans* const viewTrans, float aspect = 1.0);
    ~KBImage();

public:

    KBViewTrans*    m_viewTrans;
    float           m_aspect;
    float           m_pos;
    float           m_opacity;
    bool            m_paint;
    QOpenGLTexture* m_texture;
};

// -------------------------------------------------------------------------

class PresentationKB : public QOpenGLWidget
{
    Q_OBJECT

public:

    explicit PresentationKB(PresentationContainer* const sharedData);

    ~PresentationKB() override;

    static QStringList            effectNames();
    static QMap<QString, QString> effectNamesI18N();

    bool checkOpenGL()                      const;

private:

    float    aspect()                       const;
    bool     setupNewImage(int imageIndex);
    void     startSlideShowOnce();
    void     swapImages();
    void     setNewKBEffect();
    void     endOfShow();

    void     applyTexture(KBImage* const img, const QImage& image);
    void     paintTexture(KBImage* const img);
    unsigned suggestFrameRate(unsigned forceRate);

    void     readSettings();

protected:

    void initializeGL()                override;
    void paintGL()                     override;
    void resizeGL(int w, int h)        override;

    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*)  override;
    void keyPressEvent(QKeyEvent*)     override;

private Q_SLOTS:

    void moveSlot();
    void slotMouseMoveTimeOut();
    void slotClose();

private:

    // Disable
    explicit PresentationKB(QWidget*) = delete;

private:

    class Private;
    Private* const d;

    friend class KBEffect;
};

} // namespace DigikamGenericPresentationPlugin

#endif // DIGIKAM_PRESENTATION_KB_H
