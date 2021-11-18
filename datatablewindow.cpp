#include "datatablewindow.h"
#include "ui_datatablewindow.h"
#include "aboutdialog.h"
#include "newunitdialog.h"
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <vector>
#include <iostream>
#include <QLocale>
#include <QClipboard>
#include <QKeyEvent>
#include <QtCore>


DataTableWindow::DataTableWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DataTableWindow),
    unit_names(new QStringList),
    unit_values(new std::vector<double>),
    displayed_values(new std::vector<double>),
    unit_notes(new QStringList),
    master_name(new QString),
    enable_calcs(new bool),
    data_file_list(new QStringList),
    prev_input_value(new double),
    sig_digits(new int),
    error_entry_red(new QPalette),
    status_bar_label(new QLabel)

{
    *enable_calcs = false;
    ui->setupUi(this);

    ui->unitTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->unitTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->unitTypeList->sortItems(Qt::SortOrder::AscendingOrder);

    read_file_names();
    load_category_list();
    *prev_input_value = 1.0;
    *sig_digits = 3;
    ui->decimalSpinBox->setValue(*sig_digits);

    // Input error palette settings
    error_entry_red->setColor(QPalette::Base, Qt::red);
    error_entry_red->setColor(QPalette::Text, Qt::white);

    *enable_calcs = true;
    ui->unitTypeList->setCurrentItem(ui->unitTypeList->item(0));
    refresh_data();

    ui->statusbar->addWidget(status_bar_label);
    status_bar_label->setText("");
}

DataTableWindow::~DataTableWindow()
{
    delete ui;
    delete unit_names;
    delete unit_values;
    delete displayed_values;
    delete unit_notes;
    delete master_name;
    delete enable_calcs;
    delete data_file_list;
    delete prev_input_value;
    delete sig_digits;
    delete error_entry_red;
}


void DataTableWindow::on_actionClose_triggered()
{
    this->close();
}


void DataTableWindow::read_file_names()
{
    ui->unitTypeList->clear();

    QString path_default_files = QDir::currentPath() + "/data/default/";
    QString path_custom_files = QDir::currentPath() + "/data/";

    QDir dir_default(path_default_files);
    *data_file_list = dir_default.entryList(QDir::Files);
    QDir dir_custom(path_custom_files);
    *data_file_list += dir_custom.entryList(QDir::Files);
}


void DataTableWindow::load_category_list()
{
    ui->unitTypeList->clear();
    for (int i=0; i<data_file_list->count(); i++)
    {
        QStringList file_name_extension = data_file_list->at(i).split(".");
        if (file_name_extension[1] == "dat")
        {
            ui->unitTypeList->addItem(file_name_extension[0]);
        }
        else if (file_name_extension[1] == "csv")
        {
            ui->unitTypeList->addItem(file_name_extension[0] + " (Custom)");
        }
    }
}


void DataTableWindow::refresh_data()
{
    *enable_calcs = false;
    clear_data();
    import_csv(ui->unitTypeList->currentItem()->text().split(" (Custom)")[0]);
    load_table();
    ui->inputValueLineEdit->setText("1");
    *prev_input_value = 1.0;
    load_unit_dropdown();
    set_master_unit();
    *enable_calcs = true;
    status_bar_label->setText("");
    ui->statusbar->setStyleSheet("");
}


void DataTableWindow::set_edit_checkbox_status()
{
    if (ui->unitTypeList->selectedItems().count() == 0)
    {
        ui->editCheckBox->setEnabled(false);
        return;
    }

    QString selected = ui->unitTypeList->selectedItems()[0]->text();
    if (selected.contains("(Custom)"))
    {
        ui->editCheckBox->setEnabled(true);
    }
    else
    {
        ui->editCheckBox->setEnabled(false);
        ui->editCheckBox->setChecked(false);
    }
}


void DataTableWindow::clear_data()
{
    unit_names->clear();
    unit_values->clear();
    displayed_values->clear();
    unit_notes->clear();
}


