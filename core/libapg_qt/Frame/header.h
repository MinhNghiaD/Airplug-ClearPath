#ifndef HEADER_H
#define HEADER_H

// Qt includes
#include <QString>

namespace AirPlug
{

class Header
{
public:

    static const QString allApp;     //app_all

    static const QString localHost;  //hst_lch
    static const QString airHost;    //hst_air
    static const QString allHost;    //hst_all

    enum HeaderMode
    {
        What = 0,
        WhatWho,
        WhatWhoWhere,
    };

public:

    // Use for construct header to send message
    Header(const QString& what  = QString(),
           const QString& who   = QString(),
           const QString& where = QString());

    // use for reconstruct header from received message
    Header(HeaderMode     mode,
           bool           safeMode,
           const QString& what,
           const QString& who,
           const QString& where);

    ~Header();

    void clear();

    bool isEmpty() const;

    QString generateHeader(HeaderMode mode = HeaderMode::What) const;

    const QString& what()  const;
    const QString& who()   const;
    const QString& where() const;

private:

    class Private;
    Private* d;
};

}
#endif // HEADER_H
