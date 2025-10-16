#include "deploywidget.h"
#include "ui_deploywidget.h"
#include <QLibraryInfo>
#include <QStandardPaths>

DeployWidget::DeployWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DeployWidget)
{
    ui->setupUi(this);
    setWindowTitle("Qt部署工具");
    loadSettings();
    if (windeployqtPath.isEmpty()) {
        findWindeployqt();
    }
    process = new QProcess(this);
    connect(ui->browseExeButton, &QPushButton::clicked, this, &DeployWidget::onBrowseExeButtonClicked);
    connect(ui->browseOutputButton, &QPushButton::clicked, this, &DeployWidget::onBrowseOutputButtonClicked);
    connect(ui->deployButton, &QPushButton::clicked, this, &DeployWidget::onDeployButtonClicked);
    connect(process, &QProcess::readyReadStandardOutput, this, &DeployWidget::onProcessOutput);
    connect(process, &QProcess::readyReadStandardError, this, &DeployWidget::onProcessOutput);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &DeployWidget::onProcessFinished);
}

DeployWidget::~DeployWidget()
{
    saveSettings();
    delete process;
    delete ui;
}

void DeployWidget::onBrowseExeButtonClicked()
{
    QString path = QFileDialog::getOpenFileName(this, "选择可执行文件", ui->exePathEdit->text(), "可执行文件 (*.exe)");
    if (!path.isEmpty()) {
        ui->exePathEdit->setText(QDir::toNativeSeparators(path));
        ui->outputDirEdit->setText(QDir::toNativeSeparators(QFileInfo(path).absolutePath()));
    }
}

void DeployWidget::onBrowseOutputButtonClicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择输出目录", ui->outputDirEdit->text());
    if (!dir.isEmpty()) {
        ui->outputDirEdit->setText(QDir::toNativeSeparators(dir));
    }
}

void DeployWidget::onDeployButtonClicked()
{
    if (windeployqtPath.isEmpty()) {
        QString path = QFileDialog::getOpenFileName(this, "选择windeployqt程序", "", "可执行文件 (*.exe)");
        if (path.endsWith("windeployqt.exe") || path.endsWith("windeployqt6.exe")) {
            windeployqtPath = path;
            ui->statusLabel->setText("找到windeployqt：" + QDir::toNativeSeparators(path));
            ui->deployButton->setText("开始部署");
        }
        if (windeployqtPath.isEmpty()) {
            QMessageBox::warning(this, "错误", "所选程序不是windeployqt工具!");
            return;
        }
    } else {
        QString exePath = ui->exePathEdit->text().trimmed();
        QString outputDir = ui->outputDirEdit->text().trimmed();
        if (exePath.isEmpty() || !QFile::exists(exePath)) {
            QMessageBox::warning(this, "错误", "请选择有效的可执行文件!");
            return;
        }
        if (outputDir.isEmpty() || !QDir(outputDir).exists()) {
            QMessageBox::warning(this, "错误", "请选择有效的输出目录!");
            return;
        }
        QStringList arguments;
        arguments << "--release";
        arguments << "--dir" << outputDir;
        arguments << exePath;
        if (ui->compilerRuntimeCheck->isChecked())
            arguments << "--compiler-runtime";
        if (ui->noTranslationsCheck->isChecked())
            arguments << "--no-translations";
        if (ui->noSystemD3DCheck->isChecked())
            arguments << "--no-system-d3d-compiler";
        if (ui->noOpenglSwCheck->isChecked())
            arguments << "--no-opengl-sw";
        if (ui->verboseCheck->isChecked())
            arguments << "--verbose" << "1";
        ui->logTextEdit->clear();
        logMessage(windeployqtPath + " " + arguments.join(" "));
        process->start(windeployqtPath, arguments);
        updateUI(true);
        saveSettings();
    }
}

void DeployWidget::onProcessOutput()
{
    logMessage(process->readAllStandardOutput());
    logMessage(process->readAllStandardError());
}

