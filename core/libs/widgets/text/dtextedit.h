/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-01
 * Description : Two plain text edit widgets with spell checker capabilities based on KF5::Sonnet.
 *               Widgets can be also limited to a number of lines to show text.
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_DTEXT_EDIT_H
#define DIGIKAM_DTEXT_EDIT_H

// Qt includes

#include <QString>
#include <QTextEdit>
#include <QPlainTextEdit>

// Local includes

#include "digikam_export.h"

namespace Sonnet
{
    class SpellCheckDecorator;
}

namespace Digikam
{

class DIGIKAM_EXPORT DTextEdit : public QTextEdit
{
    Q_OBJECT

public:

    /**
     * Default constructor.
     */
    explicit DTextEdit(QWidget* const parent = nullptr);

    /**
     * Constructor with a number of lines. Zero lines do not apply a size constraint.
     */
    DTextEdit(unsigned int lines, QWidget* const parent = nullptr);

    /**
     * Constructor with text contents to use.
     */
    DTextEdit(const QString& contents, QWidget* const parent = nullptr);

    /**
     * Standard destructor.
     */
    ~DTextEdit() override;

    /**
     * Helper methods to handle text contents as plain text.
     */
    QString text() const;
    void setText(const QString& text);

    /**
     * Helper methods to handle visible lines used by the widget to show text.
     * Lines must be superior or egal to 2.
     */
    void setLinesVisible(unsigned int lines);
    unsigned int linesVisible() const;

    /**
     * Helper methods to handle a specific spell-checker language (2 letters code based as "en", "fr", "es", etc.).
     * If this property is not set, spell-checker will try to auto-detect language by parsing the text.
     */
    void setCurrentLanguage(const QString& lang);
    QString currentLanguage() const;

private:

    /**
     * Init the text widget with the spell-checker engine.
     */
    void init();

private:

    Sonnet::SpellCheckDecorator* m_spellChecker = nullptr;
    unsigned int m_lines                        = 2;
};

// ---------------------------------------------------------------------------

class DIGIKAM_EXPORT DPlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:

    /**
     * Default constructor.
     */
    explicit DPlainTextEdit(QWidget* const parent = nullptr);

    /**
     * Constructor with a number of lines. Zero lines do not apply a size constraint.
     */
    DPlainTextEdit(unsigned int lines, QWidget* const parent = nullptr);

    /**
     * Constructor with text contents to use.
     */
    DPlainTextEdit(const QString& contents, QWidget* const parent = nullptr);

    /**
     * Standard destructor.
     */
    ~DPlainTextEdit() override;

    /**
     * Helper methods to handle text contents as plain text.
     */
    QString text() const;
    void setText(const QString& text);

    /**
     * Helper methods to handle visible lines used by the widget to show text.
     * Lines must be superior or egal to 2.
     */
    void setLinesVisible(unsigned int lines);
    unsigned int linesVisible() const;

    /**
     * Helper methods to handle a specific spell-checker language (2 letters code based as "en", "fr", "es", etc.).
     * If this property is not set, spell-checker will try to auto-detect language by parsing the text.
     */
    void setCurrentLanguage(const QString& lang);
    QString currentLanguage() const;

private:

    /**
     * Init the text widget with the spell-checker engine.
     */
    void init();

private:

    Sonnet::SpellCheckDecorator* m_spellChecker = nullptr;
    unsigned int m_lines                        = 2;
};

} // namespace Digikam

#endif // DIGIKAM_LINE_EDIT_SPELL_CHECKER_H
