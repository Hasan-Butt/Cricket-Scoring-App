#include "auctionmodescreen.h"
#include "ui_auctionmodescreen.h"
#include "mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QCoreApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QTabWidget>
#include <QHeaderView>
#include <QDir>
#include <QDateTime>
#include <QTableWidgetItem>
AuctionModeScreen::AuctionModeScreen(MainWindow *mainWin, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AuctionModeScreen)
    , mainWindow(mainWin)
    , currentPlayerIndex(0)
    , turnIndex(0)
    , turnDurationSec(30) // 30 seconds per turn
    , remainingSeconds(0)
{
    ui->setupUi(this);
    ui->homeButton->setIcon(QIcon(":icons/homeicon.jpg"));
    connect(ui->homeButton, &QPushButton::clicked,mainWindow,  &MainWindow::returnToHome);

    turnTimer = new QTimer(this);
    turnTimer->setSingleShot(true);
    connect(turnTimer, &QTimer::timeout, this, &AuctionModeScreen::on_turnTimeout);

    progressTimer = new QTimer(this);
    progressTimer->setInterval(1000); // 1 second intervals
    connect(progressTimer, &QTimer::timeout, this, &AuctionModeScreen::on_progressTimeout);

    ui->timerProgressBar->setRange(0, turnDurationSec);
    ui->timerProgressBar->setValue(0);
    ui->historyTableWidget->setColumnCount(4);
    ui->historyTableWidget->setHorizontalHeaderLabels(
        { tr("Time"), tr("Team"), tr("Action"), tr("Bid Amount") });
    ui->historyTableWidget->horizontalHeader()->setStretchLastSection(true);
}

AuctionModeScreen::~AuctionModeScreen()
{
    delete ui;
}

void AuctionModeScreen::on_pushButton_clicked()
{
    if (ui->teamList->count() < 2) {
        QMessageBox::warning(this, "Not enough teams", "Please add at least two teams before starting the auction.");
        return;
    }

    teamNames.clear();
    teamBudgets.clear();


    QDir().mkpath("Auction Results");
    for(int i=0; i<ui->teamList->count(); i++){
        QListWidgetItem *item=ui->teamList->item(i);
        teamNames.append(item->text());
        teamBudgets[item->text()] = ui->spinBox->value();
        QString filePath = QString("Auction Results/%1.txt").arg(item->text());
        QFile f(filePath);
        if (f.open(QIODevice::WriteOnly | QIODevice::Text)) f.close();
    }
    teamRemainingTime.clear();
    for (const QString &team : teamNames) {
        teamRemainingTime[team] = turnDurationSec;
    }
    currentPlayerIndex = 0;
    nextPlayer();
    ui->stackedWidget->setCurrentIndex(1);

}


void AuctionModeScreen::on_pushButton_9_clicked()
{
    showAuctionSummary();
}


void AuctionModeScreen::on_pushButton_11_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void AuctionModeScreen::on_pushButton_2_clicked()
{
    QFile file("auctionPlayers.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Load Error"),
                             tr("Cannot open %1").arg("auctionPlayers.csv"));
        return;
    }

    QTextStream in(&file);
    players.clear();
    ui->playerTable->clearContents();
    ui->playerTable->setRowCount(0);

    int row = 0;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        QStringList cols = line.split(',');
        if (cols.size() != 4) {
            qWarning() << "Skipping malformed line:" << line;
            continue;
        }
        AuctionPlayer p{cols[0].trimmed(), cols[1].trimmed(), cols[2].trimmed().toInt(), cols[3].trimmed()};
        players.append(p);
        QString name      = cols[0].trimmed();
        QString role      = cols[1].trimmed();
        int     basePrice = cols[2].trimmed().toInt();
        QString country   = cols[3].trimmed();

        // grow the table
        ui->playerTable->insertRow(row);
        ui->playerTable->setItem(row, 0, new QTableWidgetItem(name));
        ui->playerTable->setItem(row, 1, new QTableWidgetItem(role));
        ui->playerTable->setItem(row, 2, new QTableWidgetItem(QString::number(basePrice)));
        ui->playerTable->setItem(row, 3, new QTableWidgetItem(country));
        ++row;
    }

    file.close();
}


