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

#include "qnnvisualiser.h"
#include "ui_qnnvisualiser.h"
#include "createxml.h"

#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>
#include <QPixmap>

QNNVisualiser::QNNVisualiser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QNNVisualiser)
{
    ui->setupUi(this);
    ui->pushButton->setEnabled(false);
}

QNNVisualiser::~QNNVisualiser()
{
    delete ui;
}

void QNNVisualiser::on_pushButton_clicked()
{
    QString file_name = ui->lineEdit->text();
    QFile file(file_name);
    if(!file.exists())
    {
        show_error_message(QString(tr("File %1 does not exists")).arg(file_name));
        return;
    }
    if(!file.open(QFile::ReadOnly))
    {
        show_error_message(QString(tr("Can not open %1: %2")).arg(file_name).arg(file.errorString()));
        return;
    }

    QXmlStreamReader reader(&file);
    QHash<int, neuron> neuron_hash;
    neuron current_neuron;

    while(!reader.atEnd())
    {
        switch(reader.readNext())
        {
        case QXmlStreamReader::StartDocument:
            break;


        case QXmlStreamReader::StartElement:
            if(reader.name() == "network")
            {
                QXmlStreamAttributes attribute = reader.attributes();
                if(!attribute.hasAttribute("type"))
                {
                    show_error_message(tr("Not a valid file: No network type"));
                    goto cleanup;
                }
                if(attribute.value("type").toString() != "GasNet" && attribute.value("type").toString() != "ModulatedSpikingNeuronsNetwork")
                {
                    show_error_message(tr("Unsupported network type: %1").arg(attribute.value("type").toString()));
                    goto cleanup;
                }
            }
            else if(reader.name() == "double")
            {
                // Parse pos_x, pos_y and gas_radius
                QXmlStreamAttributes attribute = reader.attributes();
                if(!attribute.hasAttribute("key"))
                {
                    show_error_message(tr("Not a valid file: key is missing for double"));
                    goto cleanup;
                }
                if(attribute.value("key") == "pos_x")
                {
                    current_neuron.x = attribute.value("value").toString().toDouble();
                }
                else if(attribute.value("key") == "pos_y")
                {
                    current_neuron.y = attribute.value("value").toString().toDouble();
                }
                else if(attribute.value("key") == "gas_radius")
                {
                    current_neuron.gas_radius = attribute.value("value").toString().toDouble();
                }
            }
            else if(reader.name() == "QString")
            {
                QXmlStreamAttributes attribute = reader.attributes();
                if(!attribute.hasAttribute("key"))
                {
                    show_error_message(tr("Not a valid file: key is missing for double"));
                    goto cleanup;
                }
                if(attribute.value("key") == "gas_type")
                {
                    current_neuron.gas_emitting = attribute.value("value").toString() != "No gas";
                }
                else if(attribute.value("key") == "when_gas_emitting")
                {
                    // when_gas_emitting is always after gas_type
                    current_neuron.gas_emitting = current_neuron.gas_emitting && (attribute.value("value").toString() != "Not emitting");
                }
            }
            else if(reader.name() == "neuron")
            {
                current_neuron = neuron();
                QXmlStreamAttributes attribute = reader.attributes();
                if(!attribute.hasAttribute("id"))
                {
                    show_error_message(tr("Not a valid file: id is missing for neuron"));
                    goto cleanup;
                }
                current_neuron.id = attribute.value("id").toString().toInt();
            }
            else if(reader.name() == "input_connections")
            {
                while(reader.readNext() != QXmlStreamReader::EndElement || reader.name() != "input_connections")
                {
                    if(reader.tokenType() == QXmlStreamReader::StartElement && reader.name() == "connection")
                    {
                        QXmlStreamAttributes attribute = reader.attributes();
                        if(attribute.hasAttribute("target") && attribute.hasAttribute("weight"))
                        {
                            current_neuron.connections[attribute.value("target").toString().toInt()] = attribute.value("weight").toString().toDouble();
                        }
                    }
                }
            }
            break;

        case QXmlStreamReader::Comment:
            break;

        case QXmlStreamReader::EndElement:
            if(reader.name() == "neuron")
            {
                if(current_neuron.id == -1)
                {
                    show_error_message(tr("ID of neuron not found"));
                    goto cleanup;
                }
                if(neuron_hash.contains(current_neuron.id))
                {
                    show_error_message(QString(tr("Double id: %1")).arg(current_neuron.id));
                    goto cleanup;
                }
                neuron_hash[current_neuron.id] = current_neuron;
                current_neuron = neuron();
            }
            break;

        case QXmlStreamReader::Characters:
            break;

        case QXmlStreamReader::EndDocument:
            break;

        case QXmlStreamReader::Invalid:
            show_error_message(QString(tr("Error while reading: %1")).arg(reader.errorString()));
            goto cleanup;

        default:
            show_error_message(QString(tr("Unknown token: %1")).arg(reader.tokenString()));
            goto cleanup;
        }
    }

    draw_nn(neuron_hash);

cleanup:
    file.close();
}

void QNNVisualiser::on_toolButton_clicked()
{
    QFileDialog dialog(this, tr("Open network XML"), "", "XML file (*.xml)");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    if(dialog.exec() == QFileDialog::Accepted && dialog.selectedFiles()[0].length() > 0)
    {
        ui->lineEdit->setText(dialog.selectedFiles()[0]);
    }
    ui->pushButton->setEnabled(ui->lineEdit->text() != "");
}

void QNNVisualiser::on_actionQuit_triggered()
{
    QApplication::quit();
}

void QNNVisualiser::on_actionAbout_triggered()
{
    QMessageBox::information(this,
                             tr("About qnn-visualiser"),
                             tr("qnn-visualiser is a simple graphical interface to visualise some network types of qnn\nAuthor: Marcus Soll\nLicense: GPL3+\nThis program uses qnn, which is licensed under the LGPL3+"));
}

void QNNVisualiser::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}

void QNNVisualiser::draw_nn(QHash<int, neuron> neuron_hash)
{
    ui->widget->set_neuron_hash(neuron_hash);
}

void QNNVisualiser::show_error_message(QString error)
{
    qWarning() << "Error:" << error;
    QMessageBox::warning(this,
                         tr("Error"),
                         QString(tr("An error occured:\n%1")).arg(error));
}


void QNNVisualiser::on_actionSave_Network_triggered()
{
    QFileDialog dialog(this, tr("Save Network"), "", "PNG-image (*.png)");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("png");
    if(dialog.exec() == QFileDialog::Accepted && dialog.selectedFiles()[0].length() > 0)
    {
        QPixmap::grabWidget(ui->widget).save(dialog.selectedFiles()[0]);
        QMessageBox::information(this,
                                 tr("Network saved"),
                                 QString(tr("Network was successfully saved to %1")).arg(dialog.selectedFiles()[0]));
    }
}

void QNNVisualiser::on_actionCreate_XML_triggered()
{
    CreateXML window;
    window.exec();
}
