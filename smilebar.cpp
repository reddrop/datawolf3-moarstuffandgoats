#include <QGridLayout>
#include <QFile>
#include <QMessageBox>
#include <QtCore/qmath.h>
#include <QDir>
#include <QTextEdit>
#include <QScrollBar>
#include <QTextCursor>
#include <QDateTime>
#include "common.h"
#include "smilebar.h"

#include <QDebug>

//private:
void SmileBar::loadSmiles()
{
    QFile xmlfile(settings.emoticonspath+QDir::separator()+"emoticons.xml");
    if (xmlfile.open(QFile::ReadOnly | QFile::Text))
    {
        reader.setDevice(&xmlfile);
        reader.readNext();
        while (!reader.atEnd())
        {
            if (reader.isStartElement())
            {
                if (reader.name()=="messaging-emoticon-map")
                    readXMLMessagingEmoticonsMap();
                else
                    xmlError(tr("Error reading messaging-emoticon-map"));
            }
            else
                reader.readNext();
        }
        if (reader.hasError())
            xmlError(tr("Error parsing XML: %1").arg(reader.errorString()));
        xmlfile.close();
    }
    if (xmlfile.error()!=QFile::NoError)
        xmlError(tr("Error reading emoticons list file"));

    QGridLayout *layout = new QGridLayout(this);
    const int width = (int) qSqrt(labels.size()) + 1;
    for (int i=0; i<labels.size(); i++)
        layout->addWidget(labels.at(i), (int)i/width, i % width, Qt::AlignCenter);
    setLayout(layout);

    for (int i=0; i<movies.size(); i++)
        movies.at(i)->jumpToFrame(0);
}

void SmileBar::readXMLMessagingEmoticonsMap()
{
    reader.readNext();
    while (!reader.atEnd())
    {
        if (reader.isEndElement())
        {
            reader.readNext();
            break;
        }
        if (reader.isStartElement())
        {
            if (reader.name()=="emoticon")
                readXMLEmoticon();
            else
                skipUnknown();
        }
        else
            reader.readNext();
    }
}

void SmileBar::readXMLEmoticon()
{
    bool isAnimated = false;
    QString file = settings.emoticonspath+QDir::separator()+reader.attributes().value("file").toString();
    if (QFile::exists(file+".gif"))
    {
        file.append(".gif");
        isAnimated = true;
    }
    else if (QFile::exists(file+".jpg"))
        file.append(".jpg");
    else if (QFile::exists(file+".bmp"))
        file.append(".bmp");
    else
        file.append(".png");

    QStringList strings;
    reader.readNext();
    while (!reader.atEnd())
    {
        if (reader.isEndElement())
        {
            reader.readNext();
            break;
        }
        if (reader.isStartElement())
        {
            if (reader.name()=="string")
                readXMLString(strings);
            else
                skipUnknown();
        }
        else
            reader.readNext();
    }
    /////////////////////////adding icon////////////////////////////////
    if (file.isEmpty() || strings.isEmpty())
        return;

    QLabel *label;
    label = new SmileLabel(this);
    label->setToolTip(strings.at(0));
    if (isAnimated)
    {
        QMovie *movie;
        movie = new QMovie(file, QByteArray(), this);
        movie->setCacheMode(QMovie::CacheAll);
        label->setMovie(movie);
        movies << movie;
    } else
    {
        label->setPixmap(QPixmap(file));
    }
    connect(label, SIGNAL(mouseClicked(QString)), this, SIGNAL(smileClicked(QString)));
    labels << label;

    ///////////////////filling smiles container///////////////////////////////
    foreach(QString smilestr, strings)
    {
        //smiles.insert(smilestr, QString("<img src=\"%1\" alt=\"%2\" title=\"%2\">").arg(file).arg(smilestr));
        smiles.insert(smilestr, file);
    }
}

void SmileBar::readXMLString(QStringList &stringlist)
{
    QString string = reader.readElementText();
    if (reader.isEndElement())
        reader.readNext();
    if (!string.isEmpty())
        stringlist << string;
}

void SmileBar::skipUnknown()
{
    reader.readNext();
    while (!reader.atEnd())
    {
        if (reader.isEndElement())
        {
            reader.readNext();
            break;
        }
        if (reader.isStartElement())
            skipUnknown();
        else
            reader.readNext();
    }
}

