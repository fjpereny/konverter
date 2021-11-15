#ifndef DATATABLEWINDOW_H
#define DATATABLEWINDOW_H

#include <QMainWindow>

namespace Ui {
class DataTableWindow;
}

class DataTableWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DataTableWindow(QWidget *parent = nullptr);
    ~DataTableWindow();

private slots:

    void on_actionClose_triggered();

    void on_refUnitCombo_currentIndexChanged(int index);

    void on_actionImport_triggered();

    void on_editCheckBox_toggled(bool checked);

private:
    Ui::DataTableWindow *ui;
    QStringList *unit_names;
    std::vector<double> *unit_values;
    std::vector<double> *displayed_values;
    QStringList *unit_notes;
    QString *master_name;
    bool *enable_calcs;
    QStringList *data_file_list;

    void updateColumnNames();
    void updateColumnNames(QString unit_label);
    void import_csv(QString file_name);
    void load_unit_dropdown();
    void set_master_unit();
    void load_table();
    void clear_table_data();
    void read_file_names();
    void load_category_list();

};

#endif // DATATABLEWINDOW_H
