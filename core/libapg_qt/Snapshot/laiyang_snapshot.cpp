#include "laiyang_snapshot.h"

//#include <QVector>
#include <QUuid>

#include "vector_clock.h"

namespace AirPlug
{

class Q_DECL_HIDDEN LaiYangSnapshot::Private
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

    int msgCounter;

    // System state will be encoded in Json object
    QHash<QUuid, QJsonObject> states;
};

LaiYangSnapshot::LaiYangSnapshot()
    : QObject(nullptr),
      d(new Private())
{
}

LaiYangSnapshot::~LaiYangSnapshot()
{
    delete d;
}

void LaiYangSnapshot::init()
{
    d->initiator = true;

    d->recorded  = true;

    // Send save command to Control application to record local state
    //ACLMessage command(ACLMessage::REQUEST);



    //command.addContent(QLatin1String("command"), saveCommand);

    // TODO: get local state return from base application
    //emit signalSaveState(command);
}

void LaiYangSnapshot::colorMessage(ACLMessage* message)
{
    QJsonObject content = message->getContent();

    // append color field to the content of the message
    if (d->recorded)
    {
        content[QLatin1String("color")] = QLatin1String("red");
    }
    else
    {
        content[QLatin1String("color")] = QLatin1String("white");
    }

    message->setContent(content);
}


void LaiYangSnapshot::collectState(const QJsonObject& state)
{
    // TODO: do some verification on Vector clock before saving base on siteID

/*
    if (! d->initiator)
    {
        // TODO send to initiator
        Message message;

    }
*/
}

}
