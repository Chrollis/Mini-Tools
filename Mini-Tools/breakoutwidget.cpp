#include "breakoutwidget.h"
#include "cprlib.h"
#include "ui_breakoutwidget.h"
#include <QMessageBox>
#include <QPainter>

BreakoutWidget::BreakoutWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::BreakoutWidget)
{
    ui->setupUi(this);
    setWindowTitle("打砖块");
    startTimer(1000 / 30);
    init();
    ui->rightBtn->setEnabled(false);
    ui->leftBtn->setEnabled(false);
    connect(ui->startBtn, &QPushButton::clicked, this, &BreakoutWidget::onStartBtnClicked);
    connect(ui->leftBtn, &QPushButton::pressed, this, [&]() {paddle.isLeft=true;paddle.speedUp=true; });
    connect(ui->rightBtn, &QPushButton::pressed, this, [&]() {paddle.isLeft=false;paddle.speedUp=true; });
    connect(ui->leftBtn, &QPushButton::released, this, [&]() { paddle.speedUp = false; });
    connect(ui->rightBtn, &QPushButton::released, this, [&]() { paddle.speedUp = false; });
}

BreakoutWidget::~BreakoutWidget()
{
    delete ui;
}

void BreakoutWidget::init()
{
    paddle.pos = QRectF(150, 375, 100, 5);
    paddle.tar = 200;
    paddle.speed = 0;
    paddle.speedUp = false;
    paddle.isLeft = true;
    balls.clear();
    Ball* ball = new Ball;
    ball->pos = QPointF(200, 350);
    ball->speed = QPointF(0, -3);
    balls.append(ball);
    bricks.clear();
    for (int i = 0; i < 50; i++) {
        for (int j = 0; j < 40; j++) {
            Brick* brick = new Brick;
            brick->alive = true;
            brick->color = (i + j) % 2 ? QColor(200, 200, 200) : QColor(100, 100, 100);
            brick->score = (40 - j) / 4 + 1;
            brick->pos = { i, j };
            bricks.append(brick);
        }
    }
}

void BreakoutWidget::onStartBtnClicked()
{
    if (!playing) {
        playing = true;
        ui->startBtn->setText("暂停");
        ui->rightBtn->setEnabled(true);
        ui->leftBtn->setEnabled(true);
    } else {
        playing = false;
        ui->startBtn->setText("继续");
        ui->rightBtn->setEnabled(false);
        ui->leftBtn->setEnabled(false);
    }
}

void BreakoutWidget::mousePressEvent(QMouseEvent* event)
{
    if (playing && event->button() == Qt::LeftButton) {
        int size = qMin(ui->ground->width() / 2, ui->ground->height() / 2) - 10;
        double scale = size * 0.005;
        QPoint delta = { (ui->ground->width() - size * 2) / 2, (ui->ground->height() - size * 2) / 2 };
        QRect rect(ui->ground->pos() + delta, QSize(size * 2, 2 * size));
        if (rect.contains(event->pos())) {
            paddle.tar = (event->pos().x() - rect.x()) / scale;
        }
    }
}

void BreakoutWidget::paintEvent(QPaintEvent*)
{
    int size = qMin(ui->ground->width() / 2, ui->ground->height() / 2) - 10;
    QPixmap pic(size * 2, size * 2);
    pic.fill();
    QPainter painter(&pic);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.scale(size * 0.005, size * 0.005);
    painter.setBrush(Qt::white);
    painter.drawRoundedRect(paddle.pos, 1, 1);
    for (const Ball* const ball : std::as_const(balls)) {
        painter.drawEllipse(ball->pos, 2, 2);
    }
    painter.setPen(Qt::NoPen);
    for (const Brick* const brick : std::as_const(bricks)) {
        if (brick->alive) {
            painter.setBrush(brick->color);
            painter.drawRect(brick->pos.x() * 8, brick->pos.y() * 8, 8, 8);
        }
    }
    painter.setPen(Qt::SolidLine);
    for (const Fruit* const fruit : std::as_const(fruits)) {
        switch (fruit->type) {
        case Fruit::Clone:
            painter.setBrush(Qt::cyan);
            break;
        case Fruit::Split:
            painter.setBrush(Qt::yellow);
            break;
        }
        painter.drawEllipse(fruit->pos, 2, 2);
    }
    ui->ground->setPixmap(pic);
}

