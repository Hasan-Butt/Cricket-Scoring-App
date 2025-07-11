#ifndef MATCHSUMMARYSCREEN_H
#define MATCHSUMMARYSCREEN_H

#include <QWidget>
#include "player.h"
namespace Ui {
class MatchSummaryScreen;
}

class MatchSummaryScreen : public QWidget
{
    Q_OBJECT

public:
    explicit MatchSummaryScreen(const QString teamAname,const QString teamBname,Player* teamA,Player* teamB,int count, QWidget *parent = nullptr);
    ~MatchSummaryScreen();

private slots:


    void on_btnBackToHome_clicked();

private:
    Ui::MatchSummaryScreen *ui;
    Player *firstTeam, *secondTeam;
    int Count;
    QString nameA,nameB;
    void populateTables();
};

#endif // MATCHSUMMARYSCREEN_H
