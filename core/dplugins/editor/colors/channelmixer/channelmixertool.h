/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-02-26
 * Description : image channels mixer.
 *
 * SPDX-FileCopyrightText: 2005-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_EDITOR_CHANNEL_MIXER_TOOL_H
#define DIGIKAM_EDITOR_CHANNEL_MIXER_TOOL_H

// Local includes

#include "editortool.h"

using namespace Digikam;

namespace DigikamEditorChannelMixerToolPlugin
{

class ChannelMixerTool : public EditorToolThreaded
{
    Q_OBJECT

public:

    explicit ChannelMixerTool(QObject* const parent);
    ~ChannelMixerTool()         override;

private:

    void readSettings()         override;
    void writeSettings()        override;
    void preparePreview()       override;
    void prepareFinal()         override;
    void setPreviewImage()      override;
    void setFinalImage()        override;

    void updateSettingsWidgets();

private Q_SLOTS:

    void slotResetSettings()    override;
    void slotSaveAsSettings()   override;
    void slotLoadSettings()     override;
    void slotOutChannelChanged();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamEditorChannelMixerToolPlugin

#endif // DIGIKAM_EDITOR_CHANNEL_MIXER_TOOL_H
