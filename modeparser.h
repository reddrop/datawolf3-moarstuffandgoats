#ifndef MODEPARSER_H
#define MODEPARSER_H

#include <QString>
#include <QStringList>
#include <QMultiHash>

class ModeParser
{
public:
    ModeParser(const QStringList &modewithargs);

private:
    QString _fullstring;
    QString modestr;
    QStringList parameters;
    QHash<QString, QStringList> modes;
    QChar sign;
    void parse();

public:
    const QString fullstring()
    {
        return _fullstring;
    }
    bool hasMode(const QString &mode)
    {
        return modes.contains(mode);
    }
    QStringList argsOfMode(const QString &mode)
    {
        return modes.value(mode);
    }
    QStringList modesList()
    {
        return QStringList(modes.keys());
    }
};

#endif // MODEPARSER_H
