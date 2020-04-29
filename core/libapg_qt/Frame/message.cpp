#include "message.h"

//Qt includes

#include <QVector>
#include <QStringList>
#include <QDebug>

namespace AirPlug
{

class Q_DECL_HIDDEN Message::Private
{
public:

    Private()
        : delimiter('^'),
          equalChar('~')
    {
    }

    ~Private()
    {
    }

public:

    char                    delimiter;
    char                    equalChar;

    QHash<QString, QString> content;
};

Message::Message(const QString& message)
    : d(new Private())
{
    if (! message.isEmpty())
    {
        parseText(message);
    }
}

Message::Message(const QHash<QString, QString>& contents)
    : d(new Private())
{
    if (! contents.isEmpty())
    {
        d->content = contents;
        d->content.detach();
    }
}

Message::Message(const Message& otherMessage)
    : d(new Private())
{
    d->content = QHash<QString, QString>(otherMessage.getContents());
    d->content.detach();

    d->delimiter = otherMessage.d->delimiter;
    d->equalChar = otherMessage.d->equalChar;
}

Message::~Message()
{
    delete d;
}

void Message::operator = (const Message& otherMessage)
{
    d->content = QHash<QString, QString>(otherMessage.getContents());
    d->content.detach();

    d->delimiter = otherMessage.d->delimiter;
    d->equalChar = otherMessage.d->equalChar;
}

void Message::parseText(const QString& text)
{
    d->content.clear();

    // TODO: print verbose
    QStringList msg = text.split(d->delimiter, QString::SkipEmptyParts);

    //qDebug() << "splited message " << msg;

    for (QStringList::const_iterator iter  = msg.constBegin();
                                     iter != msg.constEnd();
                                     ++iter)
    {
        QStringList pairs = (*iter).split(d->equalChar, QString::SkipEmptyParts);

        if (pairs.length() > 1)
        {
            QStringList value;

            for (int i = 1; i < pairs.length(); ++i)
            {
                value << pairs[i];
            }

            d->content.insert(pairs[0], value.join(d->equalChar));
        }
    }

    //qDebug() << "Content of parsed message: " << d->content;
}

void Message::parseTextWithKnownFields(const QString& text)
{
    //TODO: print verbose "apg.msg.parse_text_with_known_fields(text={})".format(txt),6
    QStringList msg = text.split(d->delimiter, QString::SkipEmptyParts);

    int i = 0;

    while (i < msg.length())
    {
        if (isKnownField(msg[i]))
        {
            ++i;
        }
        else if (i > 0)
        {
            msg[i - 1] += (d->delimiter + msg[i]);

            msg.removeAt(i);
        }
        else
        {
            ++i;
        }
    }

    for (QStringList::const_iterator iter = msg.constBegin();
         iter != msg.constEnd();
         ++iter)
    {
        QStringList pairs = (*iter).split(d->equalChar, QString::SkipEmptyParts);

        if (pairs.length() > 1)
        {
            QStringList value;

            for (int i = 1; i < pairs.length(); ++i)
            {
                value << pairs[i];
            }

            d->content.insert(pairs[0], value.join(d->equalChar));
        }
    }

    //qDebug() << "Content of parsed message with known field: " << d->content;
}

bool Message::isKnownField(const QString& text) const
{
    //TODO: print verbose "apg.msg.is_field(text={})".format(text),5)
    QList<QString> fields = d->content.keys();

    for (QList<QString>::const_iterator iter  = fields.constBegin();
                                        iter != fields.constEnd();
                                        ++iter)
    {
        int index = text.indexOf(*iter, 0, Qt::CaseInsensitive);

        if (index >= 0 && text[index + iter->length()] == d->equalChar)
        {
            //TODO: print verbose self.app().vrb("{} is a field of the message {}".format(text, self.init_text), 6)
            return true;
        }
    }

    //TODO: print verbose "{} is a not a field of the message {}".format(text, self.init_text), 6)
    return false;
}

void Message::addContent(const QString& key, const QString& value)
{
     d->content[key] = value;
}

QHash<QString, QString> Message::getContents() const
{
    return d->content;
}

bool Message::isEmpty() const
{
    return d->content.isEmpty();
}

void Message::clear()
{
    d->content.clear();
}

QString Message::getMessage() const
{
    QString msg;

    msg += d->delimiter;

    for (QHash<QString, QString>::const_iterator iter = d->content.constBegin();
         iter != d->content.constEnd();
         ++iter)
    {
        msg += (iter.key() + d->equalChar + iter.value() + d->delimiter);
    }

    return msg;
}

}
