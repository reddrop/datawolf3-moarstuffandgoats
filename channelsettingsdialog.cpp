#include "channelsettingsdialog.h"
#include "ui_channelsettingsdialog.h"
#include <QInputDialog>
#include <QDateTime>
#include "modeparser.h"

/////////////////////////////////////public////////////////////////////////////
ChannelSettingsDialog::ChannelSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChannelSettingsDialog)
{
    ui->setupUi(this);
    connect(ui->pushButtonUpdateBans, SIGNAL(clicked()), this, SLOT(updateBanList()));
    connect(ui->pushButtonAddBan, SIGNAL(clicked()), this, SLOT(addButtonPressed()));
    connect(ui->pushButtonDeleteBan, SIGNAL(clicked()), this, SLOT(deleteButtonPressed()));
    connect(ui->pushButtonClearBans, SIGNAL(clicked()), this, SLOT(clearButtonPressed()));
    connect(this, SIGNAL(accepted()), this, SLOT(dialogAccepted()));
}

ChannelSettingsDialog::~ChannelSettingsDialog()
{
    delete ui;
}
////////////////////////////////////////////public slots/////////////////////////////////////////
void ChannelSettingsDialog::bansReceived(const QString &channel, const QString &mask, const QString &date, const QString &user)
{
    if (channel!=this->channel)
        return;

    ui->tableWidgetBans->insertRow(0);
    QTableWidgetItem *itemmask = new QTableWidgetItem(mask);
    QTableWidgetItem *itemby = new QTableWidgetItem(user);

    QString datereal;
    bool ok;
    unsigned int dateint = date.toInt(&ok);
    if (ok)
        datereal = QDateTime::fromTime_t(dateint).toString();
    else
        datereal = date;

    QTableWidgetItem *itemdate = new QTableWidgetItem(datereal);

    ui->tableWidgetBans->setItem(0, 0, itemmask);
    ui->tableWidgetBans->setItem(0, 1, itemby);
    ui->tableWidgetBans->setItem(0, 2, itemdate);

}

void ChannelSettingsDialog::channelModeReceived(const QString &channel, const QStringList &mode)
{
    if (channel!=this->channel)
        return;
    ModeParser parser(mode);
    ui->checkBoxMode_i->setChecked(parser.hasMode("+i"));
    ui->checkBoxMode_m->setChecked(parser.hasMode("+m"));
    ui->checkBoxMode_n->setChecked(parser.hasMode("+n"));
    ui->checkBoxMode_p->setChecked(parser.hasMode("+p"));
    ui->checkBoxMode_s->setChecked(parser.hasMode("+s"));
    ui->checkBoxMode_t->setChecked(parser.hasMode("+t"));
    if (parser.hasMode("+l"))
        ui->spinBoxLimit->setValue(parser.argsOfMode("+l").at(0).toInt());
    if (parser.hasMode("+k"))
    {
        oldkey = parser.argsOfMode("+k").at(0);
        ui->lineEditPassword->setText(oldkey);
    }
}

void ChannelSettingsDialog::clearItems()
{
    ui->tableWidgetBans->clearContents();
    ui->tableWidgetBans->setRowCount(0);
    ui->lineEditTopic->clear();
    ui->checkBoxMode_i->setChecked(false);
    ui->checkBoxMode_m->setChecked(false);
    ui->checkBoxMode_n->setChecked(false);
    ui->checkBoxMode_p->setChecked(false);
    ui->checkBoxMode_s->setChecked(false);
    ui->checkBoxMode_t->setChecked(false);
    ui->lineEditPassword->clear();
    ui->spinBoxLimit->setValue(0);
}

void ChannelSettingsDialog::setTopicText(const QString &topic)
{
    this->topic = topic;
    ui->lineEditTopic->setText(topic);
}

///////////////////////////////private /////////////////////////////////

const QString ChannelSettingsDialog::makeMode()
{
    QStringList args;
    QString resultPlus("+");
    QString resultMinus("-");

    if (ui->checkBoxMode_i->isChecked())
        resultPlus.append('i');
    else
        resultMinus.append('i');

    if (ui->checkBoxMode_m->isChecked())
        resultPlus.append('m');
    else
        resultMinus.append('m');

    if (ui->checkBoxMode_n->isChecked())
        resultPlus.append('n');
    else
        resultMinus.append('n');

    if (ui->checkBoxMode_p->isChecked())
        resultPlus.append('p');
    else
        resultMinus.append('p');

    if (ui->checkBoxMode_s->isChecked())
        resultPlus.append('s');
    else
        resultMinus.append('s');

    if (ui->checkBoxMode_t->isChecked())
        resultPlus.append('t');
    else
        resultMinus.append('t');

    if (!ui->lineEditPassword->text().isEmpty())
    {
        resultPlus.append('k');
        args << ui->lineEditPassword->text();
    }
    else
    {
        resultMinus.append('k');
        args << oldkey;
    }

    if (ui->spinBoxLimit->value()>0)
    {
        resultPlus.append('l');
        args << QString::number(ui->spinBoxLimit->value(), 10);
    }
    else
        resultMinus.append('l');

    return resultPlus+resultMinus+" "+args.join(" ");
}

//////////////////////////private slots://///////////////////////////////
void ChannelSettingsDialog::updateBanList()
{
    ui->tableWidgetBans->clearContents();
    ui->tableWidgetBans->setRowCount(0);
    emit updatePressed(channel);
}

void ChannelSettingsDialog::addButtonPressed()
{
    bool ok;
    QString banstring = QInputDialog::getText(this, tr("Ban"),tr("Input ban mask"), QLineEdit::Normal, QString("user!*@*"), &ok);
    if ((!banstring.isEmpty()) && ok)
    {
        emit channelModeChanged(channel, QString("+b %1").arg(banstring));
        updateBanList();
    }
}

void ChannelSettingsDialog::deleteButtonPressed()
{
    QList<QTableWidgetItem *> items = ui->tableWidgetBans->selectedItems();
    if (items.isEmpty())
        return;
    QString modes("-");
    QString masks;
    for (int i=0; i<items.size(); i++)
    {
        if (items[i]->column()==0)
        {
            masks.append(items[i]->text());
            masks.append(QString(" "));
            modes.append("b");
        }
    }
    if (masks.right(1)==QString(" "))
        masks.remove(masks.length()-1, 1);
    emit channelModeChanged(channel, QString("%1 %2").arg(modes).arg(masks));
    updateBanList();
}

void ChannelSettingsDialog::clearButtonPressed()
{
    QString modes("-");
    QString masks;
    for(int i=0; i<ui->tableWidgetBans->rowCount(); i++)
    {
        QTableWidgetItem *item = ui->tableWidgetBans->item(i, 0);
        masks.append(item->text());
        masks.append(QString(" "));
        modes.append("b");
    }
    if (masks.right(1)==QString(" "))
        masks.remove(masks.length()-1, 1);
    emit channelModeChanged(channel, QString("%1 %2").arg(modes).arg(masks));
    updateBanList();
}

void ChannelSettingsDialog::dialogAccepted()
{
    if (topic!=ui->lineEditTopic->text())
        emit topicChanged(channel, ui->lineEditTopic->text());
    emit channelModeChanged(channel, makeMode());
}
