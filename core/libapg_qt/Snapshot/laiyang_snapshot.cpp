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

    requestSnapshot();
}

bool LaiYangSnapshot::processMessage(ACLMessage* message, bool isLocal)
{
    switch (message->getPerformative())
    {
        case ACLMessage::INFORM_STATE :

            //return processStateMessage(message, isLocal);

        case ACLMessage::PREPOST_MESSAGE:

            break;

        default:
            // Normal message
            if (isLocal)
            {
                // receive message from local application ==> add color and send
                colorMessage(message);
            }
            else
            {
                // Receive from another NET ==> get color
                QString color = getColor(message);

                if (color == QLatin1String("red") && !d->recorded)
                {
                    requestSnapshot();
                }
                else if (color == QLatin1String("white") && !d->recorded)
                {
                    // prepost message

                }
            }

            // forward the message
            return true;
    }
}


/* ----------------------------------------------- Helper functions -----------------------------------------------*/

void LaiYangSnapshot::requestSnapshot()
{
    d->recorded = true;

    ACLMessage* command = new ACLMessage(ACLMessage::REQUEST_SNAPSHOT);

    emit signalRequestSnapshot(command);
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

QString LaiYangSnapshot::getColor(ACLMessage* message) const
{
    QJsonObject content = message->getContent();

    QString color = content[QLatin1String("color")].toString();

    content.remove(QLatin1String("color"));

    message->setContent(content);

    return color;
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
