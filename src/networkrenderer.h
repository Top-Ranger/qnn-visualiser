#ifndef NETWORKRENDERER_H
#define NETWORKRENDERER_H

#include <QWidget>
#include "neuron.h"
#include <QHash>

class NetworkRenderer : public QWidget
{
    Q_OBJECT
public:
    explicit NetworkRenderer(QWidget *parent = 0);
    void setNeuronHash(QHash<int, neuron> neuron_hash);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);

signals:

public slots:

private:
    QHash<int, neuron> _neuron_hash;
};

#endif // NETWORKRENDERER_H
