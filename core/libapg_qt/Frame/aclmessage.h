/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-25
 * Description : simplify version of FIPA ACL Message in Multi-agent system
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef ACLMESSAGE_H
#define ACLMESSAGE_H

// libapg include
#include "message.h"
#include "vector_clock.h"

namespace AirPlug
{

/**
 * @brief The ACLMessage class : organize AirPlug::Message with the structure of FIPA ACL Message
 * The general form of the message will be as example below:
 * {
 *  "performative" : QString,
 *  "timestamp"    : VectorClock
 *  "content"      : JsonObject;
 * }
 */

class ACLMessage : public Message
{
public:

    enum Performative
    {
        // standard FIPA ACL Message Perfomative
        REQUEST = 0,
        INFORM,
        QUERY_IF,
        REFUSE,
        CONFIRM,
        UNKNOWN,
        PING,
        PONG,

        // custom Distributed perfomative
        REQUEST_SNAPSHOT,
        INFORM_STATE,
        PREPOST_MESSAGE,
        SNAPSHOT_RECOVER,
        READY_SNAPSHOT,
        REQUEST_MUTEX,
        ACCEPT_MUTEX,
        REFUSE_MUTEX,
    };

public:

    ACLMessage(Performative performative);
    ACLMessage(const QString& message);

    ~ACLMessage();

public:

    void setPerformative(Performative performative);
    void setContent(const QJsonObject& content);
    void setTimeStamp(const VectorClock& clock);

    // siteID of sender's NET
    void setSender(const QString& siteID);
    void setNbSequence(int nbSequence);

    Performative getPerformative() const;
    VectorClock* getTimeStamp()    const;
    QJsonObject  getContent()      const;
    QString      getSender()       const;
    int          getNbSequence()   const;

    QJsonObject  toJsonObject()    const;
};

}
#endif // ACLMESSAGE_H
