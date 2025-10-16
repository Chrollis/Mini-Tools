#include "blackjackwidget.h"
#include "ui_blackjackwidget.h"
#include <QMessageBox>
#include <QPainter>

BlackjackWidget::BlackjackWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::BlackjackWidget)
{
    ui->setupUi(this);
    setWindowTitle("21点");
    for (int i = 0; i < 4; i++) {
        for (int j = 1; j <= 13; j++) {
            poker.push_back(Card(i, j));
        }
    }
    cardBg.load("./Content/Blackjack/Background.gif");
    hit(false);
    hit(false);
    connect(ui->newBtn, &QPushButton::clicked, this, &BlackjackWidget::onNewBtnClicked);
    connect(ui->hitBtn, &QPushButton::clicked, this, [&]() { hit(true); });
    connect(ui->standBtn, &QPushButton::clicked, this, &BlackjackWidget::onStandBtnClicked);
}

BlackjackWidget::~BlackjackWidget()
{
    delete ui;
}

void BlackjackWidget::hit(bool isHuman)
{
    if (currPos == 52) {
        QMessageBox::warning(this, "警告", "牌已发空！");
        return;
    }
    if (isHuman) {
        human.append(&poker[currPos++]);
    } else {
        robot.append(&poker[currPos++]);
    }
    update();
}

int BlackjackWidget::score(bool isHuman)
{
    int less = 0, more = 0;
    QList<Card*>* cards;
    if (isHuman) {
        cards = &human;
    } else {
        cards = &robot;
    }
    for (const Card* const card : std::as_const(*cards)) {
        less += qMin(10, card->rank);
        more += card->rank == 1 ? 11 : qMin(10, card->rank);
    }
    if (more > 21) {
        return less;
    } else {
        return more;
    }
}

int BlackjackWidget::stand()
{
    while (score(false) < 17) {
        hit(false);
    }
    int humanScore = score(true);
    int computerScore = score(false);
    if ((humanScore > 21 && computerScore > 21) || (humanScore == computerScore)) {
        return 0;
    }
    if ((humanScore <= 21 && computerScore > 21) || (abs(21 - humanScore) < abs(21 - computerScore))) {
        return 1;
    } else if ((humanScore > 21 && computerScore <= 21) || (abs(21 - humanScore) > abs(21 - computerScore))) {
        return 2;
    } else {
        return 0;
    }
}

void BlackjackWidget::onNewBtnClicked()
{
    poker.clear();
    human.clear();
    robot.clear();
    for (int i = 0; i < 4; i++) {
        for (int j = 1; j <= 13; j++) {
            poker.push_back(Card(i, j));
        }
    }
    standed = false;
    ui->labelRobot->setText("庄家的牌：");
    hit(false);
    hit(false);
    for (int i = 0; i < 500; i++) {
        std::swap(poker[rand() % 52], poker[rand() % 52]);
    }
}

void BlackjackWidget::onStandBtnClicked()
{
    int over = stand();
    ui->labelRobot->setText("庄家的牌：" + QString::number(score(false)) + "分");
    standed = true;
    update();
    const QString overs[3] = { "平局！", "你赢了！", "你输了！" };
    QMessageBox::information(this, "结束", overs[over]);
    onNewBtnClicked();
}

void BlackjackWidget::paintEvent(QPaintEvent*)
{
    int width = ui->labelRobot->width() / 5 - 5;
    int height = width * 1.34;
    QPoint basePos = ui->labelRobot->pos() + ui->labelRobot->rect().bottomLeft();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    int index = 0;
    for (const Card* const card : std::as_const(robot)) {
        if (!standed && index <= 1) {
            painter.drawImage(basePos + QPoint((index % 5) * (width + 5), (index / 5) * (height + 5) + 5),
                cardBg.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            painter.drawImage(basePos + QPoint((index % 5) * (width + 5), (index / 5) * (height + 5) + 5),
                card->img.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        index += 1;
    }
    basePos = ui->labelPlayer->pos() + ui->labelPlayer->rect().bottomLeft();
    index = 0;
    for (const Card* const card : std::as_const(human)) {
        painter.drawImage(basePos + QPoint((index % 5) * (width + 5), (index / 5) * (height + 5) + 5),
            card->img.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        index += 1;
    }
    ui->labelPlayer->setText("你的牌：" + QString::number(score(true)) + "分");
}

BlackjackWidget::Card::Card(int suit, int rank)
{
    const QString suits[4] = { "Spade", "Heart", "Club", "Diamond" };
    const QString rands[13] = { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };
    this->suit = suit;
    this->rank = rank;
    img.load("./Content/Blackjack/" + suits[suit] + rands[rank - 1] + ".gif");
}
