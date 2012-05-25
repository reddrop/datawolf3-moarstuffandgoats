#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>
#include <QDateTime>
#include "modeparser.h"
#include "soundplayer.h"
#include <QToolButton>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    readSettings();
    ui->actionSound->setChecked(settings.soundsenabled);

    irc = new QIrc();
    if (!irc->createSession())
    {
        QMessageBox::critical(this, tr("Error"), tr("Can't create session"));
        close();
        return;
    }
    connect(irc, SIGNAL(sigConnected()), this, SLOT(sltConnected()));
    connect(irc, SIGNAL(sigMessage(QString,QString)), this, SLOT(sltMessage(QString,QString)));
    connect(irc, SIGNAL(sigChannelJoined(QString,QString)), this, SLOT(sltChannelJoined(QString,QString)));
    connect(irc, SIGNAL(sigChannelParted(QString,QString,QString)), this, SLOT(sltChannelParted(QString,QString,QString)));
    connect(irc, SIGNAL(sigChannelNames(QString,QString,QStringList)), this, SLOT(sltChannelNames(QString,QString,QStringList)));
    connect(irc, SIGNAL(sigChannelMessage(QString,QString,QString)), this, SLOT(sltChannelMessage(QString,QString,QString)));
    connect(irc, SIGNAL(sigPrivateMessage(QString,QString,QString)), this, SLOT(sltPrivateMessage(QString,QString,QString)));
    connect(irc, SIGNAL(sigNick(QString,QString)), this, SLOT(sltNick(QString,QString)));
    connect(irc, SIGNAL(sigNotice(QString,QString,QString)), this, SLOT(sltNotice(QString,QString,QString)));
    connect(irc, SIGNAL(sigTopic(QString,QString,QString)), this, SLOT(sltTopic(QString,QString,QString)));
    connect(irc, SIGNAL(sigTopicSet(QString,QString,QString,QString)), this, SLOT(sltTopicSet(QString,QString,QString,QString)));
    connect(irc, SIGNAL(sigKick(QString,QString,QString)), this, SLOT(sltKick(QString,QString,QString)));
    connect(irc, SIGNAL(sigQuit(QString,QString)), this, SLOT(sltQuit(QString,QString)));
    connect(irc, SIGNAL(sigChannelModeChanged(QString,QString,QStringList)), this, SLOT(sltChannelModeChanged(QString,QString,QStringList)));
    connect(irc, SIGNAL(sigUmode(QString,QString)), this, SLOT(sltUmode(QString,QString)));
    connect(irc, SIGNAL(sigInvite(QString,QString,QString)), this, SLOT(sltInvite(QString,QString,QString)));
    connect(irc, SIGNAL(sigCtcpAction(QString,QString,QString)), this, SLOT(sltCtcpAction(QString,QString,QString)));
    connect(irc, SIGNAL(sigErrorOccured(QString)), this, SLOT(sltErrorOccured(QString)));

    consolebrowser = new QTextEdit();
    consolebrowser->setReadOnly(true);
    QFont consolefont;
    if (consolefont.fromString(settings.consolefont))
        consolebrowser->setFont(consolefont);

    if (!settings.hideconsole)
        consoleindex =ui->tabWidget->insertTab(0, consolebrowser, tr("Console"));

    setsdlg = 0;

    usercontextmenu = new QMenu(this);
    usercontextmenu->addAction(ui->actionPrivateChat);
    usercontextmenu->addAction(ui->actionInsert_to_editor);
    QMenu *managementmenu = usercontextmenu->addMenu(tr("Management"));
    managementmenu->addAction(ui->actionKick);
    managementmenu->addAction(ui->actionBan);
    managementmenu->addAction(ui->actionKickBan);
    managementmenu->addSeparator();
    managementmenu->addAction(ui->actionOp);
    managementmenu->addAction(ui->actionDeOp);
    managementmenu->addSeparator();
    managementmenu->addAction(ui->actionVoice);
    managementmenu->addAction(ui->actiondeVoice);
    connect(ui->treeWidget, SIGNAL(userContextMenu(QPoint)), this, SLOT(userContextMenuRequested(QPoint)));

    channelcontextmenu = new QMenu(this);
    channelcontextmenu->addAction(ui->actionLeave_channel);
    channelcontextmenu->addAction(ui->actionChannel_settings);
    connect(ui->treeWidget, SIGNAL(channelContextMenu(QPoint)), this, SLOT(channelContextMenuRequested(QPoint)));

    channelsettingsdialog = 0;
    trayicon = 0;
    traymenu = 0;
    setTrayIcon(settings.trayicon);
    connect(ui->actionChannel_settings, SIGNAL(triggered()), this, SLOT(channelSettingsPressed()));

    //smile menu
    smilemenu = new QMenu(this);
    smilewidgetaction = new QWidgetAction(smilemenu);
    smilebar = new SmileBar();
    smilewidgetaction->setDefaultWidget(smilebar);
    smilemenu->addAction(smilewidgetaction);
    connect(smilebar, SIGNAL(smileClicked(QString)), ui->plainTextEditMessage, SLOT(insertPlainText(QString)));
    connect(smilebar, SIGNAL(smileClicked(QString)), ui->plainTextEditMessage, SLOT(setFocus()));
    ui->actionEmoticons->setMenu(smilemenu);
    smilebutton = new QToolButton();
    smilebutton->setDefaultAction(ui->actionEmoticons);
    smilebutton->setPopupMode(QToolButton::InstantPopup);
    ui->mainToolBar->addWidget(smilebutton);

    channellistdialog = new ChannelListDialog();
    connect(irc, SIGNAL(sigChannelListStart()), channellistdialog, SLOT(showAndClear()));
    connect(irc, SIGNAL(sigChannelListAddItem(QString,QString,QString)), channellistdialog,
            SLOT(addListItem(QString,QString,QString)));
    connect(irc, SIGNAL(sigChannelListEnd()), channellistdialog, SLOT(endOfList()));
    connect(channellistdialog, SIGNAL(channelDoubleClicked(QString)), irc, SLOT(joinChannel(QString)));

    connect(ui->pushButtonSend, SIGNAL(clicked()), ui->plainTextEditMessage, SIGNAL(sendKeyPressed()));
    connect(ui->plainTextEditMessage, SIGNAL(sendKeyPressed()), this, SLOT(sendPressed()));
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(connectPressed()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(disconnectPressed()));
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(settingsPressed()));
    connect(ui->actionJoin, SIGNAL(triggered()), this, SLOT(joinPressed()));
    connect(ui->treeWidget, SIGNAL(userDoubleClicked(QString)), this, SLOT(userDoubleClicked(QString)));
    connect(ui->actionPrivateChat, SIGNAL(triggered()), this, SLOT(privateChatPressed()));
    connect(ui->actionLeave_channel, SIGNAL(triggered()), this, SLOT(leaveChannelPressed()));
    connect(ui->actionKick, SIGNAL(triggered()), this, SLOT(kickPressed()));
    connect(ui->actionBan, SIGNAL(triggered()), this, SLOT(BanPressed()));
    connect(ui->actionKickBan, SIGNAL(triggered()), this, SLOT(kickBanPressed()));
    connect(ui->actionOp, SIGNAL(triggered()), this, SLOT(opPressed()));
    connect(ui->actionDeOp, SIGNAL(triggered()), this, SLOT(deOpPressed()));
    connect(ui->actionVoice, SIGNAL(triggered()), this, SLOT(voicePressed()));
    connect(ui->actiondeVoice, SIGNAL(triggered()), this, SLOT(devoicePressed()));
    connect(ui->pushButtonNick, SIGNAL(clicked()), this, SLOT(nickButtonPressed()));
    connect(ui->actionList_of_channels, SIGNAL(triggered()), irc, SLOT(getChannelsList()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequested(int)));
    connect(ui->actionInsert_to_editor, SIGNAL(triggered()), this, SLOT(insertUserToEditorPressed()));
    connect(ui->actionShowHide, SIGNAL(triggered()), this, SLOT(trayIconActivated()));
    connect(ui->actionQuit_program, SIGNAL(triggered()), this, SLOT(quitProgram()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(aboutPressed()));
    connect(ui->actionSound, SIGNAL(toggled(bool)), this, SLOT(soundActionToogled(bool)));
}

