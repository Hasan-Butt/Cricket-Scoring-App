#include "team.h"

Team::Team() {
    teamName = "";
    playerCount = 0;
    totalScore = 0;
    wicketsLost = 0;
    oversBowled = 0;

    for (int i = 0; i < 11; i++)
        players[i] = nullptr;
}

Team::Team(QString name) {
    teamName = name;
    playerCount = 0;
    totalScore = 0;
    wicketsLost = 0;
    oversBowled = 0;

    for (int i = 0; i < 11; i++)
        players[i] = nullptr;
}

void Team::setTeamName(QString name) {
    teamName = name;
}

QString Team::getTeamName() const {
    return teamName;
}

void Team::setPlayer(int index, Player* p) {
    if (index >= 0 && index < 11)
        players[index] = p;
}

Player* Team::getPlayer(int index) const {
    if (index >= 0 && index < 11)
        return players[index];
    return nullptr;
}

int Team::getPlayerCount() const {
    return playerCount;
}

void Team::incrementPlayerCount() {
    if (playerCount < 11)
        playerCount++;
}

void Team::addRuns(int runs) {
    totalScore += runs;
}

void Team::addWicket() {
    if (wicketsLost < 10)
        wicketsLost++;
}

void Team::addOver() {
    oversBowled++;
}

int Team::getTotalScore() const {
    return totalScore;
}

int Team::getWicketsLost() const {
    return wicketsLost;
}

int Team::getOversBowled() const {
    return oversBowled;
}

void Team::setScore(int score){
    totalScore=score;
}
void Team::setWicketsLost(int wk){
    wicketsLost=wk;
}
