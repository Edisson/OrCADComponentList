#include <QDebug>
#include "ReferenceComponents.h"

ReferenceComponents::ReferenceComponents(QObject *parent) :
    QObject(parent)
{
}

void ReferenceComponents::addReference(const QString &ref, const QString &denomination) {
    reference.insert(ref, denomination);
}

void ReferenceComponents::clearReference(void) {
    reference.clear();
}

QString ReferenceComponents::getRefDenomination(const QString &ref) {
    if(reference.contains(ref))
        return reference[ref];
    return "";
}

void ReferenceComponents::debugPrint() {
    for(QMap<QString, QString>::iterator i = reference.begin();
        i != reference.end();
        i++) {
        qDebug() << i.key();
        qDebug() << i.value();
    }
}
