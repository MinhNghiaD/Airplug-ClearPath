#ifndef MESSAGE_H
#define MESSAGE_H

// Qt includes
#include <QString>

namespace AirPlug
{

class Message
{
public:

    Message(const QString& initText = QString());
    ~Message();

public:

    /**
     * @brief parseText parse a text message in to Message Object
     * @param text
     */
    void parseText(const QString& text);

    void parseTextWithKnownFields(const QString& text);

    /**
     * @brief isKnownField: check of a key-value message contain a known key
     * @param pair
     * @return
     */
    bool isKnownField(const QString& pair) const;

    void addContent(const QString& key, const QString& value);

    bool isEmpty() const;

    void clear();

    QString getMessage() const;

private:
    class Private;
    Private *d;
};

}
#endif // MESSAGE_H
