/* See LICENSE file for copyright and license details. */

#ifndef __GRAPHICSBEZIEREDGE_HPP__95A023FB_C28F_48F5_9B9C_04F3DB5B7DB1
#define __GRAPHICSBEZIEREDGE_HPP__95A023FB_C28F_48F5_9B9C_04F3DB5B7DB1

#include <memory>

#include <QPen>
#include <QPoint>
#include <QPointF>
#include <QGraphicsPathItem>
#include "graphicsnodedefs.hpp"

#include "editablelabel.hpp"
#include "../connection.hpp"

class QGraphicsDropShadowEffect;
class QGraphicsSceneMouseEvent;
class GraphicsNode;
class GraphicsNodeSocket;

// TODO: move specific draw stuff out of the graphics-edge
//       this may actually lead to the proper data model for a data layer

class GraphicsDirectedEdge
    : public QObject,
      public QGraphicsPathItem,
      public std::enable_shared_from_this<GraphicsDirectedEdge> {
    Q_OBJECT

   public:
    // GraphicsDirectedEdge(qreal factor=0.5f);
    explicit GraphicsDirectedEdge();
    GraphicsDirectedEdge(QPoint start, QPoint stop, qreal factor = 0.5f);

    ~GraphicsDirectedEdge();

    ConnectionWeakPtr connection() const { return _connection; }

    void connect(GraphicsNode* n1, Port* source, GraphicsNode* n2, Port* sink);

    void connect_source(GraphicsNodeSocket* source);
    void connect_sink(GraphicsNodeSocket* sink);

    void disconnect();
    void disconnect_sink();
    void disconnect_source();

    // methods to manually set a position
    void set_start(QPoint p);
    void set_stop(QPoint p);

    void set_start(QPointF p) { set_start(p.toPoint()); }
    void set_stop(QPointF p) { set_stop(p.toPoint()); }

    // virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem *
    // option, QWidget * widget = 0);

    int type() const override { return GraphicsNodeItemTypes::TypeBezierEdge; }

    GraphicsNodeSocket* source() { return _source; }
    GraphicsNodeSocket* sink() { return _sink; }

    void setUnderlyingConnection(ConnectionWeakPtr c) { _connection = c; }

    void disableGraphicsEffects();
    void enableGraphicsEffects();

signals:
    void connectionEstablished(std::shared_ptr<GraphicsDirectedEdge> edge);
    void connectionDisrupted(std::shared_ptr<GraphicsDirectedEdge> edge);

   protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

   private:
    ConnectionWeakPtr _connection;

    EditableLabel* _label;

    bool _in_scene;

   protected:
    virtual void update_path() = 0;

    void placeLabel();

    void establishConnection();
    void setConnectionName(const QString& name);

    bool _is_connected;
    QPen _pen;
    QPen _active_pen;
    QGraphicsDropShadowEffect* _effect;
    QPoint _start;
    QPoint _stop;
    qreal _factor;

    qreal _label_max_width = 100;

    GraphicsNodeSocket* _source;
    GraphicsNodeSocket* _sink;
};

class GraphicsBezierEdge : public GraphicsDirectedEdge {
    virtual void paint(QPainter* painter,
                       const QStyleOptionGraphicsItem* option,
                       QWidget* widget = 0) override;
    int type() const override { return GraphicsNodeItemTypes::TypeBezierEdge; }

   protected:
    virtual void update_path() override;
};

#endif /* __GRAPHICSBEZIEREDGE_HPP__95A023FB_C28F_48F5_9B9C_04F3DB5B7DB1 */
