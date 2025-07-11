#ifndef STATSSCREEN_H
#define STATSSCREEN_H
#include "demomodescreen.h"
#include <QMainWindow>
class MainWindow;

namespace Ui {
class StatsScreen;
}

class StatsScreen : public QMainWindow
{
    Q_OBJECT

public:
    explicit StatsScreen(MainWindow *mainWin, QWidget *parent = nullptr);
    ~StatsScreen();

private slots:
    void on_historyTable_cellClicked(int row, int column);

    void on_refreshButton_clicked();

    void on_dateFromEdit_dateChanged(const QDate &date);

    void on_dateToEdit_dateChanged(const QDate &date);

    void on_teamFilterCombo_currentIndexChanged(int index);

    void on_searchLineEdit_textChanged(const QString &arg1);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::StatsScreen *ui;
    MainWindow *mainWindow;
    DemoModeScreen *demomodescreen;
    QVector<QJsonObject> loadedMatches;
    QList<int> displayedMatchIndexes;
    void applyFilters();
};

#endif // STATSSCREEN_H