void DataTableWindow::import_csv(QString file_name)
{
    QString file_path;
    if (QFileInfo::exists(QDir::currentPath() + "/data/" + file_name + ".csv"))
    {
        file_path = QDir::currentPath() + "/data/" + file_name + ".csv";
    }

    else if (QFileInfo::exists(QDir::currentPath() + "/data/default/" + file_name + ".dat"))
    {
        file_path = QDir::currentPath() + "/data/default/" + file_name + ".dat";
    }

    QFile file(file_path);
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);

        // Skip line in CSV file
        stream.readLine();
        *master_name = stream.readLine().split(",")[1];
        // Skip line in CSV file
        stream.readLine();

        unit_names->append(*master_name);
        unit_values->push_back(1.0);
        displayed_values->push_back(1.0);
        unit_notes->append("Master Unit");

        while (!stream.atEnd())
        {
            QString line = stream.readLine();
            QStringList line_list = line.split(",");
            unit_names->append(line_list.at(0));
            unit_values->push_back(line_list.at(1).toDouble());
            displayed_values->push_back(line_list.at(1).toDouble());
            if (line_list.size() >= 3)
            {
                unit_notes->append(line_list.at(2));
            }
            else
            {
                unit_notes->append("");
            }
        }
        file.close();
    }
}


void DataTableWindow::load_table()
{
    ui->unitTable->setRowCount(0);
    ui->unitTable->setRowCount(unit_names->count());

    for (int row_index=0; row_index < unit_names->count(); row_index++)
    {
        QTableWidgetItem *item_name = ui->unitTable->item(row_index, 0);
        if (!item_name)
        {
            item_name = new QTableWidgetItem();
            ui->unitTable->setItem(row_index, 0, item_name);
        }
        item_name->setText(unit_names->at(row_index));

        QTableWidgetItem *item_value = ui->unitTable->item(row_index, 1);
        if (!item_value)
        {
            item_value = new QTableWidgetItem();
            ui->unitTable->setItem(row_index, 1, item_value);
        }
        QLocale locale;
        item_value->setText(locale.toString(displayed_values->at(row_index), 'f', *sig_digits));

        QTableWidgetItem *item_note = ui->unitTable->item(row_index, 2);
        if (!item_note)
        {
            item_note = new QTableWidgetItem();
            ui->unitTable->setItem(row_index, 2, item_note);
        }
        item_note->setText(unit_notes->at(row_index));
    }
        ui->unitTable->sortByColumn(0, Qt::SortOrder::AscendingOrder);
}


void DataTableWindow::load_unit_dropdown()
{
    ui->refUnitCombo->clear();
    ui->refUnitCombo->addItems(*unit_names);
    ui->refUnitCombo->model()->sort(Qt::SortOrder::AscendingOrder);
    ui->refUnitCombo->setCurrentText(*master_name);

    ui->masterCombo->clear();
    ui->masterCombo->addItems(*unit_names);
    ui->masterCombo->model()->sort(Qt::SortOrder::AscendingOrder);
    ui->masterCombo->setCurrentText(*master_name);
}


void DataTableWindow::set_master_unit()
{

}


void DataTableWindow::on_refUnitCombo_currentIndexChanged(int index)
{
    if (*enable_calcs)
    {
        QString target_unit = ui->refUnitCombo->currentText();
        int target_index = unit_names->indexOf(target_unit);
        double target_value = unit_values->at(target_index);

        for (int i=0; i < unit_values->size(); i++)
        {
            displayed_values->at(i) = unit_values->at(i);
            displayed_values->at(i) /= target_value;
            displayed_values->at(i) *= *prev_input_value;
        }

        load_table();
    }
    set_edit_checkbox_status();
}


void DataTableWindow::on_editCheckBox_toggled(bool checked)
{
    if (checked)
    {
        ui->inputValueLineEdit->setText("1");
        ui->inputValueLineEdit->setEnabled(false);

        ui->addRowButton->setEnabled(true);
        ui->delRowButton->setEnabled(true);
        ui->saveButton->setEnabled(true);
        ui->changeMasterButton->setEnabled(true);
        ui->masterCombo->setEnabled(true);
        ui->delTypeButton->setEnabled(false);
        ui->addTypeButton->setEnabled(false);

        ui->unitTable->setEditTriggers(QAbstractItemView::AllEditTriggers);

        status_bar_label->setText(" Table Edit Mode Enabled! ");
        statusBar()->setStyleSheet(Ui::blue_background);

        // Prevents modification of the master unit in table edit mode.
        for (int i=0; i<unit_names->count(); i++)
        {
            if (ui->unitTable->item(i, 0)->text() == *master_name)
            {
                ui->unitTable->item(i, 0)->setFlags(Qt::ItemIsSelectable);
                ui->unitTable->item(i, 1)->setFlags(Qt::ItemIsSelectable);
                ui->unitTable->item(i, 2)->setFlags(Qt::ItemIsSelectable);
            }
        }
    }
    else
    {
        ui->inputValueLineEdit->setEnabled(true);

        ui->addRowButton->setEnabled(false);
        ui->delRowButton->setEnabled(false);
        ui->saveButton->setEnabled(false);
        ui->changeMasterButton->setEnabled(false);
        ui->masterCombo->setEnabled(false);
        ui->delTypeButton->setEnabled(true);
        ui->addTypeButton->setEnabled(true);

        ui->unitTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

        status_bar_label->clear();
        statusBar()->setStyleSheet("");
    }
}


