#include "channeltreewidget.h"
/////////////////////////////////////////////////public/////////////////////////////////////////////////////

ChannelTreeWidget::ChannelTreeWidget(QWidget *parent) :
    QTreeWidget(parent)
{
    iconchannel = new QIcon(":/icons/small/channel.png");
    iconuser = new QIcon(":/icons/small/user.png");
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(channelItemDoubleClicked(QTreeWidgetItem*,int)));
    sortItems(0, Qt::AscendingOrder);
}

ChannelTreeWidget::~ChannelTreeWidget()
{
    delete iconchannel;
    delete iconuser;
}

const QString ChannelTreeWidget::selectedUser()
{
    QList<QTreeWidgetItem *> items = selectedItems();
    if (items.size()!=1)
        return QString();
    QString user = items[0]->text(0);
    if (user.isEmpty())
        return QString();
    if (user[0]==QChar('#'))
        return QString();
    return user;
}

const QString ChannelTreeWidget::selectedChannel()
{
    QList<QTreeWidgetItem *> items = selectedItems();
    if (items.size()!=1)
        return QString();
    QString channel = items[0]->text(0);
    if (channel.isEmpty())
        return QString();
    if (channel[0]!=QChar('#'))
    {
        QTreeWidgetItem *parentitem = items[0]->parent();
        if (parentitem)
            return parentitem->text(0);
    }
    return channel;
}

const QStringList ChannelTreeWidget::channelsOfNick(const QString &nick)
{
    return QStringList(membership.values(nick));
}

/////////////////////////////////////////////////private/////////////////////////////////////////////////////
void ChannelTreeWidget::clearChannelNames(const QString &channel)
{
    if (!channels.contains(channel))
        return;
    CHANNELITEM channelitem = channels.value(channel);
    clearChannelNames(channelitem.users);
}

void ChannelTreeWidget::clearChannelNames(QHash<QString, QTreeWidgetItem *> *channelUsers)
{
    if (!channelUsers)
        return;
    QHashIterator<QString, QTreeWidgetItem *> iterator(*channelUsers);
    while(iterator.hasNext())
    {
        iterator.next();
        QTreeWidgetItem *item = iterator.value();
        delete item;
        membership.remove(iterator.key());
    }
    channelUsers->clear();
}

const QString ChannelTreeWidget::parseOper(const QString &nick)
{
    if (nick[0]==QChar('@') || nick[0]==QChar('&') || nick[0]==QChar('~') || nick[0]==QChar('%')
        || nick[0]==QChar('+'))
    {
        QString name = nick;
        name.remove(0, 1);
        return name;
    }
    return nick;
}

void ChannelTreeWidget::setItemNick(QTreeWidgetItem *item, const QString &nick)
{
    if (!item)
        return;
    item->setIcon(0, *iconuser);
    if (nick[0]==QChar('@'))
    {
        QString name = nick;
        name.remove(0, 1);
        QFont fnt = item->font(0);
        fnt.setBold(true);
        fnt.setItalic(false);
        item->setFont(0, fnt);
        item->setText(0, name);
        return;
    }
    if (nick[0]==QChar('&'))
    {
        QString name = nick;
        name.remove(0, 1);
        QFont fnt = item->font(0);
        fnt.setItalic(true);
        fnt.setBold(false);
        item->setFont(0, fnt);
        item->setText(0, name);
        return;
    }
    if (item->font(0).bold())
    {
        QFont fnt = item->font(0);
        fnt.setBold(false);
        item->setFont(0, fnt);
    }
    if (item->font(0).italic())
    {
        QFont fnt = item->font(0);
        fnt.setItalic(false);
        item->setFont(0, fnt);
    }
    if (nick[0]==QChar('~') || nick[0]==QChar('%') || nick[0]==QChar('+'))
    {
        QString name = nick;
        name.remove(0, 1);
        item->setText(0, name);
        return;
    }
    item->setText(0, nick);
}

///////////////////////////////////////////////protected////////////////////////////////////////////////////

void ChannelTreeWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QList<QTreeWidgetItem *> items = selectedItems();
    if (items.size()!=1)
        return;
    QString user = items[0]->text(0);
    if (user.isEmpty())
        return ;
    if (user[0]==QChar('#'))
    {
        emit channelContextMenu(event->globalPos());
        return;
    }
    emit userContextMenu(event->globalPos());
}

///////////////////////////////////////////////public slots////////////////////////////////////////////////////
void ChannelTreeWidget::addChannel(const QString &channel)
{
    if (channels.contains(channel))
        return;
    CHANNELITEM item;
    item.widgetitem = new QTreeWidgetItem(QStringList(channel));
    item.widgetitem->setIcon(0, *iconchannel);
    item.users = new QHash<QString, QTreeWidgetItem *>();
    channels.insert(channel, item);
    addTopLevelItem(item.widgetitem);
}

void ChannelTreeWidget::deleteChannel(const QString &channel)
{
    clearChannelNames(channel);
    if (!channels.contains(channel))
        return;
    CHANNELITEM channelitem = channels.value(channel);
    removeItemWidget(channelitem.widgetitem, 0);
    delete channelitem.widgetitem;
    delete channelitem.users;
    channels.remove(channel);
}

