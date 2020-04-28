#include "router.h"

// Qt includes
#include <QDebug>

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

    void forwardAppToNet(Header& header, ACLMessage& message);
    void forwardNetToApp(Header& header, ACLMessage& message);

public:

    bool isOldMessage(const ACLMessage& messsage);
    bool isSnapshotMessage(const ACLMessage& messsage);

public:

    CommunicationManager* communicationMngr;

    // using siteID and sequence nb to control old message
    QString               siteID;
    int                   nbSequence;

    QHash<QString, int>   recentSequences;
};


void Router::Private::forwardAppToNet(Header& header, ACLMessage& message)
{
    // mark message ID
    message.setSender(siteID);
    message.setNbSequence(++nbSequence);

    QJsonObject contents =  message.getContent();

    // set App name
    contents[QLatin1String("app")] = header.what();

    message.setContent(contents);

    communicationMngr->send(message, QLatin1String("NET"), QLatin1String("NET"), header.where());
}

void Router::Private::forwardNetToApp(Header& header, ACLMessage& message)
{
    if (! message.getContents().contains(QLatin1String("receiver")))
    {
        // broadcast the message to another net
        communicationMngr->send(message, QLatin1String("NET"), QLatin1String("NET"), header.where());
    }
    else
    {
        // TODO routing
    }

    QJsonObject contents =  message.getContent();

    QString app = contents[QLatin1String("app")].toString();
    contents.remove(QLatin1String("app"));

    message.setContent(contents);

    communicationMngr->send(message, QLatin1String("NET"), app, header.where());
}

bool Router::Private::isOldMessage(const ACLMessage& messsage)
{
    QString sender = messsage.getSender();
    int sequence   = messsage.getNbSequence();

    if ( recentSequences.contains(sender) && (recentSequences[sender] >= sequence) )
    {
        // Here we suppose the channels are FIFO
        // therefore for each router, by keeing the sequence number of each site, we can identify old repeated message
        qDebug() << "Drop old message";

        return true;
    }

    // Update recent sequence
    recentSequences[sender] = sequence;

    return false;
}

bool Router::Private::isSnapshotMessage(const ACLMessage& messsage)
{
    ACLMessage::Performative performative = messsage.getPerformative();

    if (performative == ACLMessage::REQUEST_SNAPSHOT || performative == ACLMessage::INFORM_STATE || performative == ACLMessage::PREPOST_MESSAGE)
    {
        return true;
    }
    else
    {
        return false;
    }
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
    // cast message to ACL format
    ACLMessage aclMessage(*(static_cast<ACLMessage*>(&message)));

    if (header.what() == QLatin1String("NET"))
    {
        if (d->isOldMessage(aclMessage))
        {
            return;
        }

        if (d->isSnapshotMessage(aclMessage))
        {
            // TODO: put snapshot in here ???
            emit signalSnapshotMessage(aclMessage);
        }
        else
        {
            d->forwardNetToApp(header, aclMessage);
        }
    }
    else
    {
        d->forwardAppToNet(header, aclMessage);
    }
}


}