MainWindow::~MainWindow()
{
    delete consolebrowser;
    delete smilebar;
    delete smilewidgetaction;
    delete smilebutton;
    delete channellistdialog;
    delete ui;
    QHashIterator<QString, MessageDialog *> chats(usersChatting);
    while(chats.hasNext())
    {
        chats.next();
        MessageDialog *dlg = chats.value();
        dlg->close();
        delete dlg;
    }
    usersChatting.clear();
}

void MainWindow::checkAutoConnect()
{
    if (settings.autoConnect)
        connectPressed();
}

//////////////////////////////////////////////protected//////////////////////////////////////////////////////////////////
void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (settings.trayicon && settings.minimizeonclose)
    {
        hide();
        ui->actionShowHide->setText(tr("Show"));
        event->ignore();
        return;
    }
    if (okToClose())
    {
        writeSettings();
        disconnectFromServer();
        qApp->quit();
        return;
    }
    event->ignore();
}

//////////////////////////////////////////////private//////////////////////////////////////////////////////////////////

void MainWindow::readSettings()
{
    QSettings qsets(QSettings::IniFormat, QSettings::UserScope, "hydrochat", "hydrochat", this);
    qsets.beginGroup("window");
    restoreGeometry(qsets.value("geometry").toByteArray());
    restoreState(qsets.value("state").toByteArray());
    if (qsets.value("ismaximized", false).toBool())
    {
        setWindowState(Qt::WindowMaximized);
    }
    //spl
    QByteArray spstate = qsets.value("splitter").toByteArray();
    if (spstate.isEmpty())
    {
        QList<int> sizes;
        sizes << 500 << 140;
        ui->splitter->setSizes(sizes);
    }
    else
    {
        ui->splitter->restoreState(spstate);
    }
    qsets.endGroup();

    qsets.beginGroup("irc");
    settings.nick = qsets.value("nick", QString("nick")).toString();
    settings.username = qsets.value("username", QString("username")).toString();
    settings.realname = qsets.value("realname", "realname").toString();
    settings.password = qsets.value("password", QString()).toString();
    settings.server = qsets.value("server").toString();
    settings.port = qsets.value("port", 6667).toUInt()  % 65536;
    settings.charset = qsets.value("charset", "Windows-1251").toString();
    settings.autoConnect = qsets.value("autoconnect", false).toBool();
    settings.autojoinlist = qsets.value("autojoinlist", QStringList()).toStringList();
    qsets.endGroup();

    qsets.beginGroup("program");
    settings.hideconsole = qsets.value("hideconsole", false).toBool();
    settings.trayicon = qsets.value("trayicon", false).toBool();
    settings.minimizeonclose = qsets.value("minimizeonclose", false).toBool();
    settings.consolefont = qsets.value("consolefont", QFont().toString()).toString();
    settings.chatfont = qsets.value("chatfont", QFont().toString()).toString();
    settings.timestamps = qsets.value("timestamps", true).toBool();
    settings.timestampformat = qsets.value("timestampformat", QString("[hh:mm:ss]")).toString();
    qsets.endGroup();

    qsets.beginGroup("sound");
    settings.soundsenabled = qsets.value("soundsenabled", false).toBool();
    settings.loadList(qsets.value("soundlist", QStringList()).toStringList());
    qsets.endGroup();

    qsets.beginGroup("other");
    ui->plainTextEditMessage->setHistory(qsets.value("history", QStringList()).toStringList());
    qsets.endGroup();
}

