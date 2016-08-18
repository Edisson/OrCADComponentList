#include <QDir>
#include <QFile>
#include <QDebug>
#include <QMap>
#include <QTextStream>

#include "CONST.h"
#include "StrategyRegExp.h"
#include "ParseComponentFile.h"

ParseComponentFile::ParseComponentFile(QObject *parent) :
    QObject(parent) {

    parselist.clear();
    components.clear();
    substitution = new StrategyRegExp(this);
}

void ParseComponentFile::setFilesForProcessings(const QString &fileRegExp,
                                                const QString &fileForParse) {
    substitution->setRegExpFromFile(fileRegExp);
    setFileForParse(fileForParse);
}

void ParseComponentFile::setRegExpFromFile(const QString &filePath) {
    substitution->setRegExpFromFile(filePath);
}

void ParseComponentFile::setFileForParse(const QString &filePath) {
    QFile f(QDir::toNativeSeparators(filePath));

    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream stream(&f);

    while (true) {
        QString line = stream.readLine();
        if (line.isNull())
            break;
        else
            parselist.append(line);
    }
}

QString ParseComponentFile::createString(const QString &allComponents,
                                         QString &footPrint,
                                         const QString &description,
                                         const QString &quantityComponents) {
    QString resStr("");
    resStr += allComponents;
    resStr += "\t";
    substitution->replaceEarthSymbol(footPrint);
    resStr  += footPrint;
    resStr  += " ";
    resStr  += description;
    resStr  += "\t";
    resStr  += quantityComponents;
    return resStr;
}

QString ParseComponentFile::comaDot(int firstVal, int secondVal, const QString &componentName) {
    if(secondVal - firstVal == 1)
        return (componentName + QString::number(firstVal) + ", " + componentName + QString::number(secondVal));
    else
        return (componentName + QString::number(firstVal) + "..." + componentName + QString::number(secondVal));

    return "";
}

void ParseComponentFile::checkSequence(QString &con, QStringList &resultList) {
    QString componentName = substitution->getComponentName(con);
    con.remove(QRegExp(componentName));
    QStringList tmpList = con.split(QRegExp("[,]"), QString::SkipEmptyParts);

    if(tmpList.count() == 1) {
        QString stickedCompName("");
        stickedCompName	= componentName + tmpList.at(0);
        resultList.append(stickedCompName);
        return;
    }

    QString curGroup("");

    int curStart = 0;
    int prevValue = tmpList.at(0).toInt();

    for(int i = 0; i < tmpList.count(); i++) {
        int lastIndex = tmpList.count() - 1;
        int currentValue = tmpList.at(i).toInt();

        if( (i > 0) )
            prevValue = tmpList.at(i-1).toInt();

        if( ((currentValue - prevValue) > 1) || (i == lastIndex) ) {

            if( (tmpList.at(curStart).toInt() == prevValue) && (i != lastIndex) ) {
                curGroup = componentName + tmpList.at(curStart);
            }
            else {
                if(i == lastIndex) {
                    if( (currentValue - tmpList.at(i-1).toInt() ) > 1) {
                        if(tmpList.at(curStart).toInt() == prevValue) {
                            curGroup = componentName + tmpList.at(curStart);
                            resultList.append(curGroup);

                            QString tmp = componentName;
                            tmp += tmpList.at(i);
                            resultList.append(tmp);
                            return;
                        } else {
                            if( (tmpList.at(i).toInt() - tmpList.at(i-1).toInt()) > 1 ) {
                                curGroup = comaDot(tmpList.at(curStart).toInt(), tmpList.at(i-1).toInt(), componentName);
                                resultList.append(curGroup);

                                QString tmp = componentName;
                                tmp += tmpList.at(i);
                                resultList.append(tmp);
                                return;
                            }
                        }
                    }
                    else {
                        curGroup = comaDot(tmpList.at(curStart).toInt(), tmpList.at(i).toInt(), componentName);
                    }
                }
                else {
                    curGroup = comaDot(tmpList.at(curStart).toInt(), prevValue, componentName);
                }
            }
            resultList.append(curGroup);
            curStart = i;
        }
    }
}

