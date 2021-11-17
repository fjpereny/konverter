#ifndef NEWUNITDIALOG_H
#define NEWUNITDIALOG_H

#include <QMainWindow>

namespace Ui {
class NewUnitDialog;
}

class NewUnitDialog : public QMainWindow
{
    Q_OBJECT

public:
    explicit NewUnitDialog(QWidget *parent = nullptr);
    NewUnitDialog(QWidget *parent = nullptr, QStringList *file_list = nullptr);
    ~NewUnitDialog();

private slots:

    void on_cloneCheckBox_toggled(bool checked);

    void on_buttonBox_rejected();

private:
    Ui::NewUnitDialog *ui;
};

#endif // NEWUNITDIALOG_H
