#ifndef TICTACTOEWIDGET_H
#define TICTACTOEWIDGET_H

#include <QSoundEffect>
#include <QWidget>

namespace Ui {
class TictactoeWidget;
}

class TictactoeWidget : public QWidget {
    Q_OBJECT
private:
    enum Piece {
        Offensive,
        Nothing,
        Defensive
    };

public:
    explicit TictactoeWidget(QWidget* parent = nullptr);
    ~TictactoeWidget();

private:
    bool check(Piece whom);
    void play();
    void over(Piece whom);

private:
    Ui::TictactoeWidget* ui;
    QList<QSoundEffect*> sounds;
    Piece* cells;
    Piece who = Offensive;
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent* event);
    void onRestartBtnClicked();
    void addSound(QString path);
};

#endif // TICTACTOEWIDGET_H
