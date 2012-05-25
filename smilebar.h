#ifndef SMILEBAR_H
#define SMILEBAR_H

#include <QWidget>
#include <QMovie>
#include <QXmlStreamReader>
#include <QMap>
#include <QLabel>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SmileLabel : public QLabel
{
    Q_OBJECT
public:
    explicit SmileLabel(QWidget *parent = 0) : QLabel(parent) {}
protected:
    void mouseReleaseEvent(QMouseEvent *event)
    {
        emit mouseClicked(toolTip().append(' '));
        QLabel::mouseReleaseEvent(event);
    }
signals:
    void mouseClicked(const QString &text);

};

class SmileString : public QString
{
public:
    SmileString() : QString() {}
    SmileString(const QString & other) : QString(other) {}
 };
inline bool operator<(const SmileString &str1, const SmileString &str2)
{
    if (str1.length()==str2.length())
        return static_cast<const QString &>(str1) < static_cast<const QString &>(str2);
    return str1.length()<str2.length();
}

class QTextEdit;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SmileBar : public QWidget
{
    Q_OBJECT
    QList<QLabel *> labels;
    QList<QMovie *> movies;
    QXmlStreamReader reader;
    void loadSmiles();
    void readXMLMessagingEmoticonsMap();
    void readXMLEmoticon();
    void readXMLString(QStringList &stringlist);
    void skipUnknown();
    void xmlError(const QString &text);
    void getPositions(const QString &plainmsg, QMap<int, QString> &smilepositions);

public:
    QMap<SmileString, QString> smiles;  //Table containing smile strings and its html-code replacements
                                        //Sorted by smile string length
    explicit SmileBar(QWidget *parent = 0);
    ~SmileBar();
    //QString parseSmiles(const QString &plainmsg);
    void printParsedMessage(QTextEdit *browser, const QString &nick, const QString &plainmsg);

protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *);

signals:
    void smileClicked(const QString &text);
};

#endif // SMILEBAR_H