void MainWindow::writeSettings()
{
    QSettings qsets(QSettings::IniFormat, QSettings::UserScope, "hydrochat", "hydrochat", this);
    qsets.beginGroup("window");
    qsets.setValue("geometry", saveGeometry());
    qsets.setValue("state", saveState());
    qsets.setValue("ismaximized", (windowState()==Qt::WindowMaximized));
    qsets.setValue("splitter", ui->splitter->saveState());
    qsets.endGroup();

    qsets.beginGroup("irc");
    qsets.setValue("nick", settings.nick);
    qsets.setValue("username", settings.username);
    qsets.setValue("realname", settings.realname);
    qsets.setValue("password", settings.password);
    qsets.setValue("server", settings.server);
    qsets.setValue("port", settings.port);
    qsets.setValue("charset", settings.charset);
    qsets.setValue("autoconnect", settings.autoConnect);
    qsets.setValue("autojoinlist", settings.autojoinlist);
    qsets.endGroup();

    qsets.beginGroup("program");
    qsets.setValue("hideconsole", settings.hideconsole);
    qsets.setValue("trayicon", settings.trayicon);
    qsets.setValue("minimizeonclose", settings.minimizeonclose);
    qsets.setValue("consolefont", settings.consolefont);
    qsets.setValue("chatfont", settings.chatfont);
    qsets.setValue("timestamps", settings.timestamps);
    qsets.setValue("timestampformat", settings.timestampformat);
    qsets.endGroup();

    qsets.beginGroup("sound");
    qsets.setValue("soundsenabled", settings.soundsenabled);
    qsets.setValue("soundlist", settings.saveList());
    qsets.endGroup();

    qsets.beginGroup("other");
    qsets.setValue("history", ui->plainTextEditMessage->history());
    qsets.endGroup();
}

void MainWindow::disconnectFromServer()
{
    irc->disconnectFromServer();

    ui->treeWidget->clearAllChannels();

    QHashIterator<QString, IrcTextBrowser *> chans(channelsJoined);
    while (chans.hasNext())
    {
        chans.next();
        IrcTextBrowser *browser = chans.value();
        int idx = ui->tabWidget->indexOf(browser);
        ui->tabWidget->removeTab(idx);
        delete browser;
    }
    channelsJoined.clear();

    QHashIterator<QString, MessageDialog *> chats(usersChatting);
    while(chats.hasNext())
    {
        chats.next();
        MessageDialog *dlg = chats.value();
        dlg->close();
        delete dlg;
    }
    usersChatting.clear();
}

bool MainWindow::okToClose()
{
    return (QMessageBox::question(this, tr("Warning"), tr("Do you really want to close hydrochat?"),
                                  QMessageBox::Yes | QMessageBox::No)==QMessageBox::Yes);
}

