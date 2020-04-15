#include "message.h"

//Qt includes
#include <QHash>
#include <QVector>
#include <QStringList>
#include <QDebug>

namespace AirPlug
{

class Message::Private
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

    QString                 initText;
    char                    delimiter;
    char                    equalChar;

    QHash<QString, QString> content;

    QVector<QString>        fields;
};

Message::Message(const QString& initText)
    : d(new Private())
{
    d->initText = initText;
}

Message::~Message()
{
    delete d;
}

void Message::parseText(const QString& text)
{
    d->content.clear();

    // TODO: print verbose
    QStringList msg = text.split(d->delimiter, QString::SkipEmptyParts);

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

    //qDebug() << d->content;
}

void Message::parseTextWithKnownFields(const QString& text)
{
    d->content.clear();

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
}

bool Message::isKnownField(const QString& text) const
{
    //TODO: print verbose "apg.msg.is_field(text={})".format(text),5)
    for (QVector<QString>::const_iterator iter = d->fields.constBegin();
         iter != d->fields.constEnd();
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
     d->content.insert(key, value);
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
