#ifndef TEAM_H
#define TEAM_H

#include <QString>
#include "player.h"

class Team {
private:
    QString teamName;
    Player* players[11];  // Max 11 players
    int playerCount;
    int totalScore;
    int wicketsLost;
    int oversBowled;

public:
    Team();
    Team(QString name);

    void setTeamName(QString name);
    QString getTeamName() const;

    void setPlayer(int index, Player* p);
    Player* getPlayer(int index) const;

    void setScore(int score);
    void setWicketsLost(int wk);
    int getPlayerCount() const;
    void incrementPlayerCount();

    void addRuns(int runs);
    void addWicket();
    void addOver();

    int getTotalScore() const;
    int getWicketsLost() const;
    int getOversBowled() const;
};

#endif
