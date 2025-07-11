#include "statsscreen.h"
#include "ui_statsscreen.h"
#include "mainwindow.h"
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTableWidgetItem>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextTable>
#include <QTextTableFormat>
#include <QPdfWriter>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
StatsScreen::StatsScreen(MainWindow *mainWin, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::StatsScreen)
    , mainWindow(mainWin)
{
    ui->setupUi(this);
    ui->homeButton->setIcon(QIcon(":icons/homeicon.jpg"));
    connect(ui->homeButton, &QPushButton::clicked,
            mainWindow,  &MainWindow::returnToHome);

    // 1) Read every JSON into loadedMatches, track min/max dates
    QDir dir("matches");
    auto files = dir.entryList({ "*.json" }, QDir::Files);

    QDate minDate = QDate::currentDate();
    QDate maxDate = QDate(2000,1,1);

    for (const QString &fname : files) {
        QFile file(dir.filePath(fname));
        if (!file.open(QIODevice::ReadOnly)) continue;

        auto doc = QJsonDocument::fromJson(file.readAll());
        if (!doc.isObject()) continue;

        QJsonObject obj = doc.object();
        loadedMatches.append(obj);

        // update min/max
        QDate d = QDate::fromString(obj["Date"].toString(), "yyyy-MM-dd");
        if (d.isValid()) {
            minDate = qMin(minDate, d);
            maxDate = qMax(maxDate, d);
        }
    }

    // 2) Populate your date-edits and team-combo once
    ui->dateFromEdit->setDate(minDate);
    ui->dateToEdit->setDate(maxDate);

    // build a unique, sorted list of teams
    QStringList teams;
    for (auto &m : loadedMatches) {
        teams << m["TeamA Name"].toString()
        << m["TeamB"].toString();
    }
    teams.removeDuplicates();
    teams.sort();

    ui->teamFilterCombo->addItem("All Teams");
    for (auto &t : teams)
        ui->teamFilterCombo->addItem(t);

    //Fill the table using filters or directly
    applyFilters();
}

StatsScreen::~StatsScreen()
{
    delete ui;
}

void StatsScreen::on_historyTable_cellClicked(int row, int )
{
    int idx = displayedMatchIndexes.value(row, -1);
    QJsonObject obj = loadedMatches.at(idx);
    QJsonArray performances = obj["Performances"].toArray();
    ui->performancesTable->clearContents();
    ui->performancesTable->setRowCount(performances.size());

    ui->date->setText(obj["Date"].toString());
    ui->teamA->setText(obj["TeamA Name"].toString());
    ui->teamB->setText(obj["TeamB"].toString());
    ui->result->setText(obj["Winner"].toString());
    int r = 0;
    for (auto val : obj["Performances"].toArray()) {
        QJsonObject p = val.toObject();
        int bb = p["Balls Bowled"].toInt();
        QString overs = QString::number(bb/6) + "." + QString::number(bb%6);

        ui->performancesTable->setItem(r, 0, new QTableWidgetItem(QString::number(p["Jersey"].toInt())));
        ui->performancesTable->setItem(r, 1, new QTableWidgetItem(p["Name"].toString()));
        ui->performancesTable->setItem(r, 2, new QTableWidgetItem(QString::number(p["Runs"].toInt())));
        ui->performancesTable->setItem(r, 3, new QTableWidgetItem(QString::number(p["Balls"].toInt())));
        ui->performancesTable->setItem(r, 4, new QTableWidgetItem(QString::number(p["Fours"].toInt())));
        ui->performancesTable->setItem(r, 5, new QTableWidgetItem(QString::number(p["Sixes"].toInt())));
        ui->performancesTable->setItem(r, 6, new QTableWidgetItem(QString::number(p["Strike Rate"].toDouble(), 'f', 2)));
        ui->performancesTable->setItem(r, 7, new QTableWidgetItem(overs));
        ui->performancesTable->setItem(r, 8, new QTableWidgetItem(QString::number(p["Wickets"].toInt())));
        ui->performancesTable->setItem(r, 9, new QTableWidgetItem(
                                                 p["Economy"].isNull() ? "–" : QString::number(p["Economy"].toDouble(), 'f', 2)));
        r++;
}
}

