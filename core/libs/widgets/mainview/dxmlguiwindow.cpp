/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-04-29
 * Description : digiKam XML GUI window
 *
 * SPDX-FileCopyrightText: 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dxmlguiwindow_p.h"

namespace Digikam
{

DXmlGuiWindow::DXmlGuiWindow(QWidget* const parent, Qt::WindowFlags f)
    : KXmlGuiWindow(parent, f),
      d            (new Private)
{
    m_animLogo = nullptr;

    installEventFilter(this);
}

DXmlGuiWindow::~DXmlGuiWindow()
{
    saveWindowSize();

    delete d;
}

void DXmlGuiWindow::setConfigGroupName(const QString& name)
{
    d->configGroupName = name;
}

QString DXmlGuiWindow::configGroupName() const
{
    return d->configGroupName;
}

void DXmlGuiWindow::closeEvent(QCloseEvent* e)
{
    checkFullScreenBeforeClosing();

    if (!testAttribute(Qt::WA_DeleteOnClose))
    {
        e->ignore();
        return;
    }

    KXmlGuiWindow::closeEvent(e);
}

bool DXmlGuiWindow::slotClose()
{
    checkFullScreenBeforeClosing();

    return close();
}

void DXmlGuiWindow::slotShowMenuBar()
{
    menuBar()->setVisible(d->showMenuBarAction->isChecked());
}

void DXmlGuiWindow::slotShowStatusBar()
{
    statusBar()->setVisible(d->showStatusBarAction->isChecked());
}

void DXmlGuiWindow::slotConfNotifications()
{

#ifdef HAVE_KNOTIFYCONFIG

    KNotifyConfigWidget::configure(this);

#endif

}

void DXmlGuiWindow::editKeyboardShortcuts(KActionCollection* const extraac, const QString& actitle)
{
    KShortcutsDialog dialog(KShortcutsEditor::AllActions,
                            KShortcutsEditor::LetterShortcutsAllowed, this);
    dialog.addCollection(actionCollection(), i18nc("general keyboard shortcuts", "General"));

    if (extraac)
    {
        dialog.addCollection(extraac, actitle);
    }

    dialog.configure();
}

bool DXmlGuiWindow::eventFilter(QObject* obj, QEvent* ev)
{
    if (obj == this)
    {
        if (ev && (ev->type() == QEvent::HoverMove) && fullScreenIsActive())
        {
            // We will handle a stand alone FullScreen button action on top/right corner of screen
            // only if managed window tool bar is hidden, and if we switched already in Full Screen mode.

            KToolBar* const mainbar = mainToolBar();

            if (mainbar)
            {
                if (((d->fsOptions & FS_TOOLBARS) && d->fullScreenHideToolBars) || !mainbar->isVisible())
                {
                    QHoverEvent* const mev = dynamic_cast<QHoverEvent*>(ev);

                    if (mev)
                    {
#if (QT_VERSION > QT_VERSION_CHECK(5, 99, 0))
                        QPoint pos(mev->position().toPoint());
#else
                        QPoint pos(mev->pos());
#endif
                        QRect  desktopRect = windowHandle()->screen()->geometry();

                        QRect sizeRect(QPoint(0, 0), d->fullScreenBtn->size());
                        QRect topLeft, topRight;
                        QRect topRightLarger;

                        desktopRect        = QRect(desktopRect.y(), desktopRect.y(), desktopRect.width(), desktopRect.height());
                        topLeft            = sizeRect;
                        topRight           = sizeRect;

                        topLeft.moveTo(desktopRect.x(), desktopRect.y());
                        topRight.moveTo(desktopRect.x() + desktopRect.width() - sizeRect.width() - 1, topLeft.y());

                        topRightLarger     = topRight.adjusted(-25, 0, 0, 10);

                        if (topRightLarger.contains(pos))
                        {
                            d->fullScreenBtn->move(topRight.topLeft());
                            d->fullScreenBtn->show();
                        }
                        else
                        {
                            d->fullScreenBtn->hide();
                        }

                        return false;
                    }
                }
            }
        }
    }

    // pass the event on to the parent class

    return QObject::eventFilter(obj, ev);
}

void DXmlGuiWindow::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape)
    {
        if (fullScreenIsActive())
        {
            d->fullScreenAction->activate(QAction::Trigger);
        }
    }
}

void DXmlGuiWindow::showSideBars(bool visible)
{
    Q_UNUSED(visible);
}

void DXmlGuiWindow::showThumbBar(bool visible)
{
    Q_UNUSED(visible);
}

bool DXmlGuiWindow::thumbbarVisibility() const
{
    return true;
}

void DXmlGuiWindow::slotHelpContents()
{
    openHandbook();
}

void DXmlGuiWindow::openHandbook()
{
    QUrl url(QString::fromUtf8("https://docs.kde.org/?application=%1&branch=trunk5")
             .arg(QApplication::applicationName()));

#ifdef HAVE_QWEBENGINE

    WebBrowserDlg* const browser = new WebBrowserDlg(url, qApp->activeWindow());
    browser->show();

#else

    QDesktopServices::openUrl(url);

#endif

}

