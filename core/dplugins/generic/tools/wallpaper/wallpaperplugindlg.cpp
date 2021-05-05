/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-04-02
 * Description : plugin to export image as wallpaper - Settings dialog
 *
 * Copyright (C) 2019      by Igor Antropov <antropovi at yahoo dot com>
 * Copyright (C) 2019-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "wallpaperplugindlg.h"

// Qt includes

#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QApplication>
#include <QStyle>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "wallpaperplugin.h"

namespace DigikamGenericWallpaperPlugin
{

class Q_DECL_HIDDEN WallpaperPluginDlg::Private
{
public:

    explicit Private()
      : layoutCB(nullptr),
        page    (nullptr)
    {
    }

    QComboBox* layoutCB;
    QWidget*   page;
};

WallpaperPluginDlg::WallpaperPluginDlg(DPlugin* const plugin, QWidget* const parent)
    : DPluginDialog(parent, QLatin1String("WallpaperPluginDlg")),
      d            (new Private)
{
    setPlugin(plugin);
    setWindowIcon(plugin->icon());
    setWindowTitle(i18n("WallPaper Settings"));

    m_buttons->addButton(QDialogButtonBox::Cancel);
    m_buttons->addButton(QDialogButtonBox::Ok);
    m_buttons->button(QDialogButtonBox::Ok)->setDefault(true);
    d->page                  = new QWidget(this);
    QVBoxLayout* const vbx   = new QVBoxLayout(this);
    vbx->addWidget(d->page);
    vbx->addWidget(m_buttons);
    setLayout(vbx);
    setModal(false);

    // -------------------

    const int spacing       = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    QGridLayout* const grid = new QGridLayout(d->page);
    QLabel* const lbl       = new QLabel(i18n("Wallpaper Layout:"), d->page);
    d->layoutCB             = new QComboBox(d->page);
    d->layoutCB->addItem(i18n("Adjusted"),                   WallpaperPlugin::Adjusted);

#ifndef Q_OS_WIN

    d->layoutCB->addItem(i18n("Adjusted with Aspect ratio"), WallpaperPlugin::AdjustedAspectRatio);
    d->layoutCB->addItem(i18n("Adjusted and cropped"),       WallpaperPlugin::AdjustedCropped);

#endif

    d->layoutCB->addItem(i18n("Mosaic"),                     WallpaperPlugin::Mosaic);
    d->layoutCB->addItem(i18n("Centered"),                   WallpaperPlugin::Centered);
    lbl->setBuddy(d->layoutCB);

    grid->addWidget(lbl,         0, 0, 1, 1);
    grid->addWidget(d->layoutCB, 0, 1, 1, 1);
    grid->setSpacing(spacing);

    // -------------------

    connect(m_buttons->button(QDialogButtonBox::Cancel), &QPushButton::clicked,
            this, &WallpaperPluginDlg::reject);

    connect(m_buttons->button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &WallpaperPluginDlg::accept);

    // -------------------

    adjustSize();
}

WallpaperPluginDlg::~WallpaperPluginDlg()
{
    delete d;
}

int WallpaperPluginDlg::wallpaperLayout() const
{
    return (d->layoutCB->currentData().toInt());
}

} // namespace DigikamGenericWallpaperPlugin
