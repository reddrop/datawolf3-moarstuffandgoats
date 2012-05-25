#ifndef MESSAGEDIALOG_H
#define MESSAGEDIALOG_H

#include <QDialog>
#include "common.h"
#include <QMenu>
#include <QWidgetAction>
#include "smilebar.h"

namespace Ui {
    class MessageDialog;
}

class MessageDialog : public QDialog {
    Q_OBJECT
public:
    MessageDialog(const QString &nick, QWidget *parent = 0);
    ~MessageDialog();
    void setFont(const QFont &font);

public slots:
    void messageReceived(const QString &text);

private slots:
    void sendPressed();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MessageDialog *ui;
    QString nick;
    QMenu *smilemenu;
    QWidgetAction *smilewidgetaction;
    SmileBar *smilebar;

signals:
    void messageReady(const QString &nick, const QString &message);
};

#endif // MESSAGEDIALOG_H
