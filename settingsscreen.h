#ifndef SETTINGSSCREEN_H
#define SETTINGSSCREEN_H

#include <QMainWindow>
class MainWindow;
namespace Ui {
class SettingsScreen;
}

class SettingsScreen : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingsScreen(MainWindow *mainWin, QWidget *parent = nullptr);
    ~SettingsScreen();

private:
    Ui::SettingsScreen *ui;
    MainWindow *mainWindow;
};

#endif // SETTINGSSCREEN_H
