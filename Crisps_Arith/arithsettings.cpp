#include "arithsettings.h"
#include "arithwidget.h"
#include "ui_arithsettings.h"
#include <QSettings>

ArithSettings::ArithSettings(const QVector<bool>& operator_switches, const QVector<int>& operator_maximums, bool negative_switch, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::ArithSettings)
{
    ui->setupUi(this);
    ui->addCheck->setChecked(operator_switches[Add]);
    ui->addBox->setCurrentIndex(operator_maximums[Add]);
    ui->subCheck->setChecked(operator_switches[Sub]);
    ui->subBox->setCurrentIndex(operator_maximums[Sub]);
    ui->mulCheck->setChecked(operator_switches[Mul]);
    ui->mulBox->setCurrentIndex(operator_maximums[Mul]);
    ui->divCheck->setChecked(operator_switches[Div]);
    ui->divBox->setCurrentIndex(operator_maximums[Div]);
    ui->negativeCheck->setChecked(negative_switch);
}

ArithSettings::~ArithSettings()
{
    delete ui;
}

void ArithSettings::save(QVector<bool>& operator_switches, QVector<int>& operator_maximums, bool& negative_switch)
{
    operator_switches = { ui->addCheck->isChecked(),
        ui->subCheck->isChecked(),
        ui->mulCheck->isChecked(),
        ui->divCheck->isChecked() };
    operator_maximums = { ui->addBox->currentIndex(),
        ui->subBox->currentIndex(),
        ui->mulBox->currentIndex(),
        ui->divBox->currentIndex() };
    negative_switch = ui->negativeCheck->isChecked();
    QSettings settings("Chrollis", "Crisps Arith");
    settings.setValue("switch/add", ui->addCheck->isChecked());
    settings.setValue("switch/sub", ui->subCheck->isChecked());
    settings.setValue("switch/mul", ui->mulCheck->isChecked());
    settings.setValue("switch/div", ui->divCheck->isChecked());
    settings.setValue("negativeSwitch", ui->negativeCheck->isChecked());
    settings.setValue("range/add", ui->addBox->currentIndex());
    settings.setValue("range/sub", ui->subBox->currentIndex());
    settings.setValue("range/mul", ui->mulBox->currentIndex());
    settings.setValue("range/div", ui->divBox->currentIndex());
}
