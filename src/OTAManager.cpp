/**
 * OTAManager.cpp
 *
 * OTA更新管理模块的实现
 *
 * @file OTAManager.cpp
 * @author MrQ
 * @version 1.0.0
 * @date 2023-04-20
 */

#include "OTAManager.h"
#include "WebSocketManager.h"

/**
 * 构造函数
 */
OTAManager::OTAManager(WebSocketManager *wsManager) : _wsManager(wsManager),
                                                      _contentLength(0),
                                                      _currentLength(0),
                                                      _totalLength(0),
                                                      _isUpdating(false),
                                                      _updateType(0),
                                                      _lastBytes(0),
                                                      _lastSpeedCheck(0),
                                                      _currentSpeed(0),
                                                      _ota_progress_millis(0)
{
}

/**
 * 初始化OTA更新管理器
 */
void OTAManager::begin()
{
    // 初始化变量
    _contentLength = 0;
    _currentLength = 0;
    _totalLength = 0;
    _isUpdating = false;
    _updateType = 0;
    _lastBytes = 0;
    _lastSpeedCheck = 0;
    _currentSpeed = 0;
    _ota_progress_millis = 0;

    Serial.println("OTA管理器初始化完成");
}

/**
 * 处理固件更新请求
 */
void OTAManager::handleFirmwareUpdate(void *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
{
    if (!index)
    { // 首次接收数据
        Serial.println("开始接收固件更新...");
        Serial.printf("文件名: %s\n", filename.c_str());

        // 保存内容长度
        _contentLength = ((AsyncWebServerRequest *)request)->contentLength();
        _totalLength = _contentLength;
        _currentLength = 0;
        _isUpdating = true;
        _updateType = 1; // 固件更新

        // 初始化速度计算
        _lastBytes = 0;
        _lastSpeedCheck = millis();
        _currentSpeed = 0;

// 初始化更新
#if defined(ESP8266)
        Update.runAsync(true);
        if (!Update.begin(_contentLength, U_FLASH))
        {
#else
        if (!Update.begin(_contentLength))
        {
#endif
            Update.printError(Serial);
            return;
        }
    }

    // 写入数据
    if (Update.write(data, len) != len)
    {
        Update.printError(Serial);
        return;
    }

    _currentLength += len;

    // 更新速度
    unsigned long now = millis();
    if (now - _lastSpeedCheck >= 1000)
    { // 每秒计算一次速度
        _currentSpeed = (_currentLength - _lastBytes) * 1000.0 / (now - _lastSpeedCheck);
        _lastBytes = _currentLength;
        _lastSpeedCheck = now;
    }

    // 发送进度
    if (now - _ota_progress_millis > 100)
    { // 每100ms发送一次进度
        float progress = (float)_currentLength / (float)_contentLength * 100.0;
        sendUpdateProgress(progress, _currentLength, _contentLength);
        _ota_progress_millis = now;
    }

    if (final)
    { // 文件上传完成
        if (Update.end(true))
        {
            Serial.printf("更新成功! 共计 %u bytes\n", _currentLength);
            // 发送100%进度
            sendUpdateProgress(100.0, _contentLength, _contentLength);
            // 重启设备
            delay(1000);
            ESP.restart();
        }
        else
        {
            Update.printError(Serial);
        }
    }
}

/**
 * 处理文件系统更新请求
 */
void OTAManager::handleFilesystemUpdate(void *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
{
    if (!index)
    { // 首次接收数据
        Serial.println("开始接收文件系统更新...");
        Serial.printf("文件名: %s\n", filename.c_str());

        // 保存内容长度
        _contentLength = ((AsyncWebServerRequest *)request)->contentLength();
        _totalLength = _contentLength;
        _currentLength = 0;
        _isUpdating = true;
        _updateType = 2; // 文件系统更新

        // 初始化速度计算
        _lastBytes = 0;
        _lastSpeedCheck = millis();
        _currentSpeed = 0;

// 初始化更新
#if defined(ESP8266)
        if (!Update.begin(_contentLength, U_FS))
        {
#elif defined(ESP32)
        if (!Update.begin(_contentLength, U_SPIFFS))
        {
#else
        if (!Update.begin(_contentLength))
        {
#endif
            Update.printError(Serial);
            return;
        }
    }

    // 写入数据
    if (Update.write(data, len) != len)
    {
        Update.printError(Serial);
        return;
    }

    _currentLength += len;

    // 更新速度
    unsigned long now = millis();
    if (now - _lastSpeedCheck >= 1000)
    { // 每秒计算一次速度
        _currentSpeed = (_currentLength - _lastBytes) * 1000.0 / (now - _lastSpeedCheck);
        _lastBytes = _currentLength;
        _lastSpeedCheck = now;
    }

    // 发送进度
    if (now - _ota_progress_millis > 100)
    { // 每100ms发送一次进度
        float progress = (float)_currentLength / (float)_contentLength * 100.0;
        sendUpdateProgress(progress, _currentLength, _contentLength);
        _ota_progress_millis = now;
    }

    if (final)
    { // 文件上传完成
        if (Update.end(true))
        {
            Serial.printf("文件系统更新成功! 共计 %u bytes\n", _currentLength);
            // 发送100%进度
            sendUpdateProgress(100.0, _contentLength, _contentLength);
            // 重启设备
            delay(1000);
            ESP.restart();
        }
        else
        {
            Update.printError(Serial);
        }
    }
}

/**
 * 发送OTA更新进度
 */
void OTAManager::sendUpdateProgress(float progress, size_t current, size_t total)
{
    if (_wsManager)
    {
        char speedText[32];
        snprintf(speedText, sizeof(speedText), "%s", formatSpeed(_currentSpeed).c_str());

        StaticJsonDocument<200> progressDoc;
        progressDoc["type"] = "progress";
        progressDoc["progress"] = progress;
        progressDoc["current"] = current;
        progressDoc["total"] = total;
        progressDoc["speed"] = _currentSpeed;
        progressDoc["speedText"] = speedText;

        String progressJson;
        serializeJson(progressDoc, progressJson);

        _wsManager->broadcastTXT(progressJson);
    }
}

/**
 * 获取当前更新类型
 */
uint8_t OTAManager::getUpdateType() const
{
    return _updateType;
}

/**
 * 计算并格式化传输速度
 */
String OTAManager::formatSpeed(float speed)
{
    char buf[20];
    const char *units[] = {"B/s", "KB/s", "MB/s", "GB/s", "TB/s"};
    int unit = 0;

    while (speed >= 1024.0 && unit < 4)
    {
        speed /= 1024.0;
        unit++;
    }

    sprintf(buf, "%.1f %s", speed, units[unit]);
    return String(buf);
}

/**
 * 格式化字节数为人类可读格式
 */
void OTAManager::formatBytes(size_t bytes, char *str)
{
    const char *units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit = 0;
    double size = bytes;

    while (size >= 1024.0 && unit < 4)
    {
        size /= 1024.0;
        unit++;
    }

    sprintf(str, "%.1f %s", size, units[unit]);
}