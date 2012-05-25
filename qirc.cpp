#include "qirc.h"
#include <cstdlib>

QIrc::QIrc()
{
    codec = 0;
}

void QIrc::setData(const QString &server, unsigned short port, const QString &nick, const QString &username, const QString &realname, const QString &password, const QString &codepage)
{
    this->server = server;
    this->port = port;
    this->nick = nick;
    this->username = username;
    this->realname = realname;
    this->password = password;
    codec = QTextCodec::codecForName(codepage.toAscii());
    if (!codec)
        codec = QTextCodec::codecForLocale();
}

/////////////////////////////////////////////////////PUBLIC STATIC/////////////////////////////////////////////////////////////////
void QIrc::dump_event(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    QByteArray buf;
    unsigned int cnt;
    for ( cnt = 0; cnt < count; cnt++ )
    {
        if (!buf.isEmpty())
            buf.append(' ');
        buf.append(params[cnt]);
    }

    QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
    if (strcmp(event, "ERROR")==0)
    {
        emit ctx->sigErrorOccured(ctx->decode(buf));
        return;
    }
    emit ctx->sigMessage(ctx->decode(origin), QString("%1: %2").arg(ctx->decode(event)).arg(ctx->decode(buf)));
}

void QIrc::event_connect(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    dump_event(session, event, origin, params, count);
}

void QIrc::event_join(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    if (count!=1)
    {
        dump_event(session, event, origin, params, count);
        return;
    }
    QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
    emit ctx->sigChannelJoined(ctx->decode(origin), ctx->decode(params[0]));
}

void QIrc::event_numeric(irc_session_t * session, unsigned int event, const char * origin, const char ** params, unsigned int count)
{
    switch (event)
    {
    case 1:
        {
            QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
            emit ctx->sigConnected();
            break; //no return, execute dump_event
        }
    case 353:
        {
            if (count!=4)
                break;
            QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
            QStringList names = ctx->decode(params[3]).split(" ");
            if (names.last().isEmpty())
                names.removeLast();
            emit ctx->sigChannelNames(ctx->decode(origin), ctx->decode(params[2]), names);
            return;
        }
    case 332:
        {
            if (count!=3)
                break;
            char * tpc = irc_color_strip_from_mirc(params[2]);
            QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
            emit ctx->sigTopic(ctx->decode(origin), ctx->decode(params[1]), ctx->decode(tpc));
            free(tpc);
            return;
        }
    case 333:
        {
            if (count!=4)
                break;
            QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
            emit ctx->sigTopicSet(ctx->decode(origin), ctx->decode(params[1]), ctx->decode(params[2]), ctx->decode(params[3]));
            return;
        }
    case 367:
        {
            if (count!=5)
                break;
            QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
            emit ctx->sigBanList(ctx->decode(params[1]), ctx->decode(params[2]), ctx->decode(params[4]), ctx->decode(params[3]));
            return;
        }
    case 321:
        {
            QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
            emit ctx->sigChannelListStart();
            return;
        }
    case 322:
        {
            if (count!=4)
                break;
            char * tpc = irc_color_strip_from_mirc(params[3]);
            QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
            emit ctx->sigChannelListAddItem(ctx->decode(params[1]), ctx->decode(params[2]), ctx->decode(tpc));
            free(tpc);
            return;
        }
    case 323:
        {
            QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
            emit ctx->sigChannelListEnd();
            return;
        }
    case 324:
        {
            if (count<3)
                break;
            QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
            QStringList modes;
            for (unsigned int i=2; i<count; i++)
                modes << ctx->decode(params[i]);
            emit ctx->sigChannelMode(ctx->decode(params[1]), modes);
            //emit ctx->sigChannelMode(ctx->decode(params[1]), ctx->decode(params[2]));
            break; //no return, execute dump_event
        }
    case 482:
        {
            if (count!=3)
                break;
            QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
            emit ctx->sigChannelMessage(QString(), ctx->decode(params[1]), ctx->decode(params[2]));
            return;
        }
    }
    dump_event(session, QByteArray::number(event).constData(), origin, params, count);
}

void QIrc::event_channel(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    if (count!=2)
    {
        dump_event(session, event, origin, params, count);
        return;
    }
    char * msg = irc_color_strip_from_mirc(params[1]);
    QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
    emit ctx->sigChannelMessage(ctx->decode(origin), ctx->decode(params[0]), ctx->decode(msg));
    free(msg);
}

