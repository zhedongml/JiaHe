
#ifndef SERVICEREGISTRATION_H
#define SERVICEREGISTRATION_H
#include "pluginsystem_global.h"
#include "iplugin.h"


namespace ExtensionSystem {
	namespace Internal {
		class PLUGINSYSTEM_EXPORT ServiceRegistration {
		public:
			ServiceRegistration();
			ServiceRegistration(IPlugin* plugin, QObject* service, QString className,
				const QHash<QString, QVariant>& props);
			~ServiceRegistration();
			QHash<QString, QVariant> properties;
			IPlugin* plugin;
			QObject* service;
			bool operator==(const ServiceRegistration& registration) const;
			bool operator<(const ServiceRegistration& o) const;
			QString impClassName ="";
		};
	}
}
#endif // CTKSERVICEREGISTRATION_H
