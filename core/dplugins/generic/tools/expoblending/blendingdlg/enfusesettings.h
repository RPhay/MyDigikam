/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a tool to blend bracketed images.
 *
 * SPDX-FileCopyrightText: 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2015      by Benjamin Girault <benjamin dot girault at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ENFUSE_SETTINGS_H
#define DIGIKAM_ENFUSE_SETTINGS_H

// Qt includes

#include <QUrl>
#include <QWidget>

// Locale includes

#include "dsavesettingswidget.h"

class KConfigGroup;

using namespace Digikam;

namespace DigikamGenericExpoBlendingPlugin
{

class EnfuseSettings
{
public:

    EnfuseSettings()
        : autoLevels  (true),
          hardMask    (false),
          ciecam02    (false),
          levels      (20),
          exposure    (1.0),
          saturation  (0.2),
          contrast    (0.0),
          outputFormat(DSaveSettingsWidget::OUTPUT_PNG)
    {
    }

    ~EnfuseSettings()
    {
    }

    QString asCommentString() const;

    QString inputImagesList() const;

public:

    bool                              autoLevels;
    bool                              hardMask;
    bool                              ciecam02;

    int                               levels;

    double                            exposure;
    double                            saturation;
    double                            contrast;

    QString                           targetFileName;

    QList<QUrl>                       inputUrls;
    QUrl                              previewUrl;

    DSaveSettingsWidget::OutputFormat outputFormat;
};

// ------------------------------------------------------------------------

class EnfuseSettingsWidget : public QWidget
{
    Q_OBJECT

public:

    explicit EnfuseSettingsWidget(QWidget* const parent);
    ~EnfuseSettingsWidget() override;

    void           setSettings(const EnfuseSettings& settings);
    EnfuseSettings settings() const;

    void           resetToDefault();

    void readSettings(const KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericExpoBlendingPlugin

#endif // DIGIKAM_ENFUSE_SETTINGS_H
