#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include "common.h"

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    void putSettings();
    void getSettings();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SettingsDialog *ui;
    QFont consolefont, chatfont;
    int defaultindex;
    static QString fonttostr(const QFont &font);
    int getCharsetIndex(const QString &charsetname);

private slots:
    void on_pushButtonAutoJoin_clicked();
    void on_toolButtonChatFont_clicked();
    void on_toolButtonConsoleFont_clicked();
};

#endif // SETTINGSDIALOG_H
