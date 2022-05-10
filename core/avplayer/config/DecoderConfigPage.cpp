/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// scroll area code is from Xuno: https://github.com/Xuno/Xuno-QtAV/blob/master/examples/player/config/DecoderConfigPage.cpp

#include "DecoderConfigPage.h"

// Qt includes

#include <QListView>
#include <QSpinBox>
#include <QToolButton>
#include <QLayout>
#include <QLabel>
#include <QCheckBox>
#include <QScrollArea>
#include <QSpacerItem>
#include <QPainter>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "ConfigManager.h"
#include "PropertyEditor.h"
#include "VideoDecoder.h"
#include "digikam_debug.h"

using namespace QtAV;

namespace AVPlayer
{

// shared

static QVector<QtAV::VideoDecoderId> sDecodersUi;
static QVector<QtAV::VideoDecoderId> sPriorityUi;

QStringList idsToNames(QVector<VideoDecoderId> ids)
{
    QStringList decs;

    foreach (int id, ids)
    {
        decs.append(QString::fromLatin1(VideoDecoder::name(id)));
    }

    return decs;
}

QVector<VideoDecoderId> idsFromNames(const QStringList& names)
{
    QVector<VideoDecoderId> decs;

    foreach (const QString& name, names)
    {
        if (name.isEmpty())
            continue;

        VideoDecoderId id = VideoDecoder::id(name.toLatin1().constData());

        if (id == 0)
            continue;

        decs.append(id);
    }

    return decs;
}

using namespace QtAV;

class DecoderConfigPage::DecoderItemWidget : public QFrame
{
    Q_OBJECT

public:

    DecoderItemWidget(QWidget* const parent = nullptr)
        : QFrame(parent)
    {
        mpEditorWidget = 0;

        // why no frame?

        setFrameStyle(QFrame::Panel|QFrame::Raised);
        setLineWidth(2);

        mpEditor                     = new PropertyEditor(this);
        mSelected                    = false;
        QVBoxLayout* const vb        = new QVBoxLayout;
        setLayout(vb);
        QFrame* const frame          = new QFrame();
        frame->setFrameShape(QFrame::HLine);
        vb->addWidget(frame);
        mpCheck                      = new QCheckBox();

        QHBoxLayout* const hb        = new QHBoxLayout();
        hb->addWidget(mpCheck);
        QToolButton* const expandBtn = new QToolButton();
        expandBtn->setText(QString::fromLatin1("+"));
        hb->addWidget(expandBtn);

        connect(expandBtn, SIGNAL(clicked()),
                this, SLOT(toggleEditorVisible()));

        mpDesc = new QLabel();
        vb->addLayout(hb);
        vb->addWidget(mpDesc);

        connect(mpCheck, SIGNAL(pressed()),
                this, SLOT(checkPressed())); // no this->mousePressEvent

        connect(mpCheck, SIGNAL(toggled(bool)),
                this, SIGNAL(enableChanged()));
    }

    void buildUiFor(QObject* obj)
    {
        mpEditor->getProperties(obj);

        //mpEditor->set()

        QWidget* const w = mpEditor->buildUi(obj);

        if (!w)
            return;

        mpEditorWidget = w;
        w->setEnabled(true);
        layout()->addWidget(w);
        w->setVisible(false);
    }

    QVariantHash getOptions() const
    {
        return mpEditor->exportAsHash();
    }

    void select(bool s)
    {
        mSelected = s;
        update();
    }

    void setChecked(bool c)                     { mpCheck->setChecked(c);       }
    bool isChecked() const                      { return mpCheck->isChecked();  }
    void setName(const QString& name)           { mpCheck->setText(name);       }
    QString name() const                        { return mpCheck->text();       }
    void setDescription(const QString& desc)    { mpDesc->setText(desc);        }
    QString description() const                 { return mpDesc->text();        }

Q_SIGNALS:

    void enableChanged();
    void selected(DecoderItemWidget*);

private Q_SLOTS:

    void checkPressed()
    {
        select(true);
        emit selected(this);
    }

    void toggleEditorVisible()
    {
        if (!mpEditorWidget)
            return;

        mpEditorWidget->setVisible(!mpEditorWidget->isVisible());
        QToolButton* const b = qobject_cast<QToolButton*>(sender());

        if (b)
        {
            b->setText(mpEditorWidget->isVisible()?QString::fromLatin1("-"):QString::fromLatin1("+"));
        }

        parentWidget()->adjustSize();
    }

protected:

    virtual void mousePressEvent(QMouseEvent*)
    {
        select(true);
        emit selected(this);
    }

    virtual void paintEvent(QPaintEvent* e)
    {
        if (mSelected)
        {
            QPainter p(this);
            p.fillRect(rect(), QColor(0, 100, 200, 100));
        }

        QWidget::paintEvent(e);
    }

private:

