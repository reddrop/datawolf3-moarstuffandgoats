#ifndef CHANNELLISTDIALOG_H
#define CHANNELLISTDIALOG_H

#include <QDialog>
#include <QMutexLocker>
#include <QTableWidgetItem>

namespace Ui {
    class ChannelListDialog;
}

class ChannelListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChannelListDialog(QWidget *parent = 0);
    ~ChannelListDialog();

private:
    Ui::ChannelListDialog *ui;
    QMutex mutex;
    bool inProgress;

public slots:
    void showAndClear();
    void addListItem(const QString &channel, const QString &usercount, const QString &modetopic);
    void endOfList();

private slots:
    void on_tableWidgetChannels_itemDoubleClicked(QTableWidgetItem* item);

signals:
    void channelDoubleClicked(const QString &channel);
};

#endif // CHANNELLISTDIALOG_H
