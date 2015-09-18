#include "networkrenderer.h"
#include <QPalette>
#include <QPainter>
#include <QPen>
#include <QBrush>

namespace {
static const int NEURON_SIZE = 15;
}

NetworkRenderer::NetworkRenderer(QWidget *parent) :
    QWidget(parent),
    _neuron_hash()
{
    setPalette(QPalette(QPalette::Background, Qt::white));
    setAutoFillBackground(true);
}

void NetworkRenderer::set_neuron_hash(QHash<int, neuron> neuron_hash)
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
    painter.setBrush(QBrush(Qt::SolidPattern));

    // TODO: Arrows instead of lines
    foreach (int i, _neuron_hash.keys())
    {
        foreach (int source, _neuron_hash[i].connections.keys())
        {
            if(_neuron_hash[i].connections[source] > 0)
            {
                painter.setPen(QPen(Qt::red));
            }
            else
            {
                painter.setPen(QPen(Qt::blue));
            }
            painter.drawLine(size*_neuron_hash[source].x, size*_neuron_hash[source].y, size*_neuron_hash[i].x, size*_neuron_hash[i].y);
        }
    }



    foreach (int i, _neuron_hash.keys())
    {
        painter.setPen(QPen(Qt::black));
        painter.drawEllipse(size*_neuron_hash[i].x - 0.5d*NEURON_SIZE, size*_neuron_hash[i].y - 0.5d*NEURON_SIZE, NEURON_SIZE, NEURON_SIZE);
        painter.setPen(QPen(Qt::white));
        QRect rect(size*_neuron_hash[i].x - 0.5d * NEURON_SIZE, size*_neuron_hash[i].y - 0.5d * NEURON_SIZE, size*_neuron_hash[i].x + 0.5d * NEURON_SIZE, size*_neuron_hash[i].y + 0.5d * NEURON_SIZE);
        painter.drawText(size*_neuron_hash[i].x - 0.5d * NEURON_SIZE, size*_neuron_hash[i].y - 0.5d * NEURON_SIZE, NEURON_SIZE, NEURON_SIZE, (int) Qt::AlignHCenter, QString("%1").arg(_neuron_hash[i].id), &rect);
    }
}
