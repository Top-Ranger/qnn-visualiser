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

#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>
#include <QPixmap>
#include <QDir>

QNNVisualiser::QNNVisualiser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QNNVisualiser)
{
    ui->setupUi(this);
    ui->pushButton->setEnabled(false);
    ui->actionVisualise_selected_Network->setEnabled(false);
}

QNNVisualiser::~QNNVisualiser()
{
    delete ui;
}

bool QNNVisualiser::parseFile(QString file_name)
{
    bool return_value = false;

    QFile file(file_name);
    if(!file.exists())
    {
        showErrorMessage(QString(tr("File %1 does not exists")).arg(file_name));
        return false;
    }
    if(!file.open(QFile::ReadOnly))
    {
        showErrorMessage(QString(tr("Can not open %1: %2")).arg(file_name).arg(file.errorString()));
        return false;
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
                    showErrorMessage(tr("Not a valid file: No network type"));
                    goto cleanup;
                }
                if(attribute.value("type").toString() != "GasNet" && attribute.value("type").toString() != "ModulatedSpikingNeuronsNetwork")
                {
                    showErrorMessage(tr("Unsupported network type: %1").arg(attribute.value("type").toString()));
                    goto cleanup;
                }
            }
            else if(reader.name() == "double")
            {
                // Parse pos_x, pos_y and gas_radius
                QXmlStreamAttributes attribute = reader.attributes();
                if(!attribute.hasAttribute("key"))
                {
                    showErrorMessage(tr("Not a valid file: key is missing for double"));
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
                    showErrorMessage(tr("Not a valid file: key is missing for double"));
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
                    showErrorMessage(tr("Not a valid file: id is missing for neuron"));
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
                    showErrorMessage(tr("ID of neuron not found"));
                    goto cleanup;
                }
                if(neuron_hash.contains(current_neuron.id))
                {
                    showErrorMessage(QString(tr("Double id: %1")).arg(current_neuron.id));
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
            showErrorMessage(QString(tr("Error while reading: %1")).arg(reader.errorString()));
            goto cleanup;

        default:
            showErrorMessage(QString(tr("Unknown token: %1")).arg(reader.tokenString()));
            goto cleanup;
        }
    }

    return_value = true;
    drawNN(neuron_hash);

cleanup:
    file.close();
    return return_value;
}

void QNNVisualiser::on_pushButton_clicked()
{
    parseFile(ui->lineEdit->text());
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
    ui->actionVisualise_selected_Network->setEnabled(ui->lineEdit->text() != "");
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

void QNNVisualiser::drawNN(QHash<int, neuron> neuron_hash)
{
    ui->widget->setNeuronHash(neuron_hash);
}

void QNNVisualiser::showErrorMessage(QString error)
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

void QNNVisualiser::on_actionVisualise_selected_Network_triggered()
{
    parseFile(ui->lineEdit->text());
}

void QNNVisualiser::on_actionConvert_folder_triggered()
{
    QString folder_path = QFileDialog::getExistingDirectory(this, "Select folder", "");
    if(folder_path != "")
    {
        QStringList filter;
        QStringList files;
        filter << "*.xml";
        QDir dir(folder_path);
        files = dir.entryList(filter);
        foreach (QString file_name, files)
        {
            if(parseFile(dir.absoluteFilePath(file_name)))
            {
                QPixmap::grabWidget(ui->widget).save(dir.absoluteFilePath(file_name).replace(".xml", ".png"));
            }
        }
    }
}
