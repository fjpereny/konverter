#ifndef DATATABLEWINDOW_H
#define DATATABLEWINDOW_H

#include <QMainWindow>
#include <QLabel>

namespace Ui {
class DataTableWindow;

const static QString yellow_background = "background-color: rgb(255, 255, 100);"
                                       "color: rgb(0, 0, 0);";
const static QString red_background = "background-color : rgb(125, 50, 50);"
                                      "color: rgb(255, 255, 255);";
const static QString green_background = "background-color : rgb(0, 100, 50);"
                                        "color: rgb(255, 255, 255);";
}


class DataTableWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DataTableWindow(QWidget *parent = nullptr, const QString *folder_sep = nullptr);
    ~DataTableWindow();

public :
    void refresh_data();
    void load_category_list();
    void read_file_names();

private slots:

    void on_actionClose_triggered();
    void on_refUnitCombo_currentIndexChanged(int index);
    void on_editCheckBox_toggled(bool checked);
    void on_unitTypeList_itemSelectionChanged();
    void on_inputValueLineEdit_textChanged(const QString &arg1);
    void on_decimalSpinBox_valueChanged(int arg1);
    void on_actionAbout_triggered();
    void on_unitTable_itemSelectionChanged();
    void on_delTypeButton_clicked();
    void on_addTypeButton_clicked();

    void on_addRowButton_clicked();

    void on_delRowButton_clicked();

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
    QPalette *error_entry_red;
    QLabel *status_bar_label;
    const QString *fold_sep;
    bool *unsaved_changes;

    void import_csv(QString file_name);
    void load_unit_dropdown();
    void set_master_unit();
    void load_table();
    void clear_data();
    void set_edit_checkbox_status();
    void copy_selected_cells();
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void check_table_bools();

};

#endif // DATATABLEWINDOW_H
