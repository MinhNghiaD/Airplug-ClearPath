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
#include <QJsonObject>

// Local includes
#include "header.h"

namespace AirPlug
{


class OptionParser
{
public:

    // default constructor
    OptionParser();
    explicit OptionParser(const QCoreApplication& app);

    /**
     * @brief parseOptions: parse option from application
     * @param app
     */
    void parseOptions(const QCoreApplication& app);

    /**
     * @brief showOption: display current options for debugging purpose
     */
    void showOption() const;

    /**
     * @brief convertToJson : convert application options into Json object
     * @return
     */
    QJsonObject convertToJson() const;

public:

    bool start;
    bool debug;
    bool nogui;
    bool save;
    bool safemode;
    bool autoSend;
    bool remote;

    int  verbose;
    int  delay;

    QString mode;
    QString ident;
    QString source;
    QString destination;

    QString remoteHost;
    int     remotePort;

    Header::HeaderMode headerMode;
};

}
#endif // OPTION_PARSER_H