void DXmlGuiWindow::restoreWindowSize(QWindow* const win, const KConfigGroup& group)
{

#ifdef Q_OS_WIN

    QString s = QString::fromLatin1("%1x%2").arg(win->screen()->size().width())
                                            .arg(win->screen()->size().height());

    int  w    = group.readEntry(QString::fromLatin1("DK Width %1").arg(s),     win->width());
    int  h    = group.readEntry(QString::fromLatin1("DK Height %1").arg(s),    win->height());
    int  x    = group.readEntry(QString::fromLatin1("DK PositionX %1").arg(s), win->geometry().x());
    int  y    = group.readEntry(QString::fromLatin1("DK PositionY %1").arg(s), win->geometry().y());
    bool max  = group.readEntry(QString::fromLatin1("DK Maximized %1").arg(s), (bool)(win->windowState() &
                                                                                      Qt::WindowMaximized));
    if (max)
    {
        win->setWindowState(Qt::WindowMaximized);
    }
    else
    {
        if (win->screen()->availableVirtualGeometry().contains(QRect(x, y, w, h)))
        {
            win->setPosition(x, y);
        }

        win->resize(w, h);
    }

#else

    KWindowConfig::restoreWindowSize(win, group);

#endif

}

void DXmlGuiWindow::saveWindowSize(QWindow* const win, KConfigGroup& group)
{

#ifdef Q_OS_WIN

    QString s = QString::fromLatin1("%1x%2").arg(win->screen()->size().width())
                                            .arg(win->screen()->size().height());

    group.writeEntry(QString::fromLatin1("DK Width %1").arg(s),     win->width());
    group.writeEntry(QString::fromLatin1("DK Height %1").arg(s),    win->height());
    group.writeEntry(QString::fromLatin1("DK PositionX %1").arg(s), win->geometry().x());
    group.writeEntry(QString::fromLatin1("DK PositionY %1").arg(s), win->geometry().y());
    group.writeEntry(QString::fromLatin1("DK Maximized %1").arg(s), (bool)(win->windowState() &
                                                                           Qt::WindowMaximized));

#else

    KWindowConfig::saveWindowSize(win, group);

#endif

}

void DXmlGuiWindow::restoreWindowSize()
{

#ifdef Q_OS_WIN

    winId();

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(configGroupName());

    restoreWindowSize(windowHandle(), group);

#endif

}

void DXmlGuiWindow::saveWindowSize()
{

#ifdef Q_OS_WIN

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(configGroupName());

    saveWindowSize(windowHandle(), group);

#endif

}

void DXmlGuiWindow::slotRawCameraList()
{
    showRawCameraList();
}

void DXmlGuiWindow::slotSolidHardwareList()
{
    SolidHardwareDlg* const dlg = new SolidHardwareDlg(qApp->activeWindow());
    dlg->exec();
}

void DXmlGuiWindow::slotDonateMoney()
{
    QUrl url(QLatin1String("https://www.digikam.org/donate/"));

#ifdef HAVE_QWEBENGINE

    WebBrowserDlg* const browser = new WebBrowserDlg(url, qApp->activeWindow());
    browser->show();

#else

    QDesktopServices::openUrl(url);

#endif

}

void DXmlGuiWindow::slotRecipesBook()
{
    QUrl url(QLatin1String("https://www.digikam.org/recipes_book/"));

#ifdef HAVE_QWEBENGINE

    WebBrowserDlg* const browser = new WebBrowserDlg(url, qApp->activeWindow());
    browser->show();

#else

    QDesktopServices::openUrl(url);

#endif

}

void DXmlGuiWindow::slotContribute()
{
    QUrl url(QLatin1String("https://www.digikam.org/contribute/"));

#ifdef HAVE_QWEBENGINE

    WebBrowserDlg* const browser = new WebBrowserDlg(url, qApp->activeWindow());
    browser->show();

#else

    QDesktopServices::openUrl(url);

#endif

}

void DXmlGuiWindow::setupIconTheme()
{
    // Let QStandardPaths handle this, it will look for app local stuff
    // this means e.g. for mac: "<APPDIR>/../Resources" and for win: "<APPDIR>/data".

    bool hasBreeze                = false;
    const QString breezeIcons     = QStandardPaths::locate(QStandardPaths::AppDataLocation,
                                                           QLatin1String("breeze.rcc"));

    if (!breezeIcons.isEmpty() && QFile::exists(breezeIcons))
    {
        qCDebug(DIGIKAM_WIDGETS_LOG) << "Breeze icons resource file found";
        QResource::registerResource(breezeIcons);
        hasBreeze = true;
    }

    bool hasBreezeDark            = false;
    const QString breezeDarkIcons = QStandardPaths::locate(QStandardPaths::AppDataLocation,
                                                           QLatin1String("breeze-dark.rcc"));

    if (!breezeDarkIcons.isEmpty() && QFile::exists(breezeDarkIcons))
    {
        qCDebug(DIGIKAM_WIDGETS_LOG) << "Breeze-dark icons resource file found";
        QResource::registerResource(breezeDarkIcons);
        hasBreezeDark = true;
    }

    if (hasBreeze || hasBreezeDark)
    {
        // Tell Qt about the theme

        QIcon::setThemeSearchPaths(QStringList() << QLatin1String(":/icons"));

        const QStringList iconsDirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                                QLatin1String("icons"),
                                                                QStandardPaths::LocateDirectory);

        if (!iconsDirs.isEmpty())
        {
            QIcon::setThemeSearchPaths(QStringList() << QIcon::themeSearchPaths() << iconsDirs);
        }

        if      (hasBreeze)
        {
            QIcon::setThemeName(QLatin1String("breeze"));
        }
        else if (hasBreezeDark)
        {
            QIcon::setThemeName(QLatin1String("breeze-dark"));
        }
    }
    else
    {
        qCDebug(DIGIKAM_WIDGETS_LOG) << "Use installed icons";
    }
}

} // namespace Digikam
