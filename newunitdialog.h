#ifndef NEWUNITDIALOG_H
#define NEWUNITDIALOG_H

#include <QMainWindow>
#include <datatablewindow.h>

namespace Ui {
class NewUnitDialog;
}

class NewUnitDialog : public QMainWindow
{
    Q_OBJECT

public:
    explicit NewUnitDialog(QWidget *parent = nullptr);
    NewUnitDialog(QWidget *parent = nullptr, QStringList *file_list = nullptr, QString *folder_separator = nullptr);
    ~NewUnitDialog();

private slots:

    void on_cloneCheckBox_toggled(bool checked);

    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

private:
    Ui::NewUnitDialog *ui;
};

#endif // NEWUNITDIALOG_H
