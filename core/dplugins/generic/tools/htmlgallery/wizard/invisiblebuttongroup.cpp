/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-04-04
 * Description : a tool to generate HTML image galleries
 *
 * SPDX-FileCopyrightText: 2006-2010 by Aurelien Gateau <aurelien dot gateau at free dot fr>
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "invisiblebuttongroup.h"

// Qt includes

#include <QAbstractButton>
#include <QButtonGroup>

// Local includes

#include "dconfigdlgmngr.h"

using namespace Digikam;

namespace DigikamGenericHtmlGalleryPlugin
{

class Q_DECL_HIDDEN InvisibleButtonGroup::Private
{
public:

    explicit Private()
      : group(nullptr)
    {
    }

    QButtonGroup* group;
};

InvisibleButtonGroup::InvisibleButtonGroup(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    hide();

    d->group = new QButtonGroup(this);
    d->group->setExclusive(true);

    connect(d->group, SIGNAL(buttonClicked(int)),
            SIGNAL(selectionChanged(int)) );

    const QString name = QString::fromLatin1(metaObject()->className());

    if (!DConfigDlgMngr::propertyMap()->contains(name))
    {
        DConfigDlgMngr::propertyMap()->insert(name, "current");
        DConfigDlgMngr::changedMap()->insert(name, SIGNAL(selectionChanged(int)));
    }
}

InvisibleButtonGroup::~InvisibleButtonGroup()
{
    delete d;
}

int InvisibleButtonGroup::selected() const
{
    return d->group->checkedId();
}

void InvisibleButtonGroup::addButton(QAbstractButton* button, int id)
{
    d->group->addButton(button, id);
}

void InvisibleButtonGroup::setSelected(int id)
{
    QAbstractButton* const button = d->group->button(id);

    if (button)
    {
        button->setChecked(true);
    }
}

} // namespace DigikamGenericHtmlGalleryPlugin
