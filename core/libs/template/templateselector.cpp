/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-06-23
 * Description : a widget to select metadata template.
 *
 * SPDX-FileCopyrightText: 2009-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "templateselector.h"

// Qt includes

#include <QAbstractItemView>
#include <QLabel>
#include <QToolButton>
#include <QApplication>
#include <QStyle>
#include <QIcon>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "setup.h"
#include "template.h"
#include "templatemanager.h"
#include "squeezedcombobox.h"
#include "digikam_globals.h"

namespace Digikam
{

class Q_DECL_HIDDEN TemplateSelector::Private
{
public:

    Private() = default;

    QLabel*           label             = nullptr;

    QToolButton*      setupButton       = nullptr;

    SqueezedComboBox* templateCombo     = nullptr;

    Template          metadataTemplate;
};

TemplateSelector::TemplateSelector(QWidget* const parent)
    : DHBox(parent),
      d    (new Private)
{
    d->label         = new QLabel(i18n("Template: "), this);
    d->templateCombo = new SqueezedComboBox(this);
    d->setupButton   = new QToolButton(this);
    d->setupButton->setIcon(QIcon::fromTheme(QLatin1String("document-edit")));
    d->setupButton->setWhatsThis(i18n("Open metadata template editor"));
    d->templateCombo->setWhatsThis(i18n("<p>Select here the action to perform using the metadata template.</p>"
                                        "<p><b>To remove</b>: delete already-assigned template.</p>"
                                        "<p><b>Do not change</b>: Do not touch template information.</p>"
                                        "<p>All other values are template titles managed by digiKam. "
                                        "Selecting one will assign information as well.</p>"));

    setSpacing(layoutSpacing());
    setContentsMargins(QMargins());
    setStretchFactor(d->templateCombo, 10);

    connect(d->templateCombo, SIGNAL(activated(int)),
            this, SIGNAL(signalTemplateSelected()));

    connect(d->setupButton, SIGNAL(clicked()),
            this, SLOT(slotOpenSetup()));

    TemplateManager* const tm = TemplateManager::defaultManager();

    if (tm)
    {
        connect(tm, SIGNAL(signalTemplateAdded(Template)),
                this, SLOT(slotTemplateListChanged()));

        connect(tm, SIGNAL(signalTemplateRemoved(Template)),
                this, SLOT(slotTemplateListChanged()));
    }

    populateTemplates();
}

TemplateSelector::~TemplateSelector()
{
    delete d;
}

void TemplateSelector::populateTemplates()
{
    d->templateCombo->clear();
    d->templateCombo->insertSqueezedItem(i18n("To remove"),     REMOVETEMPLATE);
    d->templateCombo->insertSqueezedItem(i18n("Do not change"), DONTCHANGE);
    d->templateCombo->insertSeparator(DONTCHANGE + 1);

    TemplateManager* const tm = TemplateManager::defaultManager();

    if (tm)
    {
        int i                 = DONTCHANGE + 2;
        QList<Template> list  = tm->templateList();

        for (const Template& t : std::as_const(list))
        {
            d->templateCombo->insertSqueezedItem(t.templateTitle(), i);
            ++i;
        }
    }
}

Template TemplateSelector::getTemplate() const
{
    switch (d->templateCombo->currentIndex())
    {
        case REMOVETEMPLATE:
        {
            Template t;
            t.setTemplateTitle(Template::removeTemplateTitle());
            return t;
        }

        case DONTCHANGE:
        {
            return Template();
        }

        default:
        {
            TemplateManager* const tm = TemplateManager::defaultManager();

            if (tm)
            {
                return tm->fromIndex(d->templateCombo->currentIndex() - 3);
            }

            break;
        }
    }

    return Template();
}

void TemplateSelector::setTemplate(const Template& t)
{
    d->metadataTemplate = t;
    QString title       = d->metadataTemplate.templateTitle();

    if      (title == Template::removeTemplateTitle())
    {
        d->templateCombo->setCurrentIndex(REMOVETEMPLATE);
    }
    else if (title.isEmpty())
    {
        d->templateCombo->setCurrentIndex(DONTCHANGE);
    }

    d->templateCombo->setCurrent(title);
}

int TemplateSelector::getTemplateIndex() const
{
    return d->templateCombo->currentIndex();
}

void TemplateSelector::setTemplateIndex(int i)
{
    d->templateCombo->setCurrentIndex(i);
}

void TemplateSelector::slotOpenSetup()
{
    Setup::execTemplateEditor(this, d->metadataTemplate);
}

void TemplateSelector::slotTemplateListChanged()
{
    populateTemplates();
}

} // namespace Digikam

#include "moc_templateselector.cpp"
