#include "chatformatwidget.h"
#include "cprlib.h"
#include "ui_chatformatwidget.h"
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTextStream>

ChatFormatWidget::ChatFormatWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ChatFormatWidget)
{
    ui->setupUi(this);
    setWindowTitle("对话整理工具");
    loadSettings();
    logMessage("对话整理工具已启动");
    logMessage("请选择输入文件并设置参数");
    logMessage("提示：分隔符支持转义字符，如\\n表示换行，\\t表示制表符");

    connect(ui->browseButton, &QPushButton::clicked, this, &ChatFormatWidget::onBrowseButtonClicked);
    connect(ui->formatButton, &QPushButton::clicked, this, &ChatFormatWidget::onFormatButtonClicked);
    connect(ui->resetSeparatorsButton, &QPushButton::clicked, this, &ChatFormatWidget::onResetSeparatorsButtomClicked);
}

ChatFormatWidget::~ChatFormatWidget()
{
    saveSettings();
    delete ui;
}

void ChatFormatWidget::onBrowseButtonClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "选择对话文件", "", "文本文件 (*.txt);;所有文件 (*.*)");
    if (!filePath.isEmpty()) {
        ui->filePathEdit->setText(filePath);
        logMessage("已选择文件：" + filePath);
    }
}

void ChatFormatWidget::onFormatButtonClicked()
{
    QString filePath = ui->filePathEdit->text().trimmed();
    myName = ui->myNameEdit->text().trimmed();
    aiName = ui->aiNameEdit->text().trimmed();
    blockSeparator = ui->blockSeparatorEdit->text().trimmed();
    aiHumanSeparator = ui->aiHumanSeparatorEdit->text().trimmed();
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请选择对话文件！");
        return;
    }
    if (myName.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入您的名字！");
        return;
    }
    if (aiName.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入AI的名字！");
        return;
    }
    if (myName == aiName) {
        QMessageBox::warning(this, "输入错误", "您和AI的名字不能相同！");
        return;
    }
    if (aiHumanSeparator.contains("\\n")) {
        QMessageBox::warning(this, "输入错误", "AI/人类分隔符不能包含'\\n'！");
        return;
    }
    if (!blockSeparator.isEmpty() || !aiHumanSeparator.isEmpty()) {
        if (blockSeparator == aiHumanSeparator) {
            QMessageBox::warning(this, "输入错误", "段分隔符与AI/人类分隔符不能相同！");
            return;
        }
    }
    QFile file(filePath);
    if (!file.exists()) {
        QMessageBox::critical(this, "文件错误", "选择的文件不存在！");
        return;
    }
    ui->formatButton->setEnabled(false);
    logMessage("开始处理对话文件...");
    logMessage("您的名字：" + myName);
    logMessage("AI的名字：" + aiName);
    logMessage("段间分隔符：" + (blockSeparator.isEmpty() ? "(默认：\\n\\n)" : blockSeparator));
    logMessage("AI/人类分隔符：" + (aiHumanSeparator.isEmpty() ? "(默认：edit)" : aiHumanSeparator));
    blockSeparator.replace("\\n", "\n").replace("\\t", "\t").replace("\\r", "\r");
    aiHumanSeparator.replace("\\n", "\n").replace("\\t", "\t").replace("\\r", "\r");
    blockSeparator = blockSeparator.isEmpty() ? "\n\n" : blockSeparator;
    aiHumanSeparator = aiHumanSeparator.isEmpty() ? "edit" : aiHumanSeparator;
    formatDialogue(filePath);
    logMessage("对话整理完成！");
    QMessageBox::information(this, "完成", "对话整理完成！");
    saveSettings();
    ui->formatButton->setEnabled(true);
}

void ChatFormatWidget::onResetSeparatorsButtomClicked()
{
    loadDefaultSeparators();
    saveSettings();
    logMessage("已恢复默认分隔符设置");
}

QStringList ChatFormatWidget::processDialogue(const QString& dialogue, bool hasEdit)
{
    QStringList result;
    if (hasEdit) {
        int editPos = dialogue.indexOf(aiHumanSeparator + "\n");
        QString userPart = dialogue.left(editPos);
        QString aiPart = dialogue.mid(editPos + aiHumanSeparator.length());
        QStringList userLines = userPart.split('\n', Qt::SkipEmptyParts);
        for (const QString& line : std::as_const(userLines)) {
            QString trimmedLine = line.trimmed();
            if (!trimmedLine.isEmpty()) {
                result.append(myName + "：" + trimmedLine);
            }
        }
        QStringList aiLines = aiPart.split('\n', Qt::SkipEmptyParts);
        for (const QString& line : std::as_const(aiLines)) {
            QString trimmedLine = line.trimmed();
            if (!trimmedLine.isEmpty()) {
                result.append(aiName + "：" + trimmedLine);
            }
        }
    } else {
        QStringList lines = dialogue.split('\n', Qt::SkipEmptyParts);
        for (const QString& line : std::as_const(lines)) {
            QString trimmedLine = line.trimmed();
            if (!trimmedLine.isEmpty()) {
                result.append(aiName + "：" + trimmedLine);
            }
        }
    }
    return result;
}

