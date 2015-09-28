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

#ifndef QNNVISUALISER_H
#define QNNVISUALISER_H

#include <QMainWindow>
#include <QHash>
#include "neuron.h"

namespace Ui {
class QNNVisualiser;
}

class QNNVisualiser : public QMainWindow
{
    Q_OBJECT

public:
    explicit QNNVisualiser(QWidget *parent = 0);
    ~QNNVisualiser();

private slots:
    bool parse_file(QString file_name);
    void on_pushButton_clicked();
    void on_toolButton_clicked();
    void on_actionQuit_triggered();
    void on_actionAbout_triggered();
    void on_actionAbout_Qt_triggered();
    void on_actionSave_Network_triggered();
    void on_actionCreate_XML_triggered();
    void on_actionVisualise_selected_Network_triggered();
    void on_actionConvert_folder_triggered();

private:
    void draw_nn(QHash<int, neuron> neuron_hash);
    void show_error_message(QString error);

    Ui::QNNVisualiser *ui;
};

#endif // QNNVISUALISER_H
