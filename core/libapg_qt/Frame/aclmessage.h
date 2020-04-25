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

class ACLMessage : public Message
{
public:

    // standard FIPA ACL Message Perfomative
    static const QString REQUEST;
    static const QString INFORM;
    static const QString QUERY_IF;
    static const QString REFUSE;
    static const QString CONFIRM;
    static const QString UNKNOWN;

    // custom Distributed perfomative
    static const QString REQUEST_SNAPSHOT;
    static const QString INFORM_STATE;
    static const QString PREPOST_MESSAGE;

public:

    ACLMessage(const QString& perfomative);

    ~ACLMessage();

public:

    void setContent(const QJsonObject& content);
    void setTimeStamp(const VectorClock& clock);

    VectorClock* getTimeStamp() const;
    QJsonObject getContent() const;
};

}
#endif // ACLMESSAGE_H
