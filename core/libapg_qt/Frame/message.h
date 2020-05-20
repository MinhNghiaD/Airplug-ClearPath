/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-14
 * Description : payload of a package
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef MESSAGE_H
#define MESSAGE_H

// Qt includes
#include <QString>
#include <QHash>

#include <QMetaType>

namespace AirPlug
{

class Message
{
public:

    explicit Message(const QString& message = QString());

    Message(const QHash<QString, QString>& contents);

    //Copied constructor
    Message(const Message& otherMessage);

    ~Message();

public:

    void operator = (const Message& otherMessage);

    /**
     * @brief parseText parse a text message in to Message Object
     * @param text
     */
    void parseText(const QString& text);

    /**
     * @brief parseTextWithKnownFields: parse a string message while keeping old contents
     * @param text
     */
    void parseTextWithKnownFields(const QString& text);

    /**
     * @brief isKnownField: check of a key-value message contain a known key
     * @param pair
     * @return
     */
    bool isKnownField(const QString& pair) const;

    void addContent(const QString& key, const QString& value);

    /**
     * @brief getContents: return QHash of message content
     * @return
     */
    QHash<QString, QString> getContents() const;

    bool isEmpty() const;

    void clear();

    /**
     * @brief getMessage: convert to String message with proper delimitor
     * @return
     */
    QString getMessage() const;

private:

    class Private;
    Private* d;
};

}

//Q_DECLARE_METATYPE(AirPlug::Message)
#endif // MESSAGE_H
