#ifndef WIDGET_H
#define WIDGET_H

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget {
    Q_OBJECT

public:
    Widget(QWidget* parent = nullptr);
    ~Widget();

private slots:
    void on_browseExeButton_clicked();
    void on_browseOutputButton_clicked();
    void on_deployButton_clicked();
    void onProcessOutput();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    Ui::Widget* ui;
    QProcess* process;
    QString windeployqtPath;
    void findWindeployqt();
    void saveSettings();
    void loadSettings();
    void updateUI(bool running);
};
#endif // WIDGET_H
