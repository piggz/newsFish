#include "Helper.h"
#include <QDebug>
#include <qstringliteral.h>

Helper::Helper(QObject *parent) :
    QObject(parent)
{
}

QVariant Helper::getSetting(const QString &settingname, QVariant def)
{
    return settings.value(QStringLiteral("settings/") + settingname, def);
}

void Helper::setSetting(const QString &settingname, QVariant val)
{
    settings.setValue(QStringLiteral("settings/") + settingname, val);
}

bool Helper::settingExists(const QString &settingname)
{
    return settings.contains(QStringLiteral("settings/") + settingname);
}
