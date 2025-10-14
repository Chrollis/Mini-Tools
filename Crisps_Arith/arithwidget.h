#ifndef ARITHWIDGET_H
#define ARITHWIDGET_H

#include <QElapsedTimer>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class ArithWidget;
}
QT_END_NAMESPACE

enum OperatorType {
    Add,
    Sub,
    Mul,
    Div
};

const QString operatorList[4] = {
    "+",
    "-",
    "×",
    "÷"
};

class ArithWidget : public QWidget {
    Q_OBJECT

public:
    ArithWidget(QWidget* parent = nullptr);
    ~ArithWidget();

private:
    void question();
    void start();
    void hand_in();

private:
    QVector<bool> operator_switches;
    bool negative_switch;
    bool is_started = 0;
    QVector<int> operator_maximums;

private:
    Ui::ArithWidget* ui;
    QStringList answers;
    OperatorType type;
    int m_score = 0;
    QElapsedTimer m_timer;
};
#endif // ARITHWIDGET_H
