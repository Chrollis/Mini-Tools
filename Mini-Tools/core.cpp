#include "core.h"
#include "arithmeticwidget.h"
#include "blackjackwidget.h"
#include "breakoutwidget.h"
#include "chatformatwidget.h"
#include "cprlib.h"
#include "deploywidget.h"
#include "tictactoewidget.h"
#include "translationwidget.h"
#include "ui_core.h"

#define onActionTriggered(widgetClassName, actionName, actionTitle) \
    if (tabs.contains(#widgetClassName)) {                          \
        widgetClose(tabs.value(#widgetClassName));                  \
        tabs.remove(#widgetClassName);                              \
        ui->actionName->setText(actionTitle);                       \
    } else {                                                        \
        widgetClassName* widget = new widgetClassName;              \
        ui->tabWidget->addTab(widget, widget->windowTitle());       \
        tabs.insert(#widgetClassName, widget);                      \
        ui->actionName->setText(QString("关闭") + actionTitle);     \
        ui->actionDock->setEnabled(true);                           \
    }

Core::Core(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::Core)
{
    ui->setupUi(this);
    setWindowTitle("小工具些");
    startTimer(3000);
    addSound("");

    connect(ui->actionDeploy, &QAction::triggered, this, [&]() { onActionTriggered(DeployWidget, actionDeploy, "Qt-dlls部署"); });
    connect(ui->actionChat, &QAction::triggered, this, [&]() { onActionTriggered(ChatFormatWidget, actionChat, "AI对话整理"); });
    connect(ui->actionArithmetic, &QAction::triggered, this, [&]() { onActionTriggered(ArithmeticWidget, actionArithmetic, "简单口算生成"); });
    connect(ui->actionTictactoe, &QAction::triggered, this, [&]() { onActionTriggered(TictactoeWidget, actionTictactoe, "井字棋"); });
    connect(ui->actionBlackjack, &QAction::triggered, this, [&]() { onActionTriggered(BlackjackWidget, actionBlackjack, "21点"); });
    connect(ui->actionBreakout, &QAction::triggered, this, [&]() { onActionTriggered(BreakoutWidget, actionBreakout, "打砖块"); });
    connect(ui->actionTranslation, &QAction::triggered, this, [&]() { onActionTriggered(TranslationWidget, actionTranslation, "翻译Json管理器"); });
    connect(ui->actionDock, &QAction::triggered, this, &Core::onActionDockTriggered);
}

Core::~Core()
{
    delete ui;
}

void Core::widgetClose(QWidget* widget)
{
    int index = ui->tabWidget->indexOf(widget);
    if (index != 1) {
        ui->tabWidget->removeTab(index);
    }
    if (docks.contains(widget)) {
        removeDockWidget(docks.value(widget));
        docks.remove(widget);
    }
    delete widget;
    if (ui->tabWidget->count() == 0) {
        ui->actionDock->setEnabled(0);
    }
}

void Core::onActionDockTriggered()
{
    int index = ui->tabWidget->currentIndex();
    if (index != -1) {
        QWidget* widget = ui->tabWidget->currentWidget();
        QDockWidget* dock = new QDockWidget(this);
        dock->setWidget(widget);
        dock->setWindowTitle(widget->windowTitle());
        dock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
        addDockWidget(Qt::RightDockWidgetArea, dock);
        ui->tabWidget->removeTab(index);
        docks.insert(widget, dock);
        if (ui->tabWidget->count() == 0) {
            ui->actionDock->setEnabled(0);
        }
    }
}

void Core::timerEvent(QTimerEvent*)
{
    sweepSound();
}
