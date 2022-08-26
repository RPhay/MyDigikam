/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * https://www.digikam.org
 *
 * Date        : 2022-08-26
 * Description : OCR Tesseract engine
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

#ifndef OCR_TESSERACT_ENGINE_H
#define OCR_TESSERACT_ENGINE_H

// Qt includes

#include <QString>

// Local includes

#include "drawdecoder.h"
#include "digikam_export.h"

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

class OcrTesseracrEngine : public QObject
{
    Q_OBJECT
public:

    enum ConvertError
    {
        PROCESS_CONTINUE       =  1,        ///< Current stages is done.
        PROCESS_COMPLETE       =  0,        ///< All stages done.
        PROCESS_FAILED         = -1,        ///< A failure happen while processing.
        PROCESS_CANCELED       = -2,        ///< User has canceled processing.
    };

public:


    explicit OcrTesseracrEngine();
    ~OcrTesseracrEngine();

    QString inputFile()          const;
    QString outputFile()         const;
    QString outputText()         const;

    void setInputFile(const QString& filePath);
    void setOutputFile(const QString& filePath);

    void setLanguagesMode(int mode);
    int  languagesMode()         const;

    void setPSMMode(int mode);
    int  PSMMode()               const;

    void setOEMMode(int mode);
    int  OEMMode()               const;

    void setDpi(int value);
    int  Dpi()                   const;

    void setIsSaveTextFile(bool check);
    bool isSaveTextFile()        const;

    void setIsSaveXMP(bool check);
    bool isSaveXMP()             const;

public:

    void saveTextFile(const QString& filePath, const QString& text);
    void saveXMP(const QString& filePath,  const QString& text);
    bool runOcrProcess();

private:

    // Disable

    OcrTesseracrEngine(const OcrTesseracrEngine&)            = delete;
    OcrTesseracrEngine& operator=(const OcrTesseracrEngine&) = delete;

private:

    void SaveOcrResult();

private:

    class Private;
    Private* const d;

};

} // namespace DigikamGenericTextConverterPlugin

#endif // OCR_TESSERACT_ENGINE_H