void AuctionModeScreen::on_addTeam_clicked()
{
    QString teamName = ui->teamText->text();
    if (!teamName.isEmpty()) {

        QListWidgetItem *newItem = new QListWidgetItem(teamName);

        ui->teamList->addItem(newItem);


        ui->teamText->clear();
    }
}


void AuctionModeScreen::on_removeTeam_clicked()
{
    QListWidgetItem *selectedItem = ui->teamList->currentItem();
    if (selectedItem) {
        delete selectedItem;
    }
}


void AuctionModeScreen::on_bid100KBtn_clicked()
{
    int inc = 100000;
    if (teamBudgets.value(currentTeam, 0) >= currentBid + inc) {
        currentBid += inc;
        currentLeader = currentTeam;
        ui->currentBidLabel->setText(QString::number(currentBid));
        ui->leadingTeamLabel->setText(currentLeader);
        addHistoryEntry(currentTeam, tr("Bid +100K"), currentBid);
    } else {
        QMessageBox::information(this, tr("Insufficient Budget"),
                                 tr("%1 cannot bid %2: budget %3").arg(currentTeam)
                                     .arg(currentBid + inc)
                                     .arg(teamBudgets.value(currentTeam)));
    }
    teamRemainingTime[currentTeam] = remainingSeconds;
    turnTimer->stop();
    progressTimer->stop();

    nextTurn();
}


void AuctionModeScreen::on_bid500KBtn_clicked()
{
    int inc = 500000;
    if (teamBudgets.value(currentTeam, 0) >= currentBid + inc) {
        currentBid += inc;
        currentLeader = currentTeam;
        ui->currentBidLabel->setText(QString::number(currentBid));
        ui->leadingTeamLabel->setText(currentLeader);
        addHistoryEntry(currentTeam, tr("Bid +500K"), currentBid);
    } else {
        QMessageBox::information(this, tr("Insufficient Budget"),
                                 tr("%1 cannot bid %2: budget %3").arg(currentTeam)
                                     .arg(currentBid + inc)
                                     .arg(teamBudgets.value(currentTeam)));
    }
    teamRemainingTime[currentTeam] = remainingSeconds;
    turnTimer->stop();
    progressTimer->stop();
    nextTurn();
}


void AuctionModeScreen::on_bid1MBtn_clicked()
{
    int inc = 1000000;
    if (teamBudgets.value(currentTeam, 0) >= currentBid + inc) {
        currentBid += inc;
        currentLeader = currentTeam;
        ui->currentBidLabel->setText(QString::number(currentBid));
        ui->leadingTeamLabel->setText(currentLeader);
        addHistoryEntry(currentTeam, tr("Bid +1M"), currentBid);
    } else {
        QMessageBox::information(this, tr("Insufficient Budget"),
                                 tr("%1 cannot bid %2: budget %3").arg(currentTeam)
                                     .arg(currentBid + inc)
                                     .arg(teamBudgets.value(currentTeam)));
    }
    teamRemainingTime[currentTeam] = remainingSeconds;
    turnTimer->stop();
    progressTimer->stop();
    nextTurn();
}


void AuctionModeScreen::on_passBtn_clicked()
{
    addHistoryEntry(currentTeam, tr("Pass"), 0);
    teamRemainingTime[currentTeam] = remainingSeconds;
    turnTimer->stop();
    progressTimer->stop();
    nextTurn();
}


void AuctionModeScreen::on_forceAssignBtn_clicked()
{
    const AuctionPlayer &p = players.at(currentPlayerIndex);
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Force Assign"),
                                                              tr("Force assign player %1 based on current highest bid? This will end bidding immediately.").arg(p.name),
                                                              QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        turnTimer->stop();
        progressTimer->stop();
        assignCurrentPlayer();
    }
}


