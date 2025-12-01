#include "translationwidget.h"
#include "ui_translationwidget.h"
#include <QColor>
#include <QFile>
#include <QFileDialog>
#include <QHeaderView>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

TranslationWidget::TranslationWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::TranslationWidget)
    , source(this, "")
{
    ui->setupUi(this);
    setWindowTitle("翻译Json管理");

    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Key" << "源翻译");
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(ui->btnSelectSource, &QPushButton::clicked, this, &TranslationWidget::onBtnSelectSourceClicked);
    connect(ui->btnSelectTarget, &QPushButton::clicked, this, &TranslationWidget::onBtnSelectTargetClicked);
    connect(ui->btnRemoveTarget, &QPushButton::clicked, this, &TranslationWidget::onBtnRemoveTargetClicked);
    connect(ui->btnClearTargets, &QPushButton::clicked, this, &TranslationWidget::onBtnClearTargetsClicked);
    connect(ui->btnAddKey, &QPushButton::clicked, this, &TranslationWidget::onBtnAddKeyClicked);
    connect(ui->btnDeleteKey, &QPushButton::clicked, this, &TranslationWidget::onBtnDeleteKeyClicked);
    connect(ui->btnSaveSource, &QPushButton::clicked, this, &TranslationWidget::onBtnSaveSourceClicked);
    connect(ui->btnSaveTarget, &QPushButton::clicked, this, &TranslationWidget::onBtnSaveTargetClicked);
    connect(ui->btnSaveAll, &QPushButton::clicked, this, &TranslationWidget::onBtnSaveAllClicked);
    connect(ui->btnNewSource, &QPushButton::clicked, this, &TranslationWidget::onBtnNewSourceClicked);
    connect(ui->btnNewTarget, &QPushButton::clicked, this, &TranslationWidget::onBtnNewTargetClicked);
    connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &TranslationWidget::onTableWidgetCellDoubleClicked);
}

TranslationWidget::~TranslationWidget()
{
    delete ui;
}

void TranslationWidget::onBtnSelectSourceClicked()
{
    if (!targets.isEmpty()) {
        int ret = QMessageBox::question(this, "选择源文件",
            "更换源文件将清空所有目标文件，是否继续？");
        if (ret != QMessageBox::Yes) {
            return;
        }
        targets.clear();
        ui->listWidgetTargets->clear();
        updateTargetColumns();
    }
    QString filePath = QFileDialog::getOpenFileName(this, "选择源JSON文件", "", "JSON文件 (*.json)");
    if (!filePath.isEmpty()) {
        source = myJson(this, filePath);
        source.load(1);
        ui->lineEditSource->setText(QFileInfo(filePath).fileName());
        refreshTable();
    }
}

void TranslationWidget::onBtnNewSourceClicked()
{
    if (!targets.isEmpty()) {
        int ret = QMessageBox::question(this, "新建源文件",
            "新建源文件将清空所有目标文件，是否继续？");
        if (ret != QMessageBox::Yes) {
            return;
        }
        targets.clear();
        ui->listWidgetTargets->clear();
        updateTargetColumns();
    }
    QString filePath = QFileDialog::getSaveFileName(this, "新建源JSON文件", "", "JSON文件 (*.json)");
    if (filePath.isEmpty()) {
        return;
    }
    if (!filePath.endsWith(".json", Qt::CaseInsensitive)) {
        filePath += ".json";
    }
    source = myJson(this, filePath);
    source.save(1);
    ui->lineEditSource->setText(QFileInfo(filePath).fileName());
    refreshTable();
}

void TranslationWidget::onBtnNewTargetClicked()
{
    if (source.path.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择或新建源文件");
        return;
    }
    QString filePath = QFileDialog::getSaveFileName(this, "新建目标JSON文件", "", "JSON文件 (*.json)");
    if (filePath.isEmpty()) {
        return;
    }
    if (!filePath.endsWith(".json", Qt::CaseInsensitive)) {
        filePath += ".json";
    }
    myJson target(this, filePath);
    target.save(0, &source);
    targets.append(target);
    ui->listWidgetTargets->addItem(QFileInfo(filePath).fileName());
    updateTargetColumns();
    refreshTable();
}

void TranslationWidget::onBtnSelectTargetClicked()
{
    QStringList filePaths = QFileDialog::getOpenFileNames(this, "选择目标JSON文件", "", "JSON文件 (*.json)");
    for (const QString& filePath : std::as_const(filePaths)) {
        myJson target(this, filePath);
        target.load(0, &source);
        targets.append(target);
        ui->listWidgetTargets->addItem(QFileInfo(filePath).fileName());
        updateTargetColumns();
        refreshTable();
    }
}

