/* See LICENSE file for copyright and license details. */

#ifndef __GRAPHICSNODESCENE_HPP__7F9E4C1E_8F4E_4BD2_BDF7_3D4ECEC206B5
#define __GRAPHICSNODESCENE_HPP__7F9E4C1E_8F4E_4BD2_BDF7_3D4ECEC206B5

#include <set>
#include <memory>

#include <QRectF>
#include <QGraphicsScene>

#include "architecture.hpp"
#include "node.hpp"
#include "connection.hpp"
#include "graphicsnode.hpp"
#include "graphicsbezieredge.hpp"

class GraphicsNodeScene : public QGraphicsScene
{
Q_OBJECT

public:
    GraphicsNodeScene(Architecture* architecture, QObject *parent);

    std::shared_ptr<GraphicsNode> add(NodePtr node);
    std::shared_ptr<GraphicsDirectedEdge> add(ConnectionPtr connection);

    std::set<std::shared_ptr<GraphicsNode>> selected();

signals:
    void nodeDeleted(NodePtr node) const;

protected:
    virtual void drawBackground(QPainter *painter, const QRectF &rect) override;
    virtual void keyPressEvent(QKeyEvent *event) override;

private:


    QColor _color_background;
    QColor _color_light;
    QColor _color_dark;
    QColor _color_null;

    QPen _pen_light;
    QPen _pen_dark;
    QPen _pen_null;

    QBrush _brush_background;

    Architecture* architecture;

    std::set<std::shared_ptr<GraphicsNode>> _nodes;
    std::set<std::shared_ptr<GraphicsBezierEdge>> _edges;
};

#endif /* __GRAPHICSNODESCENE_HPP__7F9E4C1E_8F4E_4BD2_BDF7_3D4ECEC206B5 */
