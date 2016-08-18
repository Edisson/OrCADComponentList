#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

private slots:
    void parseFiles();
    void missingComponent(const QString &authorComment,
                          const QString &component,
                          const QString &elExtFldRefValue,
                          const QString &elExtFldRefFootPrint,
                          const QString &description);

    void saveStringListToFile(const QStringList &l, const QString &filePath);

    void openDirectory();
    void readSettings();
    void writeSettings(const QString &strategyFile,
                       const QString &destinationFile,
                       const QString &outputFile);

private:
    Ui::MainWindow *ui;
    QStringList strategyList;
    QStringList destinationList;

    enum enColumnDestination {
        enItemDestination,
        enQuantityDestination,
        enRefDestination,
        enRefValueDestination,
        enRefFootPrintDestination,
        nextColumnBreakDestination
    };

    enum enColumnStrategy {
        enRefStrategy,
        enRefValueStrategy,
        enRefFootPrintStrategy,
        enOrderSalesStrategy,
        nextColumnBreakStrategy
    };
};

#endif // MAINWINDOW_H
