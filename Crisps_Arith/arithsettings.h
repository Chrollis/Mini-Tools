#ifndef ARITHSETTINGS_H
#define ARITHSETTINGS_H

#include <QDialog>

namespace Ui {
class ArithSettings;
}

class ArithSettings : public QDialog {
    Q_OBJECT

public:
    explicit ArithSettings(const QVector<bool>& operator_switches, const QVector<int>& operator_maximums, bool negative_switch, QWidget* parent = nullptr);
    ~ArithSettings();
    void save(QVector<bool>& operator_switches, QVector<int>& operator_maximums, bool& negative_switch);

private:
    Ui::ArithSettings* ui;
};

#endif // ARITHSETTINGS_H
