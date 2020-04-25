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
    ACLMessage command(ACLMessage::REQUEST);



    //command.addContent(QLatin1String("command"), saveCommand);

    // TODO: get local state return from base application
    emit signalSaveState(command);
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
            d->recorded  = true;
            // TODO: save local state
            Message command;
            command.addContent(QLatin1String("command"), saveCommand);

            emit signalSaveState(command);
        }

        contents.remove(QLatin1String("color"));

        // TODO: sort message by "type";
        //contents.remove(QLatin1String("type"));
    }

    return Message(contents);
}

void LaiYangSnapshot::addState(const QString& state)
{
    // This object should contain siteID, vector clock, local variables
    QJsonObject localState = QJsonDocument::fromJson(state.toUtf8()).object();

    // TODO: do some verification on Vector clock before saving base on siteID
    d->states.insert(QUuid(localState[QLatin1String("siteID")].toString()), localState);
/*
    if (! d->initiator)
    {
        // TODO send to initiator
        Message message;

    }
*/
}

}
