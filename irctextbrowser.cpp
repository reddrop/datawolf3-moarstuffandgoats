#include "irctextbrowser.h"
#include <QDateTime>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QTextCursor>
#include "common.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TextBrowser::resizeEvent(QResizeEvent *e)
{
    QScrollBar *vbar = verticalScrollBar();
    bool scroll = (vbar->value()==vbar->maximum());
    QTextBrowser::resizeEvent(e);
    if (scroll)
        vbar->setValue(vbar->maximum());
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

TextBrowser::TextBrowser(QWidget *parent) : QTextBrowser(parent)
{
}

IrcTextBrowser::IrcTextBrowser(const QString &channel, QWidget *parent) : QWidget(parent)
{
    this->channel = channel;

    textBrowser = new TextBrowser(this);
    textBrowser->setOpenLinks(false);

    labelTopic = new QLabel(this);
    labelTopic->setMargin(5);
    labelTopic->setTextFormat(Qt::PlainText);
    labelTopic->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    labelTopic->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(labelTopic);
    layout->addWidget(textBrowser);

    this->setLayout(layout);

    labelTopic->hide();

    connect(textBrowser, SIGNAL(anchorClicked(QUrl)), this, SLOT(linkClicked(QUrl)));
    connect(textBrowser, SIGNAL(doubleClicked()), this, SLOT(textBrowserDoubleClicked()));
    connect(textBrowser, SIGNAL(mouseReleased()), this, SIGNAL(mouseReleased()));
}

IrcTextBrowser::~IrcTextBrowser()
{
    delete textBrowser;
    delete labelTopic;
}

const QString IrcTextBrowser::topic()
{
    return labelTopic->text();
}

void IrcTextBrowser::setFont(const QFont &fnt)
{
    labelTopic->setFont(fnt);
    textBrowser->setFont(fnt);
    QWidget::setFont(fnt);
}

void IrcTextBrowser::scrollToEnd()
{
    QScrollBar *vbar = textBrowser->verticalScrollBar();
    if (vbar)
        vbar->setValue(vbar->maximum());
}

void IrcTextBrowser::setTopic(const QString &topic)
{
    labelTopic->setText(topic);
    labelTopic->setHidden(topic.isEmpty());
}

void IrcTextBrowser::append(const QString &text)
{
    QScrollBar *vbar = textBrowser->verticalScrollBar();
    bool scroll = (vbar->value()==vbar->maximum());

    QTextCursor cursor(textBrowser->document());
    cursor.movePosition(QTextCursor::End);
    if (!textBrowser->document()->isEmpty())
        cursor.insertBlock();
    if (settings.timestamps)
        cursor.insertText(QDateTime::currentDateTime().toString(settings.timestampformat).append(QChar(' ')));
    cursor.insertText(text);
    if (scroll)
        vbar->setValue(vbar->maximum());
}

//////////////private/////////////////////
void IrcTextBrowser::linkClicked(const QUrl &link)
{
    if (!link.scheme().isEmpty())
        return;
    textBrowser->moveCursor(QTextCursor::End);
    emit userNameClicked(link.toString().append(": "));
}

void IrcTextBrowser::textBrowserDoubleClicked()
{
    emit browserDoubleClicked(channel);
}
