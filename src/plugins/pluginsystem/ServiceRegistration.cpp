
#include <QMutex>

#include "ServiceRegistration.h"

#include <stdexcept>
namespace ExtensionSystem {
    namespace Internal {
        ServiceRegistration::ServiceRegistration()
        {

        }
        ServiceRegistration::ServiceRegistration(IPlugin* plugin, QObject* service,QString className,const QHash<QString, QVariant>& props)
        {
            this->service = service;
            this->plugin = plugin;
            this->properties = props;
            this->impClassName = className;
        }
        ServiceRegistration::~ServiceRegistration()
        {
        }
        bool ServiceRegistration::operator==(const ServiceRegistration& registration) const
        {
            return (plugin == registration.plugin) && (service == registration.service);

        }
        bool ServiceRegistration::operator<(const ServiceRegistration& o) const
        {
            return &service<=&(o.service);
        }
    }
}