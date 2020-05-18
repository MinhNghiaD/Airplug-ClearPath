#include "communication_manager.h"

//Qt include
#include <QCoreApplication>
#include <QChar>
#include <QDebug>
#include <QMap>
#include <QStringList>
#include <QThread>

// Local includes
#include "std_transporter.h"
#include "udp_transporter.h"

namespace AirPlug
{

class Q_DECL_HIDDEN CommunicationManager::Private
{
public:

    Private()
        : what(QCoreApplication::applicationName().toUpper()),
          who(what),
          where(Header::airHost),
          safeMode(false)
    {
    }

    ~Private()
    {
        protocols.clear();
        subscriptions.clear();
    }

public:

    bool validAppName  (const QString& name)   const;
    bool validZone     (const QString& zone)   const;

    bool isAlphaNumeric(const QString& string) const;

public:

    Header::HeaderMode headerMode;
    QString            what;
    QString            who;
    QString            where;
    bool               safeMode;

    // Map who and wheres
    QMap<QString, QStringList>     subscriptions;
    QMap<int, MessageTransporter*> protocols;

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

CommunicationManager::CommunicationManager(const QString&     what,
                                           const QString&     who,
                                           const QString&     where,
                                           Header::HeaderMode mode,
                                           QObject*           parent)
    : QObject(parent),
      d(new Private())
{
    setObjectName(QLatin1String("Communication manager"));

    d->headerMode = mode;

    // default information
    if (!what.isEmpty())
    {
        d->what = what;
    }

    if (!who.isEmpty())
    {
        d->who = who;
    }

    if (!where.isEmpty())
    {
        d->where = where;
    }
}

CommunicationManager::~CommunicationManager()
{
    delete d;
}

void CommunicationManager::addStdTransporter()
{
    MessageTransporter* transporter        = new StdTransporter(this);
    d->protocols[ProtocolType::StandardIO] = transporter;

    connect(transporter, &MessageTransporter::signalMessageReceived,
            this,        &CommunicationManager::slotReceiveMessage, Qt::DirectConnection);

}

void CommunicationManager::addUdpTransporter(const QString& host, int port,
                                             MessageTransporter::UdpType type)
{
    MessageTransporter* transporter = new UdpTransporter(host, port, type, this);
    d->protocols[ProtocolType::UDP] = transporter;

    connect(transporter, &MessageTransporter::signalMessageReceived,
            this,        &CommunicationManager::slotReceiveMessage, Qt::DirectConnection);
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

        Header header   = fillSendingHeader(what, who, where);
        QString package = header.generateHeader(d->headerMode) + message.getMessage();

        // TODO: multiple data is put to pipe at the same time ==> only 1 signal emit and 1 message is handled
        QThread::msleep(30);
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

    if (d->headerMode == Header::HeaderMode::WhatWho      ||
        d->headerMode == Header::HeaderMode::WhatWhoWhere)
    {
        // Decode package
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
    }
    else
    {
        payload = data;
    }

    if (filterMessage(what, who, where))
    {
        Header header = recoverReceivedHeader(what, who, where);

        if (! validHeader(header))
        {
            return;
        }

        Message message(payload);

        // TODO: print verbose vrb("transmitting (if safe) to application {} txt '{}', from {}, to {} from zone {}".format(self.app(), text, header["what"], header["who"], header["where"]), 6)

        emit signalMessageReceived(header, message);
    }
    else
    {
        //qWarning() << d->what << ": Message is not belong to this host ----> drop message: " << data;
    }
}


bool CommunicationManager::subscribe(const QString& who, QString where)
{
    // TODO print verbose: self.app().vrb("APP.com.subscribe({},{})".format(who, where),6)
    if (d->headerMode == Header::HeaderMode::What)
    {
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


Header CommunicationManager::fillSendingHeader(QString what, QString who, QString where) const
{
    if (what.isEmpty())
    {
       what = d->what;
    }

    if (who.isEmpty())
    {
        who = d->who;
    }

    if (where.isEmpty())
    {
        where = d->where;
    }

    return Header(what, who, where);
}

Header CommunicationManager::recoverReceivedHeader(QString what, QString who, QString where) const
{
    switch (d->headerMode)
    {
        case Header::HeaderMode::WhatWhoWhere:
            if (what.isEmpty())
            {
                if (d->safeMode)
                {
                    qFatal("non-conform header syntax, safemode option enabled, stopping app");
                }
                else
                {
                    what = d->what;
                }
            }

            if(who.isEmpty())
            {
                if (d->safeMode)
                {
                    qFatal("non-conform header syntax, safemode option enabled, stopping app");
                }
                else
                {
                    who = d->who;
                }
            }

            if(where.isEmpty())
            {
                if (d->safeMode)
                {
                    qFatal("non-conform header syntax, safemode option enabled, stopping app");
                }
                else
                {
                    d->where = where;
                }
            }

            break;

        case Header::HeaderMode::WhatWho:
            if (what.isEmpty())
            {
                if (d->safeMode)
                {
                    qFatal("non-conform header syntax, safemode option enabled, stopping app");
                }
                else
                {
                    what = d->what;
                }
            }

            if(who.isEmpty())
            {
                if (d->safeMode)
                {
                    qFatal("non-conform header syntax, safemode option enabled, stopping app");
                }
                else
                {
                    who = d->who;
                }
            }

            where.clear();

            break;

        default:
            if (what.isEmpty())
            {
                what = d->what;
            }

            who.clear();
            where.clear();
    }

    return Header(what, who, where);
}

bool CommunicationManager::filterMessage(const QString& what, const QString& who, const QString& where) const
{
    bool safeMessage = false;

    if (d->headerMode == Header::HeaderMode::What)
    {
        safeMessage = true;
        // TODO: print verbose vrb("Every message is accepted in what mode",6)
    }
    else if (who == d->what)
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
    else if (who == Header::allApp && isSubscribed(what, where))
    {
        safeMessage = true;
        // TODO print verbose ("Message designed for all apps from a subscribed app",6)
    }

    return safeMessage;
}

bool CommunicationManager::validHeader(const Header& header) const
{
    if (d->headerMode == Header::HeaderMode::WhatWho &&
        ! (d->validAppName(header.what()) && d->validAppName(header.who())) )
    {
        qWarning() << "Invalid app or zone name what=" << header.what() << ", who=" << header.who();

        return false;
    }
    else if (d->headerMode == Header::HeaderMode::WhatWhoWhere &&
             ! (d->validAppName(header.what()) && d->validAppName(header.who()) && d->validZone(header.where())) )
    {
        qWarning() << "Invalid app or zone name what=" << header.what() << ", who=" << header.who() << ", where=" << header.where();

        return false;
    }

    return true;
}

}
