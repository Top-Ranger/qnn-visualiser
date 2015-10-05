#include "networkrenderer.h"
#include <QPalette>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QtCore/qmath.h>
#include <QPolygon>
#include <QPoint>

namespace {
static const int NEURON_SIZE = 15;
static const int ARROW_SIZE = 7;
static const int RECURRENT_SIZE = 20;
}

NetworkRenderer::NetworkRenderer(QWidget *parent) :
    QWidget(parent),
    _neuron_hash()
{
    setPalette(QPalette(QPalette::Background, Qt::white));
    setAutoFillBackground(true);
}

void NetworkRenderer::setNeuronHash(QHash<int, neuron> neuron_hash)
{
    _neuron_hash = neuron_hash;
    repaint();
}

QSize NetworkRenderer::minimumSizeHint() const
{
    return QSize(500,500);
}

QSize NetworkRenderer::sizeHint() const
{
    return QSize(500,500);
}

void NetworkRenderer::paintEvent(QPaintEvent *event)
{
    // It is not important why we repaint so we can ignore the event
    Q_UNUSED(event);

    double size = qMin(height(), width());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(QBrush(Qt::yellow));

    // Draw gas radius
    foreach (int i, _neuron_hash.keys())
    {
        if(_neuron_hash[i].gas_emitting)
        {
            painter.setPen(QPen(Qt::yellow));
            painter.setBrush(QBrush(Qt::yellow));
            double radius = size * _neuron_hash[i].gas_radius;
            painter.drawEllipse(size*_neuron_hash[i].x - 0.5d*radius, size*_neuron_hash[i].y - 0.5d*radius, radius, radius);
        }
    }

    // Draw connections
    foreach (int i, _neuron_hash.keys())
    {
        foreach (int source, _neuron_hash[i].connections.keys())
        {
            if(_neuron_hash[i].connections[source] > 0)
            {
                painter.setPen(QPen(Qt::red));
                painter.setBrush(QBrush(Qt::red));
            }
            else
            {
                painter.setPen(QPen(Qt::blue));
                painter.setBrush(QBrush(Qt::blue));
            }

            if(_neuron_hash[i].id == _neuron_hash[source].id)
            {
                painter.drawArc(size*_neuron_hash[i].x, size*_neuron_hash[i].y, RECURRENT_SIZE, RECURRENT_SIZE, 0, 5760);
            }
            else
            {
                double a = size*_neuron_hash[i].x - size*_neuron_hash[source].x;
                double b = size*_neuron_hash[i].y - size*_neuron_hash[source].y;
                double c = qSqrt(qPow(size*_neuron_hash[i].x - size*_neuron_hash[source].x, 2) + qPow(size*_neuron_hash[i].y - size*_neuron_hash[source].y, 2));
                double n0_a = 1 / c * a;
                double n0_b = 1 / c * b;
                double x_arrow = n0_a * (c - 0.5d * NEURON_SIZE) + size*_neuron_hash[source].x;
                double y_arrow = n0_b * (c - 0.5d * NEURON_SIZE) + size*_neuron_hash[source].y;
                double arrow_vektor_a = ARROW_SIZE;
                double arrow_vektor_b = (-1) * ARROW_SIZE * (n0_a / n0_b);
                painter.drawLine(size*_neuron_hash[source].x, size*_neuron_hash[source].y, size*_neuron_hash[i].x, size*_neuron_hash[i].y);

                // Scale arrow heads down
                while(qAbs(arrow_vektor_b) > ARROW_SIZE)
                {
                    arrow_vektor_a /= 2;
                    arrow_vektor_b /= 2;
                }
                QPolygon arrowhead;
                arrowhead << QPoint(x_arrow, y_arrow);
                arrowhead << QPoint(n0_a * (c - 0.5d * NEURON_SIZE - ARROW_SIZE) + size*_neuron_hash[source].x + arrow_vektor_a, n0_b * (c - 0.5d * NEURON_SIZE - ARROW_SIZE) + size*_neuron_hash[source].y + arrow_vektor_b);
                arrowhead << QPoint(n0_a * (c - 0.5d * NEURON_SIZE - ARROW_SIZE) + size*_neuron_hash[source].x - arrow_vektor_a, n0_b * (c - 0.5d * NEURON_SIZE - ARROW_SIZE) + size*_neuron_hash[source].y - arrow_vektor_b);
                painter.drawPolygon(arrowhead);
            }
        }
    }


    // Draw neurons
    foreach (int i, _neuron_hash.keys())
    {
        painter.setPen(QPen(Qt::black));
        painter.setBrush(QBrush(Qt::black));
        painter.drawEllipse(size*_neuron_hash[i].x - 0.5d*NEURON_SIZE, size*_neuron_hash[i].y - 0.5d*NEURON_SIZE, NEURON_SIZE, NEURON_SIZE);
        painter.setPen(QPen(Qt::white));
        QRect rect(size*_neuron_hash[i].x - 0.5d * NEURON_SIZE, size*_neuron_hash[i].y - 0.5d * NEURON_SIZE, size*_neuron_hash[i].x + 0.5d * NEURON_SIZE, size*_neuron_hash[i].y + 0.5d * NEURON_SIZE);
        painter.drawText(size*_neuron_hash[i].x - 0.5d * NEURON_SIZE, size*_neuron_hash[i].y - 0.5d * NEURON_SIZE, NEURON_SIZE, NEURON_SIZE, (int) Qt::AlignHCenter, QString("%1").arg(_neuron_hash[i].id), &rect);
    }
}
