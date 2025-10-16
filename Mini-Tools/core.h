#ifndef CORE_H
#define CORE_H

#include <QDockWidget>
#include <QMainWindow>

namespace Ui {
class Core;
}

class Core : public QMainWindow {
    Q_OBJECT

public:
    explicit Core(QWidget* parent = nullptr);
    ~Core();

private:
    Ui::Core* ui;
    QMap<QString, QWidget*> tabs;
    QMap<QWidget*, QDockWidget*> docks;
    void widgetClose(QWidget* widget);
    void onActionDockTriggered();
};

#endif // CORE_H
