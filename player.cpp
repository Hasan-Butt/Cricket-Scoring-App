#include "player.h"
#include <QString>
void Player:: setRuns(int runs) {
    this->runs = runs;
}
void Player:: setWickets(int wickets) {
    this->wickets = wickets;
}
void Player:: setMatches(int matches) {
    this->matches = matches;
}
void Player:: setBallsFaced(int ballsFaced) {
    this->ballsFaced = ballsFaced;
}
void Player:: setBallsBowled(int ballsBowled) {
    this->ballsBowled = ballsBowled;
}
void Player:: setRunsConceded(int runsConceded) {
    this->runsConceded = runsConceded;
}
void Player:: setFours(int fours) {
    this->fours = fours;
}
void Player:: setSixes(int sixes) {
    this->sixes = sixes;
}
void Player:: setHalfCenturies(int halfCenturies) {
    this->halfCenturies = halfCenturies;
}
void Player:: setCenturies(int centuries) {
    this->centuries = centuries;
}
void Player:: setStrikeRate(int strikeRate) {
    this->strikeRate = strikeRate;
}
void Player:: setAverage(int average) {
    this->average = average;
}
void Player:: setHighestScore(int highestScore) {
    this->highestScore = highestScore;
}
void Player:: setWides(int wides) {
    this->wides = wides;
}
void Player:: setNoBalls(int noBalls) {
    this->noBalls = noBalls;
}
void Player::setOut(bool o)   { out = o; }
bool Player::isOut() const    { return out; }

int Player:: getRuns() {
    return runs;
}
int Player:: getWickets() {
    return wickets;
}
int Player:: getMatches() {
    return matches;
}
int Player:: getBallsFaced() {
    return ballsFaced;
}
int Player:: getBallsBowled() {
    return ballsBowled;
}
int Player:: getRunsConceded() {
    return runsConceded;
}
int Player:: getFours() {
    return fours;
}
int Player:: getSixes() {
    return sixes;
}
int Player:: getHalfCenturies() {
    return halfCenturies;
}
int Player:: getCenturies() {
    return centuries;
}
int Player:: getStrikeRate() {
    return strikeRate;
}
int Player:: getAverage() {
    return average;
}
int Player:: getHighestScore() {
    return highestScore;
}
int Player:: getWides() {
    return wides;
}
int Player:: getNoBalls() {
    return noBalls;
}

// Additional methods to calculate strike rate and average
double Player:: calculateStrikeRate() {
    if (ballsFaced == 0) return 0;
    return (static_cast<double>(runs) / ballsFaced) * 100;
}
double Player:: calculateAverage() {
    if (matches == 0) return 0;
    return static_cast<double>(runs) / matches;
}
double Player:: calculateBowlingAverage() {
    if (wickets == 0) return 0;
    return static_cast<double>(runsConceded) / wickets;
}
double Player:: calculateEconomyRate() {
    if (ballsBowled == 0) return 0;
    return static_cast<double>(runsConceded) / (ballsBowled / 6);
}
double Player:: calculateBowlingStrikeRate() {
    if (wickets == 0) return 0;
    return static_cast<double>(ballsBowled) / wickets;
}

QString Player:: getName() {
    return name;
}
int Player:: getjurseyNumber() {
    return jurseyNum;
}
