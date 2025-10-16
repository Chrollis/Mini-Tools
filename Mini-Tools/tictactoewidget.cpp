#include "tictactoewidget.h"
#include "ui_tictactoewidget.h"
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>

TictactoeWidget::TictactoeWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::TictactoeWidget)
{
    ui->setupUi(this);
    setWindowTitle("井字棋");
    cells = new Piece[9];
    for (int i = 0; i < 9; i++) {
        cells[i] = Nothing;
    }
    connect(ui->restartBtn, &QPushButton::clicked, this, &TictactoeWidget::onRestartBtnClicked);
}

TictactoeWidget::~TictactoeWidget()
{
    for (auto& sound : sounds) {
        delete sound;
    }
    delete[] cells;
    delete ui;
}

bool TictactoeWidget::check(Piece whom)
{
    int hasWho[9];
    for (int i = 0; i < 9; i++) {
        hasWho[i] = cells[i] == whom;
    }
    for (int i = 0; i < 3; i++) {
        if (hasWho[i * 3] + hasWho[i * 3 + 1] + hasWho[i * 3 + 2] == 3) {
            return true;
        }
        if (hasWho[i] + hasWho[i + 3] + hasWho[i + 6] == 3) {
            return true;
        }
    }
    if (hasWho[0] + hasWho[4] + hasWho[8] == 3) {
        return true;
    }
    if (hasWho[2] + hasWho[4] + hasWho[6] == 3) {
        return true;
    }
    return false;
}

void TictactoeWidget::play()
{
    bool flag = true;
    if (who == Offensive) {
        if (check(Defensive)) {
            over(Defensive);
        } else {
            for (int i = 0; i < 9; i++) {
                if (cells[i] == Nothing) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
                over(Nothing);
            }
        }
    } else if (who == Defensive) {
        if (check(Offensive)) {
            over(Offensive);
        } else {
            for (int i = 0; i < 9; i++) {
                if (cells[i] == Nothing) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
                over(Nothing);
            }
        }
    }
}

void TictactoeWidget::over(Piece whom)
{
    addSound("./Content/Tictactoe/win.wav");
    const QString overs[3] = { "先手获胜！", "平局！", "后手获胜！" };
    QMessageBox::information(this, "结束", overs[whom]);
    onRestartBtnClicked();
}

void TictactoeWidget::paintEvent(QPaintEvent*)
{
    int size = qMin(ui->chessBoard->width(), ui->chessBoard->height()) - 20;
    QPixmap pic(size, size);
    pic.fill();
    QPainter painter(&pic);
    QPen pen;
    pen.setWidth(1);
    painter.scale(size * 0.01, size * 0.01);
    painter.setRenderHint(QPainter::Antialiasing);
    pen.setColor(Qt::lightGray);
    painter.setPen(pen);
    painter.drawLine(34, 0, 34, 100);
    painter.drawLine(68, 0, 68, 100);
    painter.drawLine(0, 34, 100, 34);
    painter.drawLine(0, 68, 100, 68);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    int x, y;
    for (int i = 0; i < 9; i++) {
        x = i % 3;
        y = i / 3;
        QPoint center = { 16 + x * 34, 16 + y * 34 };
        if (cells[i] == Offensive) {
            painter.drawEllipse(center, 14, 14);
        } else if (cells[i] == Defensive) {
            painter.drawLine(center + QPoint(-12, -12), center + QPoint(12, 12));
            painter.drawLine(center + QPoint(12, -12), center + QPoint(-12, 12));
        }
    }
    ui->chessBoard->setPixmap(pic);
}

void TictactoeWidget::mousePressEvent(QMouseEvent* event)
{
    int size = qMin(ui->chessBoard->width(), ui->chessBoard->height()) - 20;
    double scale = size * 0.01;
    QPoint delta = { (ui->chessBoard->width() - size) / 2, (ui->chessBoard->height() - size) / 2 };
    QRect rect(ui->chessBoard->pos() + delta, QSize(size, size));
    if (rect.contains(event->pos())) {
        if (event->button() == Qt::LeftButton) {
            QPoint pos = event->pos() - rect.topLeft();
            int i = pos.x() / int(34 * scale) + pos.y() / int(34 * scale) * 3;
            if (i >= 0 && i < 9 && cells[i] == Nothing) {
                if (who == Offensive) {
                    cells[i] = Offensive;
                    who = Defensive;
                    addSound("./Content/Tictactoe/move.wav");
                    play();
                    update();
                } else if (who == Defensive) {
                    cells[i] = Defensive;
                    who = Offensive;
                    addSound("./Content/Tictactoe/move.wav");
                    play();
                    update();
                }
                for (auto it = sounds.begin(); it != sounds.end();) {
                    if (!(*it)->isPlaying()) {
                        delete *it;
                        it = sounds.erase(it);
                    } else {
                        it++;
                    }
                }
            }
        }
    }
}

void TictactoeWidget::onRestartBtnClicked()
{
    for (int i = 0; i < 9; i++) {
        cells[i] = Nothing;
    }
    who = Offensive;
    update();
}

void TictactoeWidget::addSound(QString path)
{
    QSoundEffect* sound = new QSoundEffect;
    sound->setSource(QUrl::fromLocalFile(path));
    sound->setLoopCount(1);
    sound->setVolume(1.0);
    sound->play();
    sounds.prepend(sound);
}