const QString MainWindow::getTopic(const QString &channel)
{
    IrcTextBrowser * browser = channelsJoined.value(channel);
    if (browser)
        return browser->topic();
    else
        return QString();
}

void MainWindow::rawMessage(const QString &message)
{
    QString msg(message);
    if (msg[0]==QChar('/'))
    {
        if (msg.left(4).compare(QString("/msg"), Qt::CaseInsensitive)==0)
        {
            msg.remove(0, 4);
            while (msg[0]==QChar(' '))
                msg.remove(0, 1);
            int sp = msg.indexOf(QChar(' '));
            if (sp<0)
                return;
            QString receiver = msg.left(sp);
            msg.remove(0, sp+1);
            irc->sendMessage(receiver, msg);
            return;
        }
        if (msg.left(6).compare(QString("/query"), Qt::CaseInsensitive)==0)
        {
            msg.remove(0, 6);
            QStringList msl = msg.simplified().split(QChar(' '), QString::SkipEmptyParts, Qt::CaseInsensitive);
            if (!msl.isEmpty())
                userDoubleClicked(msl.at(0));
            return;
        }
        msg.remove(0, 1);
    }
    irc->sendRaw(msg);
}

void MainWindow::channelMessage(IrcTextBrowser *browser, const QString &channel, const QString &message)
{
    if (channel[0]!=QChar('#'))
    {
        rawMessage(message);
        return;
    }
    QString msg(message);
    if (msg[0]==QChar('/'))
    {
        if (msg.left(3).compare(QString("/me"), Qt::CaseInsensitive)==0)
        {
            msg.remove(0, 3);
            while (msg[0]==QChar(' '))
                msg.remove(0, 1);
            irc->sendMe(channel, msg);
            browser->append(QString("%1 %2").arg(currentnick).arg(msg));
            return;
        }
        rawMessage(msg);
        return;
    }
    irc->sendMessage(channel, msg);
    //browser->appendMessage(currentnick, smilebar->parseSmiles(msg));
    smilebar->printParsedMessage(browser->textbrowser(), currentnick, msg);
}

void MainWindow::setSelectedUserMode(const QString &mode)
{
    QString user = ui->treeWidget->selectedUser();
    if (user.isEmpty())
        return;
    QString channel = ui->treeWidget->selectedChannel();
    if (channel.isEmpty() || (channel[0]!=QChar('#')))
        return;
    irc->setChannelMode(channel, QString("%1 %2").arg(mode).arg(user));
}

