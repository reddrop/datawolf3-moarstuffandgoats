# -------------------------------------------------
# Project created by QtCreator 2010-08-21T00:05:43
# -------------------------------------------------
TARGET = hydrochat
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    settingsdialog.cpp \
    qirc.cpp \
    messagedialog.cpp \
    common.cpp \
    irctextedit.cpp \
    channeltreewidget.cpp \
    channelsettingsdialog.cpp \
    irctextbrowser.cpp \
    channellistdialog.cpp \
    modeparser.cpp \
    soundstable.cpp \
    soundplayer.cpp \
    smilebar.cpp \
    autojoinlistdialog.cpp
HEADERS += mainwindow.h \
    settingsdialog.h \
    qirc.h \
    messagedialog.h \
    common.h \
    irctextedit.h \
    channeltreewidget.h \
    channelsettingsdialog.h \
    irctextbrowser.h \
    channellistdialog.h \
    modeparser.h \
    soundstable.h \
    soundplayer.h \
    smilebar.h \
    autojoinlistdialog.h
FORMS += mainwindow.ui \
    settingsdialog.ui \
    messagedialog.ui \
    channelsettingsdialog.ui \
    channellistdialog.ui

win32-g++:LIBS += libircclient
unix:LIBS += -lircclient

win32-msvc2008 {
LIBS += e:/Temp/src/libircclient-msvc2008-dynamic/Release/libircclient.lib
INCLUDEPATH += e:/Temp/src/libircclient-msvc2008-dynamic/include
}

TRANSLATIONS = hydrochat_ru.ts

OTHER_FILES += \
    icons/tools.png \
    icons/quit.png \
    icons/disconnected.png \
    icons/connecting.png \
    icons/connected.png \
    icons/channels.png \
    icons/channellist.png \
    icons/small/user.png \
    icons/small/channel.png \
    icons/small/op.png \
    icons/small/kick.png \
    icons/small/deop.png \
    icons/small/ban.png \
    icons/qt-logo.png \
    icons/small/privchat.png \
    icons/small/part.png \
    icons/small/info.png \
    icons/small/voice.png \
    icons/small/devoice.png \
    mainicon.ico \
    icons/mainicon.png \
    icon.rc \
    icons/folder.png \
    icons/play.png \
    icons/smile.png \
    emoticons/wink.png \
    emoticons/victory.png \
    emoticons/tremble.png \
    emoticons/tongue.png \
    emoticons/thinking.png \
    emoticons/terror.png \
    emoticons/smirk.png \
    emoticons/sleepy.png \
    emoticons/silly.png \
    emoticons/sick.png \
    emoticons/shut-mouth.png \
    emoticons/shout.png \
    emoticons/sarcastic.png \
    emoticons/sad.png \
    emoticons/rotfl.png \
    emoticons/rose.png \
    emoticons/question.png \
    emoticons/pissed-off.png \
    emoticons/party.png \
    emoticons/musical-note.png \
    emoticons/mail.png \
    emoticons/love.png \
    emoticons/laugh.png \
    emoticons/kiss.png \
    emoticons/hypnotized.png \
    emoticons/handshake.png \
    emoticons/good.png \
    emoticons/go-away.png \
    emoticons/glasses-cool.png \
    emoticons/giggle.png \
    emoticons/freaked-out.png \
    emoticons/fingers-crossed.png \
    emoticons/emoticons.xml \
    emoticons/embarrassed.png \
    emoticons/dont-know.png \
    emoticons/doh.png \
    emoticons/disapointed.png \
    emoticons/devil.png \
    emoticons/dance.png \
    emoticons/cute.png \
    emoticons/curl-lip.png \
    emoticons/crying.png \
    emoticons/cowboy.png \
    emoticons/confused.png \
    emoticons/clap.png \
    emoticons/bomb.png \
    emoticons/beer.png \
    emoticons/arrogant.png \
    emoticons/angel.png \
    emoticons/alien.png \
    icons/mainicon.ico \
    hydrochat_ru.ts \
    hydrochat_ru.qm \
    qt_ru.qm \
    icons/sound.png

RESOURCES += \
    resources.qrc

win32:RC_FILE = icon.rc
