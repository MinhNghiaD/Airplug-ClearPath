#include "header.h"

#include <QCoreApplication>

namespace AirPlug
{
const QString Header::allApp     = QLatin1String("ALL");  //app_all

const QString Header::localHost  = QLatin1String("LCH");  //hst_lch
const QString Header::airHost    = QLatin1String("AIR");  //hst_air
const QString Header::allHost    = QLatin1String("ALL");  //hst_all

class Q_DECL_HIDDEN Header::Private
{
public:

    Private()
        : delimiter('\x07')
    {
    }

    ~Private()
    {
    }

public:

    QString what;
    QString who;
    QString where;

    char    delimiter;
};

Header::Header(const QString& what,
               const QString& who,
               const QString& where)
    : d(new Private())
{
    d->what  = what;
    d->who   = who;
    d->where = where;
}

Header::Header(const Header& otherheader)
    : d(new Private())
{
    d->what      = otherheader.what();
    d->who       = otherheader.where();
    d->where     = otherheader.who();

    d->delimiter = otherheader.d->delimiter;
}

Header::~Header()
{
    delete d;
}

void Header::operator = (const Header& otherheader)
{
    d->what      = otherheader.what();
    d->who       = otherheader.where();
    d->where     = otherheader.who();

    d->delimiter = otherheader.d->delimiter;
}

void Header::clear()
{
    d->what.clear();
    d->who.clear();
    d->where.clear();
}

bool Header::isEmpty() const
{
    return (d->what.isEmpty() && d->who.isEmpty() && d->where.isEmpty());
}

const QString& Header::what() const
{
    return d->what;
}

const QString& Header::who() const
{
    return d->who;
}

const QString& Header::where() const
{
    return d->where;
}

QString Header::generateHeader(HeaderMode mode) const
{
    switch (mode)
    {
        case HeaderMode::WhatWhoWhere:
            return (d->delimiter + d->what  +
                    d->delimiter + d->who   +
                    d->delimiter + d->where +
                    d->delimiter);

        case HeaderMode::WhatWho:
            return (d->delimiter + d->what +
                    d->delimiter + d->who  +
                    d->delimiter);

        default:
            return QString();
    }
}

}
