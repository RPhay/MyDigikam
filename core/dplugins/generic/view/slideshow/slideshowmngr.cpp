/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-01-31
 * Description : a presentation tool.
 *
 * Copyright (C) 2006-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at googlemail dot com>
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2012-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2020      by Minh Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#include "slideshowmngr.h"

// C++ includes

#include <cstdlib>

// Qt includes
#include <QApplication>


namespace DigikamGenericSlideShowPlugin
{

SlideShowMngr::SlideShowMngr(QObject* const parent, DInfoInterface* const iface)
    : QObject(parent),
      m_plugin(nullptr),
      m_dialog(nullptr)
{
    m_loader = new SlideShowLoader(iface, SlideShowSettings());
}

SlideShowMngr::~SlideShowMngr()
{
    delete m_dialog;
    delete m_loader;
}

void SlideShowMngr::setPlugin(DPlugin* const plugin)
{
    m_plugin = plugin;
}

void SlideShowMngr::addFiles(const QList<QUrl>& urls)
{
    //m_sharedData->urlList = urls;
}


void SlideShowMngr::showConfigDialog()
{
    m_dialog = new SetupSlideShowDialog(QApplication::activeWindow());

    m_dialog->setPlugin(m_plugin);

    m_dialog->show();
}


void SlideShowMngr::slotSlideShow()
{
    m_loader->show();
}

} // namespace DigikamGenericSlideShowPlugin

