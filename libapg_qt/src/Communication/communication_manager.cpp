#include "communication_manager.h"

//Qt include
#include <QCoreApplication>
#include <QChar>
#include <QDebug>
#include <QMap>
#include <QStringList>

// Local includes
#include "src/Transporters/std_transporter.h"
#include "src/Transporters/udp_transporter.h"

namespace AirPlug
{

class Q_DECL_HIDDEN CommunicationManager::Private
{
public:

    Private()
        : safeMode(false)
    {
    }

    ~Private()
    {
        protocols.clear();
        subscriptions.clear();
    }

public:

    bool validAppName(const QString& name) const;
    bool validZone(const QString& zone) const;

    bool isAlphaNumeric(const QString& string) const;

public:

    Header::HeaderMode headerMode;

    bool safeMode;

    // Map ProtocolType with Protocol
    QMap<int, MessageTransporter*> protocols;
    // Map who and wheres
    QMap<QString, QStringList> subscriptions;
};


bool CommunicationManager::Private::validAppName(const QString& name) const
{
    // TODO: print verbose self.app().vrb("APP.com.valid_appname({})".format(text),6)

    if ((name.size() == 3)   &&
        (name[0].isLetter()) &&
        (isAlphaNumeric(name)))
    {
        return true;
    }

    // TODO print verbose self.app().vrb_dispwarning("Invalid app name : '{}'".format(text))

    return false;
}

bool CommunicationManager::Private::validZone(const QString& zone) const
{
    // TODO: print verbose self.app().vrb("APP.com.valid_zone({})".format(text),6)

    if (zone == Header::airHost || zone == Header::localHost)
    {
        return true;
    }

    // TODO: print verbose self.app().vrb_dispwarning("Invalid zone name : '{}'; should be '{}' or '{}'".format(text, self.hst_air(), self.hst_lch()))
    return false;
}

bool CommunicationManager::Private::isAlphaNumeric(const QString& string) const
{
    const QChar* data = string.constData();

    while (! data->isNull())
    {
        if (! data->isLetterOrNumber())
        {
            return false;
        }

        ++data;
    }

    return true;
}

/* ----------------------------------------------------------------------------------------------------------------*/

CommunicationManager::CommunicationManager(Header::HeaderMode mode, QObject* parent)
    : QObject(parent),
      d(new Private())
{
    setObjectName(QLatin1String("Communication manager"));

    d->headerMode = mode;
}

CommunicationManager::~CommunicationManager()
{
    delete d;
}

void CommunicationManager::addStdTransporter()
{
    MessageTransporter* transporter        = new StdTransporter(this);

    d->protocols[ProtocolType::StandardIO] = transporter;

    connect(transporter, SIGNAL(signalMessageReceived(QString)),
            this,        SLOT(slotReceiveMessage(QString)));

}

void CommunicationManager::addUdpTransporter(const QString& host,
                                            int port,
                                            MessageTransporter::UdpType type)
{
    MessageTransporter* transporter = new UdpTransporter(host, port, type, this);

    d->protocols[ProtocolType::UDP] = transporter;

    connect(transporter, SIGNAL(signalMessageReceived(QString)),
            this,        SLOT(slotReceiveMessage(QString)));
}

void CommunicationManager::setHeaderMode(Header::HeaderMode mode)
{
    d->headerMode = mode;
}

void CommunicationManager::setSafeMode(bool b)
{
    d->safeMode = b;
}

void CommunicationManager::send(const Message& message,
                                const QString& what,
                                const QString& who,
                                const QString& where,
                                ProtocolType   protocol,
                                bool           save)
{
    if (message.isEmpty())
    {
        qWarning() << "No text to send, aborting sending";

        return;
    }

    // Send message
    if (! d->protocols.contains(protocol))
    {
        qWarning() << "Not contain this protocol";
    }
    else
    {
        // TODO: print verbose self.app().debug("sending {} in mode {}".format(text, self.header_mode), 5)
        Header header(what, who, where);

        QString package = header.generateHeader(d->headerMode) + message.getMessage();

        d->protocols[protocol]->send(package);

        if (save)
        {
            // TODO save log self.app().svg.save_line("{} > {} : {}{}".format(self.app().APP(), protocol_name, header,text))
        }
    }
}

void CommunicationManager::slotReceiveMessage(const QString& data)
{
    if (data.isEmpty())
    {
        return;
    }

    QString what, who, where, payload;
    QStringList segments;

    if (d->headerMode == Header::HeaderMode::WhatWho ||
        d->headerMode == Header::HeaderMode::WhatWhoWhere)
    {
        if (! data[0].isLetter())
        {
            //separate header fields and payload
            segments = data.split(data[0], QString::SkipEmptyParts);
        }
        else
        {
            segments << data;
        }

        if (segments.size() > 1)
        {
            what = segments[0];
        }

        if (segments.size() > 2)
        {
            who = segments[1];
        }

        if (segments.size() > 3)
        {
            where = segments[2];
        }

        payload = segments.back();

        Header header(d->headerMode, d->safeMode, what, who, where);

        Message message(payload);

        // TODO: print verbose vrb("transmitting (if safe) to application {} txt '{}', from {}, to {} from zone {}".format(self.app(), text, header["what"], header["who"], header["where"]), 6)

        // Filter message
        bool safeMessage = false;

        if (d->headerMode == Header::HeaderMode::What)
        {
            safeMessage = true;
            // TODO: print verbose vrb("Every message is accepted in what mode",6)
        }
        else if (who == QCoreApplication::applicationName().toUpper())
        {
            if (isSubscribed(what, where))
            {
                safeMessage = true;
                // TODO print verbose self.app().vrb("Message designed for this app from a subscribed app",6)
            }

            if (where == Header::localHost || d->headerMode == Header::HeaderMode::WhatWho)
            {
                safeMessage = true;
                // TODO print verbose self.app().vrb("Message designed for this app from a local app",6)
            }
        }
        else if (who == Header::allHost && isSubscribed(what, where))
        {
            safeMessage = true;
            // TODO print verbose ("Message designed for all apps from a subscribed app",6)
        }

        if (d->headerMode == Header::HeaderMode::WhatWho &&
            ! (d->validAppName(header.what()) && d->validAppName(header.who())) )
        {
            qWarning() << "Invalid app or zone name what=" << header.what() << ", who=" << header.who();

            return;
        }
        else if (d->headerMode == Header::HeaderMode::WhatWhoWhere &&
                 ! (d->validAppName(header.what()) && d->validAppName(header.who()) && d->validZone(header.where())) )
        {
            qWarning() << "Invalid app or zone name what=" << header.what() << ", who=" << header.who() << ",where=" << header.where();

            return;
        }

        if (safeMessage)
        {
            emit signalMessageReceived(header, message);
        }
    }
}

bool CommunicationManager::subscribe(const QString& who, QString where)
{
    // TODO print verbose: self.app().vrb("APP.com.subscribe({},{})".format(who, where),6)
    if (d->headerMode == Header::HeaderMode::What)
    {
        qWarning() << "No subscriptions in what mode";

        return true;
    }

    if (where.isEmpty() || d->headerMode == Header::HeaderMode::WhatWho)
    {
        where = Header::localHost;
    }

    if (d->validAppName(who) && d->validZone(where))
    {
        // subscribe
        if (d->subscriptions[who].contains(where))
        {
            qWarning() << "App" << who << "already subscribed on zone" << where;

            return false;
        }

        d->subscriptions[who].append(where);

        qDebug() << "Subscriptions:" << d->subscriptions;

        return true;
    }

    return false;
}

bool CommunicationManager::subscribeLocalHost(const QString& who)
{
    return subscribe(who, Header::localHost);
}
bool CommunicationManager::subscribeAir(const QString& who)
{
    return subscribe(who, Header::airHost);
}

bool CommunicationManager::subscribeAll(const QString& who)
{
    return subscribe(who, Header::allHost);
}

void CommunicationManager::unsubscribe(const QString& who, QString where)
{
    if (d->headerMode == Header::HeaderMode::What)
    {
        qWarning() << "No subscriptions in what mode";

        return;
    }

    if (where.isEmpty() || d->headerMode == Header::HeaderMode::WhatWho)
    {
        where = Header::localHost;
    }

    if (d->subscriptions.contains(who))
    {
        d->subscriptions[who].removeOne(where);

        if (d->subscriptions[who].isEmpty())
        {
            d->subscriptions.remove(who);
        }
    }
}

void CommunicationManager::unsubscribeLocalHost(const QString& who)
{
    unsubscribe(who, Header::localHost);
}

void CommunicationManager::unsubscribeAir(const QString& who)
{
    unsubscribe(who, Header::airHost);
}

void CommunicationManager::unsubscribeAll(const QString& who)
{
    unsubscribe(who, Header::allHost);
}

bool CommunicationManager::isSubscribed(const QString& who, QString where) const
{
    if (d->headerMode == Header::HeaderMode::WhatWho)
    {
        // TODO: print verbose vrb("Impossible to subscribe apps on zone AIR in mode whatwho. Zone is {}".format(self.hst_lch()),6)
        where = Header::localHost;
    }

    if (d->subscriptions.contains(who))
    {
        return d->subscriptions[who].contains(where);
    }

    return false;
}

}
