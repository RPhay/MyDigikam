/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-01
 * Description : Text edit widgets with spellcheck support and edition limitations.
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

class LocalizeContainer;

/**
 * A text edit widget based on QTextEdit with spell checker capabilities based on KF5::Sonnet (optional).
 * Widget size can be constrained with the number of visible lines.
 * A single line constraint will emulate QLineEdit. See setLinesVisible() for details.
 * The maximum number of characters can be limited with setMaxLenght().
 * The characters can be limited in editor by setIgnoredCharacters() and setAcceptedCharacters().
 * Implementation: dtextedit.cpp
 */
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
     * This property holds whether the edit widget displays a clear button when it is not empty.
     * If enabled, the edit widget displays a trailing clear button when it contains some text,
     * otherwise the edit widget does not show a clear button.
     * This option only take effect in QLineEdit emulation mode when lines visible is set to 1.
     * See setLinesVisible() for details.
     */
    bool  isClearButtonEnabled() const;
    void  setClearButtonEnabled(bool enable);

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
     * Notes: if a single visible line is used, the widget will emulate QLineEdit.
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
     * See LocalizeContainer class for details.
     */
    LocalizeContainer spellCheckSettings() const;
    void setLocalizeSettings(const LocalizeContainer& settings);

    /**
     * Helper methods to handle the maximum of characters
     * that user can enter in editor.
     * By default no limit is set.
     * A zero lenght reset a limit.
     */
    void setMaxLength(int length);
    int  maxLength()     const;

    /**
     * Return the left characters that user can enter if a limit have been previously set with setMaxLeght().
     */
    int leftCharacters() const;

Q_SIGNALS:

    /**
     * Emmited only when mimic QLineEdit mode is enabled. See setLinesVisible() for details.
     */
    void returnPressed();
    void textEdited(const QString&);

protected:

    void insertFromMimeData(const QMimeData* source) override;
    void keyPressEvent(QKeyEvent* e)                 override;

private Q_SLOTS:

    /**
     * Internal slot used to display a tooltips of left characters available when enter text in editor.
     * This slot do nothing is no limit is set with setMaxLenght§).
     */
    void slotChanged();

private:

    class Private;
    Private* const d;
};

// ---------------------------------------------------------------------------

/**
 * A text edit widget based on QPlainTextEdit with spell checker capabilities based on KF5::Sonnet (optional).
 * Widget size can be constrained with the number of visible lines.
 * A single line constraint will emulate QLineEdit. See setLinesVisible() for details.
 * The maximum number of characters can be limited with setMaxLenght().
 * The characters can be limited in editor by setIgnoredCharacters() and setAcceptedCharacters().
 * Implementation: dplaintextedit.cpp
 */
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
     * This property holds whether the edit widget displays a clear button when it is not empty.
     * If enabled, the edit widget displays a trailing clear button when it contains some text,
     * otherwise the edit widget does not show a clear button.
     * This option only take effect in QLineEdit emulation mode when lines visible is set to 1.
     * See setLinesVisible() for details.
     */
    bool  isClearButtonEnabled() const;
    void  setClearButtonEnabled(bool enable);

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
     * Notes: if a single visible line is used, the widget emulate QLineEdit.
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
     * See LocalizeContainer class for details.
     */
    LocalizeContainer spellCheckSettings() const;
    void setLocalizeSettings(const LocalizeContainer& settings);

    /**
     * Helper methods to handle the maximum of characters
     * that user can enter in editor.
     * By default no limit is set.
     * A zero lenght reset a limit.
     */
    void setMaxLength(int length);
    int  maxLength()     const;

    /**
     * Return the left characters that user can enter if a limit have been previously set with setMaxLeght().
     */
    int leftCharacters() const;

Q_SIGNALS:

    /**
     * Emmited only when mimic QLineEdit mode is enabled. See setLinesVisible() for details.
     */
    void returnPressed();
    void textEdited(const QString&);

protected:

    void insertFromMimeData(const QMimeData* source) override;
    void keyPressEvent(QKeyEvent* e)                 override;

private Q_SLOTS:

    /**
     * Internal slot used to display a tooltips of left characters available when enter text in editor.
     * This slot do nothing is no limit is set with setMaxLenght§).
     */
    void slotChanged();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_LINE_EDIT_SPELL_CHECKER_H
