#include "option_parser.h"

// Qt include
#include <QCommandLineParser>
#include <QString>
#include <QStringList>
#include <QDebug>

namespace AirPlug
{

OptionParser::OptionParser()
    : start(false),
      debug(false),
      nogui(true),
      save(false),
      safemode(false),
      autoSend(false),
      remote(false),
      verbose(0),
      delay(0),
      remotePort(0),
      headerMode(Header::HeaderMode::What)
{
}

OptionParser::OptionParser(const QCoreApplication& app)
{
    parseOptions(app);
}

void OptionParser::parseOptions(const QCoreApplication& app)
{
    QCommandLineParser parser;
    parser.addHelpOption();

    // auto boolean option (--auto)
    QCommandLineOption autoOption("auto", QCoreApplication::translate("main", "Start application automatically"));
    parser.addOption(autoOption);

    // debug boolean option (--debug)
    QCommandLineOption debugOption("debug", QCoreApplication::translate("main", "Show debug message"));
    parser.addOption(debugOption);

    // nogui boolean option (--nogui)
    QCommandLineOption noguiOption("nogui", QCoreApplication::translate("main", "Run without graphic interface"));
    parser.addOption(noguiOption);

    // saving boolean option (--saving)
    QCommandLineOption saveOption("saving", QCoreApplication::translate("main", "Saving log"));
    parser.addOption(saveOption);

    // safemode boolean option (--safemode)
    QCommandLineOption safemodeOption("safemode", QCoreApplication::translate("main", "Run in safe mode"));
    parser.addOption(safemodeOption);

    // autosend boolean option (--autosend)
    QCommandLineOption autosendOption("autosend", QCoreApplication::translate("main", "Send message automatically"));
    parser.addOption(autosendOption);

    // whatwho boolean option (--whatwho)
    QCommandLineOption whatwhoOption("whatwho", QCoreApplication::translate("main", "Run with whatwho message format"));
    parser.addOption(whatwhoOption);

    // whatwhowhere boolean option (--whatwhowhere)
    QCommandLineOption whatwhowhereOption("whatwhowhere", QCoreApplication::translate("main", "Run with whatwhowhere message format"));
    parser.addOption(whatwhowhereOption);



    // verbose option with value (--verbose)
    QCommandLineOption verboseOption("verbose",
                                     QCoreApplication::translate("main", "Set verbose level"),
                                     QCoreApplication::translate("main", "verbose-level"));
    parser.addOption(verboseOption);

    // remote option with value (--remote)
    QCommandLineOption remoteOption("remote",
                                     QCoreApplication::translate("main", "Connect to remote host"),
                                     QCoreApplication::translate("main", "address:port"));
    parser.addOption(remoteOption);

    // mode option with value (--mode)
    QCommandLineOption modeOption("mode",
                                   QCoreApplication::translate("main", "Mode of execution"),
                                   QCoreApplication::translate("main", "mode"));
    parser.addOption(modeOption);

    // ident option with value (--ident)
    QCommandLineOption identOption("ident",
                                   QCoreApplication::translate("main", "Node identification"),
                                   QCoreApplication::translate("main", "id"));
    parser.addOption(identOption);

    // delay option with value (--delay)
    QCommandLineOption delayOption("delay",
                                   QCoreApplication::translate("main", "Period of sending message in ms"),
                                   QCoreApplication::translate("main", "period"));
    parser.addOption(delayOption);

    // source option with value (--source)
    QCommandLineOption sourceOption("source",
                                    QCoreApplication::translate("main", "Source application"),
                                    QCoreApplication::translate("main", "app"));
    parser.addOption(sourceOption);

    // dest option with value (--dest)
    QCommandLineOption destOption("dest",
                                  QCoreApplication::translate("main", "Destination application"),
                                  QCoreApplication::translate("main", "app"));
    parser.addOption(destOption);


    // Process the actual command line arguments given by the user
    parser.process(app);

    start       = parser.isSet(autoOption);
    debug       = parser.isSet(debugOption);
    nogui       = parser.isSet(noguiOption);
    save        = parser.isSet(saveOption);
    safemode    = parser.isSet(safemodeOption);

    if (start)
    {
        autoSend= parser.isSet(autosendOption);
    }
    else
    {
        autoSend= false;
    }

    verbose     = parser.value(verboseOption).toInt();
    delay       = parser.value(delayOption).toInt();
    mode        = parser.value(modeOption);
    ident       = parser.value(identOption);
    source      = parser.value(sourceOption);
    destination = parser.value(destOption);

    QString remoteAddress   = parser.value(remoteOption);

    if (! remoteAddress.isEmpty())
    {
        QStringList host = remoteAddress.split(':');

        if (host.size() != 2)
        {
            qFatal("Remote host has to be in form <address:port>");
        }

        remote = true;

        remoteHost = host[0];
        remotePort = host[1].toInt();
    }
    else
    {
        remote = false;
    }

    headerMode = Header::HeaderMode::What;

    if (parser.isSet(whatwhoOption))
    {
        headerMode = Header::HeaderMode::WhatWho;
    }

    if (parser.isSet(whatwhowhereOption))
    {
        headerMode = Header::HeaderMode::WhatWhoWhere;
    }
}

void OptionParser::showOption() const
{
    qDebug() << "auto                :" << start;
    qDebug() << "debug               :" << debug;
    qDebug() << "noGui               :" << nogui;
    qDebug() << "saving              :" << save;
    qDebug() << "safemode            :" << safemode;
    qDebug() << "autosend            :" << autoSend;
    qDebug() << "verbose             :" << verbose;
    qDebug() << "delay               :" << delay;
    qDebug() << "mode                :" << mode;
    qDebug() << "ident               :" << ident;
    qDebug() << "source              :" << source;
    qDebug() << "destination         :" << destination;

    if (remote)
    {
        qDebug() << "remote host         :" << remoteHost;
        qDebug() << "remote port         :" << remotePort;
    }

    switch (headerMode)
    {
        case Header::HeaderMode::WhatWho :
            qDebug() << "message format      :" << "whatwho";
            break;

        case Header::HeaderMode::WhatWhoWhere :
            qDebug() << "message format      :" << "whatwhowhere";
            break;

        default:
            qDebug() << "message format      :" << "what";
    }
}

QJsonObject OptionParser::convertToJson() const
{
    QJsonObject options;

    options[QLatin1String("start")]       = start;
    options[QLatin1String("debug")]       = debug;
    options[QLatin1String("nogui")]       = nogui;
    options[QLatin1String("save")]        = save;
    options[QLatin1String("safemode")]    = safemode;
    options[QLatin1String("autoSend")]    = autoSend;
    options[QLatin1String("remote")]      = start;
    options[QLatin1String("verbose")]     = verbose;
    options[QLatin1String("delay")]       = delay;
    options[QLatin1String("mode")]        = mode;
    options[QLatin1String("ident")]       = ident;
    options[QLatin1String("source")]      = source;
    options[QLatin1String("destination")] = destination;
    options[QLatin1String("remoteHost")]  = remoteHost;
    options[QLatin1String("remotePort")]  = remotePort;

    return options;
}

}

