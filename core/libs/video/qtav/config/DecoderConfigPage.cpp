/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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

#include "AVPlayerConfigMngr.h"
#include "PropertyEditor.h"
#include "digikam_debug.h"

using namespace QtAV;

namespace QtAV
{

// shared

static QVector<QtAV::VideoDecoderId> sDecodersUi;
static QVector<QtAV::VideoDecoderId> sPriorityUi;

// -------------------------------------------------------------------------

class Q_DECL_HIDDEN DecoderConfigPage::DecoderItemWidget : public QFrame
{
    Q_OBJECT

public:

    DecoderItemWidget(QWidget* const parent = nullptr, bool advOptVisible = true)
        : QFrame(parent)
    {
        // why no frame?

        setFrameStyle(QFrame::Panel | QFrame::Raised);
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
        expandBtn->setVisible(advOptVisible);
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

    void buildUiFor(QObject* const obj)
    {
        mpEditor->getProperties(obj);
/*
        mpEditor->set()
*/
        QWidget* const w = mpEditor->buildUi(obj);

        if (!w)
        {
            return;
        }

        mpEditorWidget   = w;
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
    bool isChecked()        const               { return mpCheck->isChecked();  }
    void setName(const QString& name)           { mpCheck->setText(name);       }

    QString name()          const
    {
        QString text = mpCheck->text();

        if (text.startsWith(QLatin1Char('&')))
        {
            text.remove(0, 1);
        }

        return text;
    }

    void setDescription(const QString& desc)    { mpDesc->setText(desc);        }
    QString description()   const               { return mpDesc->text();        }

Q_SIGNALS:

    void enableChanged();
    void selected(DecoderItemWidget*);

private Q_SLOTS:

    void checkPressed()
    {
        select(true);

        Q_EMIT selected(this);
    }

    void toggleEditorVisible()
    {
        if (!mpEditorWidget)
        {
            return;
        }

        mpEditorWidget->setVisible(!mpEditorWidget->isVisible());
        QToolButton* const b = qobject_cast<QToolButton*>(sender());

        if (b)
        {
            b->setText(mpEditorWidget->isVisible() ? QString::fromLatin1("-")
                                                   : QString::fromLatin1("+"));
        }

        parentWidget()->adjustSize();
    }

protected:

    virtual void mousePressEvent(QMouseEvent*)
    {
        select(true);

        Q_EMIT selected(this);
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

    bool            mSelected      = false;
    QCheckBox*      mpCheck        = nullptr;
    QLabel*         mpDesc         = nullptr;
    PropertyEditor* mpEditor       = nullptr;
    QWidget*        mpEditorWidget = nullptr;
};

// -------------------------------------------------------------------------

DecoderConfigPage::DecoderConfigPage(QWidget* const parent, bool advOptVisible)
    : ConfigPageBase(parent)
{
    setWindowTitle(i18nc("@title:window", "Video Decoder Config Page"));
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

    vb->addWidget(new QLabel(i18nc("@label", "Decoder Priorities (reopen is required)")));

    sPriorityUi                             = idsFromNames(AVPlayerConfigMngr::instance().decoderPriorityNames());
    QStringList vds                         = AVPlayerConfigMngr::instance().decoderPriorityNames();
    vds.removeDuplicates();
    QVector<VideoDecoderId> vids            = idsFromNames(vds);
    QVector<QtAV::VideoDecoderId> vds_all   = VideoDecoder::registered();
    QVector<QtAV::VideoDecoderId> all       = vids;

    Q_FOREACH (QtAV::VideoDecoderId vid, vds_all)
    {
        if (!vids.contains(vid))
        {
            all.push_back(vid);
        }
    }

    mpDecLayout                             = new QVBoxLayout;

    Q_FOREACH (QtAV::VideoDecoderId vid, all)
    {
        VideoDecoder* const vd      = VideoDecoder::create(vid);
        DecoderItemWidget* const iw = new DecoderItemWidget(scrollAreaWidgetContents, advOptVisible);
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
    mpUp->setText(i18nc("@action: go up in the list", "Up"));

    connect(mpUp, SIGNAL(clicked()),
            this, SLOT(priorityUp()));

    mpDown = new QToolButton(scrollAreaWidgetContents);
    mpDown->setText(i18nc("@action: go down in the list", "Down"));

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

    connect(&AVPlayerConfigMngr::instance(), SIGNAL(decoderPriorityNamesChanged()),
            this, SLOT(onConfigChanged()));
}

QString DecoderConfigPage::name() const
{
    return i18nc("@title", "Decoder");
}

QVariantHash DecoderConfigPage::audioDecoderOptions() const
{
    return QVariantHash();
}

QVariantHash DecoderConfigPage::videoDecoderOptions() const
{
    QVariantHash options;

    Q_FOREACH (DecoderItemWidget* const diw, mDecItems)
    {
        options[diw->name()] = diw->getOptions();
    }

    return options;
}

void DecoderConfigPage::applyFromUi()
{
    QStringList decs_all;
    QStringList decs;

    Q_FOREACH (DecoderItemWidget* const w, mDecItems)
    {
        decs_all.append(w->name());

        if (w->isChecked())
        {
            decs.append(w->name());
        }
    }

    sPriorityUi = idsFromNames(decs);
    AVPlayerConfigMngr::instance().setDecoderPriorityNames(decs);
}

void DecoderConfigPage::applyToUi()
{
    updateDecodersUi();
}

void DecoderConfigPage::videoDecoderEnableChanged()
{
    QStringList names;

    Q_FOREACH (DecoderItemWidget* const iw, mDecItems)
    {
        if (iw->isChecked())
        {
            names.append(iw->name());
        }
    }

    sPriorityUi = idsFromNames(names);

    if (applyOnUiChange())
    {
        AVPlayerConfigMngr::instance().setDecoderPriorityNames(names);
    }
    else
    {
/*
        Q_EMIT AVPlayerConfigMngr::instance().decoderPriorityChanged(sPriorityUi);
*/
    }
}

void DecoderConfigPage::priorityUp()
{
    if (!mpSelectedDec)
    {
        return;
    }

    int i = mDecItems.indexOf(mpSelectedDec);

    if (i == 0)
    {
        return;
    }

    DecoderItemWidget* const iw = mDecItems.takeAt(i - 1);
    mDecItems.insert(i, iw);
    mpDecLayout->removeWidget(iw);
    mpDecLayout->insertWidget(i, iw);
    QStringList decs_all;
    QStringList decs_p          = AVPlayerConfigMngr::instance().decoderPriorityNames();
    QStringList decs;

    Q_FOREACH (DecoderItemWidget* const w, mDecItems)
    {
        decs_all.append(w->name());

        if (decs_p.contains(w->name()))
        {
            decs.append(w->name());
        }
    }

    sDecodersUi = idsFromNames(decs_all);
    sPriorityUi = idsFromNames(decs);

    if (applyOnUiChange())
    {
        AVPlayerConfigMngr::instance().setDecoderPriorityNames(decs);
    }
    else
    {
/*
        Q_EMIT AVPlayerConfigMngr::instance().decoderPriorityChanged(idsFromNames(decs));
*/
    }
}

void DecoderConfigPage::priorityDown()
{
    if (!mpSelectedDec)
    {
        return;
    }

    int i = mDecItems.indexOf(mpSelectedDec);

    if (i == (mDecItems.size() - 1))
    {
        return;
    }

    DecoderItemWidget* const iw = mDecItems.takeAt(i + 1);
    mDecItems.insert(i, iw);

    // why takeItemAt then insertItem does not work?

    mpDecLayout->removeWidget(iw);
    mpDecLayout->insertWidget(i, iw);

    QStringList decs_all;
    QStringList decs_p          = AVPlayerConfigMngr::instance().decoderPriorityNames();
    QStringList decs;

    Q_FOREACH (DecoderItemWidget* const w, mDecItems)
    {
        decs_all.append(w->name());

        if (decs_p.contains(w->name()))
        {
            decs.append(w->name());
        }
    }

    sDecodersUi                 = idsFromNames(decs_all);
    sPriorityUi                 = idsFromNames(decs);

    if (applyOnUiChange())
    {
        AVPlayerConfigMngr::instance().setDecoderPriorityNames(decs);
    }
    else
    {
/*
        Q_EMIT AVPlayerConfigMngr::instance().decoderPriorityChanged(idsFromNames(decs));
        Q_EMIT AVPlayerConfigMngr::instance().registeredDecodersChanged(idsFromNames(decs));
*/
    }
}

void DecoderConfigPage::onDecSelected(DecoderItemWidget* iw)
{
    if (mpSelectedDec == iw)
    {
        return;
    }

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
/*
    qCDebug(DIGIKAM_AVPLAYER_LOG) << "updateDecodersUi " << this << " " << names << " all: " << all_names;
*/
    int idx = 0;

    Q_FOREACH (const QString& nm, all_names)
    {
        DecoderItemWidget* iw = nullptr;

        for (int i = idx ; i < mDecItems.size() ; ++i)
        {
            if (mDecItems.at(i)->name() != nm)
            {
               continue;
            }

            iw = mDecItems.at(i);

            break;
        }

        if (!iw)
        {
            break;
        }

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
    sPriorityUi = idsFromNames(AVPlayerConfigMngr::instance().decoderPriorityNames());
    sDecodersUi = VideoDecoder::registered();
    updateDecodersUi();
}

QStringList DecoderConfigPage::idsToNames(QVector<VideoDecoderId> ids)
{
    QStringList decs;

    Q_FOREACH (int id, ids)
    {
        decs.append(QString::fromLatin1(VideoDecoder::name(id)));
    }

    return decs;
}

QVector<VideoDecoderId> DecoderConfigPage::idsFromNames(const QStringList& names)
{
    QVector<VideoDecoderId> decs;

    Q_FOREACH (const QString& nm, names)
    {
        if (nm.isEmpty())
        {
            continue;
        }

        VideoDecoderId id = VideoDecoder::id(nm.toLatin1().constData());

        if (id == 0)
        {
            continue;
        }

        decs.append(id);
    }

    return decs;
}

} // namespace QtAV

#include "DecoderConfigPage.moc"
