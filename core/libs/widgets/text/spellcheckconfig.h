/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-08-16
 * Description : Spell-check Config widget.
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_SPELL_CHECK_CONFIG_H
#define DIGIKAM_SPELL_CHECK_CONFIG_H

// Qt includes

#include <QWidget>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT SpellCheckConfig : public QWidget
{
    Q_OBJECT

public:

    explicit SpellCheckConfig(QWidget* const parent = nullptr);
    ~SpellCheckConfig() override;

    void applySettings();
    void readSettings();

private Q_SLOTS:

    void slotSpellcheckActivated(bool);
    void slotIgnoreWordSelectionChanged();
    void slotAddWord();
    void slotDelWord();
    void slotRepWord();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SPELL_CHECK_CONFIG_H
