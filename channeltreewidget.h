#ifndef CHANNELTREEWIDGET_H
#define CHANNELTREEWIDGET_H

#include <QTreeWidget>
#include <QContextMenuEvent>

struct CHANNELITEM
{
    QTreeWidgetItem *widgetitem;
    QHash<QString, QTreeWidgetItem *> *users;
};

class ChannelTreeWidget : public QTreeWidget
{
Q_OBJECT
public:
    explicit ChannelTreeWidget(QWidget *parent = 0);
    ~ChannelTreeWidget();
    const QString selectedUser();
    const QString selectedChannel();
    const QStringList channelsOfNick(const QString &nick);

private:
    QHash<QString, CHANNELITEM> channels;
    QMultiHash<QString, QString> membership;

    void clearChannelNames(const QString &channel);
    void clearChannelNames(QHash<QString, QTreeWidgetItem *> *channelUsers);
    const QString parseOper(const QString &nick);
    void setItemNick(QTreeWidgetItem *item, const QString &text);
    QIcon *iconchannel;
    QIcon *iconuser;

protected:
    void contextMenuEvent(QContextMenuEvent *event);

public slots:
    void addChannel(const QString &channel);
    void deleteChannel(const QString &channel);
    void setChannelNames(const QString &channel, const QStringList &names);
    void addChannelName(const QString &channel, const QString &name);
    void deleteChannelName(const QString &name);
    void deleteChannelName(const QString &channel, const QString &name);
    void clearAllChannels();
    void changeNick(const QString &oldnick, const QString &newnick);
    void collapseChannel(const QString &channel);
    void expandChannel(const QString &channel);
    void setOper(const QString &channel, const QString &nick, bool isOper);

private slots:
    void channelItemDoubleClicked(QTreeWidgetItem * item, int column);

signals:
    void userDoubleClicked(const QString &user);
    void userContextMenu(const QPoint &pos);
    void channelContextMenu(const QPoint &pos);
};

#endif // CHANNELTREEWIDGET_H
