/**
 * ESP32_OTA_WS_Lib - 基本OTA更新示例
 *
 * 这个示例演示了如何使用ESP32_OTA_WS_Lib库进行基本的OTA固件更新
 * 它连接到WiFi网络，开启OTA更新功能，并通过WebSocket提供更新进度
 *
 * @author MrQ
 * @date 2025.02.26
 */

#include <ESP32_OTA_WS_Lib.h>

// 创建库的实例
ESP32_OTA_WS_Lib otaLib("ESP32_OTA_Demo", "1.0.0");

// 定义WiFi凭据
const char *ssid = "YOUR_WIFI_SSID";
const char *password = "YOUR_WIFI_PASSWORD";

void setup()
{
    // 初始化库
    otaLib.begin();

    // 获取WiFi管理器
    WiFiManager *wifiManager = otaLib.getWiFiManager();

    // 尝试连接WiFi
    if (!wifiManager->connectToWiFi(ssid, password))
    {
        Serial.println("无法连接到WiFi，启动AP模式");
        // 如果连接失败，会自动启动AP模式
    }

    // 打印设备信息
    SystemMonitor *sysMonitor = otaLib.getSystemMonitor();
    sysMonitor->printSystemInfo();

    Serial.println("\n准备就绪！");
    Serial.println("访问 http://" + WiFi.localIP().toString() + " 进入Web界面");
    Serial.println("访问 http://" + WiFi.localIP().toString() + "/update 进行OTA更新");
}

void loop()
{
    // 处理库的循环任务
    otaLib.handle();

    // 添加您自己的循环代码...

    // 如果需要，可以发送消息到WebSocket
    static unsigned long lastMsg = 0;
    if (millis() - lastMsg > 5000)
    {
        lastMsg = millis();

        // 创建一个状态消息
        StaticJsonDocument<200> doc;
        doc["type"] = "status";
        doc["uptime"] = millis() / 1000;
        doc["heap"] = ESP.getFreeHeap();

        String message;
        serializeJson(doc, message);

        // 发送到所有连接的WebSocket客户端
        otaLib.broadcastMessage(message);
    }
}