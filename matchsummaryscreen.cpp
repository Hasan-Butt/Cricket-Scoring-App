#include "matchsummaryscreen.h"
#include "ui_matchsummaryscreen.h"

MatchSummaryScreen::MatchSummaryScreen(const QString teamAname,const QString teamBname,Player* teamA,Player* teamB,int count,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MatchSummaryScreen)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground, true);
    ui->btnBackToHome->setIcon(QIcon(":/icons/exiticon.png"));
    nameA=teamAname;
    nameB=teamBname;
    firstTeam=teamA;
    secondTeam=teamB;
    Count=count;
    setWindowTitle(" Match Summary");
    ui->tabWidget->setTabText(0,nameA);
    ui->tabWidget->setTabText(1,nameB);
    ui->tableBatting->setColumnCount(7);
    ui->tableBatting->setHorizontalHeaderLabels(
        {"Batsman","Runs","Balls","4s","6s","SR","Status"});
    ui->tableBowling->setColumnCount(5);
    ui->tableBowling->setHorizontalHeaderLabels(
        {"Bowler","Overs","Runs","Wkts","Econ"});
    ui->tableBatting2->setColumnCount(7);
    ui->tableBatting2->setHorizontalHeaderLabels(
        {"Batsman","Runs","Balls","4s","6s","SR","Status"});
    ui->tableBowling2->setColumnCount(5);
    ui->tableBowling2->setHorizontalHeaderLabels(
        {"Bowler","Overs","Runs","Wkts","Econ"});
    populateTables();

}

MatchSummaryScreen::~MatchSummaryScreen()
{
    delete ui;
}

void MatchSummaryScreen::populateTables(){
    ui->tableBatting->setRowCount(Count);
    ui->tableBowling->setRowCount(Count);
    ui->tableBatting2->setRowCount(Count);
    ui->tableBowling2->setRowCount(Count);

    for (int i = 0; i < Count; ++i) {
        Player &batter = firstTeam[i];
        ui->tableBatting->setItem(i, 0, new QTableWidgetItem(batter.getName()));
        ui->tableBatting->setItem(i, 1, new QTableWidgetItem(QString::number(batter.getRuns())));
        ui->tableBatting->setItem(i, 2, new QTableWidgetItem(QString::number(batter.getBallsFaced())));
        ui->tableBatting->setItem(i, 3, new QTableWidgetItem(QString::number(batter.getFours())));
        ui->tableBatting->setItem(i, 4, new QTableWidgetItem(QString::number(batter.getSixes())));
        ui->tableBatting->setItem(i, 5, new QTableWidgetItem(
                                            QString::number(batter.calculateStrikeRate(), 'f', 2)));
        QString status;
        if(batter.isOut()){
            status="OUT";
        }
        else{
            status="Not Out";
        }
         ui->tableBatting->setItem(i, 6, new QTableWidgetItem(status));
        Player &bowler = firstTeam[i];
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

    for (int i = 0; i < Count; ++i) {
        Player &batter = secondTeam[i];
        ui->tableBatting2->setItem(i, 0, new QTableWidgetItem(batter.getName()));
        ui->tableBatting2->setItem(i, 1, new QTableWidgetItem(QString::number(batter.getRuns())));
        ui->tableBatting2->setItem(i, 2, new QTableWidgetItem(QString::number(batter.getBallsFaced())));
        ui->tableBatting2->setItem(i, 3, new QTableWidgetItem(QString::number(batter.getFours())));
        ui->tableBatting2->setItem(i, 4, new QTableWidgetItem(QString::number(batter.getSixes())));
        ui->tableBatting2->setItem(i, 5, new QTableWidgetItem(
                                            QString::number(batter.calculateStrikeRate(), 'f', 2)));
        QString status;
        if(batter.isOut()){
            status="OUT";
        }
        else{
            status="Not Out";
        }
        ui->tableBatting2->setItem(i, 6, new QTableWidgetItem(status));
        Player &bowler = secondTeam[i];
        ui->tableBowling2->setItem(i, 0, new QTableWidgetItem(bowler.getName()));
        int balls = bowler.getBallsBowled();
        int ov  = balls / 6;
        int bi  = balls % 6;
        QString overs = QString::number(ov) + "." + QString::number(bi);
        ui->tableBowling2->setItem(i, 1, new QTableWidgetItem(overs));
        ui->tableBowling2->setItem(i, 2, new QTableWidgetItem(QString::number(bowler.getRunsConceded())));
        ui->tableBowling2->setItem(i, 3, new QTableWidgetItem(QString::number(bowler.getWickets())));
        ui->tableBowling2->setItem(i, 4, new QTableWidgetItem(QString::number(bowler.calculateEconomyRate(), 'f', 2)));
    }

}


void MatchSummaryScreen::on_btnBackToHome_clicked()
{
    this->close();
}

