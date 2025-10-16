#ifndef DEPLOYWIDGET_H
#define DEPLOYWIDGET_H

#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QWidget>

namespace Ui {
class DeployWidget;
}

class DeployWidget : public QWidget {
    Q_OBJECT

public:
    explicit DeployWidget(QWidget* parent = nullptr);
    ~DeployWidget();

private:
    void onBrowseExeButtonClicked();
    void onBrowseOutputButtonClicked();
    void onDeployButtonClicked();
    void onProcessOutput();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    Ui::DeployWidget* ui;
    QProcess* process;
    QString windeployqtPath;
    void findWindeployqt();
    void saveSettings();
    void loadSettings();
    void logMessage(const QString& message);
    void updateUI(bool running);
};

#endif // DEPLOYWIDGET_H
