#include <QDir>
#include <QFile>
#include <QDebug>
#include <QRegExp>
#include <QTextCodec>
#include <QMessageBox>
#include <vector>

#include "CONST.h"
#include "StrategyRegExp.h"
#include "ReferenceComponents.h"

using namespace std;

StrategyRegExp::StrategyRegExp(QObject *parent) :
    QObject(parent) {

    refComponents = new ReferenceComponents(parent);

}

void StrategyRegExp::setRegExpFromFile(const QString &filePath) {
    QString pathToFile(QDir::toNativeSeparators(filePath));
    QFile fileOfStrategy(pathToFile);

    QStringList strategyList;
    strategyList.clear();
    refComponents->clearReference();

    if (!fileOfStrategy.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream streamOfStrategy(&fileOfStrategy);
    QTextCodec *codec = QTextCodec::codecForName(codecUTF8.toUtf8());
    streamOfStrategy.setCodec(codec);

    while (true) {
        QString line = streamOfStrategy.readLine();
        if (line.isNull())
            break;
        else
            strategyList.append(line);
    }

    QString componentDenomination("");

    for(int i = 0; i < strategyList.count(); i++) {
        QString tmpStr = strategyList.at(i);
        if(tmpStr.length()) {
            QStringList tmpStrList = tmpStr.split(QRegExp("[ ]"), QString::SkipEmptyParts);
            if(tmpStrList.count() >= 4) {
                QString Ref(tmpStrList.at(enRefStrategy));
                QString RefValue(tmpStrList.at(enRefValueStrategy));
                QString RefFootPrint(tmpStrList.at(enRefFootPrintStrategy));
                QString OrderSales(tmpStrList.at(enOrderSalesStrategy));

                if(Ref.at(enRefStrategy) == sharp.data()->toTitleCase()) // check if string is commented
                    continue;

                insertRegExp(Ref, RefValue, RefFootPrint, OrderSales);

                if(Ref.size() && componentDenomination.size())
                    refComponents->addReference(Ref, componentDenomination);

            }
            else {
                if(tmpStrList.count() >= 1)
                    componentDenomination = tmpStrList.at(enRefStrategy);
            }
        }
    }
}

QString StrategyRegExp::getRefDenomination(const QString &ref) {
    return refComponents->getRefDenomination(ref);
}

void StrategyRegExp::insertRegExp(const QString &component,
                                  const QString &elExtField1,
                                  const QString &elExtField2,
                                  const QString &elExtField3) {
    //    templateRegExp tmpInf;
    //    tmpInf.elExtendedField1 = elExtField1;
    //    tmpInf.elExtendedField2 = elExtField2;
    //    tmpInf.elExtendedField3 = elExtField3;

    allRegExp.insertMulti(component, {elExtField1, elExtField2, elExtField3} /*tmpInf*/);
}

bool StrategyRegExp::matchRegExpPattern(const QString &strPattern,
                                        const QString &strMatching) {
    QRegExp rx(strPattern);
    rx.setPatternSyntax(QRegExp::Wildcard);
    return rx.exactMatch(strMatching);
}

int StrategyRegExp::extractQuantityIndexsOfComponentName(QString stringToExtract, int &first, int &second) {
    QString cn = getComponentName(stringToExtract);
    stringToExtract.remove(QRegExp("["+cn+"]"));

    int quantityComponents = 0;
    bool check = false;
    if(stringToExtract.indexOf(QRegExp("[...]")) != -1) {
        stringToExtract.replace(QRegExp("[...]"), " ");
        vector<int> result;
        foreach( QString numStr, stringToExtract.split(" ", QString::SkipEmptyParts) ) {

            result.push_back(numStr.toInt(&check, 10));
            if( !check )
                continue;
        }

        first = result.at(0);
        second = result.at(1);
        quantityComponents = (second - first) + 1;
        return quantityComponents;
    }
    first = stringToExtract.toInt(&check, 10);
    second = 0;
    return 1;
}

QString StrategyRegExp::getComponentName(const QString &allComponents) {
    QString clearRef("");
    for(int j = 0; j != allComponents.count(); j++) {
        QChar ch = allComponents.at(j);
        if(ch.isLetter() && (ch != QChar(',')))
            clearRef += ch;
        else
            break;
    }
    return clearRef;
}

void StrategyRegExp::replaceEarthSymbol(QString &strToReplace) {
    for(int j = 0; j != strToReplace.length(); j++) {
        if(strToReplace.at(j) == QChar('_')) {
            strToReplace[j] = QChar(' ');
        }
    }
}

QStringList StrategyRegExp::extractValues(const QString &str) {
    QStringList tmpList = str.split(QRegExp("[ ]"), QString::SkipEmptyParts);
    QStringList outList; outList.clear();

    for(int i = 0; i < tmpList.count(); i++) {
        QString valueFull = tmpList.at(i);
        QString valueTmp("");
        for(int j = 0; j < valueFull.length(); j++) {
            if(valueFull.at(j).isDigit() || (valueFull.at(j).toTitleCase() == dot.data()->toTitleCase()))
                valueTmp += valueFull.at(j);
        }
        if(valueTmp.length())
            outList.append(valueTmp);
    }
    return outList;
}

QVector<int> StrategyRegExp::asterixPositions(const QString &str) {
    QVector<int> asterixPositions;
    for(int i = 0; i < str.count(); i++) {
        int idx = str.indexOf(asterix, i);
        i = idx;
        if(idx != -1)
            asterixPositions.push_back(idx);
        else
            break;
    }
    return asterixPositions;
}

void StrategyRegExp::replaceAsterix(QVector<int> &positions,
                                    QString &description,
                                    const QStringList &values) {
    if(values.count() >= positions.count()) {
        QVector<int>::const_iterator itrPos = positions.begin();
        int shift = 0;
        for(int i = 0; i < values.count(); i++) {
            int curPosAsterix = (*itrPos);
            curPosAsterix += shift;

            description.replace(curPosAsterix, 1, values.at(i));

            if(values.at(i).count() > 1)
                shift = values.at(i).count();

            itrPos++;
        }
    }
}

void StrategyRegExp::editDescription(QString &elExtFldRefValue,
                                     QString &elExtFldDescription,
                                     QString &description) {
    description = elExtFldDescription;
    replaceEarthSymbol(description);

    QStringList values(extractValues(elExtFldRefValue));
    QVector<int> positions = asterixPositions(elExtFldDescription);
    int pos = 0;
    if(values.count() < positions.count()) {
        return;
    }

    if(values.count() > positions.count()) {
        return;
    }

    for(int i = 0; i < positions.count(); i++) {
        pos = description.indexOf(asterix, pos);
        description.replace(pos, 1, values.at(i));
    }
}

bool StrategyRegExp::findComponent(const QString &component,
                                   QString &elExtFldRefValue,
                                   QString &elExtFldRefFootPrint,
                                   QString &description) {

    itrRegExp itr = allRegExp.find(component);
    while(itr != allRegExp.end() && itr.key() == component) {
        templateRegExp tmp = itr.value();
        replaceEarthSymbol(tmp.elExtFldRefValue);

        //TODO: if(elExtFldRefValue != asterix) {
        if(matchRegExpPattern(tmp.elExtFldRefValue, elExtFldRefValue)) {

            if(elExtFldRefFootPrint != asterix) {
                if(matchRegExpPattern(tmp.elExtFldRefFootPrint, elExtFldRefFootPrint)) {
                    editDescription(elExtFldRefValue, tmp.elExtFldDescription, description);
                    return true;  // if we found a match
                }
            }
            else {
                editDescription(elExtFldRefValue, tmp.elExtFldDescription, description);
                return true;  // if we found a match
            }

        }
        ++itr;
    }
    return false;
}

void StrategyRegExp::debug() {
    for(itrRegExp i = allRegExp.begin(); i != allRegExp.end(); i++) {
        qDebug() << i.key();
        templateRegExp tmp = i.value();
        qDebug() << tmp.elExtFldRefValue << " "
                 << tmp.elExtFldRefFootPrint << " "
                 << tmp.elExtFldDescription;
    }
}

