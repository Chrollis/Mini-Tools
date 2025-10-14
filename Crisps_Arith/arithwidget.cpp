#include "arithwidget.h"
#include "./ui_arithwidget.h"
#include "arithsettings.h"
#include <QRandomGenerator>
#include <QSettings>

ArithWidget::ArithWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ArithWidget)
{
    ui->setupUi(this);
    connect(ui->startBtn, &QPushButton::clicked, this, &ArithWidget::start);
    connect(ui->handInBtn, &QPushButton::clicked, this, &ArithWidget::hand_in);
    connect(ui->answerLineA, &QLineEdit::returnPressed, this, [&]() {
        if (type != Div) {
            hand_in();
        } else {
            ui->answerLineB->setFocus();
        }
    });
    connect(ui->answerLineB, &QLineEdit::returnPressed, this, &ArithWidget::hand_in);
    connect(ui->settingsBtn, &QPushButton::clicked, this, [&]() {
        ArithSettings* settings_dialog = new ArithSettings(operator_switches, operator_maximums, negative_switch);
        if (settings_dialog->exec() == QDialog::Accepted) {
            settings_dialog->save(operator_switches, operator_maximums, negative_switch);
            question();
        }
        delete settings_dialog;
    });
    QSettings settings("Chrollis", "Crisps Arith");
    m_score = settings.value("score", 0).toInt();
    operator_switches = { settings.value("switch/add", 1).toBool(),
        settings.value("switch/sub", 1).toBool(),
        settings.value("switch/mul", 0).toBool(),
        settings.value("switch/div", 0).toBool() };
    negative_switch = settings.value("negativeSwitch", 0).toBool();
    operator_maximums = { settings.value("range/add", 0).toInt(),
        settings.value("range/sub", 0).toInt(),
        settings.value("range/mul", 0).toInt(),
        settings.value("range/div", 0).toInt() };
    ui->scoreLabel->setText(QString("你目前有：%1分").arg(m_score));
}

ArithWidget::~ArithWidget()
{
    delete ui;
}

void ArithWidget::question()
{
    ui->answerLineA->clear();
    ui->answerLineB->clear();
    int max_num = 0;
    do {
        type = OperatorType(QRandomGenerator::global()->bounded(4));
        if (operator_switches[type]) {
            max_num = pow(10, operator_maximums[type] + 1);
        }
    } while (!max_num);
    int a, b;
    do {
        a = QRandomGenerator::global()->bounded(max_num);
        b = QRandomGenerator::global()->bounded(max_num);
    } while ([=]() -> bool {
        switch (type) {
        case Add:
        case Mul:
            return 0;
        case Sub:
            if (a <= b && !negative_switch) {
                return 1;
            } else {
                return 0;
            }
        case Div:
            if (b == 1 || b == 0 || a <= b) {
                return 1;
            } else {
                return 0;
            }
        }
        return 1;
    }());
    if (type != Div) {
        if (negative_switch) {
            if (QRandomGenerator::global()->bounded(100) < 40) {
                a = -a;
            }
            if (QRandomGenerator::global()->bounded(100) < 20) {
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
    ui->operatorLabel->setText(operatorList[type]);
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

void ArithWidget::start()
{
    if (!is_started) {
        if (m_timer.isValid()) {
            m_timer.start();
        } else {
            m_timer.restart();
        }
        ui->startBtn->setText("停止");
        ui->answerLineA->setFocus();
        question();
    } else {
        ui->formerLabel->setText("??");
        ui->latterLabel->setText("??");
        ui->operatorLabel->setText("?");
        ui->answerLineA->clear();
        ui->divLabel->setText("<s>余</s>");
        ui->answerLineB->setText("???");
        ui->answerLineB->setReadOnly(1);
        ui->startBtn->setText("开始");
    }
    is_started = !is_started;
}

void ArithWidget::hand_in()
{
    if (type != Div) {
        if (ui->answerLineA->text().trimmed() == answers[0]) {
            m_score += qMax(0, 10 * (1 + operator_maximums[type]) - m_timer.elapsed() / 1000);
            m_timer.restart();
            question();
        } else {
            if (!ui->answerLineA->text().isEmpty()) {
                ui->answerLineA->clear();
                m_score -= 5;
            }
        }
    } else {
        if (ui->answerLineA->text().trimmed() == answers[0] && ui->answerLineB->text().trimmed() == answers[1]) {
            m_score += qMax(0, 10 * (1 + operator_maximums[Div]) - m_timer.elapsed() / 1000);
            m_timer.restart();
            question();
        } else {
            if (!ui->answerLineA->text().isEmpty() || !ui->answerLineB->text().isEmpty()) {
                ui->answerLineA->clear();
                ui->answerLineB->clear();
                m_score -= 5;
            }
        }
    }
    ui->answerLineA->setFocus();
    ui->scoreLabel->setText(QString("你目前有：%1分").arg(m_score));
    QSettings settings("Chrollis", "Crisps Arith");
    settings.setValue("score", m_score);
}
