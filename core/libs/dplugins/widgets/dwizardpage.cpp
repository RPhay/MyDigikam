/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a template to create wizard page.
 *
 * SPDX-FileCopyrightText: 2009-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dwizardpage.h"

// Qt includes

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QStandardPaths>
#include <QApplication>
#include <QStyle>
#include <QScrollArea>
#include <QWizard>

// Local includes

#include "digikam_globals.h"

namespace Digikam
{

class Q_DECL_HIDDEN DWizardPage::Private
{
public:

    Private() = default;

    bool         isComplete      = true;
    int          id              = -1;

    QWidget*     leftView        = nullptr;
    QLabel*      logo            = nullptr;
    QLabel*      leftBottomPix   = nullptr;

    QHBoxLayout* hlay            = nullptr;

    QWizard*     dlg             = nullptr;
};

DWizardPage::DWizardPage(QWizard* const dlg, const QString& title)
    : QWizardPage(dlg),
      d          (new Private)
{
    setTitle(title);

    const int spacing     = layoutSpacing();


    QScrollArea* const sv = new QScrollArea(this);
    QWidget* const panel  = new QWidget(sv->viewport());
    sv->setWidget(panel);
    sv->setWidgetResizable(true);

    d->hlay                    = new QHBoxLayout(panel);
    d->leftView                = new QWidget(panel);
    QVBoxLayout* const vboxLay = new QVBoxLayout(d->leftView);
    d->logo                    = new QLabel(d->leftView);
    d->logo->setAlignment(Qt::AlignTop);
    d->logo->setPixmap(QIcon::fromTheme(QLatin1String("digikam")).pixmap(QSize(128, 128)));

    QWidget* const space = new QLabel(d->leftView);
    d->leftBottomPix     = new QLabel(d->leftView);
    d->leftBottomPix->setAlignment(Qt::AlignBottom);

    vboxLay->addWidget(d->logo);
    vboxLay->addWidget(space);
    vboxLay->addWidget(d->leftBottomPix);
    vboxLay->setStretchFactor(space, 10);
    vboxLay->setContentsMargins(spacing, spacing, spacing, spacing);
    vboxLay->setSpacing(spacing);

    QFrame* const vline = new QFrame(panel);
    vline->setLineWidth(1);
    vline->setMidLineWidth(0);
    vline->setFrameShape(QFrame::VLine);
    vline->setFrameShadow(QFrame::Sunken);
    vline->setMinimumSize(2, 0);
    vline->updateGeometry();

    d->hlay->addWidget(d->leftView);
    d->hlay->addWidget(vline);
    d->hlay->setContentsMargins(QMargins());
    d->hlay->setSpacing(spacing);

    QVBoxLayout* const layout = new QVBoxLayout;
    layout->addWidget(sv);
    setLayout(layout);

    d->dlg = dlg;
    d->id  = d->dlg->addPage(this);
}

DWizardPage::~DWizardPage()
{
    delete d;
}

void DWizardPage::setComplete(bool b)
{
    d->isComplete = b;
    Q_EMIT completeChanged();
}

bool DWizardPage::isComplete() const
{
    return d->isComplete;
}

int DWizardPage::id() const
{
    return d->id;
}

void DWizardPage::setShowLeftView(bool v)
{
    d->leftView->setVisible(v);
}

void DWizardPage::setPageWidget(QWidget* const w)
{
    d->hlay->addWidget(w);
    d->hlay->setStretchFactor(w, 10);
}

void DWizardPage::removePageWidget(QWidget* const w)
{
    d->hlay->removeWidget(w);
}

void DWizardPage::setLeftBottomPix(const QPixmap& pix)
{
    d->leftBottomPix->setPixmap(pix);
}

void DWizardPage::setLeftBottomPix(const QIcon& icon)
{
    d->leftBottomPix->setPixmap(icon.pixmap(128));
}

QWizard* DWizardPage::assistant() const
{
    return d->dlg;
}

} // namespace Digikam

#include "moc_dwizardpage.cpp"