void ChatFormatWidget::formatDialogue(const QString& filePath)
{
    QFile inputFile(filePath);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("无法打开输入文件！");
    }
    QTextStream in(&inputFile);
    in.setLocale(QLocale("UTF-8"));
    QString fileContent = in.readAll();
    inputFile.close();
    QString outputFilePath = filePath;
    int dotPos = outputFilePath.lastIndexOf('.');
    if (dotPos != -1) {
        outputFilePath = outputFilePath.left(dotPos) + "_formatted" + outputFilePath.mid(dotPos);
    } else {
        outputFilePath += "_formatted";
    }
    QFile outputFile(outputFilePath);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw std::runtime_error("无法创建输出文件！");
    }
    QTextStream out(&outputFile);
    out.setLocale(QLocale("UTF-8"));
    QStringList dialogueBlocks = fileContent.split(blockSeparator + "\n");
    logMessage("检测到 " + QString::number(dialogueBlocks.size()) + " 个对话块");
    QList<QStringList> allDialogues;
    for (int i = 0; i < dialogueBlocks.size(); ++i) {
        const QString& block = dialogueBlocks[i];
        if (block.trimmed().isEmpty())
            continue;
        QStringList lines;
        lines = block.split('\n', Qt::SkipEmptyParts);
        logMessage("对话块 " + QString::number(i + 1) + " 包含 " + QString::number(lines.size()) + " 行");
        QString reconstructedBlock;
        for (int j = 0; j < lines.size(); ++j) {
            if (j > 0) {
                reconstructedBlock += "\n";
            }
            reconstructedBlock += lines[j];
        }
        bool hasEdit = (reconstructedBlock.indexOf(aiHumanSeparator + "\n") != -1);
        QStringList formattedDialogue = processDialogue(reconstructedBlock, hasEdit);
        if (!formattedDialogue.isEmpty()) {
            allDialogues.append(formattedDialogue);
            logMessage("对话块 " + QString::number(i + 1) + " 格式化后包含 " + QString::number(formattedDialogue.size()) + " 行对话");
        }
    }
    for (int i = 0; i < allDialogues.size(); ++i) {
        if (i > 0) {
            out << "\r\n";
        }
        for (const QString& formattedLine : std::as_const(allDialogues[i])) {
            out << formattedLine << "\r\n";
        }
    }
    outputFile.close();
    logMessage("输出文件：" + outputFilePath);
    logMessage("总共生成 " + QString::number(allDialogues.size()) + " 个格式化对话块");
}

void ChatFormatWidget::logMessage(const QString& message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QStringList messageList = message.split("\n", Qt::SkipEmptyParts);
    for (const QString& str : std::as_const(messageList)) {
        ui->logTextEdit->append("[" + timestamp + "] " + str);
    }
}

void ChatFormatWidget::clearLog()
{
    ui->logTextEdit->clear();
}

void ChatFormatWidget::loadDefaultSeparators()
{
    ui->blockSeparatorEdit->setText("");
    blockSeparator = "";
    ui->aiHumanSeparatorEdit->setText("");
    aiHumanSeparator = "";
}

void ChatFormatWidget::saveSettings()
{
    QSettings settings(getContent("config.ini"), QSettings::IniFormat);
    settings.setValue("chatFormat/myName", myName);
    settings.setValue("chatFormat/aiName", aiName);
    settings.setValue("chatFormat/blockSeparator", blockSeparator == "\n\n" ? "" : blockSeparator);
    settings.setValue("chatFormat/aiHumanSeparator", aiHumanSeparator == "edit" ? "" : aiHumanSeparator);
}

void ChatFormatWidget::loadSettings()
{
    QSettings settings(getContent("config.ini"), QSettings::IniFormat);
    QString tempSeparator;
    myName = settings.value("chatFormat/myName", "").toString();
    ui->myNameEdit->setText(myName);
    aiName = settings.value("chatFormat/aiName", "").toString();
    ui->aiNameEdit->setText(aiName);
    tempSeparator = settings.value("chatFormat/blockSeparator", "").toString();
    blockSeparator = tempSeparator;
    ui->blockSeparatorEdit->setText(tempSeparator.replace("\n", "\\n").replace("\t", "\\t").replace("\r", "\\r"));
    tempSeparator = settings.value("chatFormat/aiHumanSeparator", "").toString();
    aiHumanSeparator = tempSeparator;
    ui->aiHumanSeparatorEdit->setText(tempSeparator.replace("\n", "\\n").replace("\t", "\\t").replace("\r", "\r"));
}
