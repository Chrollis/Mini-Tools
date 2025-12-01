#ifndef TRANSLATIONWIDGET_H
#define TRANSLATIONWIDGET_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QTableWidgetItem>
#include <QWidget>

namespace Ui {
class TranslationWidget;
}

class TranslationWidget : public QWidget {
    Q_OBJECT

public:
    explicit TranslationWidget(QWidget* parent = nullptr);
    ~TranslationWidget();

private slots:
    void onBtnNewSourceClicked();
    void onBtnNewTargetClicked();
    void onBtnSelectSourceClicked();
    void onBtnSelectTargetClicked();
    void onBtnRemoveTargetClicked();
    void onBtnClearTargetsClicked();
    void onBtnAddKeyClicked();
    void onBtnDeleteKeyClicked();
    void onBtnSaveSourceClicked();
    void onBtnSaveTargetClicked();
    void onBtnSaveAllClicked();
    void onTableWidgetCellDoubleClicked(int row, int column);

private:
    class myJson {
    public:
        QWidget* parent = nullptr;
        QString path;
        QMap<QString, QString> data;
        myJson(QWidget* parent, const QString& path);
        void save(bool isSource, const myJson* const source = nullptr) const;
        void load(bool isSource, const myJson* const source = nullptr);
        QJsonObject getObj(bool isSource) const;
    };
    Ui::TranslationWidget* ui;
    myJson source;
    QList<myJson> targets;

    void refreshTable();
    void updateTargetColumns();
    bool canDeleteKey(const QString& key);
    void saveTargetFile(int index);
    void editSourceKey(int row);
    void editSourceTranslation(int row);
    void editTargetTranslation(int row, int index);
};

#endif // TRANSLATIONWIDGET_H