void SmileBar::xmlError(const QString &text)
{
    QMessageBox *msgbox = new QMessageBox(this);
    msgbox->setIcon(QMessageBox::Warning);
    msgbox->setText(text);
    connect(msgbox, SIGNAL(buttonClicked(QAbstractButton*)), msgbox, SLOT(deleteLater()));
    msgbox->show();
}

void SmileBar::getPositions(const QString &plainmsg, QMap<int, QString> &smilepositions)
{
    QMapIterator<SmileString, QString> iter(smiles);
    while (iter.hasNext()) //searching for each smile in text
    {
        iter.next();
        int globalpos = 0;
        int foundpos = -1;
        do
        {
            foundpos = plainmsg.indexOf(iter.key(), globalpos);
            if (foundpos>=0) //if smile found
            {
                for(int j=foundpos; j<foundpos+iter.key().size(); j++) //check for overlapping smile
                {
                    if (smilepositions.contains(j))
                    {
                        smilepositions.remove(j);           //remove overlapping smile
                        break;
                    }
                }
                smilepositions.insert(foundpos, iter.key());
                globalpos = foundpos+iter.key().size();
            }
        } while (foundpos>=0);
    }
}

//public:
SmileBar::SmileBar(QWidget *parent) :
    QWidget(parent)
{
    loadSmiles();
}

SmileBar::~SmileBar()
{
    qDeleteAll(movies);
    qDeleteAll(labels);
}
/*
QString SmileBar::parseSmiles(const QString &plainmsg)
{
    QMap<int, QString> smilepositions;  //positions of smiles in message

    getPositions(plainmsg, smilepositions);

    QString msg;    //result message string, contains html code instead of smiles
    QMapIterator<int, QString> mapiter(smilepositions);
    int pos = 0;
    while (mapiter.hasNext())
    {
        mapiter.next();
        msg+=plainmsg.midRef(pos, mapiter.key()-pos);
        msg+= QString("<img src=\"%1\" alt=\"%2\" title=\"%2\">")
              .arg( smiles.value(mapiter.value() )).arg( mapiter.value() );
        pos = mapiter.key() + mapiter.value().length();
    }
    msg+=plainmsg.right(plainmsg.length()-pos);
    return msg;
}
*/
void SmileBar::printParsedMessage(QTextEdit *browser, const QString &nick, const QString &plainmsg)
{
    QMap<int, QString> smilepositions;
    getPositions(plainmsg, smilepositions);

    //scroll or not
    QScrollBar *vbar = browser->verticalScrollBar();
    bool scroll = (vbar->value()==vbar->maximum());

    QTextCursor cursor(browser->document());
    cursor.movePosition(QTextCursor::End);
    if (!browser->document()->isEmpty())
        cursor.insertBlock();

    //time stamp
    if (settings.timestamps)
    {
        cursor.insertText(QDateTime::currentDateTime().toString(settings.timestampformat));
        cursor.insertText(" ");
    }

    //nick
    if (!nick.isEmpty())
        cursor.insertHtml(QString("<b>&lt;<a href=\"%1\">%1</a>&gt;</b> ").arg(nick));

    //message with emoticons
    QMapIterator<int, QString> mapiter(smilepositions);
    int pos = 0;
    while (mapiter.hasNext())
    {
        mapiter.next();
        cursor.insertText(plainmsg.mid(pos, mapiter.key()-pos));
        QString imghtml = QString("<img src=\"%1\" alt=\"%2\" title=\"%2\">")
                       .arg( smiles.value(mapiter.value()) ).arg( mapiter.value() );
        cursor.insertHtml(imghtml);
        pos = mapiter.key() + mapiter.value().length();
    }
    cursor.insertText(plainmsg.right(plainmsg.length()-pos));

    //scrolling
    if (scroll)
        vbar->setValue(vbar->maximum());
}

//protected:
void SmileBar::showEvent(QShowEvent *event)
{
    for (int i=0; i<movies.size(); i++)
        movies.at(i)->start();
    QWidget::showEvent(event);
}

void SmileBar::hideEvent(QHideEvent *event)
{
    for (int i=0; i<movies.size(); i++)
        movies.at(i)->stop();
    QWidget::hideEvent(event);
}