    bool            mSelected;
    QCheckBox*      mpCheck;
    QLabel*         mpDesc;
    PropertyEditor* mpEditor;
    QWidget*        mpEditorWidget;
};

DecoderConfigPage::DecoderConfigPage(QWidget* const parent)
    : ConfigPageBase(parent)
{
    mpSelectedDec                           = nullptr;
    setWindowTitle(i18n("Video decoder config page"));
    QVBoxLayout* const vbs                  = new QVBoxLayout(this);
    QSpacerItem* const horizontalSpacer     = new QSpacerItem(320, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
    vbs->addItem(horizontalSpacer);
    QScrollArea* const scrollArea           = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    QWidget* const scrollAreaWidgetContents = new QWidget(this);
    QVBoxLayout* const vlsroll              = new QVBoxLayout(scrollAreaWidgetContents);
    vlsroll->setSpacing(0);
    QVBoxLayout* const vb                   = new QVBoxLayout;
    vb->setSpacing(0);

    vb->addWidget(new QLabel(QString::fromLatin1("%1 %2 (%3)").arg(i18n("Decoder")).arg(i18n("Priorities")).arg(i18n("reopen is required"))));

    sPriorityUi                             = idsFromNames(ConfigManager::instance().decoderPriorityNames());
    QStringList vds                         = ConfigManager::instance().decoderPriorityNames();
    vds.removeDuplicates();
    QVector<VideoDecoderId> vids            = idsFromNames(vds);
    QVector<QtAV::VideoDecoderId> vds_all   = VideoDecoder::registered();
    QVector<QtAV::VideoDecoderId> all       = vids;

    foreach (QtAV::VideoDecoderId vid, vds_all)
    {
        if (!vids.contains(vid))
            all.push_back(vid);
    }

    mpDecLayout                             = new QVBoxLayout;

    foreach (QtAV::VideoDecoderId vid, all)
    {
        VideoDecoder* const vd      = VideoDecoder::create(vid);
        DecoderItemWidget* const iw = new DecoderItemWidget(scrollAreaWidgetContents);
        iw->buildUiFor(vd);
        mDecItems.append(iw);
        iw->setName(vd->name());
        iw->setDescription(vd->description());
        iw->setChecked(vids.contains(vid));

        connect(iw, SIGNAL(enableChanged()),
                this, SLOT(videoDecoderEnableChanged()));

        connect(iw, SIGNAL(selected(DecoderItemWidget*)),
                this, SLOT(onDecSelected(DecoderItemWidget*)));

        mpDecLayout->addWidget(iw);
        delete vd;
    }
/*
    for (int i = 0 ; i < vds_all.size() ; ++i)
    {
        VideoDecoder* const vd      = VideoDecoder::create(vds_all.at(i));
        DecoderItemWidget* const iw = new DecoderItemWidget();
        iw->buildUiFor(vd);
        mDecItems.append(iw);
        iw->setName(vd->name());
        iw->setDescription(vd->description());
        iw->setChecked(vds.contains(vd->name()));

        connect(iw, SIGNAL(enableChanged()),
                this, SLOT(videoDecoderEnableChanged()));

        connect(iw, SIGNAL(selected(DecoderItemWidget*)),
                this, SLOT(onDecSelected(DecoderItemWidget*)));

        mpDecLayout->addWidget(iw);
        delete vd;
    }
*/
    vb->addLayout(mpDecLayout);
    vb->addSpacerItem(new QSpacerItem(width(), 10, QSizePolicy::Ignored, QSizePolicy::Expanding));

    mpUp = new QToolButton(scrollAreaWidgetContents);
    mpUp->setText(i18n("Up"));

    connect(mpUp, SIGNAL(clicked()),
            this, SLOT(priorityUp()));

    mpDown = new QToolButton(scrollAreaWidgetContents);
    mpDown->setText(i18n("Down"));

    connect(mpDown, SIGNAL(clicked()),
            this, SLOT(priorityDown()));

    QHBoxLayout* const hb = new QHBoxLayout;
    hb->addWidget(mpUp);
    hb->addWidget(mpDown);
    vb->addLayout(hb);
    vb->addSpacerItem(new QSpacerItem(width(), 10, QSizePolicy::Ignored, QSizePolicy::Expanding));
    vlsroll->addLayout(vb);
    scrollArea->setWidget(scrollAreaWidgetContents);
    vbs->addWidget(scrollArea);

    connect(&ConfigManager::instance(), SIGNAL(decoderPriorityNamesChanged()),
            this, SLOT(onConfigChanged()));
}

QString DecoderConfigPage::name() const
{
    return i18n("Decoder");
}

QVariantHash DecoderConfigPage::audioDecoderOptions() const
{
    return QVariantHash();
}

QVariantHash DecoderConfigPage::videoDecoderOptions() const
{
    QVariantHash options;

    foreach (DecoderItemWidget* const diw, mDecItems)
    {
        options[diw->name()] = diw->getOptions();
    }

    return options;
}

void DecoderConfigPage::applyFromUi()
{
    QStringList decs_all;
    QStringList decs;

    foreach (DecoderItemWidget* const w, mDecItems)
    {
        decs_all.append(w->name());

        if (w->isChecked())
            decs.append(w->name());
    }

    sPriorityUi = idsFromNames(decs);
    ConfigManager::instance().setDecoderPriorityNames(decs);
}

void DecoderConfigPage::applyToUi()
{
    updateDecodersUi();
}

void DecoderConfigPage::videoDecoderEnableChanged()
{
    QStringList names;

    foreach (DecoderItemWidget* const iw, mDecItems)
    {
        if (iw->isChecked())
            names.append(iw->name());
    }

    sPriorityUi = idsFromNames(names);

    if (applyOnUiChange())
    {
        ConfigManager::instance().setDecoderPriorityNames(names);
    }
    else
    {
//        emit ConfigManager::instance().decoderPriorityChanged(sPriorityUi);
    }
}

void DecoderConfigPage::priorityUp()
{
    if (!mpSelectedDec)
        return;

    int i = mDecItems.indexOf(mpSelectedDec);

    if (i == 0)
        return;

    DecoderItemWidget* const iw = mDecItems.takeAt(i-1);
    mDecItems.insert(i, iw);
    mpDecLayout->removeWidget(iw);
    mpDecLayout->insertWidget(i, iw);
    QStringList decs_all;
    QStringList decs_p          = ConfigManager::instance().decoderPriorityNames();
    QStringList decs;

    foreach (DecoderItemWidget* const w, mDecItems)
    {
        decs_all.append(w->name());

        if (decs_p.contains(w->name()))
            decs.append(w->name());
    }

    sDecodersUi = idsFromNames(decs_all);
    sPriorityUi = idsFromNames(decs);

    if (applyOnUiChange())
    {
        ConfigManager::instance().setDecoderPriorityNames(decs);
    }
    else
    {
        //emit ConfigManager::instance().decoderPriorityChanged(idsFromNames(decs));
    }
}

void DecoderConfigPage::priorityDown()
{
    if (!mpSelectedDec)
        return;

    int i = mDecItems.indexOf(mpSelectedDec);

    if (i == mDecItems.size()-1)
        return;

    DecoderItemWidget* const iw = mDecItems.takeAt(i + 1);
    mDecItems.insert(i, iw);

    // why takeItemAt then insertItem does not work?

    mpDecLayout->removeWidget(iw);
    mpDecLayout->insertWidget(i, iw);

    QStringList decs_all;
    QStringList decs_p          = ConfigManager::instance().decoderPriorityNames();
    QStringList decs;

    foreach (DecoderItemWidget* const w, mDecItems)
    {
        decs_all.append(w->name());

        if (decs_p.contains(w->name()))
            decs.append(w->name());
    }

    sDecodersUi = idsFromNames(decs_all);
    sPriorityUi = idsFromNames(decs);

    if (applyOnUiChange())
    {
        ConfigManager::instance().setDecoderPriorityNames(decs);
    }
    else
    {
        //emit ConfigManager::instance().decoderPriorityChanged(idsFromNames(decs));
        //emit ConfigManager::instance().registeredDecodersChanged(idsFromNames(decs));
    }
}

void DecoderConfigPage::onDecSelected(DecoderItemWidget *iw)
{
    if (mpSelectedDec == iw)
        return;

    if (mpSelectedDec) 
    {
        mpSelectedDec->select(false);
    }

    mpSelectedDec = iw;
}

void DecoderConfigPage::updateDecodersUi()
{
    QStringList names     = idsToNames(sPriorityUi);
    QStringList all_names = idsToNames(sDecodersUi);

    //qCDebug(DIGIKAM_QTAVPLAYER_LOG) << "updateDecodersUi " << this << " " << names << " all: " << all_names;

    int idx = 0;

    foreach (const QString& name, all_names)
    {
        DecoderItemWidget* iw = nullptr;

        for (int i = idx ; i < mDecItems.size() ; ++i)
        {
           if (mDecItems.at(i)->name() != name)
               continue;

           iw = mDecItems.at(i);

           break;
        }

        if (!iw)
            break;

        iw->setChecked(names.contains(iw->name()));
        int i = mDecItems.indexOf(iw);

        if (i != idx)
        {
            mDecItems.removeAll(iw);
            mDecItems.insert(idx, iw);
        }

        // why takeItemAt then insertItem does not work?

        if (mpDecLayout->indexOf(iw) != idx)
        {
            mpDecLayout->removeWidget(iw);
            mpDecLayout->insertWidget(idx, iw);
        }

        ++idx;
    }
}

void DecoderConfigPage::onConfigChanged()
{
    sPriorityUi = idsFromNames(ConfigManager::instance().decoderPriorityNames());
    sDecodersUi = VideoDecoder::registered();
    updateDecodersUi();
}

} // namespace AVPlayer

#include "DecoderConfigPage.moc"
