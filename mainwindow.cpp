#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "matchmodescreen.h"
#include "tossscreen.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , matchScreen(nullptr)
{
    ui->setupUi(this);

    ui->btnMatchMode->setIcon(QIcon(":/icons/baticon.png"));
    ui->btnTournamentMode->setIcon(QIcon(":icons/tournamenticon.png"));
    ui->btnAuctionMode->setIcon(QIcon(":icons/auctionicon.png"));
    ui->btnStatsLeaderboard->setIcon(QIcon(":/icons/statsicon.png"));
    ui->btnSettings->setIcon(QIcon(":/icons/settingsicon.png"));
    ui->btnExit->setIcon(QIcon(":/icons/exiticon.png"));

    ui->label->setText("<img src=':/icons/baticon.png' width='24' height='24'>  Cricket Scoring and Auction System");

    matchScreen = new matchmodescreen(this);
    tournamentScreen = new TournamentModeScreen(this);
    auctionScreen = new AuctionModeScreen(this);
    statsScreen = new StatsScreen(this);
    demoScreen = new DemoModeScreen(this);
    settingsScreen = new SettingsScreen(this);


    connect(ui->btnMatchMode, &QPushButton::clicked, this, &MainWindow::showMatchMode);
    connect(ui->btnTournamentMode, &QPushButton::clicked, this, &MainWindow::showTournamentMode);
    connect(ui->btnAuctionMode, &QPushButton::clicked, this, &MainWindow::showAuctionMode);
    connect(ui->btnStatsLeaderboard, &QPushButton::clicked, this, &MainWindow::showStatsLeaderboard);
    connect(ui->btnSettings, &QPushButton::clicked, this, &MainWindow::showSettings);
    connect(ui->btnExit, &QPushButton::clicked, this, &MainWindow::close);


}

MainWindow::~MainWindow()
{
    delete matchScreen;
    delete tournamentScreen;
    delete auctionScreen;
    delete statsScreen;
    delete demoScreen;
    delete settingsScreen;
    delete ui;
}

void MainWindow::showMatchMode() {
    matchScreen->show();
    this->hide();
}

void MainWindow::showTournamentMode() {
    tournamentScreen->show();
    this->hide();
}

void MainWindow::showAuctionMode() {
    auctionScreen->show();
    this->hide();
}

void MainWindow::showStatsLeaderboard() {
    statsScreen->show();
    this->hide();
}

void MainWindow::showDemoMode() {
    demoScreen->show();
    this->hide();
}

void MainWindow::showSettings() {
    settingsScreen->show();
    this->hide();
}

void MainWindow::returnToHome() {
    this->show();
    if (matchScreen) matchScreen->hide();
    if (tournamentScreen) tournamentScreen->hide();
    if (auctionScreen) auctionScreen->hide();
    if (statsScreen) statsScreen->hide();
    if (demoScreen) demoScreen->hide();
    if (settingsScreen) settingsScreen->hide();


}



