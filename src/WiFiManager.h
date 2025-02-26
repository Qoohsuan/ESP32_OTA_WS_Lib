/**
 * WiFiManager.h
 *
 * WiFi管理模块，处理WiFi连接、AP模式和凭据的存储
 *
 * @file WiFiManager.h
 * @author MrQ
 * @version 1.0.0
 * @date 2023-04-20
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>

// 平台特定包含
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#elif defined(TARGET_RP2040)
#include <WiFi.h>
#endif

#include <EEPROM.h>
#include <ArduinoJson.h>

// EEPROM 配置
#define EEPROM_SIZE 512
#define SSID_ADDR 0
#define PASS_ADDR 32
#define WIFI_SETTINGS_VALID_ADDR 100
#define VALID_SETTINGS_FLAG 0xAA

// WiFi 凭据结构
struct WiFiCredentials
{
    char ssid[32];
    char password[64];
};

/**
 * WiFi管理器类
 *
 * 处理WiFi连接、AP模式和凭据的存储
 */
class WiFiManager
{
public:
    /**
     * 构造函数
     *
     * @param apSSID AP模式的SSID
     * @param apPassword AP模式的密码
     */
    WiFiManager(const char *apSSID = "ESP32_Setup", const char *apPassword = "12345678");

    /**
     * 初始化WiFi管理器
     */
    bool begin();

    /**
     * 处理WiFi状态变化
     */
    void handle();

    /**
     * 连接到WiFi
     *
     * @param ssid WiFi的SSID
     * @param password WiFi的密码
     * @param timeout 连接超时时间（毫秒）
     * @return 是否连接成功
     */
    bool connectToWiFi(const char *ssid, const char *password, uint32_t timeout = 30000);

    /**
     * 启动AP模式
     */
    void startAPMode();

    /**
     * 扫描可用的WiFi网络
     *
     * @return JSON格式的WiFi网络列表
     */
    String scanNetworks();

    /**
     * 保存WiFi凭据到EEPROM
     *
     * @param ssid WiFi的SSID
     * @param password WiFi的密码
     */
    void saveWiFiCredentials(const char *ssid, const char *password);

    /**
     * 从EEPROM加载WiFi凭据
     *
     * @return 是否加载成功
     */
    bool loadWiFiCredentials();

    /**
     * 重置存储的WiFi凭据
     */
    void resetWiFiCredentials();

    /**
     * 检查是否已连接到WiFi
     *
     * @return 是否已连接
     */
    bool isConnected();

    /**
     * 获取当前WiFi状态的字符串表示
     *
     * @return 状态字符串
     */
    String getWiFiStatusString();

    /**
     * 获取当前连接的SSID
     *
     * @return SSID字符串
     */
    String getSSID();

    /**
     * 获取WiFi信号强度（RSSI）
     *
     * @return RSSI值
     */
    int getRSSI();

private:
    const char *_apSSID;          // AP模式SSID
    const char *_apPassword;      // AP模式密码
    WiFiCredentials _credentials; // 存储的WiFi凭据
    bool _apMode;                 // 是否处于AP模式
    unsigned long _lastWiFiCheck; // 上次检查WiFi状态的时间

    /**
     * 初始化EEPROM
     */
    void initializeEEPROM();
};

#endif // WIFI_MANAGER_H