void TranslationWidget::onBtnRemoveTargetClicked()
{
    int currentRow = ui->listWidgetTargets->currentRow();
    if (currentRow >= 0 && currentRow < targets.size()) {
        targets.removeAt(currentRow);
        ui->listWidgetTargets->takeItem(currentRow);
        updateTargetColumns();
        refreshTable();
    }
}

void TranslationWidget::onBtnClearTargetsClicked()
{
    if (!targets.isEmpty()) {
        int ret = QMessageBox::question(this, "确认", "确定要清空所有目标文件吗？");
        if (ret == QMessageBox::Yes) {
            targets.clear();
            ui->listWidgetTargets->clear();
            updateTargetColumns();
            refreshTable();
        }
    }
}

void TranslationWidget::onBtnAddKeyClicked()
{
    if (source.path.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择或新建源文件");
        return;
    }

    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);

    QString newKey = QString("new_key_%1").arg(row);
    int counter = 1;
    while (source.data.contains(newKey)) {
        newKey = QString("new_key_%1").arg(row + counter);
        counter++;
    }

    source.data[newKey] = "";
    for (auto& target : targets) {
        target.data[newKey] = "";
    }
    refreshTable();
}

void TranslationWidget::onBtnDeleteKeyClicked()
{
    int currentRow = ui->tableWidget->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "警告", "请先选择要删除的行");
        return;
    }

    QTableWidgetItem* keyItem = ui->tableWidget->item(currentRow, 0);
    if (!keyItem)
        return;

    QString key = keyItem->text();

    if (!canDeleteKey(key)) {
        QMessageBox::warning(this, "警告",
            "无法删除该Key：目标文件中存在非空翻译。\n请先将所有目标文件中的对应翻译置空后再删除。");
        return;
    }

    int ret = QMessageBox::question(this, "确认", QString("确定要删除Key '%1' 吗？").arg(key));
    if (ret == QMessageBox::Yes) {
        source.data.remove(key);
        for (auto& target : targets) {
            target.data.remove(key);
        }
        ui->tableWidget->removeRow(currentRow);
    }
}

void TranslationWidget::onBtnSaveSourceClicked()
{
    if (source.path.isEmpty()) {
        QMessageBox::warning(this, "警告", "没有源文件可保存");
        return;
    }
    source.save(1);
}

void TranslationWidget::onBtnSaveTargetClicked()
{
    int currentRow = ui->listWidgetTargets->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "警告", "请先选择要保存的目标文件");
        return;
    }
    saveTargetFile(currentRow);
}

void TranslationWidget::onBtnSaveAllClicked()
{
    if (source.path.isEmpty() && targets.isEmpty()) {
        QMessageBox::warning(this, "警告", "没有文件可保存");
        return;
    }

    if (!source.path.isEmpty()) {
        source.save(1);
    }

    for (int i = 0; i < targets.size(); i++) {
        saveTargetFile(i);
    }

    QMessageBox::information(this, "成功", "所有文件保存完成");
}

void TranslationWidget::onTableWidgetCellDoubleClicked(int row, int column)
{
    if (column == 0) {
        editSourceKey(row);
    } else if (column == 1) {
        editSourceTranslation(row);
    } else if (column >= 2) {
        editTargetTranslation(row, column - 2);
    } else {
        QMessageBox::warning(this, "错误", "双击了无效的单元格");
        return;
    }
}

void TranslationWidget::refreshTable()
{
    ui->tableWidget->setRowCount(0);

    if (source.data.isEmpty()) {
        return;
    }

    int row = 0;
    for (auto it = source.data.begin(); it != source.data.end(); ++it, ++row) {
        ui->tableWidget->insertRow(row);
        QTableWidgetItem* keyItem = new QTableWidgetItem(it.key());
        ui->tableWidget->setItem(row, 0, keyItem);
        QTableWidgetItem* sourceItem = new QTableWidgetItem(it.value());
        if (it.value().isEmpty()) {
            sourceItem->setBackground(QColor(255, 200, 200));
        }
        ui->tableWidget->setItem(row, 1, sourceItem);
        for (int i = 0; i < targets.size(); i++) {
            QString translation = targets[i].data.value(it.key(), "");
            QTableWidgetItem* targetItem = new QTableWidgetItem(translation);
            if (translation.isEmpty()) {
                targetItem->setBackground(QColor(255, 255, 200));
            }
            ui->tableWidget->setItem(row, 2 + i, targetItem);
        }
    }
}

void TranslationWidget::updateTargetColumns()
{

    ui->tableWidget->setColumnCount(2 + targets.size());

    QStringList headers;
    headers << "Key" << "源翻译";
    for (const auto& target : std::as_const(targets)) {
        headers << QFileInfo(target.path).fileName();
    }
    ui->tableWidget->setHorizontalHeaderLabels(headers);
}