void DeployWidget::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    updateUI(false);
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        logMessage("部署成功完成!");
        QMessageBox::information(this, "完成", "DLL文件部署成功!");
    } else {
        logMessage(QString("部署失败! 退出代码: %1").arg(exitCode));
        QMessageBox::critical(this, "错误", "部署过程中发生错误!");
    }
}

void DeployWidget::findWindeployqt()
{
    QStringList possiblePaths = {
        QLibraryInfo::path(QLibraryInfo::BinariesPath) + "/windeployqt.exe",
        QDir::cleanPath(QCoreApplication::applicationDirPath() + "/../../../bin/windeployqt.exe"),
        QStandardPaths::findExecutable("windeployqt")
    };
    foreach (const QString& path, possiblePaths) {
        if (QFile::exists(path)) {
            windeployqtPath = path;
            ui->statusLabel->setText("找到windeployqt：" + QDir::toNativeSeparators(path));
            ui->deployButton->setText("开始部署");
            return;
        }
    }
}

void DeployWidget::saveSettings()
{
    QSettings settings("./config.ini", QSettings::IniFormat);
    settings.setValue("deploy/exePath", ui->exePathEdit->text());
    settings.setValue("deploy/outputDir", ui->outputDirEdit->text());
    settings.setValue("deploy/windeplpyPath", this->windeployqtPath);
    settings.setValue("deploy/compilerRuntime", ui->compilerRuntimeCheck->isChecked());
    settings.setValue("deploy/noTranslations", ui->noTranslationsCheck->isChecked());
    settings.setValue("deploy/noSystemD3D", ui->noSystemD3DCheck->isChecked());
    settings.setValue("deploy/noOpenglSw", ui->noOpenglSwCheck->isChecked());
    settings.setValue("deploy/verbose", ui->verboseCheck->isChecked());
}

void DeployWidget::loadSettings()
{
    QSettings settings("./config.ini", QSettings::IniFormat);
    ui->exePathEdit->setText(settings.value("deploy/exePath").toString());
    ui->outputDirEdit->setText(settings.value("deploy/outputDir").toString());
    this->windeployqtPath = settings.value("deploy/windeplpyPath").toString();
    if (!this->windeployqtPath.isEmpty()) {
        ui->statusLabel->setText("找到windeployqt工具：" + QDir::toNativeSeparators(windeployqtPath));
        ui->deployButton->setText("开始部署");
    }
    ui->compilerRuntimeCheck->setChecked(settings.value("deploy/compilerRuntime", true).toBool());
    ui->noTranslationsCheck->setChecked(settings.value("deploy/noTranslations", false).toBool());
    ui->noSystemD3DCheck->setChecked(settings.value("deploy/noSystemD3D", false).toBool());
    ui->noOpenglSwCheck->setChecked(settings.value("deploy/noOpenglSw", false).toBool());
    ui->verboseCheck->setChecked(settings.value("deploy/verbose", false).toBool());
}

void DeployWidget::logMessage(const QString& message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QStringList messageList = message.split("\n", Qt::SkipEmptyParts);
    for (const QString& str : std::as_const(messageList)) {
        ui->logTextEdit->appendPlainText("[" + timestamp + "] " + str);
    }
}

void DeployWidget::updateUI(bool running)
{
    ui->browseExeButton->setEnabled(!running);
    ui->browseOutputButton->setEnabled(!running);
    ui->deployButton->setEnabled(!running);
    ui->exePathEdit->setEnabled(!running);
    ui->outputDirEdit->setEnabled(!running);
    ui->compilerRuntimeCheck->setEnabled(!running);
    ui->noTranslationsCheck->setEnabled(!running);
    ui->noSystemD3DCheck->setEnabled(!running);
    ui->noOpenglSwCheck->setEnabled(!running);
    ui->verboseCheck->setEnabled(!running);
    if (running) {
        ui->deployButton->setText("运行中...");
    } else {
        ui->deployButton->setText("开始部署");
    }
}
