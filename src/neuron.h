#ifndef NEURON_H
#define NEURON_H

#include <QHash>

struct neuron {
    int id;
    double x;
    double y;
    double gas_radius;
    bool gas_emitting;
    QHash<int, double> connections;

    neuron();
};
#endif // NEURON_H
