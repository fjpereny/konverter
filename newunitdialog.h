/*
Copyright © 2021 Frank Pereny

https://github.com/fjpereny/konverter

This program is free software: you can redistribute it and/or modify it under the terms 
of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.
*/

#ifndef NEWUNITDIALOG_H
#define NEWUNITDIALOG_H

#include <QMainWindow>
#include <datatablewindow.h>

namespace Ui {
class NewUnitDialog;
}

class NewUnitDialog : public QMainWindow
{
    Q_OBJECT

public:
    explicit NewUnitDialog(QWidget *parent = nullptr);
    NewUnitDialog(QWidget *parent = nullptr, QStringList *file_list = nullptr, const QString *folder_separator = nullptr);
    ~NewUnitDialog();

private slots:

    void on_cloneCheckBox_toggled(bool checked);

    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

    bool reserved_char(QString input);

private:
    Ui::NewUnitDialog *ui;
    const QString *fold_sep;
};

#endif // NEWUNITDIALOG_H
