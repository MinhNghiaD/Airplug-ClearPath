#ifndef COMMUNICATION_MANAGER_H
#define COMMUNICATION_MANAGER_H

#include <QObject>

#include "src/Transporters/message_transporter.h"
#include "src/Frame/header.h"
#include "src/Frame/message.h"

namespace AirPlug
{

class CommunicationManager : public QObject
{
    Q_OBJECT
public:

    enum ProtocolType
    {
        StandardIO = 0,
        UDP,
    };


public:

    CommunicationManager(Header::HeaderMode mode = Header::HeaderMode::What,
                         QObject* parent         = nullptr);

    ~CommunicationManager();

    void addStdTransporter();
    void addUdpTransporter(const QString& host,
                           int port,
                           MessageTransporter::UdpType type = MessageTransporter::UdpType::UniCast);

    void setHeaderMode(Header::HeaderMode mode);

    void send(const Message& message,
              const QString& what     = QString(),
              const QString& who      = QString(),
              const QString& where    = QString(),
              ProtocolType   protocol = ProtocolType::StandardIO,
              bool           save     = true);

    void setSafeMode(bool b);

    bool subscribe(const QString& who, QString where = QString());
    bool subscribeLocalHost(const QString& who);
    bool subscribeAir(const QString& who);
    bool subscribeAll(const QString& who);

    void unsubscribe(const QString& who, QString where);
    void unsubscribeLocalHost(const QString& who);
    void unsubscribeAir(const QString& who);
    void unsubscribeAll(const QString& who);

    bool isSubscribed(const QString& who, QString where) const;

public:

    Q_SIGNAL void signalMessageReceived(const Header& header, const Message& message);

private:

    Q_SLOT void slotReceiveMessage(const QString& data);


private:

    class Private;
    Private* d;
};

}
#endif // COMMUNICATION_MANAGER_H
