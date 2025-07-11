#include "matchmodescreen.h"
#include "ui_matchmodescreen.h"
#include "mainwindow.h"
#include<QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <iostream>
#include <QMap>
#include <QTableWidget>
#include <QTableWidgetItem>

matchmodescreen::matchmodescreen(MainWindow *mainWin, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::matchmodescreen)
    , mainWindow(mainWin)
{
    ui->setupUi(this);
    ui->homeButton->setIcon(QIcon(":/icons/homeicon.jpg"));
    connect(ui->homeButton, &QPushButton::clicked,mainWindow,  &MainWindow::returnToHome);
    connect(ui->homeButton, &QPushButton::clicked,this,&matchmodescreen::close);
}

matchmodescreen::~matchmodescreen()
{
    delete ui;
}

bool matchmodescreen::isTextEmpty(const QString &text) {
    for (int i = 0; i < text.length(); ++i) {
        if (text[i] != ' ' && text[i] != '\t' && text[i] != '\n') {
            return false;
        }
    }
    return true;
}

void matchmodescreen::on_spinBoxPlayerCount_valueChanged(int playerCount)
{
    ui->tableWidgetTeamA->setRowCount(playerCount);
    ui->tableWidgetTeamB->setRowCount(playerCount);
}


void matchmodescreen::on_btnProceedToToss_clicked()
{

    int rows = ui->tableWidgetTeamA->rowCount();
    int cols=ui->tableWidgetTeamA->columnCount();
    bool allFilled = true;

    // Check Team A
    for (int i = 0; i < rows; ++i) {
        QTableWidgetItem *item = ui->tableWidgetTeamA->item(i, 0);
        if (!item || isTextEmpty(item->text())) {   // Use the custom function
            allFilled = false;
            break;
        }
    }

    //Check Team B
    if (allFilled) {
        for (int i = 0; i < rows; ++i) {
            QTableWidgetItem *item = ui->tableWidgetTeamB->item(i, 0);
            if (!item || isTextEmpty(item->text())) {  // Use the custom function
                allFilled = false;
                break;
            }
        }
    }

    if(!allFilled) {
        QMessageBox::warning(this, "Incomplete", "Please fill all player names before starting the match.");
        return;
    }

    teamAName = ui->lineEditTeamA->text();
    teamBName = ui->lineEditTeamB->text();
    if (teamAName.isEmpty() || teamBName.isEmpty()) {
        QMessageBox::warning(this, "Missing Team Names", "Please enter both team names before proceeding to toss.");
        return;
    }
    if( ui->spinBox_Over_Value->value()==0){
        QMessageBox::warning(this,"Overs Not Set","Please set the overs to proceed." );
        return;
    }
    playerNamesA.clear();
    playerNamesB.clear();
    for (int row = 0; row < rows; ++row) {
        QStringList rowData;
        for(int j=0; j<cols; j++){

            QTableWidgetItem *item = ui->tableWidgetTeamA->item(row, j);
            rowData<<item->text();

        }
        playerNamesA << rowData.join(",");

    }

    for (int row = 0; row < rows; ++row) {
        QStringList rowData;
        for(int j=0; j<cols; j++){

            QTableWidgetItem *item = ui->tableWidgetTeamB->item(row, j);
            rowData<<item->text();

        }
        playerNamesB << rowData.join(",");
    }

    int overs=ui->spinBox_Over_Value->value();
    if (!savePlayersToFile()) {
        return;
    }
        this->close();
        tossScreen = new TossScreen(teamAName, teamBName, playerNamesA, playerNamesB,overs, nullptr);
        tossScreen->setAttribute(Qt::WA_DeleteOnClose);
        tossScreen->show();
        connect(tossScreen, &TossScreen::gameplayCreated, this, [this](GamePlayScreen *gp){
            connect(gp, &GamePlayScreen::matchEnded,
                    this, &matchmodescreen::matchFinished);
        });


}



void matchmodescreen::on_spinBox_Over_Value_valueChanged(int value)
{
    if (value == 0) {
        ui->spinBox_Over_Value->setValue(1); // force it to 1
        QMessageBox::warning(this, "Invalid Overs", "Overs cannot be zero. Setting to 1 as it is minimum.");
    }
}

bool matchmodescreen::savePlayersToFile() {
    QFile file("players.txt");
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QMessageBox::warning(this, "File Error", "Could not open players.txt for writing.");
        return false;
    }

    QTextStream out(&file);
    int rows = ui->tableWidgetTeamA->rowCount();
    int cols = ui->tableWidgetTeamA->columnCount();

    // Build map of existing players
    QStringList existingPlayers;
    QMap<QString, QString> jerseyToNameMap;

    while (!file.atEnd()) {
        QStringList playerData = QString::fromUtf8(file.readLine()).simplified().split(',');

        if (playerData.size() >= 2) {
            existingPlayers << playerData[0];
            jerseyToNameMap[playerData[0]] = playerData[1];
        }
    }

    auto saveTeam = [&](QTableWidget *table) {
        for (int i = 0; i < rows; ++i) {
            QString jersey = table->item(i,0)->text();
            QString name   = table->item(i,1)->text();
            // duplicate jersey with different name?
            if (existingPlayers.contains(jersey)
                && jerseyToNameMap[jersey] != name) {
                QMessageBox::warning(
                    this,
                    "Duplicate Jersey Number",
                    QString("Jersey %1 is already assigned to %2.").arg(jersey, jerseyToNameMap[jersey])
                    );
                file.close();
                return false;
            }
            // new player → append
            if (!existingPlayers.contains(jersey)) {
                QStringList rowData;
                for (int c = 0; c < cols; ++c)
                   rowData << table->item(i,c)->text();
                out << rowData.join(",") << "\n";
                createPlayerStatsFile(jersey.toInt());
                existingPlayers << jersey;
            }
        }
        return true;
    };

    // Save A, then B
    file.seek(file.size()); // move to end for appends
    if (!saveTeam(ui->tableWidgetTeamA) || !saveTeam(ui->tableWidgetTeamB)) {
        return false;
    }

    file.close();
    return true;
}

void matchmodescreen::createPlayerStatsFile(int jerseyNumber) {
    QFile playerFile(QString::number(jerseyNumber) + ".txt");
    if (playerFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&playerFile);
        out << "Runs: 0\nBalls: 0\nCenturies: 0\nHalf-Centuries: 0\nStrikeRate: 0\nEconomy: 0\nWickets: 0\nAverage Highest:\nHighest Score:\n";
        playerFile.close();
    } else {
        std::cerr << "Error creating stats file for player " << jerseyNumber << std::endl;
    }
}
