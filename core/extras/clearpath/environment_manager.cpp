#include "environment_manager.h"

namespace ClearPath
{

bool EnvironmentManager::setInfo(const QString& name, const QJsonObject& info)
{
    /*
     * How is encoded info :
        info[QLatin1String("position")] = encodeVector(position);
        info[QLatin1String("velocity")] = encodeVector(velocity);
        info[QLatin1String("maxSpeed")] = maxSpeed;
    */
    return true;
}

}
