#include "tournamentmodescreen.h"
#include "ui_tournamentmodescreen.h"
#include "mainwindow.h"
#include "matchmodescreen.h"
#include <QString>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QRandomGenerator>
#include <algorithm>
#include <random>
TournamentModeScreen::TournamentModeScreen(MainWindow *mainWin, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TournamentModeScreen)
    , mainWindow(mainWin)
    , MatchModeScreen(nullptr)
{
    ui->setupUi(this);
    ui->labelWinner->setVisible(false);

    ui->homeButton->setIcon(QIcon(":icons/homeicon.jpg"));
    connect(ui->homeButton, &QPushButton::clicked, this, [this](){
        if (MatchModeScreen && MatchModeScreen->isVisible())
            MatchModeScreen->close();
    });
    connect(ui->homeButton, &QPushButton::clicked,
            mainWindow, &MainWindow::returnToHome);
    connect(ui->homeButton, &QPushButton::clicked,
            this, &TournamentModeScreen::close);
}

TournamentModeScreen::~TournamentModeScreen()
{
    delete ui;
}

void TournamentModeScreen::on_addTeam_clicked()
{
    QString teamName = ui->teamText->text();
    if (!teamName.isEmpty()) {

        QListWidgetItem *newItem = new QListWidgetItem(teamName);

        ui->teamList->addItem(newItem);


        ui->teamText->clear();
    }
}


void TournamentModeScreen::on_removeTeam_clicked()
{
    QListWidgetItem *selectedItem = ui->teamList->currentItem();
    if (selectedItem) {
        delete selectedItem;
    }
}

void TournamentModeScreen::on_generateSchedule_clicked()
{
    teamNames.clear();
    matches.clear();
    statsMap.clear();
    matchesPlayed = 0;

    // Collect teams
    for(int i = 0; i < ui->teamList->count(); ++i)
        teamNames << ui->teamList->item(i)->text();

    if(teamNames.size() < 2) return;

    // Generate all possible match combinations
    for(int i = 0; i < teamNames.size(); ++i) {
        for(int j = i+1; j < teamNames.size(); ++j) {
            matches.append({teamNames[i], teamNames[j], nullptr});
        }
    }

    totalMatches = matches.size();

    // Shuffle matches for random order
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(matches.begin(), matches.end(), g);

    // Populate fixtures table
    auto *tw = ui->fixturesTable;
    tw->clear();
    tw->setColumnCount(3);
    tw->setHorizontalHeaderLabels({ "Team A", "Team B", "Play" });
    tw->setRowCount(matches.size());

    // Initialize stats
    for(const auto &team : teamNames)
        statsMap[team] = TeamStats();

    for(int row = 0; row < matches.size(); ++row) {
        const auto m_copy = matches[row];

        tw->setItem(row, 0, new QTableWidgetItem(m_copy.teamA));
        tw->setItem(row, 1, new QTableWidgetItem(m_copy.teamB));

        auto *btn = new QPushButton("▶️");
        matches[row].playBtn = btn;
        tw->setCellWidget(row, 2, btn);

        connect(btn, &QPushButton::clicked, this, [this, row, m_copy](){
            auto *screen = new matchmodescreen(mainWindow);
            screen->show();

            connect(screen, &matchmodescreen::matchFinished,
                    this, [this, row, m_copy](int scoreA, int scoreB) {
                        // Update statistics
                        auto& stA = statsMap[m_copy.teamA];
                        auto& stB = statsMap[m_copy.teamB];

                        stA.played++;
                        stB.played++;
                        stA.runsFor += scoreA;
                        stA.runsAgainst += scoreB;
                        stB.runsFor += scoreB;
                        stB.runsAgainst += scoreA;

                        if(scoreA > scoreB) {
                            stA.wins++;
                            stA.points += 2;
                            stB.losses++;
                        } else if(scoreB > scoreA) {
                            stB.wins++;
                            stB.points += 2;
                            stA.losses++;
                        } else {
                            stA.draws++;
                            stB.draws++;
                            stA.points++;
                            stB.points++;
                        }

                        // Update match display
                        ui->fixturesTable->item(row, 0)->setText(
                            QString("%1 (%2)").arg(m_copy.teamA).arg(scoreA));
                        ui->fixturesTable->item(row, 1)->setText(
                            QString("%1 (%2)").arg(m_copy.teamB).arg(scoreB));

                        matchesPlayed++;
                        refreshStandings();

                        if(matchesPlayed == totalMatches) {
                            updateWinnerLabel();
                        }
                    });
        });
    }
    ui->tabWidget->setCurrentIndex(0);
}
void TournamentModeScreen::refreshStandings()
{
    auto *tw = ui->standingsTable;
    tw->clear();

    QVector<QString> sorted = statsMap.keys().toVector();

    std::sort(sorted.begin(), sorted.end(), [&](const QString &a, const QString &b) {
        const TeamStats &statsA = statsMap[a];
        const TeamStats &statsB = statsMap[b];

        // Primary sort: Points
        if(statsA.points != statsB.points)
            return statsA.points > statsB.points;

        // Secondary sort: Wins
        if(statsA.wins != statsB.wins)
            return statsA.wins > statsB.wins;

        // Tertiary sort: Runs difference
        int diffA = statsA.runsFor - statsA.runsAgainst;
        int diffB = statsB.runsFor - statsB.runsAgainst;
        if(diffA != diffB)
            return diffA > diffB;

        // Final tiebreaker: Alphabetical
        return a < b;
    });

    // Configure table
    tw->setColumnCount(6);
    tw->setHorizontalHeaderLabels({"Team", "Played", "Wins", "Draws", "Losses", "Points"});
    tw->setRowCount(sorted.size());

    // Populate data
    for(int i = 0; i < sorted.size(); ++i) {
        const QString &name = sorted[i];
        const TeamStats &stats = statsMap[name];

        tw->setItem(i, 0, new QTableWidgetItem(name));
        tw->setItem(i, 1, new QTableWidgetItem(QString::number(stats.played)));
        tw->setItem(i, 2, new QTableWidgetItem(QString::number(stats.wins)));
        tw->setItem(i, 3, new QTableWidgetItem(QString::number(stats.draws)));
        tw->setItem(i, 4, new QTableWidgetItem(QString::number(stats.losses)));
        tw->setItem(i, 5, new QTableWidgetItem(QString::number(stats.points)));
    }
}
void TournamentModeScreen::updateWinnerLabel()
{
    QVector<QString> sorted = statsMap.keys().toVector();
    std::sort(sorted.begin(), sorted.end(), [&](const QString &a, const QString &b) {
        return statsMap[a].points > statsMap[b].points;
    });

    QString winnerText;
    if(sorted.size() > 0) {
        const TeamStats &winnerStats = statsMap[sorted.first()];

        // Check for ties
        QVector<QString> winners;
        for(const QString &team : sorted) {
            if(statsMap[team].points == winnerStats.points) {
                winners.append(team);
            }
        }

        if(winners.size() > 1) {
            winnerText = "Tournament Winners (Tie):\n";
            for(const QString &team : winners) {
                winnerText += QString("- %1\n").arg(team);
            }
        } else {
            winnerText = QString("Tournament Winner: %1\n"
                                 "Points: %2\n"
                                 "Wins: %3\n"
                                 "Run Difference: %4")
                             .arg(sorted.first())
                             .arg(winnerStats.points)
                             .arg(winnerStats.wins)
                             .arg(winnerStats.runsFor - winnerStats.runsAgainst);
        }
    } else {
        winnerText = "No teams participated!";
    }

    ui->labelWinner->setText(winnerText);
    ui->labelWinner->setVisible(true);
}