bool TranslationWidget::canDeleteKey(const QString& key)
{
    for (const auto& target : std::as_const(targets)) {
        if (!target.data.value(key, "").isEmpty()) {
            return false;
        }
    }
    return true;
}

void TranslationWidget::saveTargetFile(int index)
{
    if (index < 0 || index >= targets.size())
        return;

    targets[index].save(0, &source);
}

void TranslationWidget::editSourceKey(int row)
{
    QString oldKey = ui->tableWidget->item(row, 0)->text();
    bool flag = 0;
    QString newKey = QInputDialog::getText(this, "键值修改", "请输入新键值: ", QLineEdit::Normal, oldKey, &flag);
    if (!flag) {
        return;
    }
    if (oldKey == newKey) {
        return;
    }
    if (source.data.contains(newKey)) {
        QMessageBox::warning(this, "警告", "请勿输入已有的键值");
        return;
    }
    QString translation = source.data[oldKey];
    source.data.remove(oldKey);
    source.data[newKey] = translation;
    for (int i = 0; i < targets.size(); i++) {
        translation = targets[i].data[oldKey];
        targets[i].data.remove(oldKey);
        targets[i].data[newKey] = translation;
    }
    refreshTable();
}

void TranslationWidget::editSourceTranslation(int row)
{
    QString key = ui->tableWidget->item(row, 0)->text();
    QString oldTranslation = ui->tableWidget->item(row, 1)->text();
    bool flag = 0;
    QString newTranslation = QInputDialog::getText(this, "翻译修改", "请输入新翻译: ", QLineEdit::Normal, oldTranslation, &flag);
    if (!flag) {
        return;
    }
    if (oldTranslation == newTranslation) {
        return;
    }
    source.data[key] = newTranslation;
    refreshTable();
}

void TranslationWidget::editTargetTranslation(int row, int index)
{
    QString key = ui->tableWidget->item(row, 0)->text();
    QString oldTranslation = ui->tableWidget->item(row, index + 2)->text();
    bool flag = 0;
    QString newTranslation = QInputDialog::getText(this, "翻译修改", "请输入新翻译: ", QLineEdit::Normal, oldTranslation, &flag);
    if (!flag) {
        return;
    }
    if (oldTranslation == newTranslation) {
        return;
    }
    targets[index].data[key] = newTranslation;
    refreshTable();
}

TranslationWidget::myJson::myJson(QWidget* parent, const QString& path)
{
    this->parent = parent;
    this->path = path;
}

void TranslationWidget::myJson::save(bool isSource, const myJson* const source) const
{
    if (path.isEmpty()) {
        QMessageBox::warning(parent, "错误", "地址为空无法保存文件");
        return;
    }
    QJsonObject obj = getObj(isSource);
    if (!isSource) {
        QJsonObject sObj = source->getObj(1);
        for (auto it = sObj.begin(); it != sObj.end(); ++it) {
            if (it.value().isString()) {
                if (obj.find(it.key()) != obj.end()) {
                    data[it.key()] = obj[it.key()].toString();
                    obj.remove(it.key());
                } else {
                    data[it.key()] = "";
                }
            }
        }
        if (!obj.isEmpty()) {
            QMessageBox::warning(parent, "错误", "目标文件含有源文件不拥有的键");
            return;
        }
    }
    obj = getObj(isSource);
    QJsonDocument doc(obj);
    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    } else {
        QMessageBox::warning(parent, "错误", "无法保存文件");
    }
}

void TranslationWidget::myJson::load(bool isSource, const myJson* const source)
{

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(parent, "错误", "无法打开文件");
        return;
    }

    QByteArray d = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(d);
    if (doc.isNull() || !doc.isObject()) {
        QMessageBox::warning(parent, "错误", "源文件不是有效的JSON格式");
        return;
    }

    QJsonObject obj = doc.object();
    data.clear();

    if (isSource) {
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            if (it.value().isString()) {
                data[it.key()] = it.value().toString();
            }
        }
    } else {
        QJsonObject sObj = source->getObj(1);
        for (auto it = sObj.begin(); it != sObj.end(); ++it) {
            if (it.value().isString()) {
                if (obj.find(it.key()) != obj.end()) {
                    data[it.key()] = obj[it.key()].toString();
                    obj.remove(it.key());
                } else {
                    data[it.key()] = "";
                }
            }
        }
        if (!obj.isEmpty()) {
            QMessageBox::warning(parent, "错误", "目标文件含有源文件不拥有的键");
            return;
        }
    }
}

QJsonObject TranslationWidget::myJson::getObj(bool isSource) const
{
    QJsonObject obj;
    for (auto it = data.begin(); it != data.end(); ++it) {
        if (isSource && it.value().isEmpty()) {
            QMessageBox::warning(parent, "错误", "源文件不得有空值");
            return {};
        }
        obj[it.key()] = it.value();
    }
    return obj;
}