void MainWindow::setTrayIcon(bool is)
{
    if (is)
    {
        if (!trayicon)
        {
            trayicon = new QSystemTrayIcon(QIcon(":/icons/mainicon.png"), this);
            connect(trayicon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
                    SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
            connect(qApp, SIGNAL(aboutToQuit()), trayicon, SLOT(hide()));
            traymenu = new QMenu(this);
            traymenu->addAction(ui->actionShowHide);
            traymenu->addSeparator();
            traymenu->addAction(ui->actionConnect);
            traymenu->addAction(ui->actionDisconnect);
            traymenu->addSeparator();
            traymenu->addAction(ui->actionQuit_program);
            trayicon->setContextMenu(traymenu);
            trayicon->show();
        }
    }
    else
    {
        if (trayicon)
        {
            trayicon->hide();
            delete traymenu;
            delete trayicon;
            traymenu = 0;
            trayicon = 0;
        }
    }
}

void MainWindow::setConsoleHidden(bool is)
{
    if (is)
    {
        if (ui->tabWidget->tabText(0).at(0)!=QChar('#'))
            ui->tabWidget->removeTab(0);
    } else
    {
        if (ui->tabWidget->count()==0)
            consoleindex =ui->tabWidget->insertTab(0, consolebrowser, tr("Console"));
        else if (ui->tabWidget->tabText(0).at(0)==QChar('#'))
            consoleindex =ui->tabWidget->insertTab(0, consolebrowser, tr("Console"));
    }
}

void MainWindow::checkTextSettings()
{
    QFont consfont;
    if (consfont.fromString(settings.consolefont))
    {
        if (consfont!=consolebrowser->font())
            consolebrowser->setFont(consfont);
    }
    QFont textfont;
    if (textfont.fromString(settings.chatfont))
    {
        QHashIterator<QString, IrcTextBrowser *> chans(channelsJoined);
        while (chans.hasNext())
        {
            chans.next();
            IrcTextBrowser * brwsr = chans.value();
            brwsr->setFont(textfont);
        }

        QHashIterator<QString, MessageDialog *> chats(usersChatting);
        while (chats.hasNext())
        {
            chats.next();
            MessageDialog *msgdlg = chats.value();
            msgdlg->setFont(textfont);
        }
    }
}

void MainWindow::appendConsole(const QString &text)
{
    if (settings.timestamps)
        consolebrowser->append(QString("%1 %2").arg(QDateTime::currentDateTime().toString(settings.timestampformat)).arg(text));
    else
        consolebrowser->append(text);
}

//////////////////////////////////////////////private slots////////////////////////////////////////////////////////////
void MainWindow::sendPressed()
{
    if (!irc)
        return;
    if (!irc->isConnected())
        return;
    QString fullmessage = ui->plainTextEditMessage->toPlainText();
    if (fullmessage.isEmpty())
        return;
    playSound(SND_SENDMSG);
    ui->plainTextEditMessage->storeLastMessage();
    ui->plainTextEditMessage->clear();
    QStringList messages = fullmessage.split("\n", QString::SkipEmptyParts);

    QWidget *wgt = ui->tabWidget->currentWidget();
    if (!wgt)
    {
        for (int i=0; i<messages.size(); i++)
            rawMessage(messages.at(i));
        return;
    }
    IrcTextBrowser * browser= static_cast<IrcTextBrowser *>(wgt);
    const QString ch = ui->tabWidget->tabText(ui->tabWidget->currentIndex());

    for (int i=0; i<messages.size(); i++)
        channelMessage(browser, ch, messages.at(i));
}

void MainWindow::connectPressed()
{
    if (!irc)
        return;
    if (irc->isConnected())
    {
        sltMessage("PROGRAM", tr("Already connected"));
        return;
    }
    if (irc->isRunning())
    {
        sltMessage("PROGRAM", tr("Connection is establishing"));
        return;
    }
    irc->setData(settings.server, settings.port, settings.nick, settings.username, settings.realname, settings.password, settings.charset);
    irc->connectToServer();
    currentnick = settings.nick;
    ui->pushButtonNick->setText(settings.nick);
    setWindowTitle(QString("%1:%2 - %3").arg(settings.server).arg(settings.port).arg("Hydrochat"));
}

void MainWindow::disconnectPressed()
{
    if (irc->isConnected() || irc->isRunning())
    {
        if (irc->isConnected())
            playSound(SND_QUIT);
        disconnectFromServer();
        setWindowTitle(tr("Hydrochat"));
        return;
    }
    sltMessage("PROGRAM", tr("Not connected"));
}

void MainWindow::settingsPressed()
{
    if (!setsdlg)
    {
        setsdlg = new SettingsDialog(this);
        setsdlg->setModal(true);
    }
    setsdlg->putSettings();
    if (setsdlg->exec()==QDialog::Accepted)
    {
        setsdlg->getSettings();
        writeSettings();
        setConsoleHidden(settings.hideconsole);
        setTrayIcon(settings.trayicon);
        ui->actionSound->setChecked(settings.soundsenabled);
        checkTextSettings();
    }
}

void MainWindow::joinPressed()
{
    if (!irc->isConnected())
        return;
    bool ok;
    QString chan = QInputDialog::getText(this, tr("Join a channel"), tr("Please, type channel name"), QLineEdit::Normal, QString("#"), &ok);
    if ((!chan.isEmpty()) && ok)
    {
        if (chan[0]!=QChar('#'))
            chan.prepend(QChar('#'));
        irc->joinChannel(chan);
    }
}

void MainWindow::privateChatPressed()
{
    QString user = ui->treeWidget->selectedUser();
    if (!user.isEmpty())
        userDoubleClicked(user);
}

void MainWindow::leaveChannelPressed()
{
    QString channel = ui->treeWidget->selectedChannel();
    if (!channel.isEmpty())
        irc->leaveChannel(channel);
}

//management
void MainWindow::kickPressed()
{
    QString user = ui->treeWidget->selectedUser();
    if (user.isEmpty())
        return;
    QString channel = ui->treeWidget->selectedChannel();
    if (channel.isEmpty() || (channel[0]!=QChar('#')))
        return;
    irc->kick(channel, user, QString("Kicked"));
}

void MainWindow::BanPressed()
{
    QString user = ui->treeWidget->selectedUser();
    if (user.isEmpty())
        return;
    QString channel = ui->treeWidget->selectedChannel();
    if (channel.isEmpty() || (channel[0]!=QChar('#')))
        return;

    bool ok;
    QString banstring = QInputDialog::getText(this, tr("Ban"),tr("Input ban mask"), QLineEdit::Normal, QString("%1!*@*").arg(user), &ok);
    if ((!banstring.isEmpty()) && ok)
        irc->setChannelMode(channel, QString("+b %1").arg(banstring));
}

void MainWindow::kickBanPressed()
{
    QString user = ui->treeWidget->selectedUser();
    if (user.isEmpty())
        return;
    QString channel = ui->treeWidget->selectedChannel();
    if (channel.isEmpty() || (channel[0]!=QChar('#')))
        return;

    bool ok;
    QString banstring = QInputDialog::getText(this, tr("Ban"),tr("Input ban mask"), QLineEdit::Normal, QString("%1!*@*").arg(user), &ok);
    if ((!banstring.isEmpty()) && ok)
    {
        irc->setChannelMode(channel, QString("+b %1").arg(banstring));
        irc->kick(channel, user, QString("Kicked"));
    }
}


void MainWindow::opPressed()
{
    setSelectedUserMode("+o");
}

void MainWindow::deOpPressed()
{
    setSelectedUserMode("-o");
}

void MainWindow::userDoubleClicked(const QString &nick)
{
    if (!usersChatting.contains(nick))
    {
        MessageDialog *msgdlg = new MessageDialog(nick, 0);
        connect(msgdlg, SIGNAL(messageReady(QString,QString)), irc, SLOT(sendMessage(QString,QString)));

        QFont chatfont;
        if (chatfont.fromString(settings.chatfont))
            msgdlg->setFont(chatfont);

        msgdlg->show();
        usersChatting.insert(nick, msgdlg);
    }

    MessageDialog *msgdlg = usersChatting.value(nick);
    if (msgdlg)
        msgdlg->show();
}

void MainWindow::userContextMenuRequested(const QPoint &pos)
{
    QString user = ui->treeWidget->selectedUser();
    if (user.isEmpty())
        return;
    ui->actionInsert_to_editor->setText(tr("Insert %1: to editor").arg(user));
    usercontextmenu->exec(pos);
}

void MainWindow::channelContextMenuRequested(const QPoint &pos)
{
    channelcontextmenu->exec(pos);
}

void MainWindow::tabCloseRequested(int index)
{
    if ((index==0) && (ui->tabWidget->tabText(0)[0]!=QChar('#')))
    {
        close();
        return;
    }
    if (!irc->isConnected())
    {
        ui->tabWidget->removeTab(index);
        return;
    }
    QString channel = ui->tabWidget->tabText(index);
    if (!channel.isEmpty())
        irc->leaveChannel(channel);
}

void MainWindow::channelSettingsPressed(QString channel)
{
    if (channel.isEmpty())
        channel = ui->treeWidget->selectedChannel();
    if (!channelsettingsdialog)
    {
        channelsettingsdialog = new ChannelSettingsDialog(this);
        connect(irc, SIGNAL(sigBanList(QString,QString,QString,QString)), channelsettingsdialog,
                SLOT(bansReceived(QString,QString,QString,QString)));
        connect(irc, SIGNAL(sigChannelMode(QString,QStringList)), channelsettingsdialog,
                SLOT(channelModeReceived(QString,QStringList)));
        connect(channelsettingsdialog, SIGNAL(updatePressed(QString)), irc,
                SLOT(getBans(QString)));
        connect(channelsettingsdialog, SIGNAL(channelModeChanged(QString,QString)), irc,
                SLOT(setChannelMode(QString,QString)));
        connect(channelsettingsdialog, SIGNAL(topicChanged(QString,QString)), irc,
                SLOT(setTopic(QString,QString)));
    }
    channelsettingsdialog->clearItems();
    channelsettingsdialog->channel = channel;
    channelsettingsdialog->setWindowTitle(tr("%1 settings").arg(channel));
    channelsettingsdialog->setTopicText(getTopic(channelsettingsdialog->channel));
    channelsettingsdialog->show();
    irc->getBans(channelsettingsdialog->channel);
    irc->getChannelMode(channelsettingsdialog->channel);
}

void MainWindow::insertUserToEditorPressed()
{
    QString user = ui->treeWidget->selectedUser();
    if (user.isEmpty())
        return;
    ui->plainTextEditMessage->insertPlainText(QString("%1: ").arg(user));
}

void MainWindow::voicePressed()
{
    setSelectedUserMode("+v");
}

void MainWindow::devoicePressed()
{
    setSelectedUserMode("-v");
}

void MainWindow::nickButtonPressed()
{
    if (!irc->isConnected())
        return;
    bool ok;
    QString newnick = QInputDialog::getText(this, tr("Enter new nick"), tr("&New nick: "), QLineEdit::Normal, currentnick, &ok);
    if (ok)
        irc->setNick(newnick);
}

void MainWindow::soundActionToogled(bool is)
{
    settings.soundsenabled = is;
}

///////////tray icon////////////////
void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason==QSystemTrayIcon::Trigger)
    {
        if (isHidden())
        {
            ui->actionShowHide->setText(tr("Hide"));
            show();
            raise();
            activateWindow();
        } else
        {
            ui->actionShowHide->setText(tr("Show"));
            hide();
        }
    }
}

