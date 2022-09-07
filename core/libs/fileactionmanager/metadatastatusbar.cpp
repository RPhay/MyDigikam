/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-08-21
 * Description : a bar to indicate pending metadata
 *
 * Copyright (C) 2015 by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "metadatastatusbar.h"

// Qt includes

#include <QToolButton>
#include <QHBoxLayout>
#include <QApplication>
#include <QStyle>
#include <QDebug>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "metaenginesettings.h"
#include "dexpanderbox.h"

namespace Digikam
{

class Q_DECL_HIDDEN MetadataStatusBar::Private
{
public:

    enum FilterStatus
    {
        None = 0,
        Match,
        NotMatch
    };

public:

    explicit Private()
      : status(None),
        info(nullptr),
        applyBtn(nullptr)
    {
    }

    int               status;

    DAdjustableLabel* info;
    QToolButton*      applyBtn;
};

MetadataStatusBar::MetadataStatusBar(QWidget* const parent)
    : QWidget(parent),
      d(new Private)
{
    QHBoxLayout* const vlay = new QHBoxLayout(this);
    d->applyBtn             = new QToolButton(this);
    d->applyBtn->setIcon(QIcon::fromTheme(QLatin1String("view-refresh")));
    d->applyBtn->setToolTip(i18n("Apply pending changes to metadata"));
    d->applyBtn->setFocusPolicy(Qt::NoFocus);
    d->applyBtn->setAutoRaise(true);
    d->applyBtn->setDisabled(true);

    d->info                 = new DAdjustableLabel(this);
    d->info->setContextMenuPolicy(Qt::NoContextMenu);
    d->info->setAutoFillBackground(true);
    d->info->setFocusPolicy(Qt::NoFocus);
    d->info->setAdjustedText(i18n("No pending metadata synchronization"));

    d->info->setWhatsThis(i18n("If lazy synchronization is enabled in metadata settings, "
                               "the status bar will display the number of items waiting for synchronization"));

    vlay->addWidget(d->applyBtn);
    vlay->addWidget(d->info);
    vlay->setSpacing(qMin(QApplication::style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing),
                             QApplication::style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing)));
    vlay->setContentsMargins(QMargins());
    vlay->setStretchFactor(d->info, 10);

    connect(MetaEngineSettings::instance(), SIGNAL(signalSettingsChanged()),
            this, SLOT(slotSettingsChanged()));

    connect(d->applyBtn, SIGNAL(released()),
            MetadataHubMngr::instance(), SLOT(slotApplyPending()));

    connect(MetadataHubMngr::instance(), SIGNAL(signalPendingMetadata(int)),
            this, SLOT(slotSetPendingItems(int)));

    if (MetaEngineSettings::instance()->settings().useLazySync)
    {
        show();
    }
    else
    {
        hide();
    }
}

MetadataStatusBar::~MetadataStatusBar()
{
    delete d;
}

void MetadataStatusBar::slotSettingsChanged()
{
    if (MetaEngineSettings::instance()->settings().useLazySync)
    {
        show();
    }
    else
    {
        hide();
    }
}

void MetadataStatusBar::slotSetPendingItems(int number)
{
    if (number == 0)
    {
        d->info->setAdjustedText(i18n("No pending metadata synchronization"));
        d->applyBtn->setDisabled(true);
    }
    else
    {
        d->info->setAdjustedText(i18np("1 file awaits synchronization",
                                       "%1 files await synchronization",
                                       number));
        d->applyBtn->setDisabled(false);
    }
}

} // namespace Digikam
