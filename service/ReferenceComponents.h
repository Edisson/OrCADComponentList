#ifndef REFERENCECOMPONENTS_H
#define REFERENCECOMPONENTS_H

#include <QMap>
#include <QObject>

class ReferenceComponents : public QObject
{
    Q_OBJECT
public:
    explicit ReferenceComponents(QObject *parent = 0);
    
signals:
    
public slots:
    void clearReference(void);
    void addReference(const QString &ref, const QString &denomination);

    QString getRefDenomination(const QString &ref);

    void debugPrint();

private:
    QMap<QString, QString> reference;
    
};

#endif // REFERENCECOMPONENTS_H
