#ifndef IRCTEXTBROWSER_H
#define IRCTEXTBROWSER_H

#include <QWidget>
#include <QUrl>
#include <QLabel>
#include <QTextBrowser>
#include <QMouseEvent>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
class TextBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    explicit TextBrowser(QWidget *parent = 0);

protected:
    void mouseDoubleClickEvent(QMouseEvent *e)
    {
        emit doubleClicked();
        QTextBrowser::mouseDoubleClickEvent(e);
    }
    void mouseReleaseEvent(QMouseEvent *e)
    {
        if (e->button()==Qt::LeftButton && textCursor().selectedText().isEmpty())
            emit mouseReleased();
        QTextBrowser::mouseReleaseEvent(e);
    }
    void resizeEvent(QResizeEvent *e);

signals:
    void doubleClicked();
    void mouseReleased();
};
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class IrcTextBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit IrcTextBrowser(const QString &channel, QWidget *parent = 0);
    ~IrcTextBrowser();
    const QString topic();
    void setFont(const QFont &fnt);
    QTextBrowser *textbrowser()
    {
        return textBrowser;
    }

private:
    QLabel *labelTopic;
    TextBrowser *textBrowser;
    QString channel;
    void scrollToEnd();

public slots:
    void setTopic(const QString &topic);
    void append(const QString &text);

private slots:
    void linkClicked(const QUrl &link);
    void textBrowserDoubleClicked();

signals:
    void userNameClicked(const QString &name);
    void browserDoubleClicked(const QString &channel);
    void mouseReleased();
};

#endif // IRCTEXTBROWSER_H
