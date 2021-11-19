#include "newunitdialog.h"
#include "ui_newunitdialog.h"
#include <QFile>
#include <QDir>
#include <QErrorMessage>

NewUnitDialog::NewUnitDialog(QWidget *parent, QStringList *data_file_list, QString *folder_separator) :
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


void NewUnitDialog::on_buttonBox_accepted()
{
    if (ui->nameLineEdit->text().isEmpty())
    {
        QErrorMessage *e = new QErrorMessage(this);
        e->showMessage("Name cannot be empty.");
    }

    else if (ui->cloneCheckBox->isChecked())
    {
        QString old_file_name = ui->existingListBox->selectedItems()[0]->text();
        QString new_file_name = ui->nameLineEdit->text();

        QFile::copy(QDir::currentPath() + "/data/default/" + old_file_name + ".dat",
                    QDir::currentPath() + "/data/" + new_file_name + ".csv");

        ((DataTableWindow*)parent())->read_file_names();
        ((DataTableWindow*)parent())->load_category_list();

        this->close();
    }
}

