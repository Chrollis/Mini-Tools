#include "dialogformatter.h"
#include "ui_dialogformatter.h"

#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <fstream>
#include <sstream>

DialogFormatter::DialogFormatter(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::DialogFormatter)
{
    ui->setupUi(this);

    // 设置窗口标题
    setWindowTitle("对话整理工具");

    // 初始化日志
    logMessage("对话整理工具已启动");
    logMessage("请选择输入文件并设置参数");
}

DialogFormatter::~DialogFormatter()
{
    delete ui;
}

void DialogFormatter::on_browseButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "选择对话文件",
        "",
        "文本文件 (*.txt);;所有文件 (*.*)");

    if (!filePath.isEmpty()) {
        ui->filePathEdit->setText(filePath);
        logMessage("已选择文件: " + filePath);
    }
}

void DialogFormatter::on_formatButton_clicked()
{
    // 获取输入参数
    QString filePath = ui->filePathEdit->text().trimmed();
    QString myName = ui->myNameEdit->text().trimmed();
    QString aiName = ui->aiNameEdit->text().trimmed();

    // 验证输入
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

    // 检查文件是否存在
    QFile file(filePath);
    if (!file.exists()) {
        QMessageBox::critical(this, "文件错误", "选择的文件不存在！");
        return;
    }

    // 禁用按钮防止重复点击
    ui->formatButton->setEnabled(false);
    ui->exitButton->setEnabled(false);

    logMessage("开始处理对话文件...");
    logMessage("您的名字: " + myName);
    logMessage("AI的名字: " + aiName);

    try {
        // 调用格式化函数
        formatDialogue(filePath.toStdString(), myName.toStdString(), aiName.toStdString());
        logMessage("对话整理完成！");
        QMessageBox::information(this, "完成", "对话整理完成！");
    } catch (const std::exception& e) {
        logMessage("错误: " + QString(e.what()));
        QMessageBox::critical(this, "错误", "处理文件时发生错误：" + QString(e.what()));
    }

    // 重新启用按钮
    ui->formatButton->setEnabled(true);
    ui->exitButton->setEnabled(true);
}

void DialogFormatter::on_exitButton_clicked()
{
    close();
}

std::string DialogFormatter::removeQuotes(const std::string& str)
{
    if (str.length() >= 2 && str.front() == '"' && str.back() == '"') {
        return str.substr(1, str.length() - 2);
    }
    return str;
}

std::vector<std::string> DialogFormatter::processDialogue(const std::string& dialogue,
    const std::string& myName,
    const std::string& aiName,
    bool hasEdit)
{
    std::vector<std::string> result;

    if (hasEdit) {
        // 分割用户和AI的对话
        size_t editPos = dialogue.find("edit");
        if (editPos != std::string::npos) {
            // 用户说的话（在edit之前）
            std::string userPart = dialogue.substr(0, editPos);
            // AI说的话（在edit之后）
            std::string aiPart = dialogue.substr(editPos + 4); // +4 跳过 "edit"

            // 处理用户对话
            std::stringstream userStream(userPart);
            std::string line;
            while (getline(userStream, line)) {
                // 移除行尾的\r（Windows换行符）
                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }
                if (!line.empty()) { // 跳过空行
                    result.push_back(myName + "：" + line);
                }
            }

            // 处理AI对话
            std::stringstream aiStream(aiPart);
            while (getline(aiStream, line)) {
                // 移除行尾的\r（Windows换行符）
                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }
                if (!line.empty()) { // 跳过空行
                    result.push_back(aiName + "：" + line);
                }
            }
        }
    } else {
        // 没有edit行，全是AI说的话
        std::stringstream stream(dialogue);
        std::string line;
        while (getline(stream, line)) {
            // 移除行尾的\r（Windows换行符）
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (!line.empty()) { // 跳过空行
                result.push_back(aiName + "：" + line);
            }
        }
    }

    return result;
}

void DialogFormatter::formatDialogue(const std::string& filePath,
    const std::string& myName,
    const std::string& aiName)
{
    // 以二进制模式读取UTF-8文件
    std::ifstream inputFile(filePath, std::ios::binary);
    if (!inputFile.is_open()) {
        throw std::runtime_error("无法打开输入文件！");
    }

    // 生成输出文件名
    std::string outputFilePath = filePath;
    size_t dotPos = outputFilePath.find_last_of('.');
    if (dotPos != std::string::npos) {
        outputFilePath = outputFilePath.substr(0, dotPos) + "_formatted" + outputFilePath.substr(dotPos);
    } else {
        outputFilePath += "_formatted";
    }

    // 以二进制模式写入UTF-8文件
    std::ofstream outputFile(outputFilePath, std::ios::binary);
    if (!outputFile.is_open()) {
        inputFile.close();
        throw std::runtime_error("无法创建输出文件！");
    }

    std::string line;
    std::string currentDialogue;
    std::vector<std::vector<std::string>> allDialogues;

    // 读取文件内容
    while (getline(inputFile, line)) {
        // 移除行尾的\r（Windows换行符）
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        // 检查是否遇到空行（可能是对话分隔符的开始）
        if (line.empty()) {
            // 再读一行检查是否连续两个空行
            std::string nextLine;
            std::streampos currentPos = inputFile.tellg(); // 保存当前位置
            if (getline(inputFile, nextLine)) {
                // 移除行尾的\r
                if (!nextLine.empty() && nextLine.back() == '\r') {
                    nextLine.pop_back();
                }

                if (nextLine.empty()) {
                    // 找到两个连续空行，处理当前对话
                    if (!currentDialogue.empty()) {
                        // 检查是否有edit行
                        bool hasEdit = (currentDialogue.find("edit") != std::string::npos);
                        std::vector<std::string> formattedDialogue = processDialogue(currentDialogue, myName, aiName, hasEdit);
                        if (!formattedDialogue.empty()) {
                            allDialogues.push_back(formattedDialogue);
                        }
                        currentDialogue.clear();
                    }
                    continue;
                } else {
                    // 只有一个空行，可能是对话内的空行，暂时保留
                    inputFile.seekg(currentPos); // 回退到之前的位置
                    if (!currentDialogue.empty()) {
                        currentDialogue += "\n";
                    }
                    currentDialogue += line;
                    continue;
                }
            }
        }

        // 将行添加到当前对话中
        if (!currentDialogue.empty()) {
            currentDialogue += "\n";
        }
        currentDialogue += line;
    }

    // 处理最后一个对话
    if (!currentDialogue.empty()) {
        bool hasEdit = (currentDialogue.find("edit") != std::string::npos);
        std::vector<std::string> formattedDialogue = processDialogue(currentDialogue, myName, aiName, hasEdit);
        if (!formattedDialogue.empty()) {
            allDialogues.push_back(formattedDialogue);
        }
    }

    // 写入输出文件（使用Windows换行符\r\n）
    for (size_t i = 0; i < allDialogues.size(); ++i) {
        if (i > 0) {
            outputFile << "\r\n"; // 对话之间加一个空行（Windows换行符）
        }

        for (const std::string& formattedLine : allDialogues[i]) {
            outputFile << formattedLine << "\r\n"; // 使用Windows换行符
        }
    }

    inputFile.close();
    outputFile.close();

    logMessage("输出文件: " + QString::fromStdString(outputFilePath));
}

void DialogFormatter::logMessage(const QString& message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->logTextEdit->append("[" + timestamp + "] " + message);
}

void DialogFormatter::clearLog()
{
    ui->logTextEdit->clear();
}
