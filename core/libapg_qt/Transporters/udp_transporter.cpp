#include "udp_transporter.h"

// Qt include
#include <QUdpSocket>
#include <QHostAddress>
#include <QNetworkDatagram>
#include <QByteArray>

#include <QDebug>

namespace AirPlug
{

class Q_DECL_HIDDEN UdpTransporter::Private
{
public:

    Private()
        : socket(nullptr),
          host(QHostAddress::LocalHost),
          port(7777)
    {
    }

    ~Private()
    {
        socket->close();
        delete socket;
    }

public:

    QUdpSocket*  socket;
    QHostAddress host;
    int          port;
};

UdpTransporter::UdpTransporter(const QString& host,
                               int            port,
                               UdpType        type,
                               QObject*       parent)
    : MessageTransporter(parent),
      d(new Private())
{
    setObjectName(QLatin1String("UDP Protocol"));

    d->socket = new QUdpSocket();
    d->host   = QHostAddress(host);
    d->port   = port;

    switch (type)
    {
        case MultiCast:
            if (! d->socket->bind(d->host, d->port, QUdpSocket::ShareAddress))
            {
                qFatal("Network error. Aborting ...");
            }
            else
            {
                d->socket->joinMulticastGroup(d->host);
                qInfo() << "UDP Multicast listener binds on" << d->host << ":" << d->port;
            }

            break;

        default:
            // Unicast
            if (! d->socket->bind(d->host, d->port, QUdpSocket::ShareAddress))
            {
                qFatal("Network error. Aborting ...");
            }
            else
            {
                qInfo() << "UDP Unicast listener binds on" << d->host << ":" << d->port;
            }

            break;
    }

    connect(d->socket, SIGNAL(readyRead()),
            this, SLOT(slotMessageArrive()));

    connect(d->socket, SIGNAL(disconnected()),
            this, SIGNAL(signalDisconnected()));
}

UdpTransporter::~UdpTransporter()
{
    delete d;
}

void UdpTransporter::send(const QString& message)
{
    d->socket->writeDatagram(message.toUtf8(), d->host, d->port);
}

void UdpTransporter::slotMessageArrive()
{
    while (d->socket->hasPendingDatagrams())
    {
        QByteArray data =  d->socket->receiveDatagram().data();

        emit signalMessageReceived(QString::fromLatin1(data));
    }
}

}
