#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QHash>
#include <QSettings>
#include "qirc.h"
#include "settingsdialog.h"
#include "messagedialog.h"
#include "common.h"
#include "channelsettingsdialog.h"
#include "irctextbrowser.h"
#include "channellistdialog.h"
#include <QSystemTrayIcon>
#include <QWidgetAction>
#include "smilebar.h"
#include <QToolButton>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void checkAutoConnect();

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *);

private:
    Ui::MainWindow *ui;
    QTextEdit *consolebrowser;
    QIrc *irc;
    SettingsDialog *setsdlg;
    int consoleindex;
    QHash<QString, IrcTextBrowser *> channelsJoined;
    QHash<QString, MessageDialog *> usersChatting;
    QMenu *usercontextmenu;
    QMenu *channelcontextmenu;
    QMenu *traymenu;
    QMenu *smilemenu;
    QToolButton *smilebutton;
    QWidgetAction *smilewidgetaction;
    SmileBar *smilebar;
    ChannelSettingsDialog *channelsettingsdialog;
    ChannelListDialog *channellistdialog;
    QSystemTrayIcon *trayicon;
    void readSettings();         //SETTINGS!!!
    void writeSettings();         //SETTINGS!!!
    void disconnectFromServer();
    bool okToClose();
    const QString getTopic(const QString &channel);
    void rawMessage(const QString &message);
    void channelMessage(IrcTextBrowser *browser, const QString &channel, const QString &message);
    void setSelectedUserMode(const QString &mode);
    void setTrayIcon(bool is);     //tray icon
    void setConsoleHidden(bool is);
    void checkTextSettings();       //SETTINGS!!! FONTS!!!
    void appendConsole(const QString &text);

private slots:
    void sendPressed();
    void connectPressed();
    void disconnectPressed();
    void settingsPressed();         //SETTINGS!!!
    void joinPressed();
    void privateChatPressed();
    void leaveChannelPressed();
    //management
    void kickPressed();
    void BanPressed();
    void kickBanPressed();
    void opPressed();
    void deOpPressed();
    void userDoubleClicked(const QString &nick);
    void userContextMenuRequested(const QPoint &pos);
    void channelContextMenuRequested(const QPoint &pos);
    void tabCloseRequested(int index);
    void channelSettingsPressed(QString channel = QString());
    void insertUserToEditorPressed();
    void voicePressed();
    void devoicePressed();
    void nickButtonPressed();
    void soundActionToogled(bool is);
    //tray icon
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason = QSystemTrayIcon::Trigger);
    //quit
    void quitProgram();
    void aboutPressed();

    //irc slots
    void sltConnected();
    void sltMessage(const QString &origin, const QString &text);
    void sltChannelJoined(const QString &origin, const QString &channel);
    void sltChannelParted(const QString &origin, const QString &channel, const QString &reason);
    void sltChannelNames(const QString &origin, const QString &channel, const QStringList &names);
    void sltChannelMessage(const QString &origin, const QString &channel, const QString &text);
    void sltPrivateMessage(const QString &origin, const QString &receiver, const QString &text);
    void sltNick(const QString &origin, const QString &newnick);
    void sltTopic(const QString &origin, const QString &channel, const QString &topic);
    void sltTopicSet(const QString &origin, const QString &channel, const QString &user, const QString &date);
    void sltKick(const QString &origin, const QString &channel, const QString &user);
    void sltNotice(const QString &origin, const QString &channel, const QString &text);
    void sltQuit(const QString &origin, const QString &reason);
    void sltChannelModeChanged(const QString &origin, const QString &channel, const QStringList &mode);
    void sltUmode(const QString &origin, const QString &mode);
    void sltInvite(const QString &origin, const QString &nick, const QString &channel);
    void sltCtcpAction(const QString &origin, const QString &channel, const QString &message);
    void sltErrorOccured(const QString &errortext);
};

#endif // MAINWINDOW_H
