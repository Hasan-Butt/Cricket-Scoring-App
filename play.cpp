#include "play.h"
#include "team.h"
Play::Play(Team team1, Team team2, int overs)
    : team1(team1), team2(team2), overs(overs),
     runsScored(0),wicketsLost(0)
{

}

int Play::getRunsScored() { return runsScored; }
int Play::getWicketsLost() { return wicketsLost; }
Team& Play::getTeamA() { return team1; }
Team& Play::getTeamB() { return team2; }
