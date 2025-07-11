#ifndef AUCTIONMODESCREEN_H
#define AUCTIONMODESCREEN_H

#include <QMainWindow>
#include <QTimer>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QSet>
#include <QList>

class MainWindow;
struct AuctionPlayer {
    QString name;
    QString role;
    int basePrice;
    QString country;
};

namespace Ui {
class AuctionModeScreen;
}

class AuctionModeScreen : public QMainWindow
{
    Q_OBJECT

public:
    explicit AuctionModeScreen(MainWindow *mainWin, QWidget *parent = nullptr);
    ~AuctionModeScreen();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_2_clicked();

    void on_addTeam_clicked();

    void on_removeTeam_clicked();

    void on_bid100KBtn_clicked();

    void on_bid500KBtn_clicked();

    void on_bid1MBtn_clicked();

    void on_passBtn_clicked();

    void on_forceAssignBtn_clicked();

    void on_skipPlayerBtn_clicked();

    void on_turnTimeout();
    void on_progressTimeout();      // For updating progress bar each second

    // UI updates
    void updateLiveAuctionUI();
    void showAuctionSummary();

    void on_pushButton_10_clicked();

private:
    Ui::AuctionModeScreen *ui;
     MainWindow *mainWindow;

    QTimer *turnTimer;             // Timer for turn timeout
    QTimer *progressTimer;

    QStringList teamNames;
    QList<AuctionPlayer> players;
    QMap<QString,int> teamBudgets;
    int currentPlayerIndex;

    QStringList activeTeams;             // Teams still in bidding for current player
    QSet<QString> skipSet;               // Teams skipped permanently for this player
    int turnIndex;                       // Index into activeTeams for next turn
    QString currentTeam;

    int currentBid;                      // Current highest bid
    QString currentLeader;

    int turnDurationSec;
    QMap<QString,int> teamRemainingTime;        // Turn duration in seconds
    int remainingSeconds;

    void nextPlayer();                   // Move to next player in auction
    void nextTurn();                     // Move to next team's turn
    void assignCurrentPlayer();
    void addHistoryEntry(const QString &team, const QString &action, int bidAmount);
};

#endif // AUCTIONMODESCREEN_H
