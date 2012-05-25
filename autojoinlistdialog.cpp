#include "autojoinlistdialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QInputDialog>

AutoJoinListDialog::AutoJoinListDialog(QWidget *parent, const QStringList &autojoinlist) :
    QDialog(parent)
{
    setWindowTitle(tr("Auto-join channels"));
    listwidget = new QListWidget;
    addbutton = new QPushButton(tr("Add"));
    deletebutton = new QPushButton(tr("Delete"));
    buttonbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
    buttonbox->setCenterButtons(true);

    QHBoxLayout *buttonlayout = new QHBoxLayout;
    buttonlayout->addWidget(addbutton);
    buttonlayout->addWidget(deletebutton);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(listwidget);
    layout->addLayout(buttonlayout);
    layout->addWidget(buttonbox);
    setLayout(layout);

    listwidget->addItems(autojoinlist);

    connect(buttonbox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonbox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(addbutton, SIGNAL(clicked()), this, SLOT(addPressed()));
    connect(deletebutton, SIGNAL(clicked()), this, SLOT(deletePressed()));
}

QStringList AutoJoinListDialog::getList()
{
    QStringList result;
    for (int i=0; i<listwidget->count(); i++)
        result << listwidget->item(i)->text();
    return result;
}


//private slots
void AutoJoinListDialog::addPressed()
{
    QString ch = QInputDialog::getText(this, tr("Add autojoin channel"), tr("&Channel name: ")).toLower();
    if (ch.isEmpty())
        return;
    if (ch.at(0)!=QChar('#'))
        ch.prepend(QChar('#'));
    if (!listwidget->findItems(ch, Qt::MatchExactly).isEmpty())
        return;
    listwidget->addItem(ch);
}

void AutoJoinListDialog::deletePressed()
{
    QList<QListWidgetItem *> selected = listwidget->selectedItems();
    for (int i=0; i<selected.size(); i++)
        delete selected[i];
}