void DataTableWindow::on_unitTypeList_itemSelectionChanged()
{
    refresh_data();
}


void DataTableWindow::on_inputValueLineEdit_textChanged(const QString &arg1)
{
    status_bar_label->setText("");
    ui->inputValueLineEdit->setStyleSheet("");
    ui->statusbar->setStyleSheet("");
    if (*enable_calcs)
    {
        double input_value = ui->inputValueLineEdit->text().toDouble();
        if (input_value != 0)
        {
            for (int i=0; i<displayed_values->size(); i++)
            {
                displayed_values->at(i) *= input_value / (*prev_input_value);
            }

            *prev_input_value = input_value;
            load_table();
        }
        else
        {
            if (ui->inputValueLineEdit->text() != "")
            {
                ui->inputValueLineEdit->setStyleSheet(Ui::red_background);
                status_bar_label->setText("Invalid input...");
                ui->statusbar->setStyleSheet(Ui::red_background);
                ui->unitTable->selectionModel()->clearSelection();
            }
        }
    }
}


void DataTableWindow::on_decimalSpinBox_valueChanged(int decimal_places)
{
    if (*enable_calcs)
    {
        *sig_digits = decimal_places;
        load_table();
    }
}


void DataTableWindow::on_actionAbout_triggered()
{
    AboutDialog *d = new AboutDialog();
    d->show();
}


void DataTableWindow::on_unitTable_itemSelectionChanged()
{
    copy_selected_cells();
}


void DataTableWindow::copy_selected_cells()
{
    QModelIndexList index_list = ui->unitTable->selectionModel()->selectedIndexes();
    if (index_list.count() == 0)
    {
        // Check if input error to avoid clearing error from status bar
        double input_value = ui->inputValueLineEdit->text().toDouble();
        if (input_value == 0)
        {
            status_bar_label->setText("Invalid input...");
            ui->statusbar->setStyleSheet(Ui::red_background);
        }
        else
        {
            status_bar_label->setText("");
            ui->statusbar->setStyleSheet("");
        }
        return;
    }

    if (ui->unitTable->hasFocus())
    {
        QString output("");

        output += index_list.at(0).data().toString();
        int prev_row = index_list.at(0).row();
        for (int i=1; i<index_list.count(); i++)
        {
            if (index_list.at(i).row() == prev_row)
                output += "\t";
            else
                output += "\n";
            output += index_list.at(i).data().toString();
            prev_row = index_list.at(i).row();
        }

        if (index_list.count() == 1)
        {
            status_bar_label->setText(" Cell copied to clipboard... ");
            ui->statusbar->setStyleSheet(Ui::green_background);
        }
        else
        {
            status_bar_label->setText(" Cells copied to clipboard... ");
            ui->statusbar->setStyleSheet(Ui::green_background);
        }

        QGuiApplication::clipboard()->clear();
        QGuiApplication::clipboard()->setText(output);
    }
}


void DataTableWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        ui->unitTable->selectionModel()->clearSelection();
}

void DataTableWindow::on_delTypeButton_clicked()
{
    QStringList file_name_list = ui->unitTypeList->currentItem()->text().split(" (Custom)");
    if (file_name_list.count() == 1)
    {
        status_bar_label->setText("Default (built-in) data tables cannot be deleted.");
        ui->statusbar->setStyleSheet(Ui::red_background);
        return;
    }

    QString file_name = file_name_list[0];
    QString file_path = QDir::currentPath() + "/data/" + file_name + ".csv";
    QFile file(file_path);

    if (file.exists())
    {
        QDir dir;
        dir.remove(file_path);
        read_file_names();
        load_category_list();
        ui->unitTypeList->setCurrentItem(ui->unitTypeList->item(0));
        refresh_data();

        status_bar_label->setText("Custom " + file_name + "successfully removed...");
        ui->statusbar->setStyleSheet(Ui::green_background);
    }

    else
    {
        status_bar_label->setText("Custom " + file_name + " was not found...");
        ui->statusbar->setStyleSheet(Ui::red_background);
    }

    file.close();
}


void DataTableWindow::on_addTypeButton_clicked()
{
    NewUnitDialog *d = new NewUnitDialog(this, data_file_list);
    d->show();
}

