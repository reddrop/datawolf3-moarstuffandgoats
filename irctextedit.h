#ifndef IRCTEXTEDIT_H
#define IRCTEXTEDIT_H

#include <QPlainTextEdit>

class IrcTextEdit : public QPlainTextEdit
{
Q_OBJECT
public:
    explicit IrcTextEdit(QWidget *parent = 0);
    void storeLastMessage();
    const QStringList history()
    {
        return typedmessages;
    }
    void setHistory(const QStringList &messages)
    {
        typedmessages = messages;
        messagepos = messages.size();
    }
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

private:
    QStringList typedmessages;
    int messagepos;
    static const int limit = 50;

signals:
    void sendKeyPressed();

protected:
    void keyPressEvent(QKeyEvent *e);
};

#endif // IRCTEXTEDIT_H
