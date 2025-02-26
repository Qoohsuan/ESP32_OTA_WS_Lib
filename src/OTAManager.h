/**
 * OTAManager.h
 *
 * OTA更新管理模块，处理固件和文件系统的无线更新
 *
 * @file OTAManager.h
 * @author MrQ
 * @version 1.0.0
 * @date 2023-04-20
 */

#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

#include <Arduino.h>

// 平台特定包含
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <WebServer.h>
#elif defined(TARGET_RP2040)
#include <WiFi.h>
// RP2040可能需要不同的库
#endif

#include <LittleFS.h>
#include <Update.h>
#include <ArduinoJson.h>

// 前向声明
class WebSocketManager;

/**
 * OTA管理器类
 *
 * 处理固件和文件系统更新的功能
 */
class OTAManager
{
public:
    /**
     * 构造函数
     *
     * @param wsManager WebSocket管理器的引用，用于发送更新进度
     */
    OTAManager(WebSocketManager *wsManager);

    /**
     * 初始化OTA更新管理器
     */
    void begin();

    /**
     * 处理固件更新请求
     *
     * @param request 异步请求对象
     * @param filename 文件名
     * @param index 当前块索引
     * @param data 数据
     * @param len 数据长度
     * @param final 是否为最后一块
     */
    void handleFirmwareUpdate(void *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final);

    /**
     * 处理文件系统更新请求
     *
     * @param request 异步请求对象
     * @param filename 文件名
     * @param index 当前块索引
     * @param data 数据
     * @param len 数据长度
     * @param final 是否为最后一块
     */
    void handleFilesystemUpdate(void *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final);

    /**
     * 发送OTA更新进度
     *
     * @param progress 进度百分比
     * @param current 当前上传的字节数
     * @param total 总字节数
     */
    void sendUpdateProgress(float progress, size_t current, size_t total);

    /**
     * 获取当前更新类型
     *
     * @return 更新类型 (0: 无更新, 1: 固件更新, 2: 文件系统更新)
     */
    uint8_t getUpdateType() const;

private:
    WebSocketManager *_wsManager; // WebSocket管理器引用

    // 更新状态变量
    size_t _contentLength;
    size_t _currentLength;
    size_t _totalLength;
    bool _isUpdating;
    uint8_t _updateType; // 0: 无更新, 1: 固件更新, 2: 文件系统更新

    // 速度计算变量
    unsigned long _lastBytes;
    unsigned long _lastSpeedCheck;
    float _currentSpeed;
    unsigned long _ota_progress_millis;

    /**
     * 计算并格式化传输速度
     *
     * @param speed 字节每秒的速度
     * @return 格式化的速度字符串
     */
    String formatSpeed(float speed);

    /**
     * 格式化字节数为人类可读格式
     *
     * @param bytes 字节数
     * @param str 输出字符串缓冲区
     */
    void formatBytes(size_t bytes, char *str);
};

#endif // OTA_MANAGER_H