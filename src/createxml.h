/*
 * Copyright (C) 2015 Marcus Soll
 * This file is part of qnn-visualiser.
 *
 * qnn-visualiser is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * qnn-visualiser is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with qnn-ui. If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef CREATEXML_H
#define CREATEXML_H

#include <QDialog>
#include <QStringListModel>

namespace Ui {
class CreateXML;
}

class CreateXML : public QDialog
{
    Q_OBJECT

public:
    explicit CreateXML(QWidget *parent = 0);
    ~CreateXML();    

private slots:
    void on_source_toolButton_clicked();

    void on_target_toolButton_clicked();

    void on_pushButton_clicked();

private:
    void check_button_enabled();
    void show_error_message(QString error);

    Ui::CreateXML *ui;

    QStringListModel *_nn_model;
    QStringListModel *_sim_model;
};

#endif // CREATEXML_H
