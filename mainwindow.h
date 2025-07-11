#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "matchmodescreen.h"
#include "tournamentmodescreen.h"
#include "auctionmodescreen.h"
#include "statsscreen.h"
#include "demomodescreen.h"
#include "settingsscreen.h"
#include <QGraphicsPixmapItem>
#include <QPropertyAnimation>
#include <QRandomGenerator>
#include <QMessageBox>

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showMatchMode();
    void showTournamentMode();
    void showAuctionMode();
    void showStatsLeaderboard();
    void showDemoMode();
    void showSettings();


public slots:
    void returnToHome();

private:
    Ui::MainWindow *ui;
    matchmodescreen *matchScreen;
    TournamentModeScreen *tournamentScreen;
    AuctionModeScreen *auctionScreen;
    StatsScreen *statsScreen;
    DemoModeScreen *demoScreen;
    SettingsScreen *settingsScreen;

};
#endif // MAINWINDOW_H
