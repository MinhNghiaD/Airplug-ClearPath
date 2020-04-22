#include "laiyang_snapshot.h"

namespace AirPlug
{

const QString LaiYangSnapshot::commonType = QLatin1String("common");

class LaiYangSnapshot::Private
{
public:

    Private()
        : recorded(false),
          initiator(false),
          msgCounter(0)
    {
    }

    ~Private()
    {
    }

public:

    bool recorded;
    bool initiator;

    // TODO collect state
    int msgCounter;
};

LaiYangSnapshot::LaiYangSnapshot()
    : d(new Private())
{
}

LaiYangSnapshot::~LaiYangSnapshot()
{
    delete d;
}

void LaiYangSnapshot::init()
{
    d->initiator = true;

    //TODO: save local state
    d->recorded  = true;
}

void LaiYangSnapshot::colorMessage(Message& message)
{
    message.addContent(QLatin1String("type"), commonType);

    QString color;

    if (d->recorded)
    {
        color = QLatin1String("red");
    }
    else
    {
        color = QLatin1String("white");
    }

    message.addContent(QLatin1String("color"), color);
}

Message LaiYangSnapshot::preprocessMessage(const Message& message)
{
    QHash<QString, QString> contents = message.getContents();

    if (contents.contains(QLatin1String("color")))
    {
        QString color = contents[QLatin1String("color")];

        if (color == QLatin1String("red") && !d->recorded)
        {
            // TODO: save local state
            d->recorded  = true;
        }

        contents.remove(QLatin1String("color"));

        // TODO: sort message by "type";
        contents.remove(QLatin1String("type"));
    }

    return Message(contents);
}

}