void AuctionModeScreen::on_skipPlayerBtn_clicked()
{
    addHistoryEntry(currentTeam, tr("Skip"), 0);
    teamRemainingTime[currentTeam] = remainingSeconds;
    skipSet.insert(currentTeam);
    turnTimer->stop();
    progressTimer->stop();
    nextTurn();
}
void AuctionModeScreen::nextPlayer() {
    turnTimer->stop();
    progressTimer->stop();
    if (currentPlayerIndex >= players.size()) {
        showAuctionSummary();
        return;
    }
    const AuctionPlayer &p = players.at(currentPlayerIndex);
    currentBid = p.basePrice;
    currentLeader.clear();
    activeTeams = teamNames;
    skipSet.clear();
    turnIndex = 0;

    // Reset remaining times for each team
    for (const QString &team : teamNames) {
        teamRemainingTime[team] = turnDurationSec;
    }
    ui->nameLabel->setText(p.name);
    ui->roleLabel->setText(p.role);
    ui->basePriceLabel->setText(QString::number(p.basePrice));
    ui->countryLabel->setText(p.country);
    ui->currentBidLabel->setText(QString::number(currentBid));
    ui->leadingTeamLabel->clear();
    nextTurn();
}

void AuctionModeScreen::nextTurn() {
    turnTimer->stop();
    progressTimer->stop();

    // Remove teams with zero remaining time from activeTeams
    QStringList updated;
    for (const QString &team : activeTeams) {
        if (teamRemainingTime.value(team, 0) > 0 && !skipSet.contains(team)) {
            updated.append(team);
        }
    }
    activeTeams = updated;
    if (activeTeams.isEmpty() || skipSet.size() == activeTeams.size()) {
        assignCurrentPlayer();
        return;
    }
    int count = activeTeams.size();
    for (int i = 0; i < count; ++i) {
        int idx = (turnIndex + i) % count;
        QString team = activeTeams.at(idx);
        if (!skipSet.contains(team)) {
            currentTeam = team;
            turnIndex = (idx + 1) % count;
            break;
        }
    }
    remainingSeconds = teamRemainingTime.value(currentTeam, turnDurationSec);
    ui->timerProgressBar->setRange(0, turnDurationSec);
    ui->timerProgressBar->setValue(remainingSeconds);
    turnTimer->start(turnDurationSec * 1000);
    progressTimer->start();
    ui->statusLabel->setText(tr("%1's turn (%2s)").arg(currentTeam).arg(remainingSeconds));
}

// Called every second to update progress bar and status label
void AuctionModeScreen::on_progressTimeout() {
    remainingSeconds--;
    if (remainingSeconds <= 0) {
        progressTimer->stop();
        return;
    }
    ui->timerProgressBar->setValue(remainingSeconds);
    ui->statusLabel->setText(tr("%1's turn (%2s)").arg(currentTeam).arg(remainingSeconds));
    // Save remaining time
    teamRemainingTime[currentTeam] = remainingSeconds;
}

// Called when a team's turn timer expires (no action in allotted time)
void AuctionModeScreen::on_turnTimeout() {
    addHistoryEntry(currentTeam, tr("Timeout"), 0);
    // Time finished for currentTeam
    QMessageBox::information(this, tr("Time Up"),
                             tr("Time finished for %1; skipping further bids for this player.").arg(currentTeam));
    teamRemainingTime[currentTeam] = 0;
    skipSet.insert(currentTeam);
    nextTurn(); // treat as pass
}

// Assign current player to currentLeader (if any) and record purchase
void AuctionModeScreen::assignCurrentPlayer() {
    const AuctionPlayer &p = players.at(currentPlayerIndex);
    // Create results directory if missing, relative to working directory
    QDir().mkpath("Auction Results");
    if (!currentLeader.isEmpty()) {
        addHistoryEntry(currentLeader, tr("Sold %1").arg(p.name), currentBid);
        teamBudgets[currentLeader] -= currentBid;
        QString filePath = QString("Auction Results/%1.txt").arg(currentLeader);
        QFile f(filePath);
        if (f.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&f);
            out << p.name << "," << currentBid << "\n";
            f.close();
        }
        // Inform user of sale
        QMessageBox::information(this, tr("Player Sold"),
                                 tr("%1 sold to %2 for %3").arg(p.name).arg(currentLeader).arg(currentBid));
    } else {
        addHistoryEntry(QString(), tr("Unsold %1").arg(p.name), 0);
        QMessageBox::information(this, tr("No Bids"),
                                 tr("No bids received for %1; player remains unsold.").arg(p.name));
    }
    currentPlayerIndex++;
    nextPlayer();
}
void AuctionModeScreen::updateLiveAuctionUI() {
    // Purpose: Refresh any live auction widgets not directly handled in nextPlayer/nextTurn,
    // such as team budget displays, squad lists, or bid history table.
    // For example:
    // - Iterate over teamNames and update a QLabel showing remaining budget for each
    // - Update a QListWidget or QTableWidget showing acquired players so far
    // - Update bid history display if maintained
    // Implementation depends on your UI elements; if all updates happen in nextPlayer/nextTurn,
    // this can remain empty or be used for future enhancements.
}

