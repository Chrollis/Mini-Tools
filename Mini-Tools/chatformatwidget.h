#ifndef CHATFORMATWIDGET_H
#define CHATFORMATWIDGET_H

#include <QWidget>

namespace Ui {
class ChatFormatWidget;
}

class ChatFormatWidget : public QWidget {
    Q_OBJECT

public:
    explicit ChatFormatWidget(QWidget* parent = nullptr);
    ~ChatFormatWidget();

private:
    void onBrowseButtonClicked();
    void onFormatButtonClicked();
    void onResetSeparatorsButtomClicked();

private:
    Ui::ChatFormatWidget* ui;
    QString myName;
    QString aiName;
    QString blockSeparator;
    QString aiHumanSeparator;
    QStringList processDialogue(const QString& dialogue, bool hasEdit);
    void formatDialogue(const QString& filePath);
    void logMessage(const QString& message);
    void clearLog();
    void loadDefaultSeparators();
    void saveSettings();
    void loadSettings();
};

#endif // CHATFORMATWIDGET_H
