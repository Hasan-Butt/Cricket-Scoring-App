#ifndef PLAY_H
#define PLAY_H
#include "team.h"
#include "player.h"
class Play
{
private:
    Team team1;
    Team team2;
    int overs;
    int runsScored;
    int wicketsLost;

public:
    Play(Team team1, Team team2, int overs);
    int getRunsScored();
    int getWicketsLost();
    Team& getTeamA();
    Team& getTeamB();

};

#endif // PLAY_H
