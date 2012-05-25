#include "channellistdialog.h"
#include "ui_channellistdialog.h"

ChannelListDialog::ChannelListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChannelListDialog)
{
    ui->setupUi(this);
    ui->tableWidgetChannels->setColumnWidth(0, 100);
    ui->tableWidgetChannels->setColumnWidth(1, 100);
    //ui->tableWidgetChannels->setColumnWidth(2, 700);
    inProgress = false;
}

ChannelListDialog::~ChannelListDialog()
{
    delete ui;
}



//////////////////////////////////////pub slots/////////////////////
void ChannelListDialog::showAndClear()
{
    inProgress = true;
    QMutexLocker locker(&mutex);
    ui->tableWidgetChannels->clearContents();
    ui->tableWidgetChannels->setRowCount(0);
    ui->tableWidgetChannels->setSortingEnabled(false);
    show();
}

void ChannelListDialog::addListItem(const QString &channel, const QString &usercount, const QString &modetopic)
{
    if (!inProgress)
        showAndClear();
    QMutexLocker locker(&mutex);
    QTableWidgetItem *itemchannel = new QTableWidgetItem(channel);
    QTableWidgetItem *itemusercount = new QTableWidgetItem();
    itemusercount->setData(Qt::DisplayRole, usercount.toInt());
    QTableWidgetItem *itemmodetopic = new QTableWidgetItem(modetopic);

    ui->tableWidgetChannels->insertRow(0);
    ui->tableWidgetChannels->setItem(0, 0, itemchannel);
    ui->tableWidgetChannels->setItem(0, 1, itemusercount);
    ui->tableWidgetChannels->setItem(0, 2, itemmodetopic);
}

void ChannelListDialog::endOfList()
{
    inProgress = false;
    QMutexLocker locker(&mutex);
    ui->tableWidgetChannels->setSortingEnabled(true);
}


void ChannelListDialog::on_tableWidgetChannels_itemDoubleClicked(QTableWidgetItem* item)
{
    const QString &channel = ui->tableWidgetChannels->item(item->row(), 0)->text();
    emit channelDoubleClicked(channel);
}
