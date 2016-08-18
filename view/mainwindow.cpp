#include <QDir>
#include <QFile>
#include <QDebug>
#include <QSettings>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

#include "CONST.h"
#include "service/StrategyRegExp.h"
#include "service/ParseComponentFile.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

//-----------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {

    ui->setupUi(this);

    connect(ui->btnParse, SIGNAL(clicked()), this, SLOT(parseFiles()));
    connect(ui->btnOutputFile, SIGNAL(clicked()), this, SLOT(openDirectory()));
    connect(ui->btnStrategyFile, SIGNAL(clicked()), this, SLOT(openDirectory()));
    connect(ui->btnDestinationFile, SIGNAL(clicked()), this, SLOT(openDirectory()));

    ui->textLog->setVisible(false);

    readSettings();
    setFixedHeight(140);
}

//-----------------------------------------------------------------------------------------
void MainWindow::openDirectory() {
    QString btnSelected = qobject_cast<QPushButton*>(sender())->objectName();
    if(btnSelected == btnObjOutputFile) {
        QString saveFileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                    "/outSortedComponentList.txt",
                                    tr("Component Lists (*.txt)"));
        ui->edPathToOutputFile->setText(saveFileName);
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "/",
                                                    tr("Component Lists (*.bom *.txt)"));

    if(btnSelected == btnObjStrategyFile) {
        ui->edPathToStrategyFile->setText(fileName);
    }
    else if(btnSelected == btnObjDestinationFile) {
        ui->edPathToDestinationFile->setText(fileName);
    }
}

//-----------------------------------------------------------------------------------------
MainWindow::~MainWindow() {
    writeSettings(ui->edPathToStrategyFile->text(),
                  ui->edPathToDestinationFile->text(),
                  ui->edPathToOutputFile->text());
    delete ui;
}

//-----------------------------------------------------------------------------------------
void MainWindow::writeSettings(const QString &strategyFile,
                               const QString &destinationFile,
                               const QString &outputFile) {

    QSettings *settings = new QSettings(layerSoftGroup, applicationGroup);
    settings->beginGroup(SettingsGroup);
    settings->setValue(strategyFileSetting, strategyFile);
    settings->setValue(destinationFileSetting, destinationFile);
    settings->setValue(outputFilSetting, outputFile);
    settings->endGroup();
    delete settings;
}

//-----------------------------------------------------------------------------------------
void MainWindow::readSettings() {
    QSettings *settings = new QSettings(layerSoftGroup, applicationGroup);
    ui->edPathToStrategyFile->setText(settings->value(SettingsGroup + "/" + strategyFileSetting).toString());
    ui->edPathToDestinationFile->setText(settings->value(SettingsGroup + "/" + destinationFileSetting).toString());
    ui->edPathToOutputFile->setText(settings->value(SettingsGroup + "/" + outputFilSetting).toString());
    delete settings;
}

//-----------------------------------------------------------------------------------------
void MainWindow::saveStringListToFile(const QStringList &l, const QString &filePath) {
    QFile fOut(QDir::toNativeSeparators(filePath));
    if (fOut.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream s(&fOut);
        s.setCodec(codecUTF8.toUtf8());
        for (int i = 0; i < l.size(); ++i)
            s << l.at(i) << '\n';
    }
    fOut.close();
}

//-----------------------------------------------------------------------------------------
void MainWindow::missingComponent(const QString &authorComment,
                                  const QString &component,
                                  const QString &elExtFldRefValue,
                                  const QString &elExtFldRefFootPrint,
                                  const QString &description) {
    QString res("");
    res = authorComment + " <b><span style='color:red'>" + component
            + "</span></b> - <u><span style='color:blue'>"
            + elExtFldRefValue
            + "</span></u> - <i>"
            + elExtFldRefFootPrint
            + "</i> - "
            + description + "<br />";
    ui->textLog->insertHtml(res);
}

//-----------------------------------------------------------------------------------------
void MainWindow::parseFiles() {

    if(!ui->edPathToStrategyFile->text().length() ||
            !ui->edPathToDestinationFile->text().length() ||
            !ui->edPathToOutputFile->text().length()) {
        QMessageBox::warning(this, "Warning", "There is an empty field. Please, fill all fields properly!", QMessageBox::Ok);
        return;
    }

    QStringList sortedList;
    ui->textLog->clear();

    ParseComponentFile *obj = new ParseComponentFile();
    connect(obj,
            SIGNAL(showMissingComponent(QString,QString,QString,QString,QString)),
            this,
            SLOT(missingComponent(QString,QString,QString,QString,QString)));

    obj->setFilesForProcessings(ui->edPathToStrategyFile->text(),
                                ui->edPathToDestinationFile->text());
    sortedList = obj->sortComponentsList();

    saveStringListToFile(sortedList, ui->edPathToOutputFile->text());
    ui->textLog->setVisible(true);
    setFixedHeight(550);

    delete obj;
}
