

#ifndef SERVICES_P_H
#define SERVICES_P_H

#include <QDebug>
#include <QHash>
#include <QMutex>
#include <QObject>
#include <QStringList>

#include "IPlugin.h"
#include "ServiceRegistration.h"

namespace ExtensionSystem
{
namespace Internal
{
class PLUGINSYSTEM_EXPORT ObjectFactory
{
  public:
    template <typename T> static void registerClass()
    {
        constructors().insert(T::staticMetaObject.className(), &constructorHelper<T>);
        qDebug() << "maps count:" << constructors().count() << Qt::endl;
    }

    static void *createObject(const QByteArray &className);

  private:
    typedef void *(*Constructor)();

    template <typename T> static void *constructorHelper()
    {
        return new T();
    }

    static QHash<QByteArray, Constructor> &constructors();
};
class PLUGINSYSTEM_EXPORT ServicesManger
{

  public:
    static QMutex mutex;

    /**
     * Creates a new ctkDictionary object containing <code>in</code>
     * with the keys converted to lower case.
     *
     * @param classes A list of class names which will be added to the
     *        created ctkDictionary object under the key
     *        PluginConstants::OBJECTCLASS.
     * @param sid A service id which will be used instead of a default one.
     */
    static QHash<QString, QVariant> createServiceProperties(const QHash<QString, QVariant> &in,
                                                            const QStringList &classes = QStringList(), long sid = -1);

    /**
     * All registered services in the current framework.
     * Mapping of registered service to class names under which
     * the service is registerd.
     */
    static QMap<ServiceRegistration, QStringList> services;

    /**
     * Mapping of classname to registered service.
     * The List of registered services are ordered with the highest
     * ranked service first.
     */
    static QHash<QString, QList<ServiceRegistration>> classServices;

    ServicesManger();

    ~ServicesManger();

    static void clear();

    /**
     * Register a service in the framework wide register.
     *
     * @param plugin The plugin registering the service.
     * @param classes The class names under which the service can be located.
     * @param service The service object.
     * @param properties The properties for this service.
     * @return A ctkServiceRegistration object.
     * @exception ctkInvalidArgumentException If one of the following is true:
     * <ul>
     * <li>The service object is 0.</li>
     * <li>The service parameter is not a ctkServiceFactory or an
     * instance of all the named classes in the classes parameter.</li>
     * </ul>
     */
    static ServiceRegistration registerService(IPlugin *plugin, const QStringList &classes, QObject *service,
                                               const QHash<QString, QVariant> &properties = QHash<QString, QVariant>());

    template <typename T>
    static ServiceRegistration registerService(IPlugin *plugin, const QStringList &classes, QObject *service,
                                               const QHash<QString, QVariant> &properties = QHash<QString, QVariant>())
    {
        ObjectFactory::registerClass<T>();
        return registerService(plugin, classes, service, properties);
    }

    static QObject *createServiceObj(const QString &clazz,
                                     const QHash<QString, QVariant> &properties = QHash<QString, QVariant>());
    template <typename T>
    static T *createServiceObj(const QString &clazz,
                               const QHash<QString, QVariant> &properties = QHash<QString, QVariant>())
    {
        return qobject_cast<T *>(createServiceObj(clazz, properties));
    }
    /**
     * Checks that a given service object is an instance of the given
     * class name.
     *
     * @param service The service object to check.
     * @param cls     The class name to check for.
     */
    static bool checkServiceClass(QObject *service, const QString &cls);

    static QObject *getService(const QString &clazz,
                               const QHash<QString, QVariant> &properties = QHash<QString, QVariant>());
    template <typename T>
    static T *getService(const QString &clazz, const QHash<QString, QVariant> &properties = QHash<QString, QVariant>())
    {
        return qobject_cast<T *>(getService(clazz, properties));
    }

    /**
     * Get all services implementing a certain class.
     * Only used internally by the framework.
     *
     * @param clazz The class name of the requested service.
     * @return A sorted list of {@link ctkServiceRegistrationPrivate} objects.
     */
    static QList<ServiceRegistration> get(const QString &clazz);

    /**
     * Remove a registered service.
     *
     * @param sr The ctkServiceRegistration object that is registered.
     */
    static void removeServiceRegistration(const ServiceRegistration &sr);

    /**
     * Get all services that a plugin has registered.
     *
     * @param p The plugin
     * @return A set of {@link ctkServiceRegistration} objects
     */
    static QList<ServiceRegistration> getRegisteredByPlugin(IPlugin *p);
};
} // namespace Internal
} // namespace ExtensionSystem

#endif // CTKSERVICES_P_H
