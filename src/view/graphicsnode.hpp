/* See LICENSE file for copyright and license details. */

#ifndef __GRAPHICSNODE_HPP__0707C377_95A2_4E0B_B98B_7E4813001982
#define __GRAPHICSNODE_HPP__0707C377_95A2_4E0B_B98B_7E4813001982

#include <vector>

#include <QPen>
#include <QBrush>
#include <QRectF>
#include <QPointF>
#include <QGraphicsObject>
#include <QVariant>
#include <QString>

#include "edge.hpp"
#include "graphicsnodedefs.hpp"
#include "../node.hpp"
#include "../architecture.hpp"

class QWidget;
class QPushButton;
class QGraphicsProxyWidget;
class QGraphicsSceneMouseEvent;
class QGraphicsDropShadowEffect;
class QGraphicsTextItem;
class GraphicsDirectedEdge;
class GraphicsNodeSocket;
class EditableLabel;
class TinyButton;
class GraphicsNodeScene;

class GraphicsNode : public QGraphicsObject {
    Q_OBJECT

    friend class GraphicsNodeScene;

   public:
    // only GraphicsNodeScene is authorized to create instances of GraphicsNode
    GraphicsNode(NodePtr node, QGraphicsObject *parent = nullptr);

   public:
    virtual ~GraphicsNode();

    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget = 0) override;

    const NodeWeakPtr node() { return _node; }

    std::shared_ptr<GraphicsNodeSocket> getPort(Port *port);

    void disconnect();

    int type() const override { return GraphicsNodeItemTypes::TypeNode; };

    qreal width() const { return _width; }

    qreal height() const { return _height; }

    void setTitle(const QString &title);
    QString title() const { return _title; }

    void setSize(const qreal width, const qreal height);
    void setSize(const QSizeF size);
    void setSize(const QPointF size);

    void setColors(const QColor &base);

    void hideHelpers();
    void showHelpers();
    void disableGraphicsEffects();
    void enableGraphicsEffects();

    /**
        * set a regular QWidget as central widget
        */
    void setCentralWidget(QWidget *widget);

    void refreshNode();
    void updateNode(QString name);
    void updateNodePos();

    void add_sink();
    void add_source();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override {
        QGraphicsItem::mousePressEvent(event);
    }
    virtual void mouseDoubleClickEvent(
        QGraphicsSceneMouseEvent *event) override;
    virtual QVariant itemChange(GraphicsItemChange change,
                                const QVariant &value) override;

   private:
    void updateGeometry();
    void updatePath();
    void updateSizeHints();
    void propagateChanges();

    std::shared_ptr<const GraphicsNodeSocket> add_socket(PortPtr port);

   private:
    NodeWeakPtr _node;

    // TODO: change pairs of sizes to QPointF, QSizeF, or quadrupels to QRectF

    const qreal _hard_min_width = 150.0;
    const qreal _hard_min_height = 120.0;

    qreal _min_width = 150.0;
    qreal _min_height = 120.0;

    const qreal _top_margin = 30.0;
    const qreal _bottom_margin = 15.0;
    const qreal _item_padding = 5.0;
    const qreal _lr_padding = 10.0;

    const qreal _pen_width = 1.0;
    const qreal _socket_size = 6.0;

    bool _changed;

    qreal _width;
    qreal _height;

    QPen _pen_default;
    QPen _pen_selected;
    QPen _pen_sources;
    QPen _pen_sinks;

    QBrush _brush_title;
    QBrush _brush_background;
    QBrush _brush_sources;
    QBrush _brush_sinks;

    QGraphicsDropShadowEffect *_effect;
    EditableLabel *_title_item;
    QGraphicsProxyWidget *_central_proxy = nullptr;

    TinyButton *_new_sink_btn;
    TinyButton *_new_source_btn;

    QString _title;

    std::vector<std::shared_ptr<GraphicsNodeSocket>> _sources;
    std::vector<std::shared_ptr<GraphicsNodeSocket>> _sinks;

    std::unique_ptr<Architecture> _sub_structure;
    std::unique_ptr<GraphicsNodeScene> _sub_structure_scene;

};

#endif /* __GRAPHICSNODE_HPP__0707C377_95A2_4E0B_B98B_7E4813001982 */