void BreakoutWidget::timerEvent(QTimerEvent*)
{
    if (playing) {
        if (paddle.tar < paddle.pos.left()) {
            paddle.pos.moveLeft(qMax(paddle.pos.left() - (paddle.pos.left() - paddle.tar) / 2, 0.1));
        } else if (paddle.tar > paddle.pos.right()) {
            paddle.pos.moveRight(qMin(paddle.pos.right() + (paddle.tar - paddle.pos.right()) / 2, 399.9));
        } else {
            if (paddle.speedUp) {
                if (paddle.isLeft) {
                    paddle.speed -= 0.5;
                } else {
                    paddle.speed += 0.5;
                }
            } else {
                paddle.speed -= paddle.speed / 2;
            }
            double nx = paddle.pos.center().x() + paddle.speed;
            if (nx >= 50.1 && nx <= 349.9) {
                paddle.pos.moveCenter(QPointF(nx, paddle.pos.center().y()));
                paddle.tar = paddle.pos.center().x();
            }
        }
        for (auto it = fruits.begin(); it != fruits.end();) {
            Fruit* fruit = *it;
            QRectF fruitRect(fruit->pos.x() - 2, fruit->pos.y() - 2, 4, 4);
            if (fruitRect.intersects(paddle.pos)) {
                Ball* ball;
                int length = 3;
                double degree;
                QList<Ball*> newBalls;
                switch (fruit->type) {
                case Fruit::Clone:
                    for (int i = 0; i < 2; i++) {
                        if (balls.size() >= 128) {
                            break;
                        }
                        ball = new Ball;
                        ball->pos = QPointF(200, 350);
                        degree = rand() % 360 * 2 * 3.1416 / 360;
                        ball->speed = QPointF(length * cos(degree), length * sin(degree));
                        balls.prepend(ball);
                    }
                    break;
                case Fruit::Split:
                    for (const Ball* const theBall : std::as_const(balls)) {
                        if (balls.size() + newBalls.size() >= 128) {
                            break;
                        }
                        for (int i = 0; i < 2; i++) {
                            ball = new Ball;
                            ball->pos = theBall->pos;
                            degree = rand() % 360 * 2 * 3.1416 / 360;
                            length = sqrt(theBall->speed.x() * theBall->speed.x() + theBall->speed.y() * theBall->speed.y());
                            ball->speed = QPointF(length * cos(degree), length * sin(degree));
                            newBalls.prepend(ball);
                        }
                    }
                    balls.append(newBalls);
                    break;
                }
                delete fruit;
                it = fruits.erase(it);
            } else if (fruit->pos.y() - 2 >= 400) {
                delete fruit;
                it = fruits.erase(it);
            } else {
                fruit->pos.ry() += 2;
                it++;
            }
        }
        for (auto it = balls.begin(); it != balls.end();) {
            Ball* ball = *it;
            ball->pos += ball->speed;
            if (ball->pos.y() - 2 >= 400) {
                delete ball;
                it = balls.erase(it);
                if (balls.empty()) {
                    if (lives-- > 0) {
                        ball = new Ball;
                        ball->pos = QPointF(200, 350);
                        ball->speed = QPointF(0, -3);
                        balls.append(ball);
                        ui->ballLbl->setText(QString("备用球：%1个").arg(lives));
                    } else {
                        QMessageBox::information(this, "结束", "你输了！总计" + ui->scoreLbl->text());
                        score = 0;
                        ui->scoreLbl->setText(QString("得分：%1").arg(score));
                        ui->ballLbl->setText(QString("备用球：%1个").arg(lives));
                        lives = 3;
                        init();
                        ui->startBtn->setText("开始");
                        ui->rightBtn->setEnabled(false);
                        ui->leftBtn->setEnabled(false);
                        playing = false;
                        return;
                    }
                }
                continue;
            } else {
                if (ball->pos.x() - 2 < 0) {
                    ball->pos.rx() = 2;
                    ball->speed.rx() *= -1;
                    addSound("Breakout/hit.wav");
                }
                if (ball->pos.y() - 2 < 0) {
                    ball->pos.ry() = 2;
                    ball->speed.ry() *= -1;
                    addSound("Breakout/hit.wav");
                }
                if (ball->pos.x() + 2 >= 400) {
                    ball->pos.rx() = 398;
                    ball->speed.rx() *= -1;
                    addSound("Breakout/hit.wav");
                }
                QRectF ballRect(ball->pos.x() - 2, ball->pos.y() - 2, 4, 4);
                if (paddle.pos.intersects(ballRect)) {
                    double hitPos = (ball->pos.x() - paddle.pos.x()) / paddle.pos.x();
                    ball->speed.rx() = 10 * (hitPos - 0.5);
                    ball->speed.ry() *= -1;
                    addSound("Breakout/paddle.wav");
                }
                for (auto at = bricks.begin(); at != bricks.end();) {
                    Brick* brick = *at;
                    QRectF brickRect(brick->pos.x() * 8, brick->pos.y() * 8, 8, 8);
                    if (!brick->alive) {
                        delete brick;
                        at = bricks.erase(at);
                    } else {
                        if (brickRect.intersects(ballRect)) {
                            brick->alive = false;
                            score += brick->score;
                            ui->scoreLbl->setText(QString("得分：%1").arg(score));
                            addSound("Breakout/brick.wav");
                            if (ballRect.right() > brickRect.left() && ballRect.left() < brickRect.left() && ball->speed.x() > 0) {
                                ball->speed.rx() *= -1;
                            } else if (ballRect.left() < brickRect.right() && ballRect.right() > brickRect.right() && ball->speed.x() < 0) {
                                ball->speed.rx() *= -1;
                            } else if (ballRect.bottom() > brickRect.top() && ballRect.top() < brickRect.top() && ball->speed.y() > 0) {
                                ball->speed.ry() *= -1;
                            } else if (ballRect.top() < brickRect.bottom() && ballRect.bottom() > brickRect.bottom() && ball->speed.y() < 0) {
                                ball->speed.ry() *= -1;
                            }
                            if (rand() % 100 > 95) {
                                Fruit* fruit = new Fruit;
                                fruit->pos = brickRect.center();
                                fruit->type = (Fruit::Type)(rand() % 2);
                                fruits.append(fruit);
                            }
                        }
                        at++;
                    }
                }
                if (ball->speed.x() > 4) {
                    ball->speed.rx() -= (ball->speed.x() - 4) / 2;
                }
                if (ball->speed.y() > 4) {
                    ball->speed.ry() -= (ball->speed.y() - 4) / 2;
                }
                it++;
            }
        }
        if (bricks.empty()) {
            lives += 3;
            ui->ballLbl->setText(QString("备用球：%1个").arg(lives));
            init();
            playing = false;
            ui->rightBtn->setEnabled(false);
            ui->leftBtn->setEnabled(false);
            ui->startBtn->setText("继续");
        }
    }
    update();
}
