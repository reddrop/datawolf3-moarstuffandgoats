#include "modeparser.h"

ModeParser::ModeParser(const QStringList &modewithargs)
{
    _fullstring = modewithargs.join(" ");
    parameters = modewithargs;
    modestr = parameters.takeAt(0);
    parse();
}

void ModeParser::parse()
{
    if ((modestr.at(0)!=QChar('+')) && (modestr.at(0)!=QChar('-')))
        return;
    for(int pos=0; pos<modestr.length(); pos++)
    {
        char c = modestr.at(pos).toLatin1();
        QString mode;
        switch (c)
        {
        case '+':
        case '-':
            sign = QChar(c);
            break;
        case 'm':
        case 'n':
        case 'p':
        case 's':
        case 't':
        case 'i':
            mode.append(sign);
            mode.append(c);
            if (!modes.contains(mode))
                modes.insert(mode, QStringList());
            break;
        case 'k':
        case 'l':
        case 'b':
        case 'e':
        case 'o':
        case 'h':
        case 'v':
        case 'I':
            mode.append(sign);
            mode.append(c);
            if (!parameters.isEmpty())
            {
                QStringList args = modes.value(mode);
                args << parameters.takeFirst();
                modes.insert(mode, args);
            }
        }
    }
}
