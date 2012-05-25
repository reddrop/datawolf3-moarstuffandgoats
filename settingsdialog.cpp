#include <QTextCodec>
#include <QList>
#include <QByteArray>
#include <QFontDialog>
#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "autojoinlistdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    QList<QByteArray> codecs = QTextCodec::availableCodecs();
    foreach (QByteArray arr, codecs)
    {
        ui->comboBoxCharset->addItem(arr);
    }

    ui->comboBoxCharset->model()->sort(0);

    defaultindex = ui->comboBoxCharset->findText("windows-1251");
    if (defaultindex>=0)
        ui->comboBoxCharset->setCurrentIndex(defaultindex);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::putSettings()
{
    //irc
    ui->lineEditServer->setText(settings.server);
    ui->spinBoxPort->setValue(settings.port);
    ui->lineEditNick->setText(settings.nick);
    ui->lineEditUsername->setText(settings.username);
    ui->lineEditRealName->setText(settings.realname);
    ui->lineEditPassword->setText(settings.password);
    ui->comboBoxCharset->setCurrentIndex(getCharsetIndex(settings.charset));
    ui->checkBoxAutoConnect->setChecked(settings.autoConnect);

    //program
    ui->checkBoxHideConsole->setChecked(settings.hideconsole);
    ui->checkBoxTrayIcon->setChecked(settings.trayicon);
    ui->checkBoxMinimizeToTray->setChecked(settings.minimizeonclose);
    if (consolefont.fromString(settings.consolefont))
        ui->lineEditConsoleFont->setText(fonttostr(consolefont));
    if (chatfont.fromString(settings.chatfont))
        ui->lineEditChatFont->setText(fonttostr(chatfont));
    ui->checkBoxTimeStamps->setChecked(settings.timestamps);
    ui->lineEditFormat->setText(settings.timestampformat);

    //sounds
    ui->checkBoxSoundsEnabled->setChecked(settings.soundsenabled);
    ui->tableSounds->readSounds();
}

void SettingsDialog::getSettings()
{
    //irc
    settings.server = ui->lineEditServer->text();
    settings.port = ui->spinBoxPort->value() % 65536;
    settings.nick = ui->lineEditNick->text();
    settings.username = ui->lineEditUsername->text();
    settings.realname = ui->lineEditRealName->text();
    settings.password = ui->lineEditPassword->text();
    settings.charset = ui->comboBoxCharset->currentText();
    settings.autoConnect = ui->checkBoxAutoConnect->isChecked();

    //program
    settings.hideconsole = ui->checkBoxHideConsole->isChecked();
    settings.trayicon = ui->checkBoxTrayIcon->isChecked();
    settings.minimizeonclose = ui->checkBoxMinimizeToTray->isChecked();
    settings.consolefont = consolefont.toString();
    settings.chatfont = chatfont.toString();
    settings.timestamps = ui->checkBoxTimeStamps->isChecked();
    settings.timestampformat = ui->lineEditFormat->text();

    //sounds
    settings.soundsenabled = ui->checkBoxSoundsEnabled->isChecked();
    ui->tableSounds->writeSounds();
}

void SettingsDialog::changeEvent(QEvent *e)
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

QString SettingsDialog::fonttostr(const QFont &font)
{
    QString res;
    res.append(font.family());
    res.append(" ");
    res.append(QString::number(font.pointSize(), 10));
    if (font.bold())
        res.append(tr(" Bold"));
    if (font.italic())
        res.append(tr(" Italic"));
    return res;
}

int SettingsDialog::getCharsetIndex(const QString &charsetname)
{
    int idx = ui->comboBoxCharset->findText(charsetname);
    if (idx>=0)
        return idx;
    return defaultindex;
}

void SettingsDialog::on_toolButtonConsoleFont_clicked()
{
    bool ok;
    QFont fnt = QFontDialog::getFont(&ok, consolefont, this);
    if (ok)
    {
        consolefont = fnt;
        ui->lineEditConsoleFont->setText(fonttostr(fnt));
    }
}

void SettingsDialog::on_toolButtonChatFont_clicked()
{
    bool ok;
    QFont fnt = QFontDialog::getFont(&ok, chatfont, this);
    if (ok)
    {
        chatfont = fnt;
        ui->lineEditChatFont->setText(fonttostr(fnt));
    }
}

void SettingsDialog::on_pushButtonAutoJoin_clicked()
{
    AutoJoinListDialog dlg(this, settings.autojoinlist);
    if (dlg.exec()==QDialog::Accepted)
        settings.autojoinlist = dlg.getList();
}
