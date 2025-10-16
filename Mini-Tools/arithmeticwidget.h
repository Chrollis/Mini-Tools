#ifndef ARITHMETICWIDGET_H
#define ARITHMETICWIDGET_H

#include <QCheckBox>
#include <QComboBox>
#include <QElapsedTimer>
#include <QWidget>

namespace Ui {
class ArithmeticWidget;
}

class ArithmeticWidget : public QWidget {
    Q_OBJECT
private:
    enum OperatorType {
        Add,
        Sub,
        Mul,
        Div
    };
    struct MyOperator {
        QString name;
        QString signal;
        QCheckBox* check;
        QComboBox* box;
    };

public:
    explicit ArithmeticWidget(QWidget* parent = nullptr);
    ~ArithmeticWidget();

private:
    void createQuestion();
    void answerRight();
    void answerWrong();
    void onStartBtnClicked();
    void onHandInBtnClicked();
    void onAnswerLineAReturnPressed();
    bool isOperandsLegal(int a, int b);

private:
    Ui::ArithmeticWidget* ui;
    MyOperator* operators;
    QStringList answers;
    OperatorType type = Add;
    int score = 0;
    bool started = 0;
    QElapsedTimer timer;
    void saveSettings();
    void loadSettings();
};

#endif // ARITHMETICWIDGET_H
