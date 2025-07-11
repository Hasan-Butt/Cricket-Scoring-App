#ifndef SCORECARDSCREEN_H
#define SCORECARDSCREEN_H

#include <QWidget>
#include "player.h"

namespace Ui {
class ScoreCardScreen;
}

class ScoreCardScreen : public QWidget {
    Q_OBJECT

public:
    // Construct with both team names, player arrays, and player count
    explicit ScoreCardScreen(const QString &teamA,
                             const QString &teamB,
                             Player* team1Players,
                             Player* team2Players,
                             int   playerCount,
                             int currentTeamScore,
                             QWidget *parent = nullptr);
    ~ScoreCardScreen();

private slots:

private:
    Ui::ScoreCardScreen *ui;
    QString teamAName;
    QString teamBName;
    Player* m_team1Players;
    Player* m_team2Players;
    int     m_playerCount;
    int     target;
    void populateTables();
};

#endif // SCORECARDSCREEN_H
