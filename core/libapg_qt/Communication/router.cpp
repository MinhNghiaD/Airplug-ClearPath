#include "router.h"

namespace AirPlug
{

class Q_DECL_HIDDEN Router::Private
{
public:
    Private()
        : communicationMngr(nullptr),
          nbSequence(0)
    {
    }

    ~Private()
    {
    }

public:

    void forwardApptoNet(Header& header, ACLMessage& message);

public:

    CommunicationManager* communicationMngr;

    // using siteID and sequence nb to control old message
    QString               siteID;
    int                   nbSequence;

    QHash<QString, int>   recentSequences;
};


void Router::Private::forwardApptoNet(Header& header, ACLMessage& message)
{
    // mark message ID
    message.setSender(siteID);
    message.setNbSequence(++nbSequence);

    QJsonObject contents =  message.getContent();

    contents[QLatin1String("app")] = header.what();

    message.setContent(contents);

    communicationMngr->send(message, QLatin1String("NET"), QLatin1String("NET"), header.where());
}
/*----------------------------------------------------------------------------------------------------------------------------------------------------*/


Router::Router(CommunicationManager* communication, const QString& siteID)
    : QObject(nullptr),
      d(new Private())
{
    if (!communication)
    {
        qFatal("Router: Communication Manager is null");
    }

    d->communicationMngr = communication;
    d->siteID            = siteID;

    setObjectName(QLatin1String("Router"));

    connect(d->communicationMngr, SIGNAL(signalMessageReceived(Header, Message)),
            this,                 SLOT(slotReceiveMessage(Header, Message)));
}

Router::~Router()
{
    delete d;
}

void Router::slotReceiveMessage(Header header, Message message)
{
    //ACLMessage aclMessage(message);

    QString app;

    if (header.what() == QLatin1String("NET"))
    {
        // receive from other NET ==> filter message
        //message = d->snapshotManager.preprocessMessage(message);

        // remove this pair and move to app
        //QHash<QString, QString> contents = message.getContents();

        //app = contents[QLatin1String("app")];

        //contents.remove(QLatin1String("app"));

        //message = Message(contents);
    }
    else
    {
        // forward from app to other nets
        message.addContent(QLatin1String("app"), header.what());

        app = QLatin1String("NET");

        // color message before sending to other NET
        //d->snapshotManager.colorMessage(message);
    }

    //sendMessage(message, QLatin1String("NET"), app, header.where());
}


}