// Show summary page
void AuctionModeScreen::showAuctionSummary()
{
    // Stop any ongoing timers so auction halts
    if (turnTimer->isActive()) turnTimer->stop();
    if (progressTimer->isActive()) progressTimer->stop();

    // Optionally disable bidding buttons to prevent further actions
    ui->bid100KBtn->setEnabled(false);
    ui->bid500KBtn->setEnabled(false);
    ui->bid1MBtn->setEnabled(false);
    ui->passBtn->setEnabled(false);
    ui->skipPlayerBtn->setEnabled(false);
    ui->forceAssignBtn->setEnabled(false);

    // Clear status/progress display
    ui->statusLabel->clear();
    ui->timerProgressBar->setValue(0);

    ui->stackedWidget->setCurrentIndex(2);
    QTabWidget *tabWidget = ui->summaryTabWidget;
    tabWidget->clear();

    // Use the same relative folder "Auction Results" (working directory)
    QString baseDir = QDir::current().filePath("Auction Results");
    if (!QDir(baseDir).exists()) {
        qWarning() << "Auction Results folder not found at" << baseDir;
        QMessageBox::warning(this, tr("Summary Error"),
                             tr("Results folder not found:\n%1").arg(baseDir));
        return;
    }
    QString inlineQss = R"(
QTabWidget::pane {
    border: 1px solid #A0A8B0;
    background: #F0F4F8;
    border-top-left-radius: 6px;
    font-size:20px;
    border-top-right-radius: 6px;
    top: -1px;
}

/* Remove default base under the tabs */
QTabBar {
    qproperty-drawBase: 0;
}

/* Unselected tabs */
QTabBar::tab {
    background: #FFFFFF;
    color: #555F6A;
    padding: 8px 20px;
    margin-right: 2px;
    border: 1px solid #A0A8B0;
    border-bottom: none;
    border-top-left-radius: 6px;
    border-top-right-radius: 6px;
    min-width: 100px;
}

/* Hover state */
QTabBar::tab:hover {
    background: #E4E9EF;
}

/* Selected tab */
QTabBar::tab:selected {
    background: #FFFFFF;
    color: #0078D7;
     font-size:20px;
    border-color: #0078D7;
    border-bottom: 1px solid #F0F4F8; /* blend into pane */
    font-weight: bold;
}

/* Push down unselected tabs slightly */
QTabBar::tab:!selected {
    margin-top: 4px;
}


QListWidget {
    background: #ffffff;
    border: 1px solid #ccc;
    padding: 4px;
}
QListWidget::item {
    padding: 4px 8px;
    margin: 2px 0;
}
QListWidget::item:hover {
    background: #f5f5f5;
}
QListWidget::item:selected {
    background: #4285f4;
    color: white;
}
)";
    ui->summaryTabWidget->setStyleSheet(inlineQss);
    for (const QString &team : teamNames) {
        QWidget *page = new QWidget;
        QVBoxLayout *vLayout = new QVBoxLayout(page);
        QListWidget *list = new QListWidget(page);

        QString filePath = QDir(baseDir).filePath(team + ".txt");
        QFile f(filePath);
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&f);
            while (!in.atEnd()) {
                QString line = in.readLine().trimmed();
                if (line.isEmpty()) continue;
                QStringList parts = line.split(',');
                if (parts.size() >= 2) {
                    QString playerName = parts[0].trimmed();
                    QString bidStr = parts[1].trimmed();
                    QString itemText = QString("%1 – %2").arg(playerName, bidStr);
                    list->addItem(itemText);
                } else {
                    list->addItem(line);
                }
            }
            f.close();
        } else {
            list->addItem(tr("No purchases recorded"));
        }

        vLayout->addWidget(list);
        page->setLayout(vLayout);
        tabWidget->addTab(page, team);
    }
}

