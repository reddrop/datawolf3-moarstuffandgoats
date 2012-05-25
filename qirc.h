#ifndef QIRC_H
#define QIRC_H

#include <QThread>
#include <libircclient/libircclient.h>
#include <stdio.h>
#include <QStringList>
#include <QTextCodec>

using namespace std;

class QIrc : public QThread
{
    Q_OBJECT
public:
    unsigned short port;
    QString server, nick, username, realname, password;
    QIrc();
    void setData(const QString &server, unsigned short port, const QString &nick, const QString &username, const QString &realname, const QString &password, const QString &codepage);
    bool isConnected()
    {
        return irc_is_connected(session);
    }
//private static:
private:
    static void dump_event(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count);
    static void event_join(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count);
    static void event_connect(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count);
    static void event_numeric(irc_session_t * session, unsigned int event, const char * origin, const char ** params, unsigned int count);
    static void event_channel(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count);
    static void event_privmsg(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count);
    static void event_topic(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count);
    static void event_kick(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count);
    static void event_part(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count);
    static void event_mode(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count);
    static void event_nick(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count);
    static void event_quit(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count);
    static void event_invite(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count);
    static void event_notice(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count);
    static void event_umode(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count);
    static void event_ctcp_action(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count);

public slots:
    bool createSession();
    void destroySession();
    void connectToServer();
    void disconnectFromServer();
    //messages
    void sendMessage(const QString &receiver, const QString &message);
    void sendMe(const QString &receiver, const QString &message);
    void sendNotice(const QString &receiver, const QString &message);
    //channel mgmt
    void joinChannel(const QString &channel, const QString &password = QString());
    void leaveChannel(const QString &channel);
    void getNames(const QString &channel);
    void getChannelsList();
    void getTopic(const QString &channel);
    void setTopic(const QString &channel, const QString &newtopic);
    void kick(const QString &channel, const QString &nick, const QString &reason);
    void getChannelMode(const QString &channel);
    void setChannelMode(const QString &channel, const QString &mode);
    void invite(const QString &channel, const QString &nick);
    void getBans(const QString &channel);
    //other
    void quit(const QString &reason);
    void getUserMode();
    void setUserMode(const QString &mode);
    void setNick(const QString &nick);
    void whois(const QString &nick);
    void sendRaw(const QString &message);

private:
    QTextCodec *codec;
    irc_callbacks_t callbacks;
    irc_session_t *session;
    void setCallbacks();
    void run();         //THREAD FUNC!!!
    const QString decode(const char *data)
    {
        return codec->toUnicode(data);
    }
    const QString decode(const QByteArray &data)
    {
        return codec->toUnicode(data);
    }
    const QByteArray encode(const QString &text)
    {
        return codec->fromUnicode(text);
    }


signals:
    void sigConnected();
    void sigMessage(const QString &origin, const QString &text);
    void sigChannelMessage(const QString &origin, const QString &channel, const QString &text);
    void sigPrivateMessage(const QString &origin, const QString &receiver, const QString &text);
    void sigChannelJoined(const QString &origin, const QString &channel);
    void sigChannelParted(const QString &origin, const QString &channel, const QString &reason);
    void sigChannelNames(const QString &origin, const QString &channel, const QStringList &names);
    void sigTopic(const QString &origin, const QString &channel, const QString &topic);
    void sigTopicSet(const QString &origin, const QString &channel, const QString &user, const QString &date);
    void sigKick(const QString &origin, const QString &channel, const QString &user);
    void sigChannelMode(const QString &channel, const QStringList &mode);
    void sigChannelModeChanged(const QString &origin, const QString &channel, const QStringList &mode);
    void sigNick(const QString &origin, const QString &newnick);
    void sigQuit(const QString &origin, const QString &reason);
    void sigInvite(const QString &origin, const QString &nick, const QString &channel);
    void sigNotice(const QString &origin, const QString &channel, const QString &text);
    void sigUmode(const QString &origin, const QString &mode);
    void sigCtcpAction(const QString &origin, const QString &channel, const QString &message);
    void sigBanList(const QString &channel, const QString &mask, const QString &date, const QString &user);
    void sigChannelListStart();
    void sigChannelListAddItem(const QString &channel, const QString &usercount, const QString &modetopic);
    void sigChannelListEnd();
    void sigErrorOccured(const QString &errortext);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};

#endif // QIRC_H