void ChannelTreeWidget::setChannelNames(const QString &channel, const QStringList &names)
{
    if (!channels.contains(channel))
        return;
    //clearChannelNames(channel);
    CHANNELITEM channelitem = channels.value(channel);

    for (int i=0; i<names.size(); i++)
    {
        QTreeWidgetItem *widgitem;
        if (channelitem.users->contains(parseOper(names[i])))
        {
            widgitem = channelitem.users->value(parseOper(names[i]));
            setItemNick(widgitem, names[i]);
            continue;
        }
        widgitem = new QTreeWidgetItem(channelitem.widgetitem);
        setItemNick(widgitem, names[i]);
        channelitem.users->insert(parseOper(names[i]), widgitem);
        membership.insert(parseOper(names[i]), channel);
    }
}

void ChannelTreeWidget::addChannelName(const QString &channel, const QString &name)
{
    if (!channels.contains(channel))
        return;
    CHANNELITEM channelitem = channels.value(channel);

    if (channelitem.users->contains(name))
        return;

    QTreeWidgetItem *childitem = new QTreeWidgetItem(channelitem.widgetitem);
    setItemNick(childitem, name);
    channelitem.users->insert(parseOper(name), childitem);

    membership.insert(parseOper(name), channel);
}

void ChannelTreeWidget::deleteChannelName(const QString &name)
{
    const QString truename = parseOper(name);
    if (!membership.contains(truename))
        return;
    QList<QString> channelnames = membership.values(truename);
    for (int i=0; i<channelnames.size(); i++)
    {
        if (!channels.contains(channelnames[i]))
            continue;
        CHANNELITEM channelitem = channels.value(channelnames[i]);
        QTreeWidgetItem *useritem = channelitem.users->value(truename, 0);
        if (!useritem)
            continue;
        channelitem.widgetitem->removeChild(useritem);
        channelitem.users->remove(truename);
        delete useritem;
    }
    membership.remove(truename);
}

void ChannelTreeWidget::deleteChannelName(const QString &channel, const QString &name)
{
    const QString truename = parseOper(name);

    if (!channels.contains(channel))
        return;
    CHANNELITEM channelitem = channels.value(channel);

    if (!channelitem.users->contains(truename))
        return;
    QTreeWidgetItem *childItem = channelitem.users->value(truename);

    channelitem.widgetitem->removeChild(childItem);
    channelitem.users->remove(truename);
    delete childItem;

    membership.remove(truename, channel);
}

void ChannelTreeWidget::collapseChannel(const QString &channel)
{
    if (!channels.contains(channel))
        return;
    CHANNELITEM channelitem = channels.value(channel);
    collapseItem(channelitem.widgetitem);
}

void ChannelTreeWidget::expandChannel(const QString &channel)
{
    if (!channels.contains(channel))
        return;
    CHANNELITEM channelitem = channels.value(channel);
    expandItem(channelitem.widgetitem);
}


void ChannelTreeWidget::clearAllChannels()
{
    QHashIterator<QString, CHANNELITEM> channelsIterator(channels);
    while (channelsIterator.hasNext())
    {
        channelsIterator.next();
        CHANNELITEM channelitem = channelsIterator.value();
        delete channelitem.widgetitem;
        delete channelitem.users;
    }
    channels.clear();
    membership.clear();
}

void ChannelTreeWidget::changeNick(const QString &oldnickunreal, const QString &newnick)
{
    const QString oldnick = parseOper(oldnickunreal);
    if (!membership.contains(oldnick))
        return;
    QList<QString> channelnames = membership.values(oldnick);
    for (int i=0; i<channelnames.size(); i++)
    {
        if (!channels.contains(channelnames[i]))
            continue;
        CHANNELITEM channelitem = channels.value(channelnames[i]);
        QTreeWidgetItem *childItem = channelitem.users->value(oldnick, 0);
        if (!childItem)
            continue;
        channelitem.users->remove(oldnick);
        childItem->setText(0, newnick);
        channelitem.users->insert(newnick, childItem);
    }

    membership.remove(oldnick);
    for (int i=0; i<channelnames.size(); i++)
        membership.insert(newnick, channelnames[i]);
}

void ChannelTreeWidget::setOper(const QString &channel, const QString &nick, bool isOper)
{
    if (!channels.contains(channel))
        return;
    CHANNELITEM channelitem = channels.value(channel);

    QTreeWidgetItem *childItem = channelitem.users->value(nick, 0);
    if (!childItem)
        return;

    QFont fnt = childItem->font(0);
    fnt.setBold(isOper);
    childItem->setFont(0, fnt);
}

///////////////////////////////////////////////private slots////////////////////////////////////////////////////
void ChannelTreeWidget::channelItemDoubleClicked(QTreeWidgetItem *item, int)
{
    QString username = item->text(0);
    if (username.isEmpty())
        return;
    if (username[0]==QChar('#'))
        return;
    emit userDoubleClicked(username);
}
