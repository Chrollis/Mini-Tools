#ifndef DIALOGFORMATTER_H
#define DIALOGFORMATTER_H

#include <QMainWindow>
#include <QString>
#include <string>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class DialogFormatter;
}
QT_END_NAMESPACE

class DialogFormatter : public QMainWindow {
    Q_OBJECT

public:
    DialogFormatter(QWidget* parent = nullptr);
    ~DialogFormatter();

private slots:
    void on_browseButton_clicked();
    void on_formatButton_clicked();
    void on_exitButton_clicked();

private:
    Ui::DialogFormatter* ui;

    // 原C++代码中的函数
    std::string removeQuotes(const std::string& str);
    std::vector<std::string> processDialogue(const std::string& dialogue,
        const std::string& myName,
        const std::string& aiName,
        bool hasEdit);
    void formatDialogue(const std::string& filePath,
        const std::string& myName,
        const std::string& aiName);

    // 工具函数
    void logMessage(const QString& message);
    void clearLog();
};

#endif // DIALOGFORMATTER_H
