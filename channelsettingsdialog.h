#ifndef CHANNELSETTINGSDIALOG_H
#define CHANNELSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
    class ChannelSettingsDialog;
}

class ChannelSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChannelSettingsDialog(QWidget *parent = 0);
    ~ChannelSettingsDialog();
    QString channel;

public slots:
    void bansReceived(const QString &channel, const QString &mask, const QString &date, const QString &user);
    void channelModeReceived(const QString &channel, const QStringList &mode);
    void clearItems();
    void setTopicText(const QString &topic);

private:
    QString topic;
    QString oldkey;
    const QString makeMode();

private slots:
    void updateBanList();
    void addButtonPressed();
    void deleteButtonPressed();
    void clearButtonPressed();
    void dialogAccepted();

signals:
    void updatePressed(const QString &channel);
    void channelModeChanged(const QString &channel, const QString &mode);
    void topicChanged(const QString &channel, const QString &newtopic);

private:
    Ui::ChannelSettingsDialog *ui;
};

#endif // CHANNELSETTINGSDIALOG_H