void MainWindow::quitProgram()
{
    if (!okToClose())
        return;
    writeSettings();
    disconnectFromServer();
    qApp->quit();
}

void MainWindow::aboutPressed()
{
    QMessageBox::about(this, tr("About program"), tr("Hydrochat ver. 1.1 beta\n"
                                                     "Copyright (C) 2010-2011 Ivan Ponomarev <stiff.ru@gmail.com>\n"
                                                     "This program is free software, it is distributed under GNU GPL v2 licence"));
}

//irc slots
void MainWindow::sltConnected()
{
    foreach (QString channel, settings.autojoinlist)
    {
        irc->joinChannel(channel);
    }
}

void MainWindow::sltMessage(const QString &origin, const QString &text)
{
    //consolebrowser->append(QString("<%1> %2").arg(origin).arg(text));
    appendConsole(QString("<%1> %2").arg(origin).arg(text));
}

void MainWindow::sltChannelJoined(const QString &origin, const QString &channel)
{
    playSound(SND_JOINED);
    if (!channelsJoined.contains(channel))
    {
        IrcTextBrowser *browser = new IrcTextBrowser(channel);
        connect(browser, SIGNAL(userNameClicked(QString)), ui->plainTextEditMessage, SLOT(insertPlainText(QString)));
        connect(browser, SIGNAL(userNameClicked(QString)), ui->plainTextEditMessage, SLOT(setFocus()));
        connect(browser, SIGNAL(browserDoubleClicked(QString)), this, SLOT(channelSettingsPressed(QString)));
        connect(browser, SIGNAL(mouseReleased()), ui->plainTextEditMessage, SLOT(setFocus()));
        QFont chatfont;
        if (chatfont.fromString(settings.chatfont))
            browser->setFont(chatfont);
        ui->tabWidget->addTab(browser, channel);
        channelsJoined.insert(channel, browser);
        ui->treeWidget->addChannel(channel);
        ui->tabWidget->setCurrentWidget(browser);
    }

    IrcTextBrowser *browser = channelsJoined.value(channel);
    if (browser)
    {
        browser->append(tr("%1 has joined %2").arg(parseNick(origin)).arg(channel));
        if (parseNick(origin)!=currentnick)
            ui->treeWidget->addChannelName(channel, parseNick(origin));
    }
}

