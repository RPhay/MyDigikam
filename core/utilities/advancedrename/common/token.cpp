/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-08-08
 * Description : a token class
 *
 * SPDX-FileCopyrightText: 2009-2012 by Andi Clemens <andi dot clemens at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "token.h"

// Qt includes

#include <QAction>

namespace Digikam
{

Token::Token(const QString& id, const QString& description)
    : QObject(nullptr),
      m_id(id),
      m_description(description)
{
    m_action = new QAction(this);
    m_action->setText(id);
    m_action->setToolTip(description);

    connect(m_action, SIGNAL(triggered()), this, SLOT(slotTriggered()));
}

Token::~Token()
{
    delete m_action;
}

void Token::slotTriggered()
{
    Q_EMIT signalTokenTriggered(m_id);
}

} // namespace Digikam
