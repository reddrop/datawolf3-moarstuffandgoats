#include "irctextedit.h"

IrcTextEdit::IrcTextEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
    messagepos = 0;
}

void IrcTextEdit::storeLastMessage()
{
    if (typedmessages.size()>=limit)
    {
        typedmessages.removeFirst();
    }
    typedmessages << toPlainText();
    messagepos = typedmessages.size();
}

QSize IrcTextEdit::sizeHint() const
{
    QFontMetrics fm(font());
    return QSize(0, fm.height()*3);
}

QSize IrcTextEdit::minimumSizeHint() const
{
    QFontMetrics fm(font());
    return QSize(0, fm.height()*2);
}

void IrcTextEdit::keyPressEvent(QKeyEvent *e)
{
    if (e->key()==Qt::Key_Return)
    {
        if (e->modifiers() & Qt::ControlModifier)
        {
            e->setModifiers(Qt::NoModifier);
            QPlainTextEdit::keyPressEvent(e);
            return;
        }
        emit sendKeyPressed();
        return;
    }
    if (e->key()==Qt::Key_Up)
    {
        if ( (messagepos>0) && (messagepos<=typedmessages.size()) )
        {
            messagepos--;
            setPlainText(typedmessages[messagepos]);
            moveCursor(QTextCursor::EndOfLine);
        }
    }
    if (e->key()==Qt::Key_Down)
    {
        if ( (messagepos>=0) && (messagepos<typedmessages.size()) )
        {
            messagepos++;
            if (messagepos<typedmessages.size())
            {
                setPlainText(typedmessages[messagepos]);
                moveCursor(QTextCursor::EndOfLine);
            }
            else
                clear();
        }
    }
    QPlainTextEdit::keyPressEvent(e);
}