void QIrc::event_privmsg(irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    if (count!=2)
    {
        dump_event(session, event, origin, params, count);
        return;
    }
    char * msg = irc_color_strip_from_mirc(params[1]);
    QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
    emit ctx->sigPrivateMessage(ctx->decode(origin), ctx->decode(params[0]), ctx->decode(msg));
    free(msg);
}

void QIrc::event_topic(irc_session_t *session, const char * event, const char *origin, const char**params, unsigned int count)
{
    if (count!=2)
    {
        dump_event(session, event, origin, params, count);
        return;
    }
    char * tpc = irc_color_strip_from_mirc(params[1]);
    QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
    emit ctx->sigTopic(ctx->decode(origin), ctx->decode(params[0]), ctx->decode(tpc));
    free(tpc);
}

void QIrc::event_kick(irc_session_t *session, const char * event, const char *origin, const char**params, unsigned int count)
{
    if (count!=3)
    {
        dump_event(session, event, origin, params, count);
        return;
    }
    QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
    emit ctx->sigKick(ctx->decode(origin), ctx->decode(params[0]), ctx->decode(params[1]));
}

void QIrc::event_part(irc_session_t *session, const char * event, const char *origin, const char**params, unsigned int count)
{
    if (count>2)
    {
        dump_event(session, event, origin, params, count);
        return;
    }
    QString reason;
    QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
    if (count==2)
        reason = ctx->decode(params[1]);
    emit ctx->sigChannelParted(ctx->decode(origin), ctx->decode(params[0]), reason);
}

void QIrc::event_mode(irc_session_t *session, const char * event, const char *origin, const char**params, unsigned int count)
{
    if (count<2)
    {
        dump_event(session, event, origin, params, count);
        return;
    }
    QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));

    QStringList mode;
    for(unsigned int i=1; i<count; i++)
    {
        mode << ctx->decode(params[i]);
    }
    emit ctx->sigChannelModeChanged(ctx->decode(origin), ctx->decode(params[0]), mode);
}

void QIrc::event_nick(irc_session_t *session, const char * event, const char *origin, const char**params, unsigned int count)
{
    if (count!=1)
    {
        dump_event(session, event, origin, params, count);
        return;
    }
    QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
    emit ctx->sigNick(ctx->decode(origin), ctx->decode(params[0]));
}

void QIrc::event_quit(irc_session_t *session, const char * event, const char *origin, const char**params, unsigned int count)
{
    if (count!=1)
    {
        dump_event(session, event, origin, params, count);
        return;
    }
    QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
    emit ctx->sigQuit(ctx->decode(origin), ctx->decode(params[0]));
}

void QIrc::event_invite(irc_session_t *session, const char * event, const char *origin, const char**params, unsigned int count)
{
    if (count!=2)
    {
        dump_event(session, event, origin, params, count);
        return;
    }
    QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
    emit ctx->sigInvite(ctx->decode(origin), ctx->decode(params[0]), ctx->decode(params[1]));
}

void QIrc::event_notice(irc_session_t *session, const char * event, const char *origin, const char**params, unsigned int count)
{
    if (count!=2)
    {
        dump_event(session, event, origin, params, count);
        return;
    }
    char * ntc = irc_color_strip_from_mirc(params[1]);
    QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
    emit ctx->sigNotice(ctx->decode(origin), ctx->decode(params[0]), ctx->decode(ntc));
    free(ntc);
}

void QIrc::event_umode(irc_session_t *session, const char * event, const char *origin, const char**params, unsigned int count)
{
    if (count!=1)
    {
        dump_event(session, event, origin, params, count);
        return;
    }
    QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
    emit ctx->sigUmode(ctx->decode(origin), ctx->decode(params[0]));
}

void QIrc::event_ctcp_action(irc_session_t *session, const char * event, const char *origin, const char**params, unsigned int count)
{
    if (count!=2)
    {
        dump_event(session, event, origin, params, count);
        return;
    }
    QIrc *ctx = static_cast<QIrc *>(irc_get_ctx(session));
    emit ctx->sigCtcpAction(ctx->decode(origin), ctx->decode(params[0]), ctx->decode(params[1]));
}
///////////////////////////////////////////////public slots:///////////////////////////////////////////////////////////////////////
bool QIrc::createSession()
{
    memset(&callbacks, 0, sizeof(callbacks));
    setCallbacks();
    session = irc_create_session(&callbacks);
    if (session)
        return true;
    else
        return false;
}

