#ifndef PARSECOMPONENTFILE_H
#define PARSECOMPONENTFILE_H

#include <QObject>
#include <QStringList>
#include <QMultiMap>

class StrategyRegExp;

typedef QMultiMap<QString, QString> ClusterType;
typedef QList<QMultiMap<QString, QString> > ClustersType;

typedef QMultiMap<QString, QString>::const_iterator itrClusterType;
typedef QList<QMultiMap<QString, QString> >::const_iterator itrClustersType;

class ParseComponentFile : public QObject {
    Q_OBJECT
signals:
    void showMissingComponent(const QString &authorComment,
                              const QString &component,
                              const QString &elExtFldRefValue,
                              const QString &elExtFldRefFootPrint,
                              const QString &description);

public:
    explicit ParseComponentFile(QObject *parent = 0);

public slots:
    QStringList sortComponentsList();
    void setFilesForProcessings(const QString &fileRegExp,
                                const QString &fileForParse);

private slots:
    QStringList convert();
    QStringList manageCluster(ClustersType clusters);
    QStringList reGroupFinalList(const QStringList &procList);
    QStringList reGroupToCluster(const QStringList &parsedList);

    QString createString(const QString &allComponents,
                         QString &footPrint,
                         const QString &description,
                         const QString &quantityComponents);
    QString comaDot(int firstVal,
                    int secondVal,
                    const QString &componentName);

    int countComponents(const QString &components);
    void setFileForParse(const QString &filePath);
    void setRegExpFromFile(const QString &filePath);
    void addStringToComponentList(const QString &components,
                                  const QString &extField,
                                  const QString &count);


    void checkSequence(QString &con, QStringList &resultList);

private:
    QStringList parselist;
    StrategyRegExp *substitution;

    enum enColumnDestination {
        enItemDestination,
        enQuantityDestination,
        enRefDestination,
        enRefValueDestination,
        enRefFootPrintDestination,
        nextColumnBreakDestination
    };

    struct resultComponentList {
        QString elComponents;
        QString elExtendedField1;
        QString elCount;
    };

    struct tmpContainer {
        QString _component;
        QString _description;
        int _quantity;
        //bool deleteGroup;
    };

    QMultiMap<int, resultComponentList> components;
    typedef QMultiMap<int, resultComponentList>::iterator itrComponents;

    ClusterType cluster;
    ClustersType clusters;

    ClusterType groupedCluster;

};

#endif // PARSECOMPONENTFILE_H
