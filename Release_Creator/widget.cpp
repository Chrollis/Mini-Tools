#include "widget.h"
#include "ui_widget.h"
#include <QLibraryInfo>
#include <QStandardPaths>

Widget::Widget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , process(new QProcess(this))
{
    ui->setupUi(this);
    setWindowTitle("Qt 部署工具");

    loadSettings();

    connect(process, &QProcess::readyReadStandardOutput, this, &Widget::onProcessOutput);
    connect(process, &QProcess::readyReadStandardError, this, &Widget::onProcessOutput);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, &Widget::onProcessFinished);
}

Widget::~Widget()
{
    saveSettings();
    delete ui;
}
void Widget::findWindeployqt()
{
    QStringList possiblePaths = {
        QLibraryInfo::path(QLibraryInfo::BinariesPath) + "/windeployqt.exe",
        QDir::cleanPath(QCoreApplication::applicationDirPath() + "/../../../bin/windeployqt.exe"),
        QStandardPaths::findExecutable("windeployqt")
    };

    foreach (const QString& path, possiblePaths) {
        if (QFile::exists(path)) {
            windeployqtPath = path;
            ui->statusLabel->setText("找到 windeployqt: " + QDir::toNativeSeparators(path));
            return;
        }
    }

    ui->statusLabel->setText("未找到 windeployqt! 请手动指定");
    windeployqtPath.clear();
    QMessageBox msgBox(QMessageBox::Icon::Question, "手动指定", "未找到 windeployqt! 请手动指定", QMessageBox::StandardButton::Ok);
    if (msgBox.exec() == QMessageBox::Ok) {
        QString path = QFileDialog::getOpenFileName(
            this, "选择windeployqt文件",
            ui->exePathEdit->text(),
            "可执行文件 (*.exe)");
        if (!path.isEmpty()) {
            windeployqtPath = path;
            ui->statusLabel->setText("找到 windeployqt: " + QDir::toNativeSeparators(path));
            return;
        }
    }
}

void Widget::on_browseExeButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(
        this, "选择可执行文件",
        ui->exePathEdit->text(),
        "可执行文件 (*.exe)");
    if (!path.isEmpty()) {
        ui->exePathEdit->setText(QDir::toNativeSeparators(path));

        if (ui->outputDirEdit->text().isEmpty()) {
            ui->outputDirEdit->setText(QDir::toNativeSeparators(QFileInfo(path).absolutePath()));
        }
    }
}

void Widget::on_browseOutputButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(
        this, "选择输出目录",
        ui->outputDirEdit->text());
    if (!dir.isEmpty()) {
        ui->outputDirEdit->setText(QDir::toNativeSeparators(dir));
    }
}

void Widget::on_deployButton_clicked()
{
    if (windeployqtPath.isEmpty()) {
        findWindeployqt();
        if (windeployqtPath.isEmpty()) {
            QMessageBox::warning(this, "错误", "未找到 windeployqt 工具!");
            return;
        }
    }

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
    ui->logTextEdit->appendPlainText("> " + windeployqtPath + " " + arguments.join(" "));

    process->start(windeployqtPath, arguments);
    updateUI(true);
}

void Widget::onProcessOutput()
{
    ui->logTextEdit->appendPlainText(process->readAllStandardOutput());
    ui->logTextEdit->appendPlainText(process->readAllStandardError());
}

void Widget::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    updateUI(false);

    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        ui->logTextEdit->appendPlainText("\n部署成功完成!");
        QMessageBox::information(this, "完成", "DLL文件部署成功!");
    } else {
        ui->logTextEdit->appendPlainText(QString("\n部署失败! 退出代码: %1").arg(exitCode));
        QMessageBox::critical(this, "错误", "部署过程中发生错误!");
    }
}

void Widget::saveSettings()
{
    QSettings settings("Chrollis", "QtDeployTool");
    settings.setValue("exePath", ui->exePathEdit->text());
    settings.setValue("outputDir", ui->outputDirEdit->text());
    settings.setValue("windeplpyPath", this->windeployqtPath);
    settings.setValue("compilerRuntime", ui->compilerRuntimeCheck->isChecked());
    settings.setValue("noTranslations", ui->noTranslationsCheck->isChecked());
    settings.setValue("noSystemD3D", ui->noSystemD3DCheck->isChecked());
    settings.setValue("noOpenglSw", ui->noOpenglSwCheck->isChecked());
    settings.setValue("verbose", ui->verboseCheck->isChecked());
}

void Widget::loadSettings()
{
    QSettings settings("Chrollis", "QtDeployTool");
    ui->exePathEdit->setText(settings.value("exePath").toString());
    ui->outputDirEdit->setText(settings.value("outputDir").toString());
    this->windeployqtPath = settings.value("windeplpyPath").toString();
    if (this->windeployqtPath.isEmpty()) {
        ui->statusLabel->setText("未找到 windeployqt");
    } else {
        ui->statusLabel->setText("找到 windeployqt: " + QDir::toNativeSeparators(windeployqtPath));
    }
    ui->compilerRuntimeCheck->setChecked(settings.value("compilerRuntime", true).toBool());
    ui->noTranslationsCheck->setChecked(settings.value("noTranslations", false).toBool());
    ui->noSystemD3DCheck->setChecked(settings.value("noSystemD3D", false).toBool());
    ui->noOpenglSwCheck->setChecked(settings.value("noOpenglSw", false).toBool());
    ui->verboseCheck->setChecked(settings.value("verbose", false).toBool());
}

void Widget::updateUI(bool running)
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
