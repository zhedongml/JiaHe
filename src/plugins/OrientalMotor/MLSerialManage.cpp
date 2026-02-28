// Copyright 2023 MLOptic

#include "MLSerialManage.h"
#include "MLSpdlog.h"
#include <utility>

MLSerialManage* MLSerialManage::m_instance = nullptr;
std::mutex m_mutex;


MLSerialManage::MLSerialManage() {
    ML::MLSpdlog::instance()->info("MLSerialManage: create instance");
}

MLSerialManage* MLSerialManage::instance() {
    if (!m_instance) {
        static std::mutex mutex;
        std::unique_lock<std::mutex> locker(mutex);
        if (!m_instance) {
            m_instance = new MLSerialManage();
        }
    }
    return m_instance;
}

MLSerialManage::~MLSerialManage() {
    for (auto& it : m_SerialMap) {
        delete it.second;
    }
    std::map<std::string, MLSerial*>().swap(m_SerialMap);
    std::map<std::string, int>().swap(m_ConnectMap);
}

void MLSerialManage::createMLSerial(std::string port) {
    std::unique_lock<std::mutex> locker(m_mutex);
    if (m_SerialMap.count(port) == 0) {
        MLSerial* new_serial = new MLSerial();
        m_SerialMap.insert(std::make_pair(port, new_serial));
        m_ConnectMap.insert(std::make_pair(port, 0));
    }
}

MLSerial* MLSerialManage::getMLSerial(std::string port) {
    std::unique_lock<std::mutex> locker(m_mutex);
    for (auto it = m_SerialMap.begin(); it != m_SerialMap.end(); it++) {
        if (it->first == port) {
            return it->second;
        }
    }
    return nullptr;
}

void MLSerialManage::setConnection(std::string port) {
    std::unique_lock<std::mutex> locker(m_mutex);
    std::map<std::string, int>::iterator iter = m_ConnectMap.find(port);
    if (iter != m_ConnectMap.end()) {
        int value = m_ConnectMap[port];
        m_ConnectMap[port] = value + 1;
    }
}

void MLSerialManage::closeConnection(std::string port) {
    std::unique_lock<std::mutex> locker(m_mutex);
    std::map<std::string, int>::iterator iter = m_ConnectMap.find(port);
    if (iter != m_ConnectMap.end()) {
        int value = m_ConnectMap[port];
        m_ConnectMap[port] = value - 1;
    }
}

int MLSerialManage::getConnection(std::string port) {
    std::unique_lock<std::mutex> locker(m_mutex);
    std::map<std::string, int>::iterator iter = m_ConnectMap.find(port);
    if (iter != m_ConnectMap.end()) {
        return m_ConnectMap[port];
    }
    return -1;
}
