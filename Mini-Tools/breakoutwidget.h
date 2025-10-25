#ifndef BREAKOUTWIDGET_H
#define BREAKOUTWIDGET_H

#include <QMouseEvent>
#include <QWidget>

namespace Ui {
class BreakoutWidget;
}

class BreakoutWidget : public QWidget {
    Q_OBJECT
private:
    struct Paddle {
        QRectF pos;
        double tar;
        double speed;
        bool isLeft;
        bool speedUp;
    };
    struct Ball {
        QPointF pos;
        QPointF speed;
    };
    struct Brick {
        QPoint pos;
        QColor color;
        int score;
        bool alive;
    };
    struct Fruit {
        QPointF pos;
        enum Type {
            Clone,
            Split,
            Add
        } type;
    };

public:
    explicit BreakoutWidget(QWidget* parent = nullptr);
    ~BreakoutWidget();

private:
    Ui::BreakoutWidget* ui;
    Paddle paddle;
    QList<Ball*> balls;
    QList<Brick*> bricks;
    QList<Fruit*> fruits;
    int score = 0;
    int lives = 3;
    bool playing = 0;

private:
    void init();
    void onStartBtnClicked();
    void onBackBtnClicked();
    void mousePressEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent*);
    void timerEvent(QTimerEvent*);
};

#endif // BREAKOUTWIDGET_H
