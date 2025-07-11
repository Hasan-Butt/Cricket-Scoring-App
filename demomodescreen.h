#ifndef DEMOMODESCREEN_H
#define DEMOMODESCREEN_H

#include <QMainWindow>
#include <QList>
#include <QMap>
#include <QString>
struct DemoPlayer {
    int    jersey;
    QString name;
    QString role;
};
class MainWindow;

namespace Ui {
class DemoModeScreen;
}

class DemoModeScreen : public QMainWindow
{
    Q_OBJECT

public:
    explicit DemoModeScreen(MainWindow *mainWin, QWidget *parent = nullptr);
    ~DemoModeScreen();

private slots:
    void on_playersTable_cellClicked(int row, int column);

    void on_jerseyFilter_valueChanged(int jersey);

    void on_pushButton_clicked();

private:
    Ui::DemoModeScreen *ui;
    MainWindow *mainWindow;
    QList<DemoPlayer>                       playersList;
    QMap<int, QMap<QString,QString>>   statsByJersey;
};

#endif // DEMOMODESCREEN_H
