#include "scorecardscreen.h"
#include "ui_scorecardscreen.h"
#include "demomodescreen.h"
ScoreCardScreen::ScoreCardScreen(const QString &teamA,
                                 const QString &teamB,
                                 Player* team1Players,
                                 Player* team2Players,
                                 int playerCount,
                                 int currentTeamScore,
                                 QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ScoreCardScreen)
    , teamAName(teamA)
    , teamBName(teamB)
    , m_team1Players(team1Players)
    , m_team2Players(team2Players)
    , m_playerCount(playerCount)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground, true);
    setWindowTitle(teamA + " vs " + teamB + " Scorecard");

    ui->target->hide();
    ui->tableBatting->setColumnCount(6);
    ui->tableBatting->setHorizontalHeaderLabels(
        {"Batsman","Runs","Balls","4s","6s","SR"});
    ui->tableBowling->setColumnCount(5);
    ui->tableBowling->setHorizontalHeaderLabels(
        {"Bowler","Overs","Runs","Wkts","Econ"});
    target = currentTeamScore;
    populateTables();
}

ScoreCardScreen::~ScoreCardScreen() {
    delete ui;
}

void ScoreCardScreen::populateTables() {
    ui->tableBatting->setRowCount(m_playerCount);
    ui->tableBowling->setRowCount(m_playerCount);

    for (int i = 0; i < m_playerCount; ++i) {
        Player &batter = m_team1Players[i];
        ui->tableBatting->setItem(i, 0, new QTableWidgetItem(batter.getName()));
        ui->tableBatting->setItem(i, 1, new QTableWidgetItem(QString::number(batter.getRuns())));
        ui->tableBatting->setItem(i, 2, new QTableWidgetItem(QString::number(batter.getBallsFaced())));
        ui->tableBatting->setItem(i, 3, new QTableWidgetItem(QString::number(batter.getFours())));
        ui->tableBatting->setItem(i, 4, new QTableWidgetItem(QString::number(batter.getSixes())));
        ui->tableBatting->setItem(i, 5, new QTableWidgetItem(
                                            QString::number(batter.calculateStrikeRate(), 'f', 2)));

        Player &bowler = m_team2Players[i];
        ui->tableBowling->setItem(i, 0, new QTableWidgetItem(bowler.getName()));
        int balls = bowler.getBallsBowled();
        int ov  = balls / 6;
        int bi  = balls % 6;
        QString overs = QString::number(ov) + "." + QString::number(bi);
        ui->tableBowling->setItem(i, 1, new QTableWidgetItem(overs));
        ui->tableBowling->setItem(i, 2, new QTableWidgetItem(QString::number(bowler.getRunsConceded())));
        ui->tableBowling->setItem(i, 3, new QTableWidgetItem(QString::number(bowler.getWickets())));
        ui->tableBowling->setItem(i, 4, new QTableWidgetItem(QString::number(bowler.calculateEconomyRate(), 'f', 2)));
    }

    ui->tableBatting->resizeColumnsToContents();
    ui->tableBowling->resizeColumnsToContents();
    if(target>0){
        ui->target->setText("Need "+ QString::number(target+1)+ " runs to win.");
        ui->target->show();
    }
}


