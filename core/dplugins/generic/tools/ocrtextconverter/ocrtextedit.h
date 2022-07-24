
#ifndef OCR_TEXT_EDIT_H
#define OCR_TEXT_EDIT_H

// Qt includes 

#include <QTextEdit>
#include <QUrl>
#include <QString>
#include <QTextImageFormat>
#include <QTabWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>

// KDE includes 

#include <KTextEdit>

// Local includes 

class OcrWordData : public QTextCharFormat
{
public:
    enum DataType 
    {
        Rectangle = QTextFormat::UserProperty,		// QRect
        Alternatives,					            // QStringList
        KNode						                // int - not sure what this ever did
    };

    OcrWordData() : QTextCharFormat(){}
};




class OcrTextEdit : KTextEdit
{
    Q_OBJECT

public:

    /**
     * Create an instance of OCR editor 
     * of post-processing 
     */
    explicit OcrTextEdit(QWidget* parent); 

public Q_SLOTS:  

    /**
     * 
     * 
     */
    void slotSelectWord(const QPoint& pos);

    /**
     *
     * 
     */
    void slotUpdateHighlight();

Q_SIGNALS:
    
    /**
     *
     * 
     */
    void highlightWord(const QRect& r);

    /**
     * 
     * 
     */
    void scrollToWord(const QRect& r);
};



#endif // OCR_TEXT_EDIT_H
