#ifndef StrategyRegExp_H
#define StrategyRegExp_H

#include <QMap>
#include <QObject>
#include <QVector>
#include <QStringList>

class ReferenceComponents;
class StrategyRegExp : public QObject {
    Q_OBJECT
signals:

public:
    explicit StrategyRegExp(QObject *parent = 0);

public slots:
    void insertRegExp(const QString &component,
                      const QString &elExtField1,
                      const QString &elExtField2,
                      const QString &elExtField3);

    void setRegExpFromFile(const QString &filePath);

    void debug();
    bool matchRegExpPattern(const QString &strPattern,
                            const QString &strMatching);
    void editDescription(QString &elExtFldRefValue,
                         QString &elExtFldDescription,
                         QString &description);

    void replaceAsterix(QVector<int> &positions,
                        QString &description,
                        const QStringList &values);

    QString getRefDenomination(const QString &ref);
    QStringList extractValues(const QString &str);
    QVector<int> asterixPositions(const QString &str);

    bool findComponent(const QString &component,
                       QString &extField1Components,
                       QString &extField2Components,
                       QString &description);

    int extractQuantityIndexsOfComponentName(QString stringToExtract,
                                             int &first,
                                             int &second);

    void replaceEarthSymbol(QString &strToReplace);
    QString getComponentName(const QString &allComponents);

private:
    enum enColumnStrategy {
        enRefStrategy,
        enRefValueStrategy,
        enRefFootPrintStrategy,
        enOrderSalesStrategy,
        nextColumnBreakStrategy
    };

    struct templateRegExp {
        QString elExtFldRefValue;
        QString elExtFldRefFootPrint;
        QString elExtFldDescription;
    };

    QMultiMap<QString, templateRegExp> allRegExp;
    typedef QMultiMap<QString, templateRegExp>::iterator itrRegExp;

    ReferenceComponents *refComponents;
    
};

#endif // StrategyRegExp_H
