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


#include "createxml.h"
#include "ui_createxml.h"
#include <QFileDialog>
#include <QStringList>
#include <QMessageBox>
#include <QDebug>

#include <network/abstractneuralnetwork.h>
#include <network/gasnet.h>
#include <network/modulatedspikingneuronsnetwork.h>
#include <network/lengthchanginggene.h>

#include <simulation/genericsimulation.h>
#include <simulation/tmazesimulation.h>
#include <simulation/rebergrammarsimulation.h>

CreateXML::CreateXML(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateXML),
    _nn_model(NULL),
    _sim_model(NULL)
{
    ui->setupUi(this);

    _nn_model = new QStringListModel(this);
    ui->nnView->setModel(_nn_model);
    _sim_model = new QStringListModel(this);
    ui->simView->setModel(_sim_model);

    QStringList nn;
    nn << "GasNet";
    nn << "ModulatedSpikingNeuronsNetwork";

    _nn_model->setStringList(nn);

    QStringList sim;
    sim << "GenericSimulation";
    sim << "TMazeSimulation";
    sim << "ReberGrammarSimulation (DetectGrammar)";
    sim << "ReberGrammarSimulation (CreateWords)";

    _sim_model->setStringList(sim);
}

CreateXML::~CreateXML()
{
    delete ui;
}

void CreateXML::check_button_enabled()
{
    ui->pushButton->setEnabled(ui->source_lineEdit->text() != "" && ui->target_lineEdit->text() != "");
}

void CreateXML::on_source_toolButton_clicked()
{
    QFileDialog dialog(this, tr("Select gene"), "", "Gene (*.gene)");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setDefaultSuffix("gene");
    if(dialog.exec() == QFileDialog::Accepted && dialog.selectedFiles()[0].length() > 0)
    {
        ui->source_lineEdit->setText(dialog.selectedFiles()[0]);
    }
    check_button_enabled();
}

void CreateXML::on_target_toolButton_clicked()
{
    QFileDialog dialog(this, tr("Select output file"), "", "XML-file (*.xml)");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("xml");
    if(dialog.exec() == QFileDialog::Accepted && dialog.selectedFiles()[0].length() > 0)
    {
        ui->target_lineEdit->setText(dialog.selectedFiles()[0]);
    }
    check_button_enabled();
}

void CreateXML::on_pushButton_clicked()
{
    QFile gene_file(ui->source_lineEdit->text());
    QFile network_file(ui->target_lineEdit->text());
    QString selection;

    GenericGene *gene = NULL;
    AbstractNeuralNetwork *network = NULL;
    GenericSimulation *simulation = NULL;

    if(GenericGene(1).canLoad(&gene_file))
    {
        gene = GenericGene::loadThisGene(&gene_file);
        if(gene == NULL)
        {
            show_error_message(tr("Problem while loading GenericGene"));
            return;
        }
    }
    if(LengthChangingGene(1).canLoad(&gene_file))
    {
        gene = LengthChangingGene::loadThisGene(&gene_file);
        if(gene == NULL)
        {
            show_error_message(tr("Problem while loading LengthChangingGene"));
            return;
        }
    }
    else
    {
        show_error_message(tr("Can not load gene"));
        return;
    }

    selection = ui->simView->currentIndex().data().toString();
    if(selection == "GenericSimulation")
    {
        simulation = new GenericSimulation();
    }
    else if(selection == "TMazeSimulation")
    {
        simulation = new TMazeSimulation();
    }
    else if(selection == "ReberGrammarSimulation (DetectGrammar)")
    {
        simulation = new ReberGrammarSimulation();
    }
    else if(selection == "ReberGrammarSimulation (CreateWords)")
    {
        ReberGrammarSimulation::config config;
        config.mode = ReberGrammarSimulation::CreateWords;
        simulation = new ReberGrammarSimulation(config);
    }
    else
    {
        show_error_message(QString(tr("Unknown simulation %1")).arg(selection));
        goto cleanup;
    }

    selection = ui->nnView->currentIndex().data().toString();
    if(selection == "GasNet")
    {
        network = new GasNet(simulation->needInputLength(), simulation->needOutputLength());
    }
    else if(selection == "ModulatedSpikingNeuronsNetwork")
    {
        network = new ModulatedSpikingNeuronsNetwork(simulation->needInputLength(), simulation->needOutputLength());
    }
    else
    {
        show_error_message(QString(tr("Unknown network %1")).arg(selection));
        goto cleanup;
    }

    network->initialise(gene);
    if(network->saveNetworkConfig(&network_file))
    {
        QMessageBox::information(this,
                             tr("Save successful"),
                             QString(tr("Network was successfully saved to:\n%1")).arg(ui->target_lineEdit->text()));
    }
    else
    {
        show_error_message(tr("Can not save network"));
    }

cleanup:
    delete gene;
    delete network;
    delete simulation;
}

void CreateXML::show_error_message(QString error)
{
    qWarning() << "Error:" << error;
    QMessageBox::warning(this,
                         tr("Error"),
                         QString(tr("An error occured:\n%1")).arg(error));
}
