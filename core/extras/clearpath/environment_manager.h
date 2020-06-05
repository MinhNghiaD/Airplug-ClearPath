/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-6-4
 * Description : Environment controller of clearpath agents
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef ENVIRONMENT_MANAGER_H
#define ENVIRONMENT_MANAGER_H

#include <QString>
#include <QJsonObject>

namespace ClearPath
{

class EnvironmentManager
{
public:
    // Prototype
    EnvironmentManager();


    // TODO get Agent according to name and call setInfo:
    // This function use for updating information of distance agents
    bool setInfo(const QString& name, const QJsonObject& info);
};

}
#endif // ENVIRONMENT_MANAGER_H
