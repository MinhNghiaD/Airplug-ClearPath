#include "laiyang_snapshot.h"

#include <QVector>
#include "vector_clock.h"

namespace AirPlug
{

const QString LaiYangSnapshot::commonType  = QLatin1String("common");
const QString LaiYangSnapshot::saveCommand = QLatin1String("save local");

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

    // TODO collect state
    int msgCounter;

    // System state will be encoded in Json object
    QVector<QJsonObject> states;
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

    //TODO: save local state
    d->recorded  = true;

    // Send save command to Control application to record local state
    Message command;
    command.addContent(QLatin1String("command"), saveCommand);

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
            // TODO: save local state
            d->recorded  = true;
        }

        contents.remove(QLatin1String("color"));

        // TODO: sort message by "type";
        //contents.remove(QLatin1String("type"));
    }

    return Message(contents);
}

void LaiYangSnapshot::addState(const QString& state)
{
    // This object should contain siteID, vector clock, local state
    QJsonObject localState = QJsonDocument::fromJson(state.toUtf8()).object();

    // TODO: do some verification on Vector clock before saving
    d->states.append(localState);
}

}
