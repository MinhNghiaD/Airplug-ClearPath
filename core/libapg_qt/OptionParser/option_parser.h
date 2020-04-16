/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-16
 * Description : command line parser
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef OPTION_PARSER_H
#define OPTION_PARSER_H

// Qt includes
#include <QCoreApplication>

// Local includes
#include "header.h"

namespace AirPlug
{


class OptionParser
{
public:
    OptionParser(const QCoreApplication& app);

    void showOption() const;

public:

    bool start;
    bool debug;
    bool nogui;
    bool save;
    bool safemode;
    bool autoSend;

    int  verbose;
    int  delay;

    QString mode;
    QString ident;

    QString source;
    QString destination;

    QString remoteHost;
    int  remotePort;
    bool remote;

    Header::HeaderMode headerMode;
};

}
#endif // OPTION_PARSER_H