void StatsScreen::on_refreshButton_clicked()
{
     ui->performancesTable->clearContents();
}
void StatsScreen::applyFilters()
{
    QDate from = ui->dateFromEdit->date();
    QDate to   = ui->dateToEdit->date();
    QString teamFilter = ui->teamFilterCombo->currentText();
    QString textFilter = ui->searchLineEdit->text().trimmed();

    //Rebuild the table from scratch
    ui->historyTable->setRowCount(0);
    displayedMatchIndexes.clear();             // reset our mapping

    for (int i = 0; i < loadedMatches.size(); ++i) {
        const QJsonObject &m = loadedMatches[i];
        QDate matchDate = QDate::fromString(m["Date"].toString(), "yyyy-MM-dd");

        QString a = m["TeamA Name"].toString();
        QString b = m["TeamB"].toString();
        QString matchLabel = a + " vs " + b;
        QString winnerLabel = m["Winner"].toString()
                              + " by "
                              + m["Margin"].toString();

        // Apply filters
        if (matchDate < from || matchDate > to)                   continue;
        if (teamFilter != "All Teams" &&
            teamFilter != a && teamFilter != b)                   continue;
        if (!textFilter.isEmpty() &&
            !matchLabel.contains(textFilter, Qt::CaseInsensitive) &&
            !winnerLabel.contains(textFilter, Qt::CaseInsensitive)) continue;

        //Insert the row AND record its original index
        int row = ui->historyTable->rowCount();
        ui->historyTable->insertRow(row);
        ui->historyTable->setItem(row, 0, new QTableWidgetItem(m["Date"].toString()));
        ui->historyTable->setItem(row, 1, new QTableWidgetItem(matchLabel));
        ui->historyTable->setItem(row, 2, new QTableWidgetItem(winnerLabel));

        displayedMatchIndexes.append(i);
    }
}


void StatsScreen::on_dateFromEdit_dateChanged(const QDate &)
{
    applyFilters();
}


void StatsScreen::on_dateToEdit_dateChanged(const QDate &)
{
     applyFilters();
}


void StatsScreen::on_teamFilterCombo_currentIndexChanged(int)
{
     applyFilters();
}


void StatsScreen::on_searchLineEdit_textChanged(const QString &)
{
     applyFilters();
}


void StatsScreen::on_pushButton_clicked()
{
    this->close();
    demomodescreen=new DemoModeScreen(mainWindow);
    demomodescreen->show();
}


void StatsScreen::on_pushButton_2_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save Match Stats as PDF"),
        QString(),
        tr("PDF Files (*.pdf)")
        );
    if (fileName.isEmpty()) return;
    if (!fileName.endsWith(".pdf", Qt::CaseInsensitive))
        fileName += ".pdf";

    // 2) Create HTML with BIGGER text
    QString html = R"(
      <!DOCTYPE html>
      <html><head><meta charset='utf-8'/>
      <style>
        body  { font-family: Arial, sans-serif; font-size: 16pt; margin:0; }
        h1    { font-size: 28pt; margin-bottom: 0.5em; }
        p     { font-size: 14pt; margin: 0.4em 0; }
        table { width:100%; border-collapse: collapse; font-size: 12pt; }
        th, td { border:1px solid #444; padding:8px; }
        th    { background-color:#ddd; }
      </style>
      </head><body>
    )";

    html += "<h1 align='center'>Match Statistics</h1>";
    html += "<p><b>Match Between:</b> " + ui->teamA->text()
            + " vs " + ui->teamB->text() + "</p>";
    html += "<p><b>Date:</b> "   + ui->date->text()
            + " &nbsp;&nbsp; <b>Result:</b> " + ui->result->text() + "</p>";
    html += "<table><tr>";
    for (int c = 0; c < ui->performancesTable->columnCount(); ++c) {
        html += "<th>" + ui->performancesTable
                             ->horizontalHeaderItem(c)
                             ->text() + "</th>";
    }
    html += "</tr>";
    for (int r = 0; r < ui->performancesTable->rowCount(); ++r) {
        html += "<tr>";
        for (int c = 0; c < ui->performancesTable->columnCount(); ++c) {
            QString txt = ui->performancesTable->item(r,c)
            ? ui->performancesTable->item(r,c)->text()
            : QString();
            html += "<td>" + txt + "</td>";
        }
        html += "</tr>";
    }
    html += "</table></body></html>";

    // 3) Set up PDF writer — A4 Landscape, small margins
    QPdfWriter writer(fileName);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setPageOrientation(QPageLayout::Landscape);
    writer.setPageMargins(QMarginsF(10, 10, 10, 10), QPageLayout::Millimeter);

    // 4) Render HTML
    QTextDocument doc;
    doc.setDefaultFont(QFont("Arial", 16));
    doc.setHtml(html);
    doc.setTextWidth(writer.pageLayout().paintRectPoints().width());

    QPainter painter(&writer);
    doc.drawContents(&painter);
    painter.end();

    // 5) Notify user
    QMessageBox::information(
        this, tr("PDF Exported"),
        tr("Match stats PDF saved to:\n%1").arg(fileName)
        );
}

