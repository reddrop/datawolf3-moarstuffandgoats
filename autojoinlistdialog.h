#ifndef AUTOJOINLISTDIALOG_H
#define AUTOJOINLISTDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QDialogButtonBox>

class AutoJoinListDialog : public QDialog
{
    Q_OBJECT
    QListWidget *listwidget;
    QPushButton *addbutton, *deletebutton;
    QDialogButtonBox *buttonbox;

public:
    explicit AutoJoinListDialog(QWidget *parent, const QStringList &autojoinlist);
    QStringList getList();

private slots:
    void addPressed();
    void deletePressed();
};

#endif // AUTOJOINLISTDIALOG_H
