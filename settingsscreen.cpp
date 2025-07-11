#include "settingsscreen.h"
#include "ui_settingsscreen.h"
#include "mainwindow.h"

SettingsScreen::SettingsScreen(MainWindow *mainWin, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SettingsScreen)
    , mainWindow(mainWin)
{
    ui->setupUi(this);

    ui->homeButton->setIcon(QIcon(":icons/homeicon.jpg"));

    connect(ui->homeButton, &QPushButton::clicked,
            mainWindow, &MainWindow::returnToHome);
}

SettingsScreen::~SettingsScreen()
{
    delete ui;
}
