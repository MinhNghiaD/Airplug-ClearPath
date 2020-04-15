#include "header.h"

#include <QCoreApplication>

namespace AirPlug
{
const QString Header::allApp     = QLatin1String("ALL");  //app_all

const QString Header::localHost  = QLatin1String("LCH");  //hst_lch
const QString Header::airHost    = QLatin1String("AIR");  //hst_air
const QString Header::allHost    = QLatin1String("ALL");  //hst_all

class Header::Private
{
public:

    Private()
        : what(QCoreApplication::applicationName().toUpper()),
          who(what),
          where(Header::airHost),
          delimiter('\x07')
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

Header::Header(HeaderMode mode,
               bool safeMode,
               const QString& what,
               const QString& who,
               const QString& where)
    : d(new Private())
{
    switch (mode)
    {
        case WhatWhoWhere:
            if (what.isEmpty())
            {
                if (safeMode)
                {
                    qFatal("non-conform header syntax, safemode option enabled, stopping app");
                }
            }
            else
            {
                d->what = what;
            }

            if(who.isEmpty())
            {
                if (safeMode)
                {
                    qFatal("non-conform header syntax, safemode option enabled, stopping app");
                }
            }
            else
            {
                d->who = who;
            }

            if(where.isEmpty())
            {
                if (safeMode)
                {
                    qFatal("non-conform header syntax, safemode option enabled, stopping app");
                }
            }
            else
            {
                d->where = where;
            }

            break;

        case WhatWho:
            if (what.isEmpty())
            {
                if (safeMode)
                {
                    qFatal("non-conform header syntax, safemode option enabled, stopping app");
                }
            }
            else
            {
                d->what = what;
            }

            if(who.isEmpty())
            {
                if (safeMode)
                {
                    qFatal("non-conform header syntax, safemode option enabled, stopping app");
                }
            }
            else
            {
                d->who = who;
            }

            d->where.clear();

            break;

        default:
            if (! what.isEmpty())
            {
                d->what = what;
            }

            d->who.clear();
            d->where.clear();

            break;
    }
}

Header::~Header()
{
    delete d;
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
