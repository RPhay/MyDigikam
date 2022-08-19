/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-01
 * Description : Two text edit widgets with spell checker capabilities based on KF5::Sonnet (optional).
 *               Widgets can be also limited to a number of lines to show text.
 *               A single line constraint will mimic QLineEdit. See setLinesVisible() for details.
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

namespace Digikam
{

class SpellCheckContainer;

class DIGIKAM_EXPORT DTextEdit : public QTextEdit
{
    Q_OBJECT

public:

    /**
     * Default constructor.
     */
    DTextEdit(QWidget* const parent = nullptr);

    /**
     * Constructor with a number of lines. Zero lines do not apply a size constraint.
     */
    explicit DTextEdit(unsigned int lines, QWidget* const parent = nullptr);

    /**
     * Constructor with text contents to use.
     */
    explicit DTextEdit(const QString& contents, QWidget* const parent = nullptr);

    /**
     * Standard destructor.
     */
    ~DTextEdit() override;

    /**
     * Helper methods to handle text contents as plain text.
     * If ignored or accepted characters masks are set, text is filtered accordingly.
     */
    QString text() const;
    void setText(const QString& text);

    /**
     * Helper methods to handle the mask of ignored characters in text editor.
     * The mask of characters is passed as string (ex: "+/!()").
     * By default the mask is empty.
     */
    QString ignoredCharacters() const;
    void setIgnoredCharacters(const QString& mask);

    /**
     * Helper methods to handle the mask of accepted characters in text editor.
     * The mask of characters is passed as string (ex: "abcABC").
     * By default the mask is empty.
     */
    QString acceptedCharacters() const;
    void setAcceptedCharacters(const QString& mask);

    /**
     * Helper methods to handle visible lines used by the widget to show text.
     * Lines must be superior or egal to 1 to apply a size constraint.
     * Notes: if a single visible line is used, the widget mimic QLineEdit.
     *        a null value do not apply a size constraint.
     */
    void setLinesVisible(unsigned int lines);
    unsigned int linesVisible() const;

    /**
     * Helper methods to handle a specific spell-checker language (2 letters code based as "en", "fr", "es", etc.).
     * If this property is not set, spell-checker will try to auto-detect language by parsing the text.
     * To reset this setting, pass a empty string as language.
     * If KF5::Sonnet depedencies is not resolved, these method do nothing.
     */
    void setCurrentLanguage(const QString& lang);
    QString currentLanguage() const;

    /**
     * Helper methods to handle the Spellcheck settings.
     * See SpellCheckContainer class for details.
     */
    SpellCheckContainer spellCheckSettings() const;
    void setSpellCheckSettings(const SpellCheckContainer& settings);

Q_SIGNALS:

    /**
     * Emmited only when mimic QLineEdit mode is enabled. See setLinesVisible() for details.
     */
    void returnPressed();
    void textEdited(const QString&);

protected:

    void insertFromMimeData(const QMimeData* source) override;
    void keyPressEvent(QKeyEvent* e)                 override;

private:

    class Private;
    Private* const d;
};

// ---------------------------------------------------------------------------

class DIGIKAM_EXPORT DPlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:

    /**
     * Default constructor.
     */
    DPlainTextEdit(QWidget* const parent = nullptr);

    /**
     * Constructor with a number of lines. Zero lines do not apply a size constraint.
     */
    explicit DPlainTextEdit(unsigned int lines, QWidget* const parent = nullptr);

    /**
     * Constructor with text contents to use.
     */
    explicit DPlainTextEdit(const QString& contents, QWidget* const parent = nullptr);

    /**
     * Standard destructor.
     */
    ~DPlainTextEdit() override;

    /**
     * Helper methods to handle text contents as plain text.
     * If ignored or accepted characters masks are set, text is filtered accordingly.
     */
    QString text() const;
    void setText(const QString& text);

    /**
     * Helper methods to handle the mask of ignored characters in text editor.
     * The mask of characters is passed as string (ex: "+/!()").
     * By default the mask is empty.
     */
    QString ignoredCharacters() const;
    void setIgnoredCharacters(const QString& mask);

    /**
     * Helper methods to handle the mask of accepted characters in text editor.
     * The mask of characters is passed as string (ex: "abcABC").
     * By default the mask is empty.
     */
    QString acceptedCharacters() const;
    void setAcceptedCharacters(const QString& mask);

    /**
     * Helper methods to handle visible lines used by the widget to show text.
     * Lines must be superior or egal to 1 to apply a size constraint.
     * Notes: if a single visible line is used, the widget mimic QLineEdit.
     *        a null value do not apply a size constraint.
     */
    void setLinesVisible(unsigned int lines);
    unsigned int linesVisible() const;

    /**
     * Helper methods to handle a specific spell-checker language (2 letters code based as "en", "fr", "es", etc.).
     * If this property is not set, spell-checker will try to auto-detect language by parsing the text.
     * To reset this setting, pass a empty string as language.
     * If KF5::Sonnet depedencies is not resolved, these method do nothing.
     */
    void setCurrentLanguage(const QString& lang);
    QString currentLanguage() const;

    /**
     * Helper methods to handle the Spellcheck settings.
     * See SpellCheckContainer class for details.
     */
    SpellCheckContainer spellCheckSettings() const;
    void setSpellCheckSettings(const SpellCheckContainer& settings);

Q_SIGNALS:

    /**
     * Emmited only when mimic QLineEdit mode is enabled. See setLinesVisible() for details.
     */
    void returnPressed();
    void textEdited(const QString&);

protected:

    void insertFromMimeData(const QMimeData* source) override;
    void keyPressEvent(QKeyEvent* e)                 override;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_LINE_EDIT_SPELL_CHECKER_H
