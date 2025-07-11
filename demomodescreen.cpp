#include "demomodescreen.h"
#include "ui_demomodescreen.h"
#include "mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QTableWidgetItem>
#include <algorithm>
DemoModeScreen::DemoModeScreen(MainWindow *mainWin, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DemoModeScreen)
    ,mainWindow(mainWin)
{
    ui->setupUi(this);
    ui->homeButton->setIcon(QIcon(":icons/homeicon.jpg"));
    connect(ui->homeButton, &QPushButton::clicked,mainWindow,  &MainWindow::returnToHome);
    connect(ui->homeButton, &QPushButton::clicked,this,&DemoModeScreen::close);
    QFile pf("players.txt");
    if (pf.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&pf);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.isEmpty()) continue;
            auto parts = line.split(',');
            int jersey = parts[0].toInt();
            QString name = parts[1], role = parts[2];
            playersList.append({ jersey, name, role });
        }
        pf.close();
    }

    ui->playersTable->setRowCount(playersList.size());

    for (int i = 0; i < playersList.size(); ++i) {
        const auto &p = playersList[i];
        ui->playersTable->setItem(i, 0, new QTableWidgetItem(QString::number(p.jersey)));
        ui->playersTable->setItem(i, 1, new QTableWidgetItem(p.name));
        ui->playersTable->setItem(i, 2, new QTableWidgetItem(p.role));
    }

    for (auto &p : playersList) {
        QFile sf(QString("%1.txt").arg(p.jersey));
        if (!sf.open(QIODevice::ReadOnly | QIODevice::Text)) continue;
        QTextStream in(&sf);
        QMap<QString,QString> stat;
        while (!in.atEnd()) {
            auto kv = in.readLine().split(':');
            stat[kv[0].trimmed()] = kv[1].trimmed();
        }
        statsByJersey[p.jersey] = stat;
        sf.close();
    }

    // 2) Build sorted lists
    //   a) For batsmen: sort by Runs desc
    auto batList = playersList;
    std::sort(batList.begin(), batList.end(), [&](auto &a, auto &b){
        return statsByJersey[a.jersey]["Runs"].toInt() >
               statsByJersey[b.jersey]["Runs"].toInt();
    });

    //   b) For bowlers: sort by Wickets desc
    auto bowlList = playersList;
    std::sort(bowlList.begin(), bowlList.end(), [&](auto &a, auto &b){
        return statsByJersey[a.jersey]["Wickets"].toInt() >
               statsByJersey[b.jersey]["Wickets"].toInt();
    });

    // 3) Populate batsmenTable (top 5)
    ui->batsmenTable->setColumnCount(4);
    ui->batsmenTable->setHorizontalHeaderLabels({ "Jersey", "Name", "Runs", "Ave" });
    ui->batsmenTable->setRowCount(5);
    for (int i = 0; i < 5 && i < batList.size(); ++i) {
        auto &p = batList[i];
        auto &s = statsByJersey[p.jersey];
        ui->batsmenTable->setItem(i, 0, new QTableWidgetItem(QString::number(p.jersey)));
        ui->batsmenTable->setItem(i, 1, new QTableWidgetItem(p.name));
        ui->batsmenTable->setItem(i, 2, new QTableWidgetItem(s["Runs"]));
        ui->batsmenTable->setItem(i, 3, new QTableWidgetItem(s.value("StrikeRate", "–")));
    }
    // 4) Populate bowlersTable (top 5)
    ui->bowlersTable->setColumnCount(4);
    ui->bowlersTable->setHorizontalHeaderLabels({ "Jersey", "Name", "Wickets", "Eco" });
    ui->bowlersTable->setRowCount(5);
    for (int i = 0; i < 5 && i < bowlList.size(); ++i) {
        auto &p = bowlList[i];
        auto &s = statsByJersey[p.jersey];
        ui->bowlersTable->setItem(i, 0, new QTableWidgetItem(QString::number(p.jersey)));
        ui->bowlersTable->setItem(i, 1, new QTableWidgetItem(p.name));
        ui->bowlersTable->setItem(i, 2, new QTableWidgetItem(s["Wickets"]));
        ui->bowlersTable->setItem(i, 3, new QTableWidgetItem(s["Economy"]));
    }
}

DemoModeScreen::~DemoModeScreen()
{
    delete ui;
}

void DemoModeScreen::on_playersTable_cellClicked(int row, int)
{
    int jersey = ui->playersTable->item(row,0)->text().toInt();
    const auto &s = statsByJersey[jersey];

    ui->playerStatsTable->clearContents();
    ui->playerStatsTable->setRowCount(s.size());
    ui->playerStatsTable->setColumnCount(2);
    ui->playerStatsTable->setHorizontalHeaderLabels({ "Stat", "Value" });

    int r = 0;
    for (auto it = s.constBegin(); it != s.constEnd(); ++it, ++r) {
        ui->playerStatsTable->setItem(r, 0, new QTableWidgetItem(it.key()));
        ui->playerStatsTable->setItem(r, 1, new QTableWidgetItem(it.value()));
    }
}


void DemoModeScreen::on_jerseyFilter_valueChanged(int jersey)
{
    for (int r = 0; r < playersList.size(); ++r) {
        if (playersList[r].jersey == jersey) {
            ui->playersTable->selectRow(r);
            ui->playersTable->scrollToItem(ui->playersTable->item(r,0));
            on_playersTable_cellClicked(r, 0);
            break;
        }
    }
}

void DemoModeScreen::on_pushButton_clicked()
{
    ui->playerStatsTable->clear();
}

