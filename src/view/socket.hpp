/* See LICENSE file for copyright and license details. */

#ifndef __GRAPHICSNODESOCKET_HPP__99275D3E_35A8_4D63_8E10_995E5DC83C8C
#define __GRAPHICSNODESOCKET_HPP__99275D3E_35A8_4D63_8E10_995E5DC83C8C

#include <memory>
#include <set>
#include <QString>
#include <QRectF>
#include <QGraphicsItem>
#include <QPen>
#include <QBrush>
#include <QPointF>
#include <QSizeF>

#include "editablelabel.hpp"
#include "graphicsnodedefs.hpp"
#include "../node.hpp"
#include "../connection.hpp"

class QGraphicsSceneMouseEvent;
class QGraphicsSceneDragDropEvent;
class GraphicsDirectedEdge;
class TinyButton;

/**
 * visual representation of a socket. the visual representation consists of a
 * circle for User Interaction and a label
 */
class GraphicsNodeSocket : public QObject, public QGraphicsItem {
    Q_OBJECT

   public:
    GraphicsNodeSocket(Socket socket, QGraphicsItem *parent = nullptr);
    virtual ~GraphicsNodeSocket();

    virtual QRectF boundingRect() const override;

    /*
    */
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget = 0) override;

    /**
     * add/remove an edge for this socket
     */
    void connect_edge(std::shared_ptr<GraphicsDirectedEdge> edge);
    void disconnect_edge(std::shared_ptr<GraphicsDirectedEdge> edge);
    void disconnect();
    bool is_connected_to(std::shared_ptr<GraphicsDirectedEdge> edge) const;

    std::set<std::shared_ptr<GraphicsDirectedEdge>> get_edges() {
        return _edges;
    }

    /**
     * notify the socket that its position has changed. this may be either
     * due to movement within the parent, or due to movement of the parent
     * within the parent's parent context.
     */
    void notifyPositionChange();

    /**
     * get the socket-type of this socket
     */
    Port::Direction socket_type() const;

    bool is_sink() const;
    bool is_source() const;

    QSizeF getSize() const;
    QSizeF getMinimalSize() const;

    /**
     * type of the class. usefull within a QGraphicsScene to distinguish
     * what is really behind a pointer
     */
    int type() const override { return GraphicsNodeItemTypes::TypeSocket; };

    /**
     * determine if a point is actually within the socket circle.
     */
    bool isInSocketCircle(const QPointF &p) const;

    // return the anchor position relative to the scene in which the socket
    // is living in
    QPointF sceneAnchorPos() const;

    Socket socket() const { return _socket; }

    void hideHelpers();
    void showHelpers();

   protected:
    // event handling
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

   private:
    void onDeletion();
    void placeLabel();

    void setPortName(const QString &name) {
        _socket.port.lock()->name = name.toStdString();
    }

   private:
    Socket _socket;

    const Port::Direction _socket_type;
    QPen _pen_circle;
    QPen _pen_unconnected_circle;
    const QPen _pen_text;
    const QBrush _brush_circle;
    EditableLabel *_text;
    TinyButton *_delete_button;

    /*
     * edges with which this socket is connected
     */
    std::set<std::shared_ptr<GraphicsDirectedEdge>> _edges;

   private:  // some constants. TODO: need to be defined somewhere else
             // (customizable?)
    const qreal _pen_width = 1.0;
    const qreal _circle_radius = 6.0;
    const qreal _text_offset = 3.0;

    const qreal _min_width = 30;
    const qreal _min_height = 12.0;
};

#endif /* __GRAPHICSNODESOCKET_HPP__99275D3E_35A8_4D63_8E10_995E5DC83C8C */
