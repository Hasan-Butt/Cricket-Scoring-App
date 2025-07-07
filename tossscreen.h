#ifndef TOSSSCREEN_H
#define TOSSSCREEN_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPropertyAnimation>
#include "gameplayscreen.h"
namespace Ui {
class TossScreen;
}

class TossScreen : public QMainWindow
{
    Q_OBJECT

public:
    explicit TossScreen(const QString &teamA, const QString &teamB, QStringList playersA, QStringList playersB, int overs, QWidget *parent = nullptr);
    ~TossScreen();

private slots:
    void on_ballFirst_clicked();

    void on_batFirst_clicked();
    void startCoinSpin();
    void updateCoinFrame();


    void on_confirmBtn_clicked();
signals:
    /// Emitted right after we do `new GamePlayScreen(...)`
    void gameplayCreated(GamePlayScreen *gp);
private:
    Ui::TossScreen *ui;
    QPixmap coinPixmap; // Full coin image
    int frameWidth;
    int frameHeight;
    int frameCount;
    int currentFrame;
    int totalSpins;
    QTimer *timer;
    GamePlayScreen* gameplayscreen;
    QString teamAName;
    QString teamBName;
    QStringList teamAplayers;
    QStringList teamBplayers;
    int oversNum;
    int win;
    bool bat=false;


};

#endif // TOSSSCREEN_H
