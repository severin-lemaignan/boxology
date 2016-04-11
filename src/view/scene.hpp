/* See LICENSE file for copyright and license details. */

#ifndef __GRAPHICSNODESCENE_HPP__7F9E4C1E_8F4E_4BD2_BDF7_3D4ECEC206B5
#define __GRAPHICSNODESCENE_HPP__7F9E4C1E_8F4E_4BD2_BDF7_3D4ECEC206B5

#include <set>
#include <memory>

#include <QRectF>
#include <QGraphicsScene>

#include "../architecture.hpp"
#include "../node.hpp"
#include "../connection.hpp"
#include "graphicsnode.hpp"
#include "edge.hpp"

class GraphicsNodeScene : public QGraphicsScene {
    Q_OBJECT

   public:
    GraphicsNodeScene(Architecture* architecture, 
                      GraphicsNode* parent_node = nullptr, 
                      QObject* parent = 0);

    std::shared_ptr<GraphicsNode> add(NodePtr node);
    void remove(std::shared_ptr<GraphicsNode> node);
    void remove(NodePtr node);
    std::shared_ptr<GraphicsDirectedEdge> add(ConnectionPtr connection);
    std::shared_ptr<GraphicsDirectedEdge> make_edge();

    std::set<std::shared_ptr<GraphicsNode>> selected() const;

    Architecture* architecture;
    GraphicsNode* parent_node;

    void set_description(const std::string& name, const std::string& version,
                         const std::string& desc);

    // slots
    void onConnectionEstablished(std::shared_ptr<GraphicsDirectedEdge> edge);
    void onConnectionDisrupted(std::shared_ptr<GraphicsDirectedEdge> edge);
    void onDescriptionChanged(const QString& content);

    bool dontGrabKeyPresses;

    void hideHelpers();
    void showHelpers();
    void disableGraphicsEffects();
    void enableGraphicsEffects();

   protected:
    virtual void drawBackground(QPainter* painter, const QRectF& rect) override;
    virtual void keyPressEvent(QKeyEvent* event) override;

   private:
    QColor _color_background;
    QColor _color_light;
    QColor _color_dark;
    QColor _color_null;
    QColor _color_bg_text;

    bool _paintBackground;

    QPen _pen_light;
    QPen _pen_dark;
    QPen _pen_null;

    QBrush _brush_background;

    EditableLabel* _arch_name;
    EditableLabel* _arch_version;
    EditableDescription* _arch_desc;

    std::set<std::shared_ptr<GraphicsNode>> _nodes;
    std::set<std::shared_ptr<GraphicsDirectedEdge>> _edges;
};

#endif /* __GRAPHICSNODESCENE_HPP__7F9E4C1E_8F4E_4BD2_BDF7_3D4ECEC206B5 */
