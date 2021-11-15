#include "datatablewindow.h"
#include "ui_datatablewindow.h"
#include <QFile>
#include <QFileDialog>
#include <vector>

DataTableWindow::DataTableWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DataTableWindow),
    unit_names(new QStringList),
    unit_values(new std::vector<double>),
    displayed_values(new std::vector<double>),
    unit_notes(new QStringList),
    master_name(new QString),
    enable_calcs(new bool),
    data_file_list(new QStringList)

{
    ui->setupUi(this);
    ui->unitTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->unitTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    updateColumnNames();
    *enable_calcs = false;
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
}


void DataTableWindow::on_actionClose_triggered()
{
    this->close();
}


void DataTableWindow::updateColumnNames()
{
    QStringList col_names = {"Unit Name", "Conversion Value", "Notes"};
    ui->unitTable->setHorizontalHeaderLabels(col_names);
}

void DataTableWindow::updateColumnNames(QString unit_label)
{
    QStringList col_names = {"Unit Name", "Conversion Value ( x / " + unit_label + " )", "Notes"};
    ui->unitTable->setHorizontalHeaderLabels(col_names);
}


void DataTableWindow::read_file_names()
{
    ui->unitTypeList->clear();

    QString path = QDir::currentPath() + "/data/";
    QDir dir(path);
    *data_file_list = dir.entryList(QDir::Files);
}


void DataTableWindow::load_category_list()
{
    ui->unitTypeList->clear();
    for (int i=0; i<data_file_list->count(); i++)
    {
        QString file_name = data_file_list->at(i).split(".").at(0);
        ui->unitTypeList->addItem(file_name);
    }
}


void DataTableWindow::on_actionImport_triggered()
{
    *enable_calcs = false;
    clear_table_data();
    read_file_names();
    import_csv(QDir::currentPath() + "/data/Length.csv");
    load_table();
    load_unit_dropdown();
    load_category_list();
    set_master_unit();
    *enable_calcs = true;
}


void DataTableWindow::clear_table_data()
{
    unit_names->clear();
    unit_values->clear();
    unit_notes->clear();

    ui->unitTable->clear();
    ui->unitTable->setRowCount(0);
    updateColumnNames();;
}


void DataTableWindow::import_csv(QString file_name)
{
    // "/home/frank/kde/src/konverter/data/length.csv"
    QFile file(file_name);
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);

        QString unit_type = stream.readLine().split(",")[1];
        *master_name = stream.readLine().split(",")[1];
        QStringList csv_headers = stream.readLine().split(",");

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
            if (line_list.size() == 3)
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
        item_value->setText(QString::number(displayed_values->at(row_index), 'f', 7));

        QTableWidgetItem *item_note = ui->unitTable->item(row_index, 2);
        if (!item_note)
        {
            item_note = new QTableWidgetItem();
            ui->unitTable->setItem(row_index, 2, item_note);
        }
        item_note->setText(unit_notes->at(row_index));
    }
        ui->unitTable->item(0, 0)->setFlags(Qt::ItemIsSelectable);
        ui->unitTable->item(0, 1)->setFlags(Qt::ItemIsSelectable);
        ui->unitTable->item(0, 2)->setFlags(Qt::ItemIsSelectable);
        ui->unitTable->sortByColumn(0, Qt::SortOrder::AscendingOrder);
}


void DataTableWindow::load_unit_dropdown()
{
    ui->refUnitCombo->clear();
    ui->refUnitCombo->addItems(*unit_names);
    ui->refUnitCombo->model()->sort(Qt::SortOrder::AscendingOrder);
    ui->refUnitCombo->setCurrentText(*master_name);
}


void DataTableWindow::set_master_unit()
{
    ui->masterLineEdit->setText(*master_name);
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
        }

        load_table();
        updateColumnNames(ui->refUnitCombo->currentText());
    }
}


void DataTableWindow::on_editCheckBox_toggled(bool checked)
{
    if (checked)
    {
        ui->addRowButton->setEnabled(true);
        ui->delRowButton->setEnabled(true);
        ui->unitTable->setEditTriggers(QAbstractItemView::AllEditTriggers);
    }
    else
    {
        ui->addRowButton->setEnabled(false);
        ui->delRowButton->setEnabled(false);
        ui->unitTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}

