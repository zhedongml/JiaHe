// Copyright 2023 MLOptic

#pragma once

#include <map>
#include <string>

#include "MLSerial.h"

/// <summary>
///  This file contains the declaration of the singleton pattern class MLSerial.
/// The class provides the interface to manage creation and destruction for
/// MLSerial.
/// </summary>

class MLSerialManage {
 public:
    static MLSerialManage* instance();
    ~MLSerialManage();

    /// <summary>
    /// Create a mlserial object.
    /// </summary>
    /// <param name="port">the port of the serial to be created</param>
    void createMLSerial(std::string port);

    /// <summary>
    /// Obtain the mlserial object.
    /// </summary>
    /// <param name="port">the port of the serial to get</param>
    MLSerial* getMLSerial(std::string port);

    /// <summary>
    /// Set the connection of the port.
    /// </summary>
    /// <param name="port">the port to set the connection</param>
    void setConnection(std::string port);

    /// <summary>
    /// Close the connection of the port.
    /// </summary>
    /// <param name="port">the port to close the connection</param>
    void closeConnection(std::string port);

    /// <summary>
    /// Get the connection of the port.
    /// </summary>
    /// <param name="port">the port to get the connection</param>
    int getConnection(std::string port);

 private:
    MLSerialManage();

 private:
    std::map<std::string, MLSerial*> m_SerialMap;
    std::map<std::string, int> m_ConnectMap;
    static MLSerialManage* m_instance;
};
