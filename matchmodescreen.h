#ifndef MATCHMODESCREEN_H
#define MATCHMODESCREEN_H
#include <QMainWindow>
#include "tossscreen.h"
class MainWindow;
namespace Ui {
class matchmodescreen;
}

class matchmodescreen : public QMainWindow
{
    Q_OBJECT

public:

    explicit matchmodescreen(MainWindow *mainWin, QWidget *parent = nullptr);
    ~matchmodescreen();
    bool savePlayersToFile();
    void createPlayerStatsFile(int jerseyNumber);
private slots:
    void on_spinBoxPlayerCount_valueChanged(int playerCount);
    void on_btnProceedToToss_clicked();
    void on_spinBox_Over_Value_valueChanged(int value);
signals:
    /// Will carry the final scores up to TournamentModeScreen
    void matchFinished(int scoreA, int scoreB);
private:
    Ui::matchmodescreen *ui;
    MainWindow *mainWindow;
    TossScreen *tossScreen;
    bool isTextEmpty(const QString &text);
    QString teamAName;
    QString teamBName;
    QStringList playerNamesA;
    QStringList playerNamesB;
    //bool isNumeric(const QString &str);

};

#endif // MATCHMODESCREEN_H
