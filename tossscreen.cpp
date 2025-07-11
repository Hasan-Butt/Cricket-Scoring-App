#include "tossscreen.h"
#include "ui_tossscreen.h"
#include "gameplayscreen.h"
#include "mainwindow.h"
#include <QRandomGenerator>
#include <QMessageBox>
#include <QPainter>
#include <QTimer>
#include <QPixmap>
#include <QPushButton>
#include <QTime>

TossScreen::TossScreen(const QString &teamA, const QString &teamB, QStringList playersA, QStringList PlayersB,int overs, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TossScreen)
{
    ui->setupUi(this);
    teamAName=teamA;
    teamBName=teamB;
    teamAplayers=playersA;
    teamBplayers=PlayersB;
    oversNum=overs;
    ui->labelTeamA->setText(teamAName);
    ui->labelTeamB->setText(teamBName);
    ui->batFirst->setIcon(QIcon(":icons/baticon.png"));
    ui->ballFirst->setIcon(QIcon(":icons/ballicon.png"));

    coinPixmap.load(":/images/coin.jpg"); // or "coinimage.png" if in folder


    frameWidth = coinPixmap.width() / 4.9;
    frameHeight = coinPixmap.height() / 2;
    frameCount = 4;
    currentFrame = 0;
    totalSpins = 0;

    QPixmap firstFrame = coinPixmap.copy(0, 0, frameWidth, frameHeight);

    QIcon icon(firstFrame);
    ui->btnToss->setIcon(icon);
    ui->btnToss->setIconSize(firstFrame.size());

    // Timer setup
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TossScreen::updateCoinFrame);
    connect(ui->btnToss, &QPushButton::clicked, this, &TossScreen::startCoinSpin);

}

TossScreen::~TossScreen()
{
    delete ui;
}

void TossScreen::on_ballFirst_clicked()
{
    bat = false;
    ui->ballFirst->setStyleSheet(R"(
    QRadioButton{
    font-size:15px;
    color:white;
    background-color:blue;
    border-radius: 10px;
    padding: 5px;
    }

    QRadioButton:hover {

        border: 2px solid #00aaff;
        color:white;
    }

)");
    ui->batFirst->setStyleSheet(R"(
    QRadioButton {
        font-size: 15px;
        color: black;
        background-color: #e0f7ff;
        border-radius: 10px;
        padding: 5px;
    }

    QRadioButton:hover {
        background-color: blue;
        border: 2px solid #00aaff;
        color: white;
    }
)");
    ui->confirmBtn->setEnabled(true);
}


void TossScreen::on_batFirst_clicked()
{
    ui->batFirst->setStyleSheet(R"(
    QRadioButton {
        font-size: 15px;
        color: white;
        background-color: blue;
        border-radius: 10px;
        padding: 5px;
    }

    QRadioButton:hover {

        border: 2px solid #00aaff;
        color: white;
    }
)");
    ui->ballFirst->setStyleSheet(R"(
    QRadioButton{
    font-size:15px;
    color:black;
    background-color:#e0f7ff;
    border-radius: 10px;
    padding: 5px;
    }

    QRadioButton:hover {
        background-color:blue;
        border: 2px solid #00aaff;
        color:white;
    }

)");
    bat=true;
    ui->confirmBtn->setEnabled(true);
}

void TossScreen::startCoinSpin()
{
    totalSpins = 0;
    currentFrame = 0;
    timer->start(100);
}
void TossScreen::updateCoinFrame()
{

    currentFrame = (currentFrame + 1) % frameCount;

    QPixmap frame = coinPixmap.copy(currentFrame * frameWidth, 0, frameWidth, frameHeight);

    QIcon icon(frame);
    ui->btnToss->setIcon(icon);

    totalSpins++;


    if (totalSpins > 30) {
        timer->stop();
        currentFrame = 0;

        QString winner;
        if (QRandomGenerator::global()->bounded(2) == 0){

            win=0;
            winner = teamAName;
        }
        else{

            win=1;
            winner = teamBName;
        }

        if(teamAName=="Babar Azam" || teamBName=="Babar Azam"){
            if(teamAName=="Babar Azam"){
                win=0;
                winner = teamAName;
            }
            else if(teamBName=="Babar Azam"){
                win=1;
                winner = teamBName;
            }
        }
        QPixmap frame = coinPixmap.copy(currentFrame * frameWidth, 0, frameWidth, frameHeight);

        QPainter painter(&frame);
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 12, QFont::Bold));
        painter.drawText(frame.rect(), Qt::AlignCenter, "Toss Winner\n"+winner);
        painter.end();

        QIcon icon(frame);
        ui->btnToss->setIcon(icon);
        ui->ballFirst->setEnabled(true);
        ui->batFirst->setEnabled(true);
    }
}

void TossScreen::on_confirmBtn_clicked()
{
    this->close();
    if(bat && win==0){
        gameplayscreen=new GamePlayScreen(teamAName, teamBName,teamAplayers, teamBplayers,oversNum, nullptr);
    }
    else if(!bat && win==0){
        gameplayscreen=new GamePlayScreen(teamBName, teamAName,teamBplayers, teamAplayers,oversNum, nullptr);
    }
    else{
        if(bat && win==1){
            gameplayscreen=new GamePlayScreen(teamBName, teamAName,teamBplayers, teamAplayers,oversNum, nullptr);
        }
        else if(!bat && win==1){
            gameplayscreen=new GamePlayScreen(teamAName, teamBName,teamAplayers, teamBplayers,oversNum, nullptr);
        }
    }
    emit gameplayCreated(gameplayscreen);
    gameplayscreen->show();
}
