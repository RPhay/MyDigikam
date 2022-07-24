

// Qt includes

#include <QDebug>

// KDE includes



// Local includes

#include "ocrtextedit.h"


static void moveForward(QTextCursor& curs, bool once = true)
{
    if (once) 
    {
        curs.movePosition(QTextCursor::NextCharacter);
    }

    while (curs.atBlockStart()) 
    {
        curs.movePosition(QTextCursor::NextCharacter);
    }
}

OcrTextEdit::OcrTextEdit(QWidget* parent)
    : KTextEdit(parent) 
{
    setObjectName(QLatin1String("OcrTextEdit"));
}

void OcrTextEdit::slotSelectWord(const QPoint& pos)
{
    
    if (document()->isEmpty()) 
    {
        return;    // nothing to search
    }

    qDebug() << pos;

    QTextCursor curs(document());
    QRect wordRect;

    moveForward(curs, false);

    while (!curs.atEnd())
    {
        QTextCharFormat fmt = curs.charFormat();
        
        QRect rect = fmt.property(OcrWordData::Rectangle).toRect();
        
        if (rect.isValid() && rect.contains(pos, true)) 
        {
            wordRect = rect;
            break;
        }
    }
    
    moveForward(curs);

    
    qDebug()    << QLatin1String("found rect") 
                << wordRect 
                << QLatin1String("at") 
                << curs.position();

    if (!wordRect.isValid()) 
    {
        return;    // no word found
    }

    // Then find the end of the word.  That is an OCR result word, i.e. a
    // span with the same character format, not a text word ended by whitespace.

    QTextCursor wordStart = curs;
    QTextCharFormat ref   = wordStart.charFormat();

    moveForward(curs);
    
    while (!curs.atEnd())
     {
        QTextCharFormat fmt = curs.charFormat();
        if (fmt != ref) 
        {
            break;
        }

        moveForward(curs);
    }
}



void OcrTextEdit::slotUpdateHighlight()
{
    if (isReadOnly()) 
    {
        return;
    }

    QTextCursor curs = textCursor();
    
    if (curs.hasSelection()) 
    {
        int send = curs.selectionEnd();
        curs.setPosition(curs.selectionStart());
        curs.movePosition(QTextCursor::NextCharacter);
        QTextCharFormat ref = curs.charFormat();
        bool same = true;

        while (curs.position() != send) 
        {
            curs.movePosition(QTextCursor::NextCharacter);
            QTextCharFormat fmt = curs.charFormat();
            if (fmt != ref) 
            {
                same = false;
                break;
            }
        }

        if (same) 
        {                 // valid word selection
            QRect r = ref.property(OcrWordData::Rectangle).toRect();
            Q_EMIT highlightWord(r);
            return;
        }
    }

    Q_EMIT highlightWord(QRect());            // no valid word selection,

    // clear highlight
    QTextCharFormat fmt = textCursor().charFormat();
    QRect r = fmt.property(OcrWordData::Rectangle).toRect();
    if (r.isValid()) {
        Q_EMIT scrollToWord(r);    // scroll to cursor position
    }
}

