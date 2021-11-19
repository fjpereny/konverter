#include "newunitdialog.h"
#include "ui_newunitdialog.h"
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <iostream>

NewUnitDialog::NewUnitDialog(QWidget *parent, QStringList *data_file_list, const QString *folder_separator) :
    QMainWindow(parent),
    ui(new Ui::NewUnitDialog),
    fold_sep(new const QString)

{
    ui->setupUi(this);
    fold_sep = folder_separator;

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
         QMessageBox *mb = new QMessageBox(this);
         mb->setText("File name cannot be empty.");
         mb->show();
    }

    else if (ui->nameLineEdit->text().contains(" (Custom)"))
    {
        QMessageBox *mb = new QMessageBox(this);
        mb->setText("Invalid filename:\nThe file name cannot contain \"(Custom)\"");
        mb->show();
    }

    else if (reserved_char(ui->nameLineEdit->text()))
    {
        QMessageBox *mb = new QMessageBox(this);
        mb->setText("Invalid filename:\nThe file name contains invalid characters.");
        mb->show();
    }

    else
    {
        if (ui->cloneCheckBox->isChecked())
        {
            QString new_file_name = ui->nameLineEdit->text();
            QString old_file = ui->existingListBox->selectedItems()[0]->text();
            QString old_file_path;
            if (old_file.contains(" (Custom)"))
            {
                QString old_file_name = old_file.split(" (Custom)")[0];
                old_file_path = QDir::currentPath() +  *fold_sep + "data" + *fold_sep +
                        old_file_name + ".csv";
            }
            else
            {
                old_file_path = QDir::currentPath() +  *fold_sep + "data" + *fold_sep +
                        "default" + *fold_sep + old_file + ".dat";
            }

            QString new_file_path = QDir::currentPath() + *fold_sep + "data" + *fold_sep +
                    new_file_name + ".csv";

            QFile::copy(old_file_path, new_file_path);
        }

        // Clone button NOT checked.
        else
        {
            if (ui->masterLineEdit->text().isEmpty())
            {
                QMessageBox *mb = new QMessageBox(this);
                mb->setText("Mater Unit name invald.");
                mb->show();
            }

            else
            {
                QString new_file_name = ui->nameLineEdit->text() + ".csv";
                QString file_path = QDir::currentPath() + *fold_sep + "data" + *fold_sep;

                QFile file(file_path + new_file_name);
                file.open(QFile::OpenModeFlag::ReadWrite);

                QTextStream stream(&file);

                stream << "Unit Type, " + new_file_name << "\n";
                stream << "Master Unit, " + ui->masterLineEdit->text() + "\n";
                stream << "Unit Name, Conversion Value, Notes";
                stream.flush();
                file.close();
            }
        }
        ((DataTableWindow*)parent())->read_file_names();
        ((DataTableWindow*)parent())->load_category_list();
        this->close();
    }
}

bool NewUnitDialog::reserved_char(QString input)
{
    QString reserved = "<>:*\"/\\|?";
    for (int i=0; i<reserved.count(); ++i)
    {
        if (input.contains(reserved.at(i)))
        {
            return true;
        }
    }
    return false;
}
