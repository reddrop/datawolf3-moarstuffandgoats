#include "messagedialog.h"
#include "ui_messagedialog.h"
#include <QDateTime>
#include "soundplayer.h"

MessageDialog::MessageDialog(const QString &nick, QWidget *parent) : QDialog(parent), ui(new Ui::MessageDialog)
{
    ui->setupUi(this);
    this->nick = nick;

    setWindowTitle(nick);

    QList<int> sizes;
    sizes << 600 << 100;
    ui->splitter->setSizes(sizes);

    connect(ui->pushButtonSend, SIGNAL(clicked()), this, SLOT(sendPressed()));
    connect(ui->plainTextEdit, SIGNAL(sendKeyPressed()), this, SLOT(sendPressed()));

    //smiles
    smilemenu = new QMenu(this);
    smilewidgetaction = new QWidgetAction(smilemenu);
    smilebar = new SmileBar();
    smilewidgetaction->setDefaultWidget(smilebar);
    smilemenu->addAction(smilewidgetaction);
    connect(smilebar, SIGNAL(smileClicked(QString)), ui->plainTextEdit, SLOT(insertPlainText(QString)));
    connect(smilebar, SIGNAL(smileClicked(QString)), ui->plainTextEdit, SLOT(setFocus()));
    ui->toolButtonEmoticons->setMenu(smilemenu);
}

MessageDialog::~MessageDialog()
{
    delete smilebar;
    delete smilewidgetaction;
    delete smilemenu;
    delete ui;
}

void MessageDialog::setFont(const QFont &font)
{
    ui->textBrowser->setFont(font);
}

void MessageDialog::messageReceived(const QString &text)
{
    show();
    smilebar->printParsedMessage(ui->textBrowser, nick, text);
}

void MessageDialog::sendPressed()
{
    QString mess = ui->plainTextEdit->toPlainText();
    if (mess.isEmpty())
        return;
    ui->plainTextEdit->storeLastMessage();
    ui->plainTextEdit->clear();
    playSound(SND_SENDMSG);

    QStringList messages = mess.split(QString("\n"));
    for (int i=0; i<messages.size(); i++)
    {
        if (messages[i].isEmpty())
            continue;
        emit messageReady(nick, messages[i]);
        smilebar->printParsedMessage(ui->textBrowser, currentnick, messages[i]);
    }
}

void MessageDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
