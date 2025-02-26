/**
 * ESP32_OTA_WS_Lib.h - ESP32/ESP8266/RP2040 OTA WebSocket Library
 *
 * 提供完整的OTA更新、WebSocket通信和Web管理功能
 *
 * @file ESP32_OTA_WS_Lib.h
 * @author MrQ
 * @version 1.0.0
 * @date 2023-04-20
 */

#ifndef ESP32_OTA_WS_LIB_H
#define ESP32_OTA_WS_LIB_H

// 检查平台
#if defined(ESP8266)
#define PLATFORM_ESP8266
#elif defined(ESP32)
#define PLATFORM_ESP32
#elif defined(TARGET_RP2040)
#define PLATFORM_RP2040
#else
#error "不支持的平台! 仅支持ESP8266, ESP32和RP2040(Pico W)"
#endif

// 包含必要的标准库
#include <Arduino.h>

// 模块头文件包含
#include "OTAManager.h"
#include "WiFiManager.h"
#include "WebSocketManager.h"
#include "WebServerManager.h"
#include "SystemMonitor.h"
#include "StatusIndicator.h"

// 库版本
#define ESP32_OTA_WS_LIB_VERSION "1.0.0"

/**
 * ESP32_OTA_WS_Lib 类
 *
 * 此类是库的主要接口，管理所有模块并提供简化的API
 */
class ESP32_OTA_WS_Lib
{
public:
    /**
     * 构造函数
     *
     * @param deviceName 设备名称前缀
     * @param firmwareVersion 固件版本
     * @param webServerPort Web服务器端口号
     * @param wsServerPort WebSocket服务器端口号
     * @param ledPin NeoPixel LED引脚 (仅支持部分平台)
     */
    ESP32_OTA_WS_Lib(const char *deviceName = "ESP_Device",
                     const char *firmwareVersion = "1.0.0",
                     uint16_t webServerPort = 80,
                     uint16_t wsServerPort = 81,
                     uint8_t ledPin = 8);

    /**
     * 初始化库
     *
     * @param mountFS 是否挂载文件系统
     * @return 初始化是否成功
     */
    bool begin(bool mountFS = true);

    /**
     * 处理循环函数，需要在loop()中调用
     */
    void handle();

    /**
     * 启用或禁用API监控
     *
     * @param enable 是否启用监控
     */
    void enableApiMonitoring(bool enable);

    /**
     * 设置自动重启间隔
     *
     * @param hours 重启间隔小时数, 0表示禁用
     */
    void setRebootInterval(int hours);

    /**
     * 发送消息到所有连接的WebSocket客户端
     *
     * @param message 要发送的消息
     */
    void broadcastMessage(const String &message);

    // 获取各模块的实例
    OTAManager *getOTAManager();
    WiFiManager *getWiFiManager();
    WebSocketManager *getWebSocketManager();
    WebServerManager *getWebServerManager();
    SystemMonitor *getSystemMonitor();
    StatusIndicator *getStatusIndicator();

private:
    // 模块实例
    OTAManager *_otaManager;
    WiFiManager *_wifiManager;
    WebSocketManager *_wsManager;
    WebServerManager *_webServer;
    SystemMonitor *_sysMonitor;
    StatusIndicator *_statusIndicator;

    // 配置参数
    String _deviceName;
    String _firmwareVersion;
    uint16_t _webServerPort;
    uint16_t _wsServerPort;
    uint8_t _ledPin;

    // 标志
    bool _apiMonitoring;
    unsigned long _lastApiStatsUpdate;
};

#endif // ESP32_OTA_WS_LIB_H