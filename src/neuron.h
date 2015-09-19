#ifndef NEURON_H
#define NEURON_H

#include <QHash>

struct neuron {
    int id;
    double x;
    double y;
    double gas_radius;
    QHash<int, double> connections;

    neuron();
};
#endif // NEURON_H