void MainWindow::sltChannelParted(const QString &origin, const QString &channel, const QString &reason)
{
    if (!channelsJoined.contains(channel))
        return;

    playSound(SND_PARTED);

    const QString &nick = parseNick(origin);

    IrcTextBrowser *browser = channelsJoined.value(channel);
    if (!browser)
        return;

    if (nick==currentnick)
    {
        int index = ui->tabWidget->indexOf(browser);
        ui->tabWidget->removeTab(index);
        delete browser;
        channelsJoined.remove(channel);
        ui->treeWidget->deleteChannel(channel);
    }
    else
    {
        browser->append(tr("%1 has leaved %2 %3").arg(nick).arg(channel).arg(reason));
        ui->treeWidget->deleteChannelName(channel, nick);
    }
}

void MainWindow::sltChannelNames(const QString &, const QString &channel, const QStringList &names)
{
    ui->treeWidget->setChannelNames(channel, names);
    ui->treeWidget->expandChannel(channel);
}

void MainWindow::sltChannelMessage(const QString &origin, const QString &channel, const QString &text)
{
    IrcTextBrowser * browser = channelsJoined.value(channel);
    if (!browser)
        return;
    if (origin.isEmpty())
        browser->append(text);
    else
    {
        playSound(SND_CHANNELMESSAGE);
        smilebar->printParsedMessage(browser->textbrowser(), parseNick(origin), text);
    }
}

void MainWindow::sltPrivateMessage(const QString &origin, const QString &, const QString &text)
{
    playSound(SND_PRIVATEMESSAGE);
    const QString nick = parseNick(origin);
    if (!usersChatting.contains(nick))
    {
        MessageDialog *msgdlg = new MessageDialog(nick, 0);
        connect(msgdlg, SIGNAL(messageReady(QString,QString)), irc, SLOT(sendMessage(QString,QString)));

        QFont chatfont;
        if (chatfont.fromString(settings.chatfont))
            msgdlg->setFont(chatfont);

        msgdlg->show();
        usersChatting.insert(nick, msgdlg);
    }

    MessageDialog *msgdlg = usersChatting.value(nick);
    if (msgdlg)
    {
        msgdlg->messageReceived(text);
    }
}

void MainWindow::sltNick(const QString &origin, const QString &newnick)
{
    const QString oldnick = parseNick(origin);
    ui->treeWidget->changeNick(oldnick, newnick);

    const QStringList channels = ui->treeWidget->channelsOfNick(newnick);
    for (int i=0; i<channels.size(); i++)
    {
        IrcTextBrowser *browser = channelsJoined.value(channels.at(i));
        if (browser)
            browser->append(tr("* %1 changed nick to %2").arg(parseNick(oldnick)).arg(newnick));
    }

    if (oldnick==currentnick)
    {
        currentnick = newnick;
        ui->pushButtonNick->setText(newnick);
    }
}