void QIrc::destroySession()
{
    irc_destroy_session(session);
}

void QIrc::connectToServer()
{
    if (irc_is_connected(session) || isRunning())
        return;
    irc_set_ctx(session, this);
    start();
}

void QIrc::disconnectFromServer()
{
    irc_disconnect(session);
}

void QIrc::sendMessage(const QString &receiver, const QString &message)
{
    irc_cmd_msg(session, encode(receiver).data(), encode(message).data());
}

void QIrc::sendMe(const QString &receiver, const QString &message)
{
    irc_cmd_me(session, encode(receiver).data(), encode(message).data());
}

void QIrc::sendNotice(const QString &receiver, const QString &message)
{
    irc_cmd_notice(session, encode(receiver).data(), encode(message).data());
}

void QIrc::joinChannel(const QString &channel, const QString &password)
{
    irc_cmd_join(session, encode(channel).data(), encode(password).data());
}

void QIrc::leaveChannel(const QString &channel)
{
    irc_cmd_part(session, encode(channel).data());
}

void QIrc::getNames(const QString &channel)
{
    irc_cmd_names(session, encode(channel).data());
}

void QIrc::getChannelsList()
{
    irc_cmd_list(session, 0);
}

void QIrc::getTopic(const QString &channel)
{
    irc_cmd_topic(session, encode(channel).data(), 0);
}

void QIrc::setTopic(const QString &channel, const QString &newtopic)
{
    irc_cmd_topic(session, encode(channel).data(), encode(newtopic).data());
}

void QIrc::kick(const QString &channel, const QString &nick, const QString &reason)
{
    irc_cmd_kick(session, encode(nick).data(), encode(channel).data(), encode(reason).data());
}

void QIrc::getChannelMode(const QString &channel)
{
    irc_cmd_channel_mode(session, encode(channel).data(), 0);
}

void QIrc::setChannelMode(const QString &channel, const QString &mode)
{
    irc_cmd_channel_mode(session, encode(channel).data(), encode(mode).data());
}

void QIrc::invite(const QString &channel, const QString &nick)
{
    irc_cmd_invite(session, encode(nick).data(), encode(channel).data());
}

void QIrc::getBans(const QString &channel)
{
    this->sendRaw(QString("MODE %1 +b").arg(channel));
}

void QIrc::quit(const QString &reason)
{
    irc_cmd_quit(session, encode(reason).data());
}

void QIrc::getUserMode()
{
    irc_cmd_user_mode(session, 0);
}

void QIrc::setUserMode(const QString &mode)
{
    irc_cmd_user_mode(session, encode(mode).data());
}

void QIrc::setNick(const QString &nick)
{
    irc_cmd_nick(session, encode(nick).data());
}

void QIrc::whois(const QString &nick)
{
    irc_cmd_whois(session, encode(nick).data());
}

void QIrc::sendRaw(const QString &message)
{
    irc_send_raw(session, encode(message).data());
}

///////////////////////////////////////////////////////////private//////////////////////////////////////////////////////////////////
void QIrc::run()
{
    emit sigMessage("PROGRAM", tr("Connecting to server: %1:%2").arg(server).arg(port));
    int res = irc_connect(session, server.toAscii().data(), port, encode(password).data(), encode(nick).data(), encode(username).data(), encode(realname).data());
    if (res==0)
    {
        irc_run(session);
        irc_disconnect(session);
        emit sigMessage("PROGRAM", tr("Disconnected"));
        return;
    }
    emit sigMessage("PROGRAM", tr("Error occured: %1").arg(irc_strerror(irc_errno(session))));
}

void QIrc::setCallbacks()
{
    callbacks.event_connect = event_connect;
    callbacks.event_join = event_join;
    callbacks.event_nick = event_nick;
    callbacks.event_quit = event_quit;
    callbacks.event_part = event_part;
    callbacks.event_mode = event_mode;
    callbacks.event_topic = event_topic;
    callbacks.event_kick = event_kick;
    callbacks.event_channel = event_channel;
    callbacks.event_privmsg = event_privmsg;
    callbacks.event_notice = event_notice;
    callbacks.event_invite = event_invite;
    callbacks.event_umode = event_umode;

    callbacks.event_ctcp_rep = dump_event;
    callbacks.event_ctcp_action = event_ctcp_action;
    callbacks.event_unknown = dump_event;
    callbacks.event_numeric = event_numeric;
}
