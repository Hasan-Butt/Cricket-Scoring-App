#ifndef GAMEPLAYSCREEN_H
#define GAMEPLAYSCREEN_H

#include <QWidget>
#include <QString>
#include <QStringList>
#include <QPair>
#include <QPixmap>
#include <QColor>
#include <QVector>
#include <QSoundEffect>
struct MatchSummary {
    QString date;
    QString teamAName;
    QString teamAScore;
    QString teamBName;
    QString teamBScore;
    QString winner;
    QString margin;
};
struct PlayerPerformance {
    int     jersey;
    QString name;
    int     runs;
    int     balls;
    int     fours;
    int     sixes;
    double     strikerate;
    int     ballerruns;
    int     wickets;
    double     economy;
    int     ballsbowled;
};
namespace Ui { class GamePlayScreen; }
class Player;
class Play;
class ScoreCardScreen;
class MatchSummaryScreen;

class GamePlayScreen : public QWidget
{
    Q_OBJECT

public:
    explicit GamePlayScreen(const QString &teamA,
                            const QString &teamB,
                            QStringList playersA,
                            QStringList playersB,
                            int overs,
                            QWidget *parent = nullptr);
    ~GamePlayScreen();

private slots:
    // Batsman / non‑striker selection
    void on_comboBox_currentIndexChanged(int index);
    void on_comboBox_3_currentIndexChanged(int index);

    // Bowler selection
    void on_comboBox_2_currentTextChanged(const QString &raw);

    // Run buttons
    void on_pushButton_3_clicked();  // 1 run
    void on_pushButton_6_clicked();  // 2 runs
    void on_pushButton_clicked();    // 3 runs
    void on_pushButton_4_clicked();  // 4 runs
    void on_pushButton_5_clicked();  // 5 runs
    void on_pushButton_2_clicked();  // 6 runs
    void on_pushButton_12_clicked(); // dot ball (0)


    void on_pushButton_11_clicked(); // wide/no‑ball run
    void on_pushButton_9_clicked();  // back‑date last ball

    // Wicket
    void on_pushButton_8_clicked();

    // Score‐card viewer
    void on_pushButton_10_clicked();

signals:
    void matchEnded(int scoreA, int scoreB);
private:
    Ui::GamePlayScreen *ui;

    // Teams & players
    QString  teamAName, teamBName;
    QStringList  teamAplayers, teamBplayers;
    int  overnum;
    int playerCount;
    Player *team1Players = nullptr, *team2Players = nullptr;

    Player *currentBatsman  = nullptr;
    Player *nonStriker      = nullptr;
    Player *currentBowler   = nullptr;
    Player *lastBowler      = nullptr;
    int    inningsCount    = 0;
    int    totalBallsPerInning = 0;
    int    currentBallCount   = 0;
    int    currentTeamscore   = 0;
    int    currentTeamWickets = 0;
    int target;
    bool isfreeHit=false;
    bool isNoBallActive  = false;
    bool isFreeHitActive = false;
    bool Swap=false;
    MatchSummaryScreen *matchsummaryscreen;

    Play        *playSession      = nullptr;
    ScoreCardScreen *scorecardscreen = nullptr;

    // Helpers
    QPair<int,QString> parsePlayerString(const QString &raw);
    void addRunsToPlayers(int runs);
    Player* activeBattingArray() const;
    void refreshBattingCombos();
    void refreshBowlerCombo();
    void showPopup(const QString& title,
                   const QString& text,
                   const QPixmap& icon,
                   const QColor& borderColor);
    void updateOversDisplay();
    void UpdatePlayerStatsFile(int jerseyNumber, Player *player);
    void updateMatchStats();
    void saveMatch();
    bool saveMatchJSON(
        const MatchSummary &summary,
        const QVector<PlayerPerformance> &performances
        );
};

#endif // GAMEPLAYSCREEN_H
