#ifndef TOURNAMENTMODESCREEN_H
#define TOURNAMENTMODESCREEN_H
#include <QMainWindow>
#include <QPushButton>
class MainWindow;
class matchmodescreen;
namespace Ui {
class TournamentModeScreen;
}

class TournamentModeScreen : public QMainWindow
{
    Q_OBJECT

public:
    explicit TournamentModeScreen(MainWindow *mainWin, QWidget *parent = nullptr);
    ~TournamentModeScreen();

private slots:

    void on_addTeam_clicked();

    void on_removeTeam_clicked();

    void on_generateSchedule_clicked();

private:
    Ui::TournamentModeScreen *ui;
    MainWindow *mainWindow;
    matchmodescreen *MatchModeScreen;
    struct Match {
        QString teamA;
        QString teamB;
        QPushButton *playBtn = nullptr;
    };

    struct TeamStats {
        int played = 0;
        int wins   = 0;
        int losses = 0;
        int points = 0;
        int runsFor = 0;
        int runsAgainst = 0;
        int draws = 0;
    };
    QStringList             teamNames;
    QVector<Match>          matches;
    QMap<QString,TeamStats> statsMap;
    void refreshStandings();
    int totalMatches;
    int matchesPlayed = 0;
    void updateWinnerLabel();
};

#endif // TOURNAMENTMODESCREEN_H
