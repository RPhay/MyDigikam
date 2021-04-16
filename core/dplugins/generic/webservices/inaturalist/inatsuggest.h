/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-03-20
 * Description : a tool to export images to iNaturalist web service
 *
 * Copyright (C) 2021      by Joerg Lohse <joergmlpts at gmail dot com>
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

#ifndef DIGIKAM_INAT_SUGGEST_H
#define DIGIKAM_INAT_SUGGEST_H

// Qt includes

#include <QObject>
#include <QLineEdit>
#include <QEvent>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTimer>
#include <QList>
#include <QVector>
#include <QHash>

// Local includes

#include "inattaxon.h"
#include "inattaxonedit.h"
#include "inatscore.h"

namespace DigikamGenericINatPlugin
{

class INatTalker;

typedef QPair<QString, QList<Taxon> > AutoCompletions;

class SuggestTaxonCompletion : public QObject
{
    Q_OBJECT

public:

    explicit SuggestTaxonCompletion(TaxonEdit* const parent);
    ~SuggestTaxonCompletion();

    void setTalker(INatTalker* const talker);

Q_SIGNALS:

    void signalTaxonSelected(const Taxon&, bool fromVision);
    void signalTaxonDeselected();
    void signalComputerVision();

private Q_SLOTS:

    void slotDoneCompletion();
    void slotPreventSuggest();
    void slotAutoSuggest();
    void slotInFocus();
    void slotTaxonAutoCompletions(const AutoCompletions&);
    void slotComputerVisionResults(const ImageScores&);
    void slotImageLoaded(const QUrl&, const QByteArray&);
    void slotTextEdited(const QString&);

private:

    bool    eventFilter(QObject* obj, QEvent* ev) override;
    void    showCompletion(const struct Completions&);
    void    taxon2Item(const Taxon&, QTreeWidgetItem*, const QString&);
    QString getText() const;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericINatPlugin

#endif // DIGIKAM_INAT_SUGGEST_H

