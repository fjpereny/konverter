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
#include <QMessageBox>


DataTableWindow::DataTableWindow(QWidget *parent, const QString *folder_seperator) :
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
    status_bar_label(new QLabel),
    fold_sep(new const QString),
    unsaved_changes(new bool)

{   
    *enable_calcs = false;
    fold_sep = folder_seperator;

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

    *unsaved_changes = false;
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

    QString path_default_files = QDir::currentPath() + *fold_sep + "data" + *fold_sep + "default" + *fold_sep;
    QString path_custom_files = QDir::currentPath() + *fold_sep + "data" + *fold_sep;

    QDir dir_default(path_default_files);
    *data_file_list = dir_default.entryList(QDir::Files);
    QDir dir_custom(path_custom_files);
    *data_file_list += dir_custom.entryList(QDir::Files);
}


void DataTableWindow::load_category_list()
{
    ui->unitTypeList->clear();
    for (int i=0; i<data_file_list->count(); ++i)
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
    if (QFileInfo::exists(QDir::currentPath() + *fold_sep + "data" + *fold_sep + file_name + ".csv"))
    {
        file_path = QDir::currentPath() + *fold_sep + "data" + *fold_sep + file_name + ".csv";
    }

    else if (QFileInfo::exists(QDir::currentPath() + *fold_sep + "data" + *fold_sep + "default" + *fold_sep + file_name + ".dat"))
    {
        file_path = QDir::currentPath() + *fold_sep + "data" + *fold_sep + "default" + *fold_sep + file_name + ".dat";
    }

    // Stop if file not found
    if (file_path.isEmpty())
        return;

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
    // In case user toggles the checkbox before a key entry or selection change
    check_table_bools();
    ui->unitTable->selectionModel()->clear();

    if (checked)
    {
        *unsaved_changes = true;

        ui->refUnitCombo->setCurrentText(*master_name);
        ui->refUnitCombo->setEnabled(false);

        ui->inputValueLineEdit->setText("1");
        ui->inputValueLineEdit->setEnabled(false);

        ui->addRowButton->setEnabled(true);
        ui->delRowButton->setEnabled(true);
        ui->changeMasterButton->setEnabled(true);
        ui->masterCombo->setEnabled(true);
        ui->delTypeButton->setEnabled(false);
        ui->addTypeButton->setEnabled(false);

        ui->unitTable->setEditTriggers(QAbstractItemView::AllEditTriggers);

        status_bar_label->setText(" Table Edit Mode Enabled! ");
        statusBar()->setStyleSheet(Ui::yellow_background);

        // Prevents modification of the master unit in table edit mode.
        for (int i=0; i<unit_names->count(); i++)
        {
            if (ui->unitTable->item(i, 0)->text() == *master_name)
            {
                Qt::ItemFlags current_flags;
                current_flags = ui->unitTable->item(i, 0)->flags();
                ui->unitTable->item(i, 0)->setFlags(current_flags & (~Qt::ItemFlag::ItemIsEditable));
                current_flags = ui->unitTable->item(i, 1)->flags();
                ui->unitTable->item(i, 1)->setFlags(current_flags & (~Qt::ItemFlag::ItemIsEditable));
                current_flags = ui->unitTable->item(i, 2)->flags();
                ui->unitTable->item(i, 2)->setFlags(current_flags & (~Qt::ItemFlag::ItemIsEditable));
            }
        }
    }
    else
    {
        ui->refUnitCombo->setEnabled(true);
        ui->inputValueLineEdit->setEnabled(true);

        ui->addRowButton->setEnabled(false);
        ui->delRowButton->setEnabled(false);
        ui->changeMasterButton->setEnabled(false);
        ui->masterCombo->setEnabled(false);
        ui->delTypeButton->setEnabled(true);
        ui->addTypeButton->setEnabled(true);

        ui->unitTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

        status_bar_label->clear();
        statusBar()->setStyleSheet("");
    }

    if ((!checked) && *unsaved_changes)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,
                                      "Unsaved Changes",
                                      "Save all changes to conversion table?");
        if (reply == QMessageBox::Yes)
        {
            QString selected_name = ui->unitTypeList->selectedItems()[0]->text().split(" (Custom)")[0];
            QString output("Unit Type, " + selected_name + "\n");
            output.append("Master Unit, " + *master_name + "\n");
            output.append("Unit Name, Conversion Value, Notes\n");

            for (int i=0; i<ui->unitTable->rowCount(); ++i)
            {
                // Skip the master unit
                if (ui->unitTable->item(i, 0)->text() != *master_name)
                {
                    output.append(ui->unitTable->item(i, 0)->text());
                    output.append(",");
                    QString value = ui->unitTable->item(i, 1)->text();
                    value.replace(",", "");
                    output.append(value);
                    output.append(",");
                    output.append(ui->unitTable->item(i, 2)->text());
                    output.append("\n");
                }
            }

            QString  file_path(QDir::currentPath() + *fold_sep + "data" + *fold_sep + selected_name + ".csv");
            QFile file(file_path);
            if (file.open(QFile::WriteOnly))
            {
                QDataStream stream(&file);
                stream << output;
                file.flush();
            }
            file.close();
            refresh_data();
        }
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
                status_bar_label->setText(" Invalid input... ");
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
    if (ui->editCheckBox->isChecked())
    {
        status_bar_label->setText(" Table Edit Mode Enabled! ");
        statusBar()->setStyleSheet(Ui::yellow_background);
        check_table_bools();
    }

    if (!ui->editCheckBox->isChecked())
    {
        copy_selected_cells();
    }

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
            status_bar_label->setText(" Invalid input... ");
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
    {
        ui->unitTable->selectionModel()->clearSelection();
    }
}


