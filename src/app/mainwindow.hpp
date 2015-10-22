/* See LICENSE file for copyright and license details. */

#ifndef __MAINWINDOW_HPP__0E042426_E73E_4382_BC86_DE7F000B57CC
#define __MAINWINDOW_HPP__0E042426_E73E_4382_BC86_DE7F000B57CC

#include <QMainWindow>
#include <QPainterPath>

#include <memory>

#include "../architecture.hpp"
#include "../cognitive_function.hpp"

class QResizeEvent;
class GraphicsNodeView;
class GraphicsNodeScene;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow();
    ~MainWindow();

    void set_active_scene(GraphicsNodeScene* scene);

   protected:
    virtual void resizeEvent(QResizeEvent *event);

   private slots:
    void on_actionAdd_node_triggered();
    void on_actionToJson_triggered();
    void on_actionFromJson_triggered();
    void onCogButtonTriggered(CognitiveFunction cognitive_function);

   private:
    void save(const std::string& file) const;

    std::string hierarchy_name(const std::string& name, GraphicsNodeScene* scene);
    void spawnInitialNodes();

    std::unique_ptr<Architecture> _root_arch;
    Architecture* _active_arch;

    Ui::MainWindow *ui;

    std::shared_ptr<GraphicsNodeView> _view;
    std::shared_ptr<GraphicsNodeScene> _root_scene;
    GraphicsNodeScene* _active_scene;

    QPainterPath _path;
};

#endif /* __MAINWINDOW_HPP__0E042426_E73E_4382_BC86_DE7F000B57CC */
