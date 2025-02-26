/**
 * ESP32_OTA_WS_Lib.cpp
 *
 * ESP32/ESP8266/RP2040 OTA WebSocket库的主实现文件
 *
 * @file ESP32_OTA_WS_Lib.cpp
 * @author MrQ
 * @version 1.0.0
 * @date 2023-04-20
 */

#include "ESP32_OTA_WS_Lib.h"

/**
 * 构造函数
 */
ESP32_OTA_WS_Lib::ESP32_OTA_WS_Lib(const char *deviceName,
                                   const char *firmwareVersion,
                                   uint16_t webServerPort,
                                   uint16_t wsServerPort,
                                   uint8_t ledPin) : _deviceName(deviceName),
                                                     _firmwareVersion(firmwareVersion),
                                                     _webServerPort(webServerPort),
                                                     _wsServerPort(wsServerPort),
                                                     _ledPin(ledPin),
                                                     _apiMonitoring(false),
                                                     _lastApiStatsUpdate(0)
{
    // 初始化各个模块
    _wifiManager = new WiFiManager();
    _wsManager = new WebSocketManager(wsServerPort);
    _webServer = new WebServerManager(webServerPort);
    _otaManager = new OTAManager(_wsManager);
    _sysMonitor = new SystemMonitor(_deviceName, _firmwareVersion);

// 根据平台决定是否创建状态指示器
#if defined(ESP32) || defined(ESP8266)
    _statusIndicator = new StatusIndicator(ledPin);
#else
    _statusIndicator = nullptr;
#endif
}

/**
 * 初始化库
 */
bool ESP32_OTA_WS_Lib::begin(bool mountFS)
{
    Serial.begin(115200);
    Serial.println("\n--------------------------");
    Serial.printf("ESP32_OTA_WS_Lib v%s\n", ESP32_OTA_WS_LIB_VERSION);
    Serial.printf("设备名称: %s\n", _deviceName.c_str());
    Serial.printf("固件版本: %s\n", _firmwareVersion.c_str());
    Serial.println("--------------------------\n");

    // 初始化状态指示器
    if (_statusIndicator)
    {
        _statusIndicator->begin();
    }

    // 挂载文件系统
    bool fsInitialized = true;
    if (mountFS)
    {
#if defined(ESP32) || defined(ESP8266)
        fsInitialized = LittleFS.begin();
        if (!fsInitialized)
        {
            Serial.println("文件系统挂载失败");
        }
        else
        {
            Serial.println("文件系统挂载成功");
        }
#else
        Serial.println("当前平台不支持LittleFS");
        fsInitialized = false;
#endif
    }

    // 初始化WiFi管理器
    _wifiManager->begin();

    // 初始化WebSocket服务器
    _wsManager->begin();

    // 初始化Web服务器并配置路由
    _webServer->begin();

    // 配置OTA管理器
    _otaManager->begin();

    // 配置系统监控器
    _sysMonitor->begin();

    // 设置状态指示器模式
    if (_statusIndicator)
    {
        if (_wifiManager->isConnected())
        {
            _statusIndicator->setMode(LED_CONNECTED);
        }
        else
        {
            _statusIndicator->setMode(LED_AP_MODE);
        }
    }

    return fsInitialized;
}

/**
 * 处理循环函数
 */
void ESP32_OTA_WS_Lib::handle()
{
    // 处理WiFi状态变化
    _wifiManager->handle();

    // 处理WebSocket消息
    _wsManager->handle();

    // 更新状态指示器
    if (_statusIndicator)
    {
        _statusIndicator->handle();
    }

    // API监控
    if (_apiMonitoring && millis() - _lastApiStatsUpdate > 30000)
    {
        // 每30秒更新一次API统计
        _webServer->displayApiStats();
        _lastApiStatsUpdate = millis();
    }

    // 处理系统监控
    _sysMonitor->handle();
}

/**
 * 启用或禁用API监控
 */
void ESP32_OTA_WS_Lib::enableApiMonitoring(bool enable)
{
    _apiMonitoring = enable;
    _webServer->enableApiMonitoring(enable);
}

/**
 * 设置自动重启间隔
 */
void ESP32_OTA_WS_Lib::setRebootInterval(int hours)
{
    _sysMonitor->setupRebootTimer(hours);
}

/**
 * 发送消息到所有连接的WebSocket客户端
 */
void ESP32_OTA_WS_Lib::broadcastMessage(const String &message)
{
    if (_wsManager)
    {
        _wsManager->broadcastTXT(message);
    }
}

// 获取各模块的实例
OTAManager *ESP32_OTA_WS_Lib::getOTAManager()
{
    return _otaManager;
}

WiFiManager *ESP32_OTA_WS_Lib::getWiFiManager()
{
    return _wifiManager;
}

WebSocketManager *ESP32_OTA_WS_Lib::getWebSocketManager()
{
    return _wsManager;
}

WebServerManager *ESP32_OTA_WS_Lib::getWebServerManager()
{
    return _webServer;
}

SystemMonitor *ESP32_OTA_WS_Lib::getSystemMonitor()
{
    return _sysMonitor;
}

StatusIndicator *ESP32_OTA_WS_Lib::getStatusIndicator()
{
    return _statusIndicator;
}