QStringList ParseComponentFile::convert() {
    QStringList resList;
    for(int i = 0; i < parselist.count(); i++) {
        if(parselist.at(i).length() != 0) {
            QString tmpStr = parselist.at(i);
            QStringList tmpStrList = tmpStr.split(QRegExp("[\t]"), QString::SkipEmptyParts);
            int columnsCount = tmpStrList.count();

            QString allComponents("");
            if(columnsCount > enRefDestination)
                allComponents = tmpStrList.at(enRefDestination);

            QString componentName = substitution->getComponentName(allComponents);

            QString quantityComponents("");
            if(columnsCount > enQuantityDestination)
                quantityComponents  = tmpStrList.at(enQuantityDestination);

            QString extFldRefVal("");
            if(columnsCount > enRefValueDestination)
                extFldRefVal = tmpStrList.at(enRefValueDestination);

            QString extFldFootPrint("");
            if(columnsCount > enRefFootPrintDestination)
                extFldFootPrint = tmpStrList.at(enRefFootPrintDestination);

            QString extFldDescription("");

            if(substitution->findComponent(componentName, extFldRefVal, extFldFootPrint, extFldDescription)) { // found component
                QString sortedComponents("");
                QStringList list;
                checkSequence(allComponents, list);

                if(list.length() == 1) {
                    sortedComponents = list.at(0);
                    //resList.append(createString(sortedComponents, extFldFootPrint, extFldDescription, quantityComponents));
                }

                if(list.length() > 1) {
                    for(int i = 0; i < list.count(); i++) {
                        sortedComponents += list.at(i);
                        if(i != list.count() - 1)
                            sortedComponents += ", ";
                    }
                }
                resList.append(createString(sortedComponents, extFldFootPrint, extFldDescription, quantityComponents));
            }
            else {
                emit showMissingComponent(authorCommNOTFOUND, allComponents, extFldRefVal, extFldFootPrint, extFldDescription);
            }
        }
    }
    return resList;
}

inline int findNumberPart(const QString& sIn) {
    QString s = "";
    int i = 0;
    bool isNum = false;
    while (i < sIn.length()) {
        if (isNum) {
            if (!sIn[i].isNumber())
                break;
            s += sIn[i];
        }
        else {
            if(sIn[i] == char('.') || sIn[i] == char('\t'))
                break;
            if (sIn[i].isNumber())
                s += sIn[i];
        }
        ++i;
    }
    if (s == "")
        return 0;

    return s.toInt();
}

bool naturalSortCallback(const QString& s1, const QString& s2) {
    int idx1 = findNumberPart(s1);
    int idx2 = findNumberPart(s2);
    return (idx1 < idx2);
}

QStringList ParseComponentFile::manageCluster(ClustersType clusters) {
    QStringList tmpCompGrouppedList;
    QStringList tmpCompGrouppedListClusters;
    for (itrClustersType i = clusters.constBegin(); i != clusters.constEnd(); ++i) {
        ClusterType tmpCl = *i;
        for(itrClusterType itrCl = tmpCl.constBegin(); itrCl != tmpCl.constEnd(); ++itrCl) {
            QString components = itrCl.key();
            QString description = itrCl.value();

            QStringList tmpCompList = components.split(QRegExp("[,]"), QString::SkipEmptyParts);
            for(int iCL = 0; iCL < tmpCompList.size(); iCL++) {
                QString fullStr = tmpCompList.at(iCL).trimmed() + "\t" + description.trimmed();
                tmpCompGrouppedList.append(fullStr);
            }
        }
        qSort(tmpCompGrouppedList.begin(),
              tmpCompGrouppedList.end(),
              naturalSortCallback);

        tmpCompGrouppedListClusters.append(tmpCompGrouppedList);
        tmpCompGrouppedList.clear();
    }
    //    foreach(QString s, tmpCompGrouppedListClusters)
    //        qDebug() << s;
    return tmpCompGrouppedListClusters;
}

QStringList ParseComponentFile::reGroupToCluster(const QStringList &parsedList) {
    QString prevComponent("");
    for(int i = 0; i < parsedList.count(); i++) {
        if(parsedList.at(i).length() != 0) {
            QString tmpStr = parsedList.at(i);
            QStringList tmpStrList = tmpStr.split(QRegExp("[\t]"), QString::SkipEmptyParts);
            int columnsCount = tmpStrList.count();

            QString allComponents("");
            if(columnsCount > enRefDestination)
                allComponents = tmpStrList.at(enItemDestination);

            QString componentName = substitution->getComponentName(allComponents);
            if(i == 0) {
                cluster.clear();
                cluster.insert(tmpStrList.at(enItemDestination), tmpStrList.at(enQuantityDestination));
            }
            else {
                if(prevComponent == componentName) { // if the next element is the same though add to cluster list
                    cluster.insert(tmpStrList.at(enItemDestination), tmpStrList.at(enQuantityDestination));
                }
                else {
                    clusters.append(cluster);

                    cluster.clear();
                    cluster.insert(tmpStrList.at(enItemDestination), tmpStrList.at(enQuantityDestination));
                }
            }
            prevComponent = componentName;
        }
    }

    QStringList resList;
    if(clusters.size() > 0) {
        clusters.append(cluster);
        resList = manageCluster(clusters);
    }
    return resList;
}

int ParseComponentFile::countComponents(const QString &components) {
    int fIdx = 0; int sIdx = 0;
    return substitution->extractQuantityIndexsOfComponentName(components, fIdx, sIdx);
}

QStringList ParseComponentFile::reGroupFinalList(const QStringList &procList) {
    tmpContainer tmpResContainer;
    tmpResContainer._component = "";
    tmpResContainer._description = "";
    tmpResContainer._quantity = 0;

    QMap<int, tmpContainer> resList;

    QString prevDescriptionComponent("");
    for(int i = 0; i < procList.count(); i++) {
        QString tmpStr = procList.at(i);
        QStringList tmpStrList = tmpStr.split(QRegExp("[\t]"), QString::SkipEmptyParts);

        QString component("");
        QString descriptionComponent("");

        if(tmpStrList.count() > enItemDestination) {
            component = tmpStrList.at(enItemDestination);
            descriptionComponent = tmpStrList.at(enQuantityDestination);

            //if(prevDescriptionComponent != descriptionComponent) {
            tmpResContainer._component = component;
            tmpResContainer._description = descriptionComponent;

            tmpResContainer._quantity = countComponents(component);
            resList.insert(i, tmpResContainer);
            //}
            //            else {
            //                QMap<int, tmpContainer>::iterator itrContainer = resList.end();
            //                --itrContainer;

            //                (*itrContainer)._component += ", ";
            //                (*itrContainer)._component += component;
            //                //(*itrContainer)._description = descriptionComponent;
            //                (*itrContainer)._quantity += countComponents(component);
            //            }
        }
        prevDescriptionComponent = descriptionComponent;
    }

    QStringList resListFinal;
    QString denomination;
    QString prevDenomination;
    for(QMap<int, tmpContainer>::const_iterator itr = resList.constBegin();
        itr != resList.constEnd();
        itr++) {

        QString componentName = substitution->getComponentName((*itr)._component);
        denomination = substitution->getRefDenomination(componentName);

        if((denomination != prevDenomination))
            resListFinal.append(denomination);
        /*
        && (denomination != deleteComponent)
        else if(denomination == deleteComponent) {
            emit showMissingComponent(authorCommDELETED, (*itr)._component, (*itr)._description, "", "");
        }
        */

        QString str = (*itr)._component + "\t" + (*itr)._description + "\t" + QString::number((*itr)._quantity);
        resListFinal.append(str);

        prevDenomination = denomination;
    }
    return resListFinal;
}

void ParseComponentFile::addStringToComponentList(const QString &components,
                                                  const QString &extField,
                                                  const QString &count){
    static int keyCounter = 0;
    if(this->components.count()) {
        itrComponents itr = this->components.end();
        --itr;
        keyCounter = itr.key();
        ++keyCounter;
    }
    this->components.insertMulti(keyCounter, {components, extField, count});
}

QStringList ParseComponentFile::sortComponentsList() {
    QStringList convertedList;
    convertedList.clear();

    QStringList groupedList;
    groupedList.clear();

    convertedList.append(convert());
    groupedList.append(reGroupToCluster(convertedList));

    return reGroupFinalList(groupedList);
}

/* Old works variant but ut needed to get components in different strings
QStringList ParseComponentFile::reGroupFinalList(const QStringList &procList) {
    tmpContainer tmpResContainer;
    tmpResContainer._component = "";
    tmpResContainer._description = "";
    tmpResContainer._quantity = 0;

    QMap<int, tmpContainer> resList;

    QString prevDescriptionComponent("");
    for(int i = 0; i < procList.count(); i++) {
        QString tmpStr = procList.at(i);
        QStringList tmpStrList = tmpStr.split(QRegExp("[\t]"), QString::SkipEmptyParts);

        QString component("");
        QString descriptionComponent("");

        if(tmpStrList.count() > enItemDestination) {
            component = tmpStrList.at(enItemDestination);
            descriptionComponent = tmpStrList.at(enQuantityDestination);

            if(prevDescriptionComponent != descriptionComponent) {
                tmpResContainer._component = component;
                tmpResContainer._description = descriptionComponent;

                tmpResContainer._quantity = countComponents(component);
                resList.insert(i, tmpResContainer);
            }
            else {
                QMap<int, tmpContainer>::iterator itrContainer = resList.end();
                --itrContainer;

                (*itrContainer)._component += ", ";
                (*itrContainer)._component += component;
                //(*itrContainer)._description = descriptionComponent;
                (*itrContainer)._quantity += countComponents(component);
            }
        }
        prevDescriptionComponent = descriptionComponent;
    }

    QStringList resListFinal;
    QString denomination;
    QString prevDenomination;
    for(QMap<int, tmpContainer>::const_iterator itr = resList.constBegin();
        itr != resList.constEnd();
        itr++) {

        QString componentName = substitution->getComponentName((*itr)._component);
        denomination = substitution->getRefDenomination(componentName);
        if(denomination != prevDenomination)
            resListFinal.append(denomination);

        QString str = (*itr)._component + "\t" + (*itr)._description + "\t" + QString::number((*itr)._quantity);
        resListFinal.append(str);

        prevDenomination = denomination;
    }
    return resListFinal;
}
*/
