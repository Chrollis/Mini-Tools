#include "arithmeticwidget.h"
#include "ui_arithmeticwidget.h"
#include <QMessageBox>
#include <QSettings>

ArithmeticWidget::ArithmeticWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ArithmeticWidget)
{
    ui->setupUi(this);
    setWindowTitle("简单口算生成器");
    operators = new MyOperator[4];
    operators[Add] = { "Addition", "+", ui->addCheck, ui->addBox };
    operators[Sub] = { "Subtraction", "-", ui->subCheck, ui->subBox };
    operators[Mul] = { "Multiplication", "×", ui->mulCheck, ui->mulBox };
    operators[Div] = { "Division", "÷", ui->divCheck, ui->divBox };
    loadSettings();
    connect(ui->startBtn, &QPushButton::clicked, this, &ArithmeticWidget::onStartBtnClicked);
    connect(ui->handInBtn, &QPushButton::clicked, this, &ArithmeticWidget::onHandInBtnClicked);
    connect(ui->answerLineA, &QLineEdit::returnPressed, this, &ArithmeticWidget::onAnswerLineAReturnPressed);
    connect(ui->answerLineB, &QLineEdit::returnPressed, this, &ArithmeticWidget::onHandInBtnClicked);
}

ArithmeticWidget::~ArithmeticWidget()
{
    saveSettings();
    delete ui;
    delete[] operators;
}

void ArithmeticWidget::createQuestion()
{
    ui->answerLineA->clear();
    ui->answerLineB->clear();
    int max = 0;
    do {
        type = OperatorType(rand() % 4);
        if (operators[type].check->isChecked()) {
            max = pow(10, operators[type].box->currentIndex() + 1);
        }
    } while (max == 0);
    int a = 0, b = 0;
    do {
        a = rand() % max;
        b = rand() % max;
    } while (!isOperandsLegal(a, b));
    if (type != Div) {
        if (ui->negativeCheck->isChecked()) {
            if (rand() % 100 < 40) {
                a = -a;
            }
            if (rand() % 100 < 20) {
                b = -b;
            }
        }
        ui->divLabel->setText("<s>余</s>");
        ui->answerLineB->setText("???");
        ui->answerLineB->setReadOnly(1);
    } else {
        ui->divLabel->setText("余");
        ui->answerLineB->setText("");
        ui->answerLineB->setReadOnly(0);
    }
    ui->formerLabel->setText(QString::number(a));
    ui->latterLabel->setText(QString::number(b));
    ui->operatorLabel->setText(operators[type].signal);
    answers.clear();
    switch (type) {
    case Add:
        answers.append(QString::number(a + b));
        break;
    case Sub:
        answers.append(QString::number(a - b));
        break;
    case Mul:
        answers.append(QString::number(a * b));
        break;
    case Div:
        answers.append(QString::number(a / b));
        answers.append(QString::number(a % b));
        break;
    }
}

void ArithmeticWidget::answerRight()
{
    score += qMax(0, 10 * (1 + operators[type].box->currentIndex()) - timer.elapsed() / 1000);
    timer.restart();
    createQuestion();
}

void ArithmeticWidget::answerWrong()
{
    if (!ui->answerLineA->text().isEmpty() || type == Div && !ui->answerLineB->text().isEmpty()) {
        ui->answerLineA->clear();
        if (type == Div) {
            ui->answerLineB->clear();
        }
        score -= 5;
    }
}

void ArithmeticWidget::onStartBtnClicked()
{
    if (!started) {
        bool flag = true;
        for (int i = 0; i < 4; i++) {
            if (operators[i].check->isChecked()) {
                flag = false;
                break;
            }
        }
        if (flag) {
            QMessageBox::warning(this, "设置错误", "请至少启用一种运算！");
            return;
        }
        if (timer.isValid()) {
            timer.start();
        } else {
            timer.restart();
        }
        ui->startBtn->setText("停止");
        ui->answerLineA->setFocus();
        createQuestion();
        ui->negativeCheck->setEnabled(false);
        for (int i = 0; i < 4; i++) {
            operators[i].check->setEnabled(false);
            operators[i].box->setEnabled(false);
        }
        started = true;
    } else {
        ui->formerLabel->setText("??");
        ui->latterLabel->setText("??");
        ui->operatorLabel->setText("?");
        ui->answerLineA->clear();
        ui->divLabel->setText("<s>余</s>");
        ui->answerLineB->setText("???");
        ui->answerLineB->setReadOnly(1);
        ui->startBtn->setText("开始");
        ui->negativeCheck->setEnabled(true);
        for (int i = 0; i < 4; i++) {
            operators[i].check->setEnabled(true);
            operators[i].box->setEnabled(true);
        }
        started = false;
    }
}

void ArithmeticWidget::onHandInBtnClicked()
{
    if (ui->answerLineA->text().trimmed() == answers.front() && (type != Div || ui->answerLineB->text().trimmed() == answers.back())) {
        answerRight();
    } else {
        answerWrong();
    }
    ui->answerLineA->setFocus();
    ui->scoreLabel->setText("你目前有：" + QString::number(score) + "分");
    QSettings settings("./config.ini", QSettings::IniFormat);
    settings.setValue("arithmetic/score", score);
}

void ArithmeticWidget::onAnswerLineAReturnPressed()
{
    if (type != Div) {
        onHandInBtnClicked();
    } else {
        ui->answerLineB->setFocus();
    }
}

bool ArithmeticWidget::isOperandsLegal(int a, int b)
{
    switch (type) {
    case Add:
    case Mul:
        if (a == 0 && b == 0) {
            return false;
        }
    case Sub:
        if (a <= b && !ui->negativeCheck->isChecked()) {
            return false;
        }
    case Div:
        if (b == 1 || b == 0 || a <= b) {
            return false;
        }
    }
    return true;
}

void ArithmeticWidget::saveSettings()
{
    QSettings settings("./config.ini", QSettings::IniFormat);
    settings.setValue("arithmetic/score", score);
    settings.setValue("arithmetic/negative", ui->negativeCheck->isChecked());
    for (int i = 0; i < 4; i++) {
        settings.setValue("arithmetic/check/" + operators[i].name, operators[i].check->isChecked());
        settings.setValue("arithmetic/box/" + operators[i].name, operators[i].box->currentIndex());
    }
}

void ArithmeticWidget::loadSettings()
{
    QSettings settings("./config.ini", QSettings::IniFormat);
    score = settings.value("arithmetic/score").toInt();
    ui->negativeCheck->setChecked(settings.value("arithmetic/negative").toBool());
    ui->scoreLabel->setText("你目前有：" + QString::number(score) + "分");
    for (int i = 0; i < 4; i++) {
        operators[i].check->setChecked(settings.value("arithmetic/check/" + operators[i].name).toBool());
        operators[i].box->setCurrentIndex(settings.value("arithmetic/box/" + operators[i].name).toInt());
    }
}
