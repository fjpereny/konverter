#include "newunitdialog.h"
#include "ui_newunitdialog.h"

NewUnitDialog::NewUnitDialog(QWidget *parent, QStringList *data_file_list) :
    QMainWindow(parent),
    ui(new Ui::NewUnitDialog)
{
    ui->setupUi(this);


    for (int i=0; i<data_file_list->count(); i++)
    {
        QStringList file_name_extension = data_file_list->at(i).split(".");
        if (file_name_extension[1] == "dat")
        {
            ui->existingListBox->addItem(file_name_extension[0]);
        }
        else if (file_name_extension[1] == "csv")
        {
            ui->existingListBox->addItem(file_name_extension[0] + " (Custom)");
        }
    }
}

NewUnitDialog::~NewUnitDialog()
{
    delete ui;
}


void NewUnitDialog::on_cloneCheckBox_toggled(bool checked)
{
    if (checked)
    {
        ui->existingListBox->setEnabled(true);
        ui->masterLineEdit->setEnabled(false);
        ui->masterLineEdit->clear();
    }
    else
    {
        ui->existingListBox->setEnabled(false);
        ui->masterLineEdit->setEnabled(true);
    }
}


void NewUnitDialog::on_buttonBox_rejected()
{
    this->close();
}

