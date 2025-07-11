#ifndef PLAYER_H
#define PLAYER_H
#include <QString>
#include <string>
class Player
{
private:
    QString name;
    int jurseyNum;

    int runs;
    int ballsFaced;
    int fours;
    int sixes;
    int halfCenturies;
    int centuries;
    int strikeRate;
    int average;
    int highestScore;

    int wickets;
    int ballsBowled;
    int wides;
    int noBalls;
    int runsConceded;
    int bowlerbestWickets;
    int bowlerbestruns;
    bool out = false;

    int matches;
public:
    Player(QString name, int jurseyNum)
        : name(name), jurseyNum(jurseyNum), runs(0), ballsFaced(0), fours(0), sixes(0), halfCenturies(0), centuries(0),
        strikeRate(0), average(0), highestScore(0), wickets(0), ballsBowled(0), wides(0), noBalls(0),
        runsConceded(0), matches(0) {
    }
    Player() : name(""), jurseyNum(0) {}
    void setRuns(int runs);
    void setWickets(int wickets);
    void setMatches(int matches);
    void setBallsFaced(int ballsFaced);
    void setBallsBowled(int ballsBowled);
    void setRunsConceded(int runsConceded);
    void setFours(int fours);
    void setSixes(int sixes);
    void setHalfCenturies(int halfCenturies);
    void setCenturies(int centuries);
    void setStrikeRate(int strikeRate);
    void setAverage(int average);
    void setHighestScore(int highestScore);
    void setWides(int wides);
    void setNoBalls(int noBalls);
    void setOut(bool o);
    bool isOut() const;
    double calculateStrikeRate();
    double calculateAverage();
    double calculateBowlingAverage();
    double calculateEconomyRate();
    double calculateBowlingStrikeRate();
    QString getName();
    int getjurseyNumber();
    int getRuns();
    int getWickets();
    int getMatches();
    int getBallsFaced();
    int getBallsBowled();
    int getRunsConceded();
    int getFours();
    int getSixes();
    int getHalfCenturies();
    int getCenturies();
    int getStrikeRate();
    int getAverage();
    int getHighestScore();
    int getWides();
    int getNoBalls();


};

#endif // PLAYER_H
