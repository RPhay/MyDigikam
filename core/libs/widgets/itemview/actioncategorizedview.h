/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-01-20
 * Description : action categorized view
 *
 * SPDX-FileCopyrightText: 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ACTION_CATEGORIZED_VIEW_H
#define DIGIKAM_ACTION_CATEGORIZED_VIEW_H

// Local includes

#include "digikam_export.h"
#include "dcategorizedview.h"

class QWidget;
class QEvent;
class QScrollBar;
class QMouseEvent;
class QPropertyAnimation;

namespace Digikam
{

class DIGIKAM_EXPORT ActionCategorizedView : public DCategorizedView
{
    Q_OBJECT

public:

    explicit ActionCategorizedView(QWidget* const parent = nullptr, bool autoScroll = false);
    ~ActionCategorizedView()            override;

    void setupIconMode();
    void adjustGridSize();

protected:

    int  autoScrollDuration(float relativeDifference, QPropertyAnimation* animation);
    void autoScroll(float relativePos, QScrollBar* scrollBar, QPropertyAnimation* animation);
    void mouseMoveEvent(QMouseEvent* e) override;
    void leaveEvent(QEvent* e)          override;

protected:

    QPropertyAnimation* m_verticalScrollAnimation;
    QPropertyAnimation* m_horizontalScrollAnimation;
    bool                m_autoScroll;
};

} // namespace Digikam

#endif // DIGIKAM_ACTION_CATEGORIZED_VIEW_H
