/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#include "Services.h"

#include <QStringListIterator>
#include <QMutexLocker>
#include <QBuffer>

#include <algorithm>
#include <QHashIterator>
namespace ExtensionSystem {
    namespace Internal {
        void* ObjectFactory::createObject(const QByteArray& className)
        {
            Constructor constructor = constructors().value(className);
            if (constructor == NULL)
                return NULL;
            return (*constructor)();
        };
        QHash<QByteArray, ObjectFactory::Constructor>& ObjectFactory::constructors()
        {
            static QHash<QByteArray, Constructor> instance;
            return instance;
        }
         QMap<ServiceRegistration, QStringList> ServicesManger::services;
         QMutex ServicesManger::mutex;
        /**
         * Mapping of classname to registered service.
         * The List of registered services are ordered with the highest
         * ranked service first.
         */
         QHash<QString, QList<ServiceRegistration> > ServicesManger::classServices;
         QHash<QString, QVariant> ServicesManger::createServiceProperties(const QHash<QString, QVariant>& in,
             const QStringList& classes,
             long sid)
         {
             static qlonglong nextServiceID = 1;
             QHash<QString, QVariant> props = in;

             if (!classes.isEmpty())
             {
                 props.insert("objectclass", classes);
             }

             props.insert("service.id", sid != -1 ? sid : nextServiceID++);

             return props;
         }



         ServicesManger::ServicesManger()
         {
         }

         //----------------------------------------------------------------------------
         ServicesManger::~ServicesManger()
         {
             clear();
         }

         //----------------------------------------------------------------------------
         void ServicesManger::clear()
         {
             services.clear();
             classServices.clear();
         }

         //----------------------------------------------------------------------------
         ServiceRegistration ServicesManger::registerService(IPlugin* plugin,
             const QStringList& classes,
             QObject* service,
             const QHash<QString, QVariant>& properties)
         {
             if (service == 0)
             {
                 throw std::exception("Can't register 0 as a service");
             }

             // Check if service implements claimed classes and that they exist.
             for (QStringListIterator i(classes); i.hasNext();)
             {
                 QString cls = i.next();
                 if (cls.isEmpty())
                 {
                     throw std::exception("Can't register as null class");
                 }

                 if (!checkServiceClass(service, cls))
                 {
                     throw std::exception("Service class %1 is not an instance");
                 }
             }
             
             QString clsName =QString::fromStdString(service->metaObject()->className());
             ServiceRegistration res(plugin, service,clsName,
                 createServiceProperties(properties, classes));
             
             {
                 QMutexLocker lock(&mutex);
                 services.insert(res, classes);
                 for (QStringListIterator i(classes); i.hasNext(); )
                 {
                     QString currClass = i.next();
                     QList<ServiceRegistration>& s = classServices[currClass];
                     s.append(res);
                 }
             }

             return res;
         }


         QObject* ServicesManger::createServiceObj(const QString& clazz, const QHash<QString, QVariant>& properties)
         {
             
             QMutexLocker lock(&mutex);
             QList<ServiceRegistration>  regLst = classServices.value(clazz);
             QObject* serv = nullptr;
             for each (ServiceRegistration item in regLst)
             {
                 if (properties.count() > 0) {
                     bool isMatch = true;
                     QHash<QString, QVariant>::const_iterator it = properties.constBegin();
                     while (it != properties.constEnd())
                     {
                         if (item.properties[it.key()] != it.value()) {
                             isMatch = false;
                             break;
                         }
                         ++it;
                     }
                     if (isMatch) {
                         QString className = item.impClassName;
                         serv =(QObject*) ObjectFactory::createObject(className.toUtf8());
                         break;
                     }
                 }
                 else {
                     QString className = item.impClassName;
                     serv = (QObject*)ObjectFactory::createObject(className.toUtf8());
                 }
             }
             return serv;
         }

         //----------------------------------------------------------------------------
         bool ServicesManger::checkServiceClass(QObject* service, const QString& cls)
         {
             return service->inherits(cls.toLatin1());
         }
        
         QObject* ServicesManger::getService(const QString& clazz, const QHash<QString, QVariant>& properties)
         {
             QMutexLocker lock(&mutex);
             QList<ServiceRegistration>  regLst = classServices.value(clazz);
             QObject* serv = nullptr;
             for each (ServiceRegistration item in regLst)
             {
                 if (properties.count() > 0) {
                     bool isMatch = true;
                     QHash<QString, QVariant>::const_iterator it = properties.constBegin();
                     while (it != properties.constEnd())
                     {
                         if (item.properties[it.key()] != it.value()) {
                             isMatch = false;
                             break;
                         }
                         ++it;
                     }
                     if (isMatch) {
                         serv = item.service;
                         break;
                     }
                 }
                 else {
                     serv = item.service;
                 }
             }

             return serv;
         }
         //----------------------------------------------------------------------------
         QList<ServiceRegistration> ServicesManger::get(const QString& clazz)
         {
             QMutexLocker lock(&mutex);
             return classServices.value(clazz);
         }


         //----------------------------------------------------------------------------
         void ServicesManger::removeServiceRegistration(const ServiceRegistration& sr)
         {
             QMutexLocker lock(&mutex);

             QStringList classes = sr.properties.value("objectclass").toStringList();
             services.remove(sr);

             for (int i = 0; i < classes.size(); i++) {
                 QString currClass = classes.at(i);
                 QList<ServiceRegistration>& s = classServices[currClass];
                 if (s.size() > 1)
                 {
                     s.removeAll(sr);
                 }
                 else
                 {
                     classServices.remove(currClass);
                 }
             }
         }

         //----------------------------------------------------------------------------
         QList<ServiceRegistration> ServicesManger::getRegisteredByPlugin(IPlugin* p)
         {
             QMutexLocker lock(&mutex);

             QList<ServiceRegistration> res;

             QMap<ServiceRegistration, QStringList>::ConstIterator itr = services.constBegin();
             while (itr != services.constEnd()) {
                 ServiceRegistration sr = itr.key();
                 if (sr.plugin == p)
                 {
                     res.push_back(sr);
                 }
                 ++itr;
             }
             return res;
         }

    }
}