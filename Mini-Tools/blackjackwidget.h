#ifndef BLACKJACKWIDGET_H
#define BLACKJACKWIDGET_H

#include <QWidget>

namespace Ui {
class BlackjackWidget;
}

class BlackjackWidget : public QWidget {
    Q_OBJECT
private:
    enum CardSuit {
        Spade,
        Heart,
        Club,
        Diamond
    };
    struct Card {
        int suit;
        int rank;
        QImage img;
        Card(int suit, int rank);
    };

public:
    explicit BlackjackWidget(QWidget* parent = nullptr);
    ~BlackjackWidget();

private:
    Ui::BlackjackWidget* ui;
    QVector<Card> poker;
    QList<Card*> human;
    QList<Card*> robot;
    QImage cardBg;
    int currPos = 0;
    bool standed = false;

private:
    void hit(bool isHuman);
    int score(bool isHuman);
    int stand();
    void onNewBtnClicked();
    void onStandBtnClicked();
    void paintEvent(QPaintEvent*);
};

#endif // BLACKJACKWIDGET_H
