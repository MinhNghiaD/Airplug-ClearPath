/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-14
 * Description : header of a package
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef HEADER_H
#define HEADER_H

// Qt includes
#include <QString>
#include <QMetaType>

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

    // Default constructor
    explicit Header(const QString& what,
                    const QString& who,
                    const QString& where);

    // Copy constructor
    Header(const Header& otherheader);

    ~Header();

    void operator = (const Header& otherheader);

    /**
     * @brief clear: erase the fields of header
     */
    void clear();

    bool isEmpty() const;

    /**
     * @brief generateHeader: convert header to String format
     * @param mode
     * @return
     */
    QString generateHeader(HeaderMode mode = HeaderMode::What) const;

    const QString& what()  const;
    const QString& who()   const;
    const QString& where() const;

private:

    class Private;
    Private* d;
};

}

//Q_DECLARE_METATYPE(AirPlug::Header)
#endif // HEADER_H
