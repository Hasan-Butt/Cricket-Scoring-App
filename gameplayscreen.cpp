#include "gameplayscreen.h"
#include "ui_gameplayscreen.h"
#include "scorecardscreen.h"
#include "matchsummaryscreen.h"
#include "player.h"
#include "play.h"
#include <QDebug>
#include<QMessageBox>
#include <QTimer>
#include<iostream>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDir>             // for creating the “matches” folder
#include <QFile>            // for reading/writing files
#include <QJsonDocument>    // for serializing QJsonObject to text
#include <QJsonObject>      // for building JSON objects
#include <QJsonArray>       // for building JSON arrays
#include <QVector>          // for your QVector<PlayerPerformance>
#include <QDate>
GamePlayScreen::GamePlayScreen(const QString &teamA, const QString &teamB, QStringList playersA, QStringList playersB,int overs, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GamePlayScreen)
    , lastBowler(nullptr)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground, true);
    ui->comboBox   ->blockSignals(true);
    ui->comboBox_2 ->blockSignals(true);
    ui->comboBox_3 ->blockSignals(true);

    for (auto &raw : playersA)
        ui->comboBox->addItem(raw);
    for (auto &raw : playersB)
        ui->comboBox_2->addItem(raw);
    for (auto &raw : playersA)
        ui->comboBox_3->addItem(raw);

    ui->comboBox   ->blockSignals(false);
    ui->comboBox_2 ->blockSignals(false);
    ui->comboBox_3 ->blockSignals(false);

    playerCount = playersA.size();

    team1Players = new Player[playerCount];
    for (int i = 0; i < playerCount; ++i) {
        auto parsed = parsePlayerString(playersA.at(i));
        team1Players[i] = Player(parsed.second, parsed.first); // name, jersey
    }

    team2Players =new Player[playerCount];
    for (int i = 0; i < playerCount; ++i) {
        auto parsed = parsePlayerString(playersB.at(i));
        team2Players[i] = Player(parsed.second, parsed.first); // name, jersey
    }

    ui->comboBox->setCurrentIndex(0);
    currentBatsman = &team1Players[0];
    currentBowler = &team2Players[0];
    inningsCount = 1;
    currentTeamscore=0;
    qDebug() << "[INIT] currentBatsman =" << currentBatsman->getName();

    teamAName = teamA;
    teamBName = teamB;
    teamAplayers = playersA;
    teamBplayers = playersB;
    overnum=overs;
    ui->nameteamA->setText(teamAName);
    ui->nameteamB->setText(teamBName);
    ui->comboBox_3->setCurrentIndex(1);
    ui->nameBat1->setText(parsePlayerString(ui->comboBox->currentText()).second);
    ui->nameBat2->setText(parsePlayerString(ui->comboBox_3->itemText(1)).second);
    ui->nameBaller->setText(parsePlayerString(ui->comboBox_2->currentText()).second);
    ui->selectedBat1->setText(parsePlayerString(ui->comboBox->currentText()).second);
    ui->selectedBat2->setText(parsePlayerString(ui->comboBox_3->itemText(1)).second);
    ui->selectedBowler->setText(parsePlayerString(ui->comboBox_2->currentText()).second);
    ui->label->setText("<img src=':/icons/scorecard.png' width='31' height='31'/> Live Match Scoreboard");
    ui->label_8->setPixmap(QPixmap(":/icons/baticon.png").scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->label_10->setPixmap(QPixmap(":/icons/ballicon.png").scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->label_12->setPixmap(QPixmap(":/icons/setticon.png").scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->Bat1run->setText("0");
    ui->Bat2run->setText("0");
    ui->Ball1->setText("0");
    ui->Ball2->setText("0");
    ui->bowlerOver->setText("0");
    ui->bowlerRun->setText("0");
    ui->wickets->setText("0");
    ui->teamScore->setText("0");
    ui->teamOvers->setText("0");
    ui->teamWickets->setText("0");
    ui->strike1->setText("*");
    ui->strike2->setText("");
    ui->pushButton_12->setText("⚪DOT");
    ui->pushButton_3->setText("🏃‍ 1");
    ui->pushButton_6->setText("🏃 2");
    ui->pushButton->setText("🏃 3");
    ui->pushButton_4->setText("🎉 4");
    ui->pushButton_5->setText("🎯 5");
    ui->pushButton_2->setText("🚀 6");
    ui->pushButton_8->setText("⚰️ W");
    ui->pushButton_11->setText("↔️ WD");
    ui->pushButton_9->setText("🚫 NB");
    ui->pushButton_10->setText("📊 Scoreboard");
    Team A(teamAName);
    Team B(teamBName);

    playSession=new Play(A,B, overnum);
    totalBallsPerInning = overnum * 6;
    currentBallCount = 0;
    currentTeamWickets=0;
    target=0;
    isFreeHitActive=false;
    isNoBallActive=false;
    Swap=false;
    nonStriker = &team1Players[1];
}

GamePlayScreen::~GamePlayScreen()
{
    delete ui;
    delete[] team1Players;
    delete[] team2Players;
    delete playSession;
}

QPair<int, QString> GamePlayScreen::parsePlayerString(const QString &raw) {
    QString jerseyStr, nameStr;
    int commaCount = 0;

    for (int j = 0; j < raw.length(); j++) {
        QChar ch = raw[j];

        if (ch == ',') {
            commaCount++;
            continue;
        }

        if (commaCount == 0) {
            jerseyStr += ch;
        } else if (commaCount == 1) {
            nameStr += ch;
        } else {
            break;
        }
    }

    return QPair<int, QString>(jerseyStr.toInt(), nameStr);
}
void GamePlayScreen::addRunsToPlayers(int runs){
    ui->comboBox->setEnabled(false);
    ui->comboBox_2->setEnabled(false);
    ui->comboBox_3->setEnabled(false);
    if (isNoBallActive) {

        int total = 1 + runs;
        currentTeamscore += total;
        currentBowler->setRunsConceded(currentBowler->getRunsConceded() + total);
        currentBatsman->setRuns(currentBatsman->getRuns() + runs);


        updateOversDisplay();
        ui->teamScore->setText(QString::number(currentTeamscore));
        ui->bowlerRun->setText(QString::number(currentBowler->getRunsConceded()));


        if (ui->nameBat1->text() == currentBatsman->getName())
            ui->Bat1run->setText(QString::number(currentBatsman->getRuns()));
        else
            ui->Bat2run->setText(QString::number(currentBatsman->getRuns()));

        if (runs % 2 != 0) {
            std::swap(currentBatsman, nonStriker);
            if (ui->strike1->text() == "*") {
                ui->strike1->setText("");
                ui->strike2->setText("*");
            } else {
                ui->strike1->setText("*");
                ui->strike2->setText("");
            }
        }
        isNoBallActive  = false;
        isFreeHitActive = true;
        return;
    }

    // ——— FREE-HIT branch ———
    if (isFreeHitActive) {

        currentTeamscore += runs;
        currentBowler->setRunsConceded(currentBowler->getRunsConceded() + runs);
        currentBatsman->setRuns(currentBatsman->getRuns() + runs);


        currentBallCount++;
        currentBatsman->setBallsFaced(currentBatsman->getBallsFaced() + 1);
        currentBowler->setBallsBowled(currentBowler->getBallsBowled() + 1);


        updateOversDisplay();
        ui->teamScore->setText(QString::number(currentTeamscore));
        ui->bowlerRun->setText(QString::number(currentBowler->getRunsConceded()));
        ui->bowlerOver->setText(
            QString::number(currentBowler->getBallsBowled() / 6) + "." +
            QString::number(currentBowler->getBallsBowled() % 6)
            );


        if (ui->nameBat1->text() == currentBatsman->getName())
            ui->Bat1run->setText(QString::number(currentBatsman->getRuns())),
                ui->Ball1  ->setText(QString::number(currentBatsman->getBallsFaced()));
        else
            ui->Bat2run->setText(QString::number(currentBatsman->getRuns())),
                ui->Ball2  ->setText(QString::number(currentBatsman->getBallsFaced()));


        if (runs % 2 != 0) {
            std::swap(currentBatsman, nonStriker);
            if (ui->strike1->text() == "*") {
                ui->strike1->setText("");
                ui->strike2->setText("*");
            } else {
                ui->strike1->setText("*");
                ui->strike2->setText("");
            }
        }

        if (currentBallCount % 6 == 0) {
            std::swap(currentBatsman, nonStriker);
            if (ui->strike1->text() == "*") {
                ui->strike1->setText("");
                ui->strike2->setText("*");
            } else {
                ui->strike1->setText("*");
                ui->strike2->setText("");
            }
            lastBowler = currentBowler;
            refreshBowlerCombo();
            ui->wickets->setText(QString::number(currentBowler->getWickets()));
        }
        isFreeHitActive = false;
        return;
    }
    currentTeamscore+=runs;
    ui->teamScore->setText(QString::number(currentTeamscore));

    qDebug() << "[DEBUG] currentBatsman:" << (currentBatsman ? currentBatsman->getName() : "nullptr");
    qDebug() << "[DEBUG] currentBowler:" << (currentBowler ? currentBowler->getName() : "nullptr");
    // Update batsman runs
    if (currentBatsman && currentBowler) {
        currentBatsman->setRuns(currentBatsman->getRuns() + runs);
        currentBowler->setRunsConceded(currentBowler->getRunsConceded() + runs);

        if (ui->nameBat1->text() == currentBatsman->getName()) {
            ui->Bat1run->setText(QString::number(currentBatsman->getRuns()));
        } else if (ui->nameBat2->text() == currentBatsman->getName()) {
            ui->Bat2run->setText(QString::number(currentBatsman->getRuns()));
        }

        ui->bowlerRun->setText(QString::number(currentBowler->getRunsConceded()));
        qDebug() << currentBatsman->getName() << " now has " << currentBatsman->getRuns() << " runs";
        qDebug() << currentBowler->getName() << " conceded total " << currentBowler->getRunsConceded() << " runs";
    }

    // Ball tracking
    currentBallCount++;
    if(currentBatsman && currentBowler){
        currentBatsman->setBallsFaced(currentBatsman->getBallsFaced()+1);
        currentBowler->setBallsBowled(currentBowler->getBallsBowled()+1);
        if(ui->nameBat1->text()==currentBatsman->getName()){
            ui->Ball1->setText(QString::number(currentBatsman->getBallsFaced()));
        }
        else if(ui->nameBat2->text() == currentBatsman->getName()){
            ui->Ball2->setText(QString::number(currentBatsman->getBallsFaced()));
        }
        ui->bowlerOver->setText(QString::number(currentBowler->getBallsBowled()/6) + "." + QString::number(currentBowler->getBallsBowled()%6));
        ui->wickets->setText(QString::number(currentBowler->getWickets()));
    }
    int completedOvers = currentBallCount / 6;
    int remainingBalls = currentBallCount % 6;

    ui->teamOvers->setText(QString::number(completedOvers) + "." + QString::number(remainingBalls));
    if (runs % 2 != 0) {
        // Odd runs => rotate striker
        std::swap(currentBatsman, nonStriker);

        if(ui->strike1->text()=="*"){
            ui->strike1->setText("");
            ui->strike2->setText("*");
        }
        else{
            ui->strike1->setText("*");
            ui->strike2->setText("");
        }

    }

    if (currentBallCount % 6 == 0) {
        // Over complete => rotate strike
        std::swap(currentBatsman, nonStriker);


        if(ui->strike1->text()=="*"){
            ui->strike1->setText("");
            ui->strike2->setText("*");
        }
        else{
            ui->strike1->setText("*");
            ui->strike2->setText("");
        }
        lastBowler = currentBowler;
        refreshBowlerCombo();
        if(currentBowler)
            ui->wickets->setText(QString::number(currentBowler->getWickets()));
        qDebug() << "[INFO] Over completed. Striker and non-striker swapped.";
    }

    // Check for innings over
    if (currentBallCount >= totalBallsPerInning || currentTeamWickets==playerCount-1 || (inningsCount == 2
                                                                                            && currentTeamscore > playSession->getTeamA().getTotalScore())) {
        inningsCount++;
        if (inningsCount == 2) {
            playSession->getTeamA().setScore(currentTeamscore);
            playSession->getTeamA().setWicketsLost(currentTeamWickets);
            if(currentBatsman->getRuns()>=100){

                currentBatsman->setCenturies(1);
            }
            else if(currentBatsman->getRuns()>=50){
                 currentBatsman->setHalfCenturies(1);
            }
            target=currentTeamscore;
            QString target=QString::number(playSession->getTeamA().getTotalScore()+1);
            showPopup("Target!", "🎯Need "+target+"\nruns to\nWIN!", QPixmap(":/icons/baticon.png"), QColor("#4CAF50"));
            qDebug()<<playSession->getTeamA().getTotalScore();
        } else if (inningsCount == 3) {
            playSession->getTeamB().setScore(currentTeamscore);
            playSession->getTeamB().setWicketsLost(currentTeamWickets);
            qDebug()<<playSession->getTeamB().getTotalScore();
            qDebug()<<"Team B score: "<<playSession->getTeamB().getTotalScore()<<" WK: "<<playSession->getTeamB().getWicketsLost();
        }


        ui->comboBox  ->blockSignals(true);
        ui->comboBox_3->blockSignals(true);
        ui->comboBox->clear();
        for (auto &raw : teamBplayers)
            ui->comboBox->addItem(raw);
        ui->comboBox_2->clear();
        for (auto &raw : teamAplayers)
            ui->comboBox_2->addItem(raw);
        ui->comboBox_3->clear();
        for (auto &raw : teamBplayers)
            ui->comboBox_3->addItem(raw);
        ui->comboBox  ->blockSignals(false);
        ui->comboBox_3->blockSignals(false);

        ui->comboBox->setCurrentIndex(0);
        ui->comboBox_2->setCurrentIndex(0);
        ui->comboBox_3->setCurrentIndex(playerCount>1 ? 1 : 0);

        ui->comboBox->setEnabled(true);
        ui->comboBox_2->setEnabled(true);
        ui->comboBox_3->setEnabled(true);
        ui->Bat1run->setText("0");
        ui->Bat2run->setText("0");
        ui->Ball1->setText("0");
        ui->Ball2->setText("0");
        ui->bowlerOver->setText("0");
        ui->bowlerRun->setText("0");
        ui->wickets->setText("0");
        ui->teamScore->setText("0");
        ui->teamOvers->setText("0");
        ui->teamWickets->setText("0");
        ui->strike1->setText("*");
        ui->strike2->setText("");
        ui->nameteamA->setText(teamBName);
        ui->nameteamB->setText(teamAName);


        qDebug() << "[INFO] Innings Over!";
        // Trigger innings switch logic here
        currentBatsman = &team2Players[0];
        nonStriker = &team2Players[1];
        currentBowler = &team1Players[0];
        ui->nameBat1->setText(currentBatsman->getName());
        ui->nameBat2->setText(nonStriker->getName());
        ui->selectedBat1->setText(currentBatsman->getName());
        ui->selectedBat2->setText(nonStriker->getName());

        if(inningsCount==3){
            ui->comboBox->setEnabled(false);
            ui->comboBox_2->setEnabled(false);
            ui->comboBox_3->setEnabled(false);
            ui->pushButton->setEnabled(false);
            ui->pushButton_2->setEnabled(false);
            ui->pushButton_3->setEnabled(false);
            ui->pushButton_4->setEnabled(false);
            ui->pushButton_5->setEnabled(false);
            ui->pushButton_6->setEnabled(false);
            ui->pushButton_8->setEnabled(false);
            ui->pushButton_9->setEnabled(false);
            ui->pushButton_11->setEnabled(false);
            ui->pushButton_12->setEnabled(false);

            int scoreA = playSession->getTeamA().getTotalScore();
            int scoreB = playSession->getTeamB().getTotalScore();
            QString result;
            if (scoreA > scoreB) {
                result = teamAName + " won by " + QString::number(scoreA - scoreB) + " runs 🏆!";
            } else if (scoreB > scoreA) {
                result = teamBName + " won by " + QString::number(playerCount - playSession->getTeamB().getWicketsLost()) + " wickets 🏆!";
            } else {
                result = "The match is a tie! 🤝";
            }
            QMessageBox::information(this, "Match Over", result);

            qDebug() << "=== Team A batting card ===";
            for (int i = 0; i < playerCount; ++i) {
                qDebug() << "  " << teamAplayers[i] << ": "
                         << team1Players[i].getRuns()
                         << " (" << team1Players[i].getBallsFaced() << " balls)";
            }
            qDebug() << "=== Team B batting card ===";
            for (int i = 0; i < playerCount; ++i) {
                qDebug() << "  " << teamBplayers[i] << ": "
                         << team2Players[i].getRuns()
                         << " (" << team2Players[i].getBallsFaced() << " balls)";
            }
            QMessageBox::StandardButton reply =
                QMessageBox::question(
                    this,
                    tr("Save Match Stats"),
                    tr("Do you want to save this match's stats?"),
                    QMessageBox::Ok | QMessageBox::Cancel
                    );

            if (reply == QMessageBox::Ok) {
                updateMatchStats();
                saveMatch();
                QMessageBox::information(this, tr("Saved"), tr("Match stats have been saved."));
            } else {
                QMessageBox::information(this, tr("Skipped"), tr("Match stats were not saved."));
            }
            int scoresA = playSession->getTeamA().getTotalScore();
            int scoresB = playSession->getTeamB().getTotalScore();

            emit matchEnded(scoresA, scoresB);
            this->close();
            matchsummaryscreen = new MatchSummaryScreen(teamAName, teamBName, team1Players, team2Players, playerCount, nullptr);
            matchsummaryscreen->setAttribute(Qt::WA_DeleteOnClose);
            matchsummaryscreen->show();
            return;

        }

        currentBallCount=0;
        currentTeamscore=0;
        currentTeamWickets=0;
        Swap=false;

    }
}

void GamePlayScreen::updateOversDisplay() {
    int completedOvers = currentBallCount / 6;
    int remBalls      = currentBallCount % 6;
    ui->teamOvers->setText(
        QString::number(completedOvers) + "." + QString::number(remBalls)
        );
}
Player* GamePlayScreen::activeBattingArray() const{
    return (inningsCount == 1 ? team1Players : team2Players);
}

void GamePlayScreen::refreshBattingCombos() {
    Player* arr = activeBattingArray();
    QStringList& names = (inningsCount == 1 ? teamAplayers : teamBplayers);

    ui->comboBox  ->blockSignals(true);
    ui->comboBox_3->blockSignals(true);
    ui->comboBox  ->clear();
    ui->comboBox_3->clear();

    for (int i = 0; i < playerCount; ++i) {
        if (!arr[i].isOut()) {
            ui->comboBox  ->addItem(names[i]);
            ui->comboBox_3->addItem(names[i]);
        }
    }

    ui->comboBox  ->blockSignals(false);
    ui->comboBox_3->blockSignals(false);
}

void GamePlayScreen::refreshBowlerCombo() {
    Player* arr = (inningsCount == 1 ? team2Players : team1Players);
    QStringList& names = (inningsCount == 1 ? teamBplayers : teamAplayers);

    ui->comboBox_2->blockSignals(true);
    ui->comboBox_2->clear();

    for (int i = 0; i < playerCount; ++i) {
        if (&arr[i] != lastBowler) {
            ui->comboBox_2->addItem(names[i]);
        }
    }

    ui->comboBox_2->blockSignals(false);

    if (ui->comboBox_2->count() > 0) {
        ui->comboBox_2->setCurrentIndex(0);
        int jerseyNum = parsePlayerString(ui->comboBox_2->currentText()).first;
        Player* searchArray = (inningsCount == 1)? team2Players: team1Players;

        for (int i = 0; i < playerCount; ++i) {
            if (searchArray[i].getjurseyNumber() == jerseyNum) {
                currentBowler = &searchArray[i];
                break;
            }
        }
        QString name = currentBowler->getName();
        ui->nameBaller    ->setText(name);
        ui->selectedBowler->setText(name);
        ui->wickets->setText(QString::number(currentBowler->getWickets()));
        ui->bowlerOver->setText(QString::number(currentBowler->getBallsBowled()/6) + "." + QString::number(currentBowler->getBallsBowled()%6));
        ui->bowlerRun->setText(QString::number(currentBowler->getRunsConceded()));

    }
    ui->comboBox_2->setEnabled(true);
}

void GamePlayScreen::showPopup(const QString& title,
                               const QString& text,
                               const QPixmap& icon,
                               const QColor& borderColor)
{
    QMessageBox* msg = new QMessageBox(this);
    msg->setAttribute(Qt::WA_DeleteOnClose);
    msg->setWindowTitle(title);
    msg->setText(text);
    msg->setIconPixmap(icon.scaled(390,372));
    msg->setIcon(QMessageBox::NoIcon);
    msg->setStandardButtons(QMessageBox::NoButton);
    msg->setStyleSheet(QString(R"(
        QMessageBox {
            background: qlineargradient(x1:0,y1:0, x2:0,y2:1,
                                        stop:0 #f6f7fa, stop:1 #dadbde);
            background-color:blue;
            border: 10px solid %1;
            border-radius: 15px;
        }
        QLabel { font-size: 40px; font-weight: bold; color: White; }
    )").arg(borderColor.name()));

    msg->show();
    QTimer::singleShot(0, this, [this, msg]() {
        msg->setFixedSize(280, 200);

        QPoint center = this->geometry().center()
                        - QPoint(msg->width()/2, msg->height()/2);
        msg->move(center);
    });
    QTimer::singleShot(3000, msg, &QMessageBox::accept);

}

//Striker Selection
//Baller Selection
void GamePlayScreen::on_comboBox_2_currentTextChanged(const QString &arg1)
{
    int jerseyNum = parsePlayerString(arg1).first;
    Player* searchArray = (inningsCount == 1)? team2Players: team1Players;

    for (int i = 0; i < playerCount; ++i) {
        if (searchArray[i].getjurseyNumber() == jerseyNum) {
            currentBowler = &searchArray[i];
            break;
        }
    }
    QString name = parsePlayerString(arg1).second;
    ui->nameBaller    ->setText(name);
    ui->selectedBowler->setText(name);
}
//1
void GamePlayScreen::on_pushButton_3_clicked()
{
    addRunsToPlayers(1);
}


//2
void GamePlayScreen::on_pushButton_6_clicked()
{
    addRunsToPlayers(2);
}

//3
void GamePlayScreen::on_pushButton_clicked()
{
    addRunsToPlayers(3);
}

//4
void GamePlayScreen::on_pushButton_4_clicked()
{
    addRunsToPlayers(4);
    if(inningsCount<3)
        showPopup("Boundary!", "🎊 Its a\nFOUR !", QPixmap(":/icons/baticon.png"), QColor("#4CAF50"));
    currentBatsman->setFours(currentBatsman->getFours()+1);
}

//5
void GamePlayScreen::on_pushButton_5_clicked()
{
    addRunsToPlayers(5);
}


//6
void GamePlayScreen::on_pushButton_2_clicked()
{
    addRunsToPlayers(6);
    QSoundEffect *clickSound = new QSoundEffect(this);
    clickSound->setSource(QUrl::fromLocalFile(":/sounds/crowdnew.wav"));
    clickSound->setVolume(2.0);
    clickSound->play();

    if(inningsCount<3)
        showPopup("Boundary!", "🚀 Its a\nSIX ! 🔥", QPixmap(":/icons/baticon.png"), QColor("#4CAF50"));
    currentBatsman->setSixes(currentBatsman->getSixes()+1);
}

//Score Card
void GamePlayScreen::on_pushButton_10_clicked()
{
    Player* battingArr = (inningsCount == 1 ? team1Players : team2Players);
    Player* bowlingArr = (inningsCount == 1 ? team2Players : team1Players);
    QString battingName = (inningsCount == 1 ? teamAName : teamBName);
    QString bowlingName = (inningsCount == 1 ? teamBName : teamAName);

    scorecardscreen = new ScoreCardScreen(battingName,
                                          bowlingName,
                                          battingArr,
                                          bowlingArr,
                                          playerCount,
                                          target-currentTeamscore,
                                          nullptr);
    scorecardscreen->setAttribute(Qt::WA_DeleteOnClose);
    scorecardscreen->show();
}

void GamePlayScreen::on_pushButton_12_clicked()
{
    addRunsToPlayers(0);
}


void GamePlayScreen::on_pushButton_11_clicked()
{
    currentTeamscore++;
    currentBowler->setRunsConceded(currentBowler->getRunsConceded() + 1);
    currentBatsman->setBallsFaced(currentBatsman->getBallsFaced()-1);
    currentBowler->setBallsBowled(currentBowler->getBallsBowled()-1);
    addRunsToPlayers(0);

    if (currentBallCount > 0)
        --currentBallCount;

    {
        int completedOvers = currentBallCount / 6;
        int remainingBalls = currentBallCount % 6;
        ui->teamOvers->setText(
            QString::number(completedOvers) + "." +
            QString::number(remainingBalls)
            );
    }

    ui->bowlerRun->setText(QString::number(currentBowler->getRunsConceded()));
    ui->teamScore ->setText(QString::number(currentTeamscore));

}


void GamePlayScreen::on_pushButton_9_clicked()
{
    isNoBallActive = true;
    if (inningsCount < 3)
        showPopup("No Ball!", "🆓 🏏 Its a\nFree-Hit!",
                  QPixmap(":/icons/baticon.png"), QColor("#4CAF50"));
}


void GamePlayScreen::on_pushButton_8_clicked()
{
    if (isNoBallActive || isFreeHitActive) {
        isNoBallActive  = false;
        isFreeHitActive = false;
        addRunsToPlayers(0);
        return;
    }

    currentBatsman->setOut(true);
    currentTeamWickets++;
    if(currentBatsman->getRuns()>=100){
        currentBatsman->setCenturies(1);
    }
    else if(currentBatsman->getRuns()>=50){
        currentBatsman->setHalfCenturies(1);
    }

    ui->teamWickets->setText(QString::number(currentTeamWickets));
    currentBowler->setWickets(currentBowler->getWickets()+1);
    ui->wickets->setText(QString::number(currentBowler->getWickets()));
    if(inningsCount<3)
        showPopup("OUT!", "⚰️ Its a\nWicket!", QPixmap(":/icons/baticon.png"), QColor("#4CAF50"));

    addRunsToPlayers(0);
    refreshBattingCombos();

    if(ui->strike1->text()=="*"){
        ui->comboBox->setEnabled(true);
    }
    else{
        ui->comboBox_3->setEnabled(true);
    }
    if(ui->strike2->text()=="*"){
        std::swap(currentBatsman, nonStriker);
    }
    Swap=true;
}

void GamePlayScreen::on_comboBox_currentIndexChanged(int)
{
    if(ui->comboBox->currentText()==ui->comboBox_3->currentText()){
        QMessageBox::warning(this,"Invalid selection","You cannot select the same player as both striker and non‑striker.\nSelecting a valid.");
        if(ui->comboBox->currentIndex()==ui->comboBox->count()-1){
            ui->comboBox->setCurrentIndex(0);
        }
        else {
            ui->comboBox->setCurrentIndex(ui->comboBox->currentIndex()+1);
        }
    }
    QString raw = ui->comboBox->currentText();
    int jersey = parsePlayerString(raw).first;

    Player* arr = activeBattingArray();
    for (int i = 0; i < playerCount; ++i) {
        if (arr[i].getjurseyNumber() == jersey) {
            currentBatsman = &arr[i];
            break;
        }
    }
    QString name = parsePlayerString(raw).second;
    ui->nameBat1    ->setText(name);
    ui->selectedBat1->setText(name);
}


void GamePlayScreen::on_comboBox_3_currentIndexChanged(int)
{
    if(ui->comboBox->currentText()==ui->comboBox_3->currentText()){
        QMessageBox::warning(this,"Invalid selection","You cannot select the same player as both striker and non‑striker.\nSelecting a valid.");
        if(ui->comboBox_3->currentIndex()==ui->comboBox_3->count()-1){
            ui->comboBox_3->setCurrentIndex(0);
        }
        else {
            ui->comboBox_3->setCurrentIndex(ui->comboBox_3->currentIndex()+1);
        }
    }
    QString raw = ui->comboBox_3->currentText();
    int jersey  = parsePlayerString(raw).first;

    Player* arr = activeBattingArray();
    for (int i = 0; i < playerCount; ++i) {
        if (arr[i].getjurseyNumber() == jersey) {
            nonStriker = &arr[i];
            break;
        }
    }
    if(Swap){
        std::swap(currentBatsman, nonStriker);
    }
    QString name = parsePlayerString(raw).second;
    ui->nameBat2    ->setText(name);
    ui->selectedBat2->setText(name);
}
void GamePlayScreen::UpdatePlayerStatsFile(int jerseyNumber, Player *player) {
    QString filename = QString::number(jerseyNumber) + ".txt";
    QFile file(filename);

    // Only update if the file already exists
    if (!file.exists()) return;

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qWarning() << "Could not open stats file for player" << jerseyNumber;
        return;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.resize(0);
    file.seek(0);
    QTextStream out(&file);

    // Split into lines and set defaults
    QStringList lines = content.split('\n', Qt::SkipEmptyParts);
    int prevRuns = 0, prevBalls = 0, prevCenturies = 0, prevHalf = 0, prevWickets = 0, prevHigh = 0;

    // Parse each existing stat
    for (int i = 0, n = lines.size(); i < n; ++i) {
        const QString &ln = lines[i];
        if (ln.startsWith("Runs:"))
            prevRuns = ln.section(':',1).trimmed().toInt();
        else if (ln.startsWith("Balls:"))
            prevBalls = ln.section(':',1).trimmed().toInt();
        else if (ln.startsWith("Centuries:"))
            prevCenturies = ln.section(':',1).trimmed().toInt();
        else if (ln.startsWith("Half-Centuries:"))
            prevHalf = ln.section(':',1).trimmed().toInt();
        else if (ln.startsWith("Wickets:"))
            prevWickets = ln.section(':',1).trimmed().toInt();
        else if (ln.startsWith("Highest Score:"))
            prevHigh = ln.section(':',1).trimmed().toInt();
    }

    // Add this match's stats
    int totalRuns    = prevRuns    + player->getRuns();
    int totalBalls   = prevBalls   + player->getBallsFaced();
    int totalCents   = prevCenturies + player->getCenturies();
    int totalHalves  = prevHalf    + player->getHalfCenturies();
    int totalWicks   = prevWickets + player->getWickets();
    int totalHigh    = qMax(prevHigh, player->getRuns());

    // Derive new rates from totals
    double newStrike = totalBalls ? (100.0 * totalRuns / totalBalls) : 0.0;
    double newEcon   = totalBalls ? (6.0 * totalWicks / totalBalls) : 0.0;

    // Write back the updated cumulative stats
    out << "Runs: "           << totalRuns    << "\n"
        << "Balls: "         << totalBalls   << "\n"
        << "Centuries: "     << totalCents   << "\n"
        << "Half-Centuries: "<< totalHalves  << "\n"
        << "StrikeRate: "    << QString::number(newStrike, 'f', 2) << "\n"
        << "Economy: "       << QString::number(newEcon,   'f', 2) << "\n"
        << "Wickets: "       << totalWicks   << "\n"
        << "Highest Score: " << totalHigh    << "\n";

    file.close();
}
void GamePlayScreen::updateMatchStats() {

    for (int i = 0; i <playerCount; ++i) {
        UpdatePlayerStatsFile(team1Players[i].getjurseyNumber(), &team1Players[i]);
    }

    for (int i = 0; i < playerCount; ++i) {
        UpdatePlayerStatsFile(team2Players[i].getjurseyNumber(), &team2Players[i]);
    }
}
void GamePlayScreen::saveMatch(){
    MatchSummary match;
    match.date = QDate::currentDate().toString("yyyy-MM-dd");
    match.teamAName = playSession->getTeamA().getTeamName();
    match.teamAScore = QString::number(playSession->getTeamA().getTotalScore()) + "/" + QString::number(playSession->getTeamA().getWicketsLost());
    match.teamBName =playSession->getTeamB().getTeamName();
    match.teamBScore = QString::number(playSession->getTeamB().getTotalScore()) + "/" + QString::number(playSession->getTeamB().getWicketsLost());

    if (playSession->getTeamA().getTotalScore() > playSession->getTeamB().getTotalScore()) {
        int runDiff = playSession->getTeamA().getTotalScore()
                      - playSession->getTeamB().getTotalScore();

        match.winner = match.teamAName;
        match.margin = QString::number(runDiff) + " runs";
    } else if (playSession->getTeamB().getTotalScore() > playSession->getTeamA().getTotalScore()) {
        int wicketsRemaining = playerCount - playSession->getTeamB().getWicketsLost();
        match.winner = match.teamBName;
        match.margin = QString::number(wicketsRemaining) + " wickets";
    } else {
        match.winner = "Tie";
        match.margin = "N/A";
    }

    QVector<PlayerPerformance> performances;
    for (int i = 0; i < playerCount; ++i) {
        PlayerPerformance p;
        Player &pl = team1Players[i];
        p.jersey = pl.getjurseyNumber();
        p.name   = pl.getName();
        p.runs   = pl.getRuns();
        p.balls  = pl.getBallsFaced();
        p.fours  = pl.getFours();
        p.sixes  = pl.getSixes();
        p.strikerate = pl.calculateStrikeRate();
        p.ballerruns = pl.getRunsConceded();
        p.ballsbowled = pl.getBallsBowled();
        p.wickets = pl.getWickets();
        p.economy = pl.calculateEconomyRate();
        performances.append(p);
    }

    // second innings’ batting card
    for (int i = 0; i < playerCount; ++i) {
        PlayerPerformance p;
        Player &pl = team2Players[i];
        p.jersey = pl.getjurseyNumber();
        p.name   = pl.getName();
        p.runs   = pl.getRuns();
        p.balls  = pl.getBallsFaced();
        p.fours  = pl.getFours();
        p.sixes  = pl.getSixes();
        p.strikerate = pl.calculateStrikeRate();
        p.ballerruns = pl.getRunsConceded();
        p.ballsbowled = pl.getBallsBowled();
        p.wickets = pl.getWickets();
        p.economy = pl.calculateEconomyRate();
        performances.append(p);
    }

    // write a per-match JSON file under matches
    saveMatchJSON(match, performances);
}
bool GamePlayScreen::saveMatchJSON(
    const MatchSummary &m,
    const QVector<PlayerPerformance> &performances)
{
    QDir().mkpath("matches");

    QString fname = QString("matches/%1_%2_vs_%3.json")
                        .arg(m.date)
                        .arg(m.teamAName)
                        .arg(m.teamBName);
    //Building json
    QJsonObject root;
    root["Date"]    = m.date;
    root["TeamA Name"]   = m.teamAName;
    root["ScoreA"]  = m.teamAScore;
    root["TeamB"]   = m.teamBName;
    root["ScoreB"]  = m.teamBScore;
    root["Winner"]  = m.winner;
    root["Margin"]  = m.margin;
    QJsonArray arr;
    for (auto &p : performances) {
        QJsonObject o;
        o["Name"]   = p.name;
        o["Jersey"] = p.jersey;
        o["Runs"]   = p.runs;
        o["Balls"]  = p.balls;
        o["Fours"]  = p.fours;
        o["Sixes"]  = p.sixes;
        o["Strike Rate"] = p.strikerate;
        o["Runs Conceded"] = p.ballerruns;
        o["Balls Bowled"] = p.ballsbowled;
        o["Wickets"] = p.wickets;
        o["Economy"] = p.economy;
        arr.append(o);
    }
    root["Performances"] = arr;

    // 4) write file
    QFile f(fname);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("Cannot write %s", qPrintable(fname));
        return false;
    }
    f.write(QJsonDocument(root).toJson());
    f.close();
    return true;
}