void MainWindow::sltTopic(const QString &origin, const QString &channel, const QString &topic)
{
    IrcTextBrowser * browser = channelsJoined.value(channel);
    if (browser)
    {
        browser->setTopic(topic);
        browser->append(tr("* %1 has changed topic to %2").arg(parseNick(origin)).arg(topic));
    }
}

void MainWindow::sltTopicSet(const QString &, const QString &channel, const QString &user, const QString &date)
{
    IrcTextBrowser * browser = channelsJoined.value(channel);
    if (browser)
    {
        bool ok = false;
        unsigned int timet = date.toInt(&ok);
        if (!ok)
            return;
        QString time = QDateTime::fromTime_t(timet).toString();
        browser->append(tr("* Topic was set %1 by %2").arg(parseNick(time)).arg(user));
    }
}

void MainWindow::sltKick(const QString &origin, const QString &channel, const QString &user)
{
    IrcTextBrowser * browser = channelsJoined.value(channel);
    if (!browser)
        return;
    playSound(SND_KICK);
    if (user==currentnick)
    {
        int index = ui->tabWidget->indexOf(browser);
        ui->tabWidget->removeTab(index);
        delete browser;
        channelsJoined.remove(channel);
        ui->treeWidget->deleteChannel(channel);
        //consolebrowser->append(tr("* You was kicked from channel %1 by %2").arg(channel).arg(parseNick(origin)));
        appendConsole(tr("* You was kicked from channel %1 by %2").arg(channel).arg(parseNick(origin)));
        return;
    }
    browser->append(tr("* %1 has kicked %2 from %3").arg(parseNick(origin)).arg(user).arg(channel));
    ui->treeWidget->deleteChannelName(channel, user);
}

void MainWindow::sltNotice(const QString &origin, const QString &, const QString &text)
{
    playSound(SND_NOTICE);
    //consolebrowser->append(tr("* <%1> %2").arg(parseNick(origin)).arg(text));
    appendConsole(tr("* <%1> %2").arg(parseNick(origin)).arg(text));
}

void MainWindow::sltQuit(const QString &origin, const QString &reason)
{
    playSound(SND_QUIT);
    const QString nick = parseNick(origin);
    const QStringList channels = ui->treeWidget->channelsOfNick(nick);
    for (int i=0; i<channels.size(); i++)
    {
        IrcTextBrowser *browser = channelsJoined.value(channels.at(i));
        if (browser)
            browser->append(tr("%1 has quit IRC: %2").arg(nick).arg(reason));
    }

    ui->treeWidget->deleteChannelName(nick);
}

void MainWindow::sltChannelModeChanged(const QString &origin, const QString &channel, const QStringList &mode)
{
    QString nick = parseNick(origin);
    IrcTextBrowser * browser = channelsJoined.value(channel);
    if (browser)
        browser->append(tr("* %1 has changed mode: %2").arg(nick).arg(mode.join(" ")));

    ModeParser parser(mode);
    if (parser.hasMode("+o"))
    {
        QStringList args = parser.argsOfMode("+o");
        foreach (QString arg, args)
        {
            ui->treeWidget->setOper(channel, arg, true);
            browser->append(tr("* %1 (%2) has given operator privilegies to %3").arg(nick).arg(origin).arg(arg));
        }
    }
    if (parser.hasMode("-o"))
    {
        QStringList args = parser.argsOfMode("-o");
        foreach (QString arg, args)
        {
            ui->treeWidget->setOper(channel, arg, false);
            browser->append(tr("* %1 (%2) has taken operator privilegies from %3").arg(nick).arg(origin).arg(arg));
        }
    }
}

void MainWindow::sltUmode(const QString &origin, const QString &mode)
{
    //consolebrowser->append(tr("* mode of user %1 is changed: %2").arg(parseNick(origin)).arg(mode));
    appendConsole(tr("* mode of user %1 is changed: %2").arg(parseNick(origin)).arg(mode));
}

void MainWindow::sltInvite(const QString &origin, const QString &nick, const QString &channel)
{
    //consolebrowser->append(tr("* %1 invites %2 to channel %3").arg(parseNick(origin)).arg(nick).arg(channel));
    appendConsole(tr("* %1 invites %2 to channel %3").arg(parseNick(origin)).arg(nick).arg(channel));
}

void MainWindow::sltCtcpAction(const QString &origin, const QString &channel, const QString &message)
{
    IrcTextBrowser * browser = channelsJoined.value(channel);
    if (browser)
        browser->append(QString("%1 %2").arg(parseNick(origin)).arg(message));
}

void MainWindow::sltErrorOccured(const QString &errortext)
{
   //consolebrowser->append(tr("Error occured: %1").arg(errortext));
   appendConsole(tr("Error occured: %1").arg(errortext));
   disconnectFromServer();
}
