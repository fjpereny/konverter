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
    void on_editCheckBox_toggled(bool checked);
    void on_unitTypeList_itemSelectionChanged();
    void on_inputValueLineEdit_textChanged(const QString &arg1);

    void on_decimalSpinBox_valueChanged(int arg1);

    void on_actionAbout_triggered();

private:
    Ui::DataTableWindow *ui;
    QStringList *unit_names;
    std::vector<double> *unit_values;
    std::vector<double> *displayed_values;
    QStringList *unit_notes;
    QString *master_name;
    bool *enable_calcs;
    QStringList *data_file_list;
    double *prev_input_value;
    int *sig_digits;

    void import_csv(QString file_name);
    void load_unit_dropdown();
    void set_master_unit();
    void load_table();
    void clear_data();
    void read_file_names();
    void load_category_list();
    void refresh_data();

};

#endif // DATATABLEWINDOW_H