void DataTableWindow::check_table_bools()
{
    bool *valid = new bool(false);
    for (int i=0; i<ui->unitTable->rowCount(); ++i)
    {
        // If row doesn't have an item (such as after adding row) add an item.
        QTableWidgetItem *item = ui->unitTable->item(i, 1);
        if (!item)
        {
            item = new QTableWidgetItem();
            ui->unitTable->setItem(i, 1, item);
        }

        item->text().toDouble(valid);
        if (*valid)
        {
            ui->unitTable->item(i, 1)->setBackground(Qt::NoBrush);
            ui->unitTable->item(i, 1)->setForeground(Qt::NoBrush);
        }
        else
        {
            ui->unitTable->item(i, 1)->setBackground(Qt::red);
            ui->unitTable->item(i, 1)->setForeground(Qt::white);
            status_bar_label->setText(" Invalid input... ");
            ui->statusbar->setStyleSheet(Ui::red_background);
        }
    }
    delete valid;
    valid = nullptr;
}

void DataTableWindow::keyReleaseEvent(QKeyEvent *event)
{
    // Check for errors in edited text (must be a double)
    if (ui->editCheckBox->isChecked())
    {
        status_bar_label->setText(" Table Edit Mode Enabled! ");
        statusBar()->setStyleSheet(Ui::yellow_background);
        ui->unitTable->selectionModel()->clear();
        check_table_bools();
    }
}

void DataTableWindow::on_delTypeButton_clicked()
{
    QStringList file_name_list = ui->unitTypeList->currentItem()->text().split(" (Custom)");
    if (file_name_list.count() == 1)
    {
        status_bar_label->setText(" Default (built-in) data tables cannot be deleted.");
        ui->statusbar->setStyleSheet(Ui::red_background);
        return;
    }

    QString file_name = file_name_list[0];
    QString file_path = QDir::currentPath() + *fold_sep + "data" + *fold_sep + file_name + ".csv";
    QFile file(file_path);

    if (file.exists())
    {
        QDir dir;
        dir.remove(file_path);
        read_file_names();
        load_category_list();
        ui->unitTypeList->setCurrentItem(ui->unitTypeList->item(0));
        refresh_data();

        status_bar_label->setText(" " + file_name + " successfully removed... ");
        ui->statusbar->setStyleSheet(Ui::green_background);
    }

    else
    {
        status_bar_label->setText(" Warning: " + file_name + ".csv was not found... ");
        ui->statusbar->setStyleSheet(Ui::red_background);
    }

    file.close();
}


void DataTableWindow::on_addTypeButton_clicked()
{
    NewUnitDialog *d = new NewUnitDialog(this, data_file_list, fold_sep);
    d->show();
}


void DataTableWindow::on_addRowButton_clicked()
{
    ui->unitTable->setRowCount(ui->unitTable->rowCount() + 1);
    int new_row = ui->unitTable->rowCount() - 1;

    QTableWidgetItem *new_unit = new QTableWidgetItem("New Unit");
    QTableWidgetItem *new_value = new QTableWidgetItem("1.0");
    QTableWidgetItem *new_note = new QTableWidgetItem();
    ui->unitTable->setItem(new_row, 0, new_unit);
    ui->unitTable->setItem(new_row, 1, new_value);
    ui->unitTable->setItem(new_row, 2, new_note);
}


void DataTableWindow::on_delRowButton_clicked()
{
    QList<QTableWidgetItem *> selected_items = ui->unitTable->selectedItems();
    QList<int> selected_rows;
    for (QTableWidgetItem *item : selected_items)
    {
        if (!(ui->unitTable->item(item->row(), 0)->text() == *master_name))
        {
            if (!selected_rows.contains(item->row()))
            {
                selected_rows.append(item->row());
            }
        }
    }

    // Offset for removed rows (all rows shift up when one is removed)
    int removed_counter = 0;
    for (int i=0; i<selected_rows.count(); ++i)
    {
        ui->unitTable->removeRow(selected_rows.at(i) - removed_counter);
        ++removed_counter;
    }
}

