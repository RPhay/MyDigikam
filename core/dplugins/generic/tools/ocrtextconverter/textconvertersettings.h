/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * https://www.digikam.org
 *
 * Date        : 2022-08-26
 * Description : OCR settings widgets
 *
 * Copyright (C) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2022      by Quoc Hung Tran <quochungtran1999 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_TEXT_CONVERTER_SETTINGS_H
#define DIGIKAM_TEXT_CONVERTER_SETTINGS_H

// Qt includes

#include <QWidget>
#include <QString>


namespace DigikamGenericTextConverterPlugin
{

class TextConverterSettings  : public QWidget
{
    Q_OBJECT

public:

    explicit TextConverterSettings(QWidget* const parent = nullptr);
    ~TextConverterSettings() override;

    void setLanguagesMode(int mode);
    int  LanguagesMode()        const;

    void setPSMMode(int mode);
    int  PSMMode()              const;

    void setOEMMode(int mode);
    int  OEMMode()              const;

    void setDpi(int value);
    int  Dpi()                  const;

    void setIsSaveTextFile(bool check);
    bool isSaveTextFile()       const;

    void setIsSaveXMP(bool check);
    bool isSaveXMP()            const;

    void setDefaultSettings();

Q_SIGNALS:

    void signalSettingsChanged();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericTextConverterPlugin

#endif // DIGIKAM_TEXT_CONVERTER_SETTINGS_H