void AuctionModeScreen::addHistoryEntry(const QString &team,
                                        const QString &action,
                                        int bidAmount)
{
    int row = ui->historyTableWidget->rowCount();
    ui->historyTableWidget->insertRow(row);
    // Time column
    QString ts = QDateTime::currentDateTime().toString("HH:mm:ss");
    ui->historyTableWidget->setItem(row, 0, new QTableWidgetItem(ts));
    // Team column
    ui->historyTableWidget->setItem(row, 1, new QTableWidgetItem(team));
    // Action column
    ui->historyTableWidget->setItem(row, 2, new QTableWidgetItem(action));
    // Bid Amount column (empty or “-” if not applicable)
    QString bidText = (bidAmount > 0 ? QString::number(bidAmount) : QString());
    ui->historyTableWidget->setItem(row, 3, new QTableWidgetItem(bidText));
}

void AuctionModeScreen::on_pushButton_10_clicked()
{
    // Get current tab index and name
    int idx = ui->summaryTabWidget->currentIndex();
    if (idx < 0) {
        QMessageBox::warning(this, tr("Export Error"), tr("No tab selected."));
        return;
    }
    QString teamName = ui->summaryTabWidget->tabText(idx);

    // Locate the QListWidget inside the current tab page
    QWidget *page = ui->summaryTabWidget->widget(idx);
    if (!page) {
        QMessageBox::warning(this, tr("Export Error"), tr("Cannot find current tab page."));
        return;
    }
    // Find the first QListWidget in this page
    QListWidget *list = page->findChild<QListWidget *>();
    if (!list) {
        QMessageBox::warning(this, tr("Export Error"), tr("No list found in current tab."));
        return;
    }
    if (list->count() == 0) {
        QMessageBox::information(this, tr("No Data"), tr("No purchases to export for %1.").arg(teamName));
        return;
    }

    // Ask user for save location
    QString defaultName = teamName + ".csv";
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Save Auction Summary for %1").arg(teamName),
        QDir::current().filePath(defaultName),
        tr("CSV Files (*.csv);;All Files (*)")
        );
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Export Error"),
                             tr("Cannot open file for writing:\n%1").arg(filePath));
        return;
    }
    QTextStream out(&file);

    // Write header
    out << "Team,Player,Bid\n";

    // Iterate list items
    // Expect item text format: "PlayerName – bid" or similar
    for (int row = 0; row < list->count(); ++row) {
        QString text = list->item(row)->text().trimmed();
        if (text.isEmpty()) continue;
        // Split by " – " (en dash) or "-" if needed
        QString playerName;
        QString bidStr;
        if (text.contains("–")) {
            // some formats use en dash
            QStringList parts = text.split("–");
            playerName = parts.value(0).trimmed();
            bidStr = parts.value(1).trimmed();
        } else if (text.contains("-")) {
            QStringList parts = text.split("-");
            playerName = parts.value(0).trimmed();
            bidStr = parts.value(1).trimmed();
        } else {
            // If no delimiter, put entire text as player, leave bid blank
            playerName = text;
            bidStr.clear();
        }
        // Write CSV row, quoting fields if needed
        // Simple approach: escape quotes by doubling
        auto quote = [&](const QString &s) {
            QString t = s;
            t.replace("\"", "\"\"");
            return "\"" + t + "\"";
        };
        out << quote(teamName) << "," << quote(playerName) << "," << quote(bidStr) << "\n";
    }
    file.close();
    QMessageBox::information(this, tr("Export Successful"),
                             tr("Exported auction summary for %1 to:\n%2").arg(teamName, filePath));
}

