/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-08-08
 * Description : an option to add a sequence number to the parser
 *
 * Copyright (C) 2009-2012 by Andi Clemens <andi dot clemens at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "sequencenumberoption.h"

// Qt includes

#include <QLabel>
#include <QPointer>
#include <QGroupBox>
#include <QGridLayout>
#include <QRegularExpression>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "ui_sequencenumberoptiondialogwidget.h"

namespace Digikam
{

SequenceNumberDialog::SequenceNumberDialog(Rule* const parent)
    : RuleDialog(parent),
      ui(new Ui::SequenceNumberOptionDialogWidget())
{
    QWidget* const mainWidget = new QWidget(this);
    ui->setupUi(mainWidget);
    setSettingsWidget(mainWidget);
    ui->digits->setFocus();
}

SequenceNumberDialog::~SequenceNumberDialog()
{
    delete ui;
}

// --------------------------------------------------------

SequenceNumberOption::SequenceNumberOption()
    : Option(i18nc("Sequence Number", "Number..."),
             i18n("Add a sequence number"),
             QLatin1String("accessories-calculator"))
{
    addToken(QLatin1String("#"),                                 i18n("Sequence number"));
    addToken(QLatin1String("#[||options||]"),                    i18n("Sequence number (||options||: "
                                                                      "||c|| = file counter aware, "
                                                                      "||e|| = extension aware, "
                                                                      "||f|| = folder aware, "
                                                                      "||r|| = random aware"));
    addToken(QLatin1String("#[||options||,||start||]"),          i18n("Sequence number (custom start)"));
    addToken(QLatin1String("#[||options||,||step||]"),           i18n("Sequence number (custom step)"));
    addToken(QLatin1String("#[||options||,||start||,||step||]"), i18n("Sequence number (custom start + step)"));

    QRegularExpression reg(QLatin1String("(#+)(\\[(c?e?f?r?,?)?((-?\\d+)(,(-?\\d+))?)?\\])?"));
    setRegExp(reg);
}

SequenceNumberOption::~SequenceNumberOption()
{
}

void SequenceNumberOption::slotTokenTriggered(const QString& token)
{
    Q_UNUSED(token)

    QPointer<SequenceNumberDialog> dlg = new SequenceNumberDialog(this);

    QString result;

    if (dlg->exec() == QDialog::Accepted)
    {
        int digits          = dlg->ui->digits->value();
        int start           = dlg->ui->start->value();
        int step            = dlg->ui->step->value();
        bool extensionAware = dlg->ui->extensionAware->isChecked();
        bool counterAware   = dlg->ui->counterAware->isChecked();
        bool folderAware    = dlg->ui->folderAware->isChecked();
        bool randomAware    = dlg->ui->randomAware->isChecked();

        result = QString::fromUtf8("%1").arg(QLatin1String("#"), digits, QLatin1Char('#'));

        if ((start > 1) || (step > 1) || extensionAware || folderAware || counterAware || randomAware)
        {
            result.append(QLatin1Char('['));

            if (extensionAware)
            {
                result.append(QLatin1Char('e'));
            }

            if (counterAware)
            {
                result.append(QLatin1Char('c'));
            }

            if (folderAware)
            {
                result.append(QLatin1Char('f'));
            }

            if (randomAware)
            {
                result.append(QLatin1Char('r'));
            }

            if ((start > 1) || (step > 1))
            {
                if (extensionAware && !randomAware)
                {
                    result.append(QLatin1Char(','));
                }

                if (!counterAware && !randomAware)
                {
                    result.append(QString::number(start));
                }
            }

            if (step > 1 && !randomAware)
            {
                result.append(QString::fromUtf8(",%1").arg(QString::number(step)));
            }

            result.append(QLatin1Char(']'));
        }
    }

    delete dlg;

    Q_EMIT signalTokenTriggered(result);
}

QString SequenceNumberOption::parseOperation(ParseSettings& settings, const QRegularExpressionMatch& match)
{
    QString result;
    QString random;
    int slength         = 0;
    int start           = 0;
    int step            = 0;
    int number          = 0;
    int index           = 0;

    bool extensionAware = false;
    bool counterAware   = false;
    bool folderAware    = false;
    bool randomAware    = false;

    if (settings.manager)
    {
        extensionAware = !match.captured(3).isEmpty() && match.captured(3).contains(QLatin1Char('e'));
        counterAware   = !match.captured(3).isEmpty() && match.captured(3).contains(QLatin1Char('c'));
        folderAware    = !match.captured(3).isEmpty() && match.captured(3).contains(QLatin1Char('f'));
        randomAware    = !match.captured(3).isEmpty() && match.captured(3).contains(QLatin1Char('r'));

        index          = settings.manager->indexOfFile(settings.fileUrl.toLocalFile());

        if (extensionAware)
        {
            index = settings.manager->indexOfFileGroup(settings.fileUrl.toLocalFile());
        }

        if (counterAware)
        {
            index = settings.manager->indexOfFolder(settings.fileUrl.toLocalFile());
            start = settings.manager->indexOfFileCounter(settings.fileUrl.toLocalFile());
        }

        if (folderAware)
        {
            index = settings.manager->indexOfFolder(settings.fileUrl.toLocalFile());
        }

        if (randomAware)
        {
            random = settings.manager->randomStringOfFile(settings.fileUrl.toLocalFile());
        }
    }

    // --------------------------------------------------------

    slength = match.captured(1).length();

    if (!counterAware)
    {
        start = match.captured(5).isEmpty() ? settings.startIndex : match.captured(5).toInt();
        step  = match.captured(7).isEmpty() ? 1 : match.captured(7).toInt();
    }
    else
    {
        step  = match.captured(5).isEmpty() ? 1 : match.captured(5).toInt();
    }

    if (step < 1)
    {
        step = 1;
    }

    if      (start < 1)
    {
        start = settings.startIndex;
    }
    else if (counterAware)
    {
        start += step;
    }

    if (randomAware && !random.isEmpty())
    {
         result = random.left(slength);
    }
    else
    {
        number  = start + ((index - 1) * step);
        result  = QString::fromUtf8("%1").arg(number, slength, 10, QLatin1Char('0'));
    }

    return result;
}

} // namespace Digikam
