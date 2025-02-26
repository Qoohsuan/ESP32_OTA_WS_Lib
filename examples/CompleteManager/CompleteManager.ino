/**
 * ESP32_OTA_WS_Lib - 完整管理示例
 *
 * 这个示例展示了ESP32_OTA_WS_Lib库的所有主要功能：
 * - WiFi配置和管理
 * - OTA固件和文件系统更新
 * - WebSocket实时通信
 * - 系统监控和状态指示
 * - API路由和功能
 *
 * @author MrQ
 * @date 2025.02.26
 */

#include <ESP32_OTA_WS_Lib.h>

// 创建库的实例
ESP32_OTA_WS_Lib otaLib("ESP32_Manager", "1.1.0");

// GPIO定义
#define BUTTON_PIN 0 // 按钮引脚

// 全局变量
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; // 防抖延迟（毫秒）
int lastButtonState = HIGH;       // 上一次的按钮状态
int buttonState;                  // 当前的按钮状态
int buttonPressCount = 0;         // 按钮按下次数计数

// 函数声明
void handleButton();
void sendButtonEvent();
void setupCustomRoutes();

void setup()
{
    // 初始化库
    otaLib.begin();

    // 设置按钮引脚
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // 获取模块引用
    WiFiManager *wifiManager = otaLib.getWiFiManager();
    WebServerManager *webServer = otaLib.getWebServerManager();
    SystemMonitor *sysMonitor = otaLib.getSystemMonitor();
    StatusIndicator *statusIndicator = otaLib.getStatusIndicator();

    // 启用API监控
    otaLib.enableApiMonitoring(true);

    // 设置自动重启间隔（小时），0禁用
    otaLib.setRebootInterval(24);

    // 设置自定义路由
    setupCustomRoutes();

    // 显示设备信息
    sysMonitor->printSystemInfo();

    if (wifiManager->isConnected())
    {
        Serial.println("\n设备就绪！");
        Serial.print("IP地址: ");
        Serial.println(WiFi.localIP());
        Serial.println("访问 http://" + WiFi.localIP().toString() + " 进入Web界面");
        Serial.println("访问 http://" + WiFi.localIP().toString() + "/update 进行OTA更新");

        // 设置状态指示为已连接
        if (statusIndicator)
        {
            statusIndicator->setMode(LED_CONNECTED);
        }
    }
    else
    {
        Serial.println("\nAP模式启动，SSID: ESP32_Setup");
        Serial.println("访问 http://192.168.4.1 配置WiFi");

        // 设置状态指示为AP模式
        if (statusIndicator)
        {
            statusIndicator->setMode(LED_AP_MODE);
        }
    }
}

void loop()
{
    // 处理库的循环任务
    otaLib.handle();

    // 处理按钮
    handleButton();

    // 添加传感器数据或其他状态更新
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 10000)
    { // 每10秒更新一次
        lastUpdate = millis();

        // 创建状态消息
        StaticJsonDocument<256> doc;
        doc["type"] = "device_status";
        doc["uptime"] = millis() / 1000;
        doc["heap"] = ESP.getFreeHeap();
        doc["button_presses"] = buttonPressCount;

        // 添加一些模拟传感器数据
        JsonObject sensors = doc.createNestedObject("sensors");
        sensors["temperature"] = random(20, 30); // 模拟温度数据
        sensors["humidity"] = random(40, 80);    // 模拟湿度数据
        sensors["light"] = random(0, 1000);      // 模拟光照数据

        String message;
        serializeJson(doc, message);

        // 广播状态
        otaLib.broadcastMessage(message);
    }
}

/**
 * 处理按钮逻辑
 */
void handleButton()
{
    // 读取按钮状态
    int reading = digitalRead(BUTTON_PIN);

    // 检查按钮状态是否改变
    if (reading != lastButtonState)
    {
        lastDebounceTime = millis();
    }

    // 如果状态稳定超过防抖时间
    if ((millis() - lastDebounceTime) > debounceDelay)
    {
        // 如果按钮状态改变
        if (reading != buttonState)
        {
            buttonState = reading;

            // 如果按钮被按下
            if (buttonState == LOW)
            {
                buttonPressCount++;
                Serial.printf("按钮被按下! 按下次数: %d\n", buttonPressCount);

                // 发送按钮事件
                sendButtonEvent();
            }
        }
    }

    lastButtonState = reading;
}

/**
 * 发送按钮事件到WebSocket客户端
 */
void sendButtonEvent()
{
    StaticJsonDocument<100> doc;
    doc["type"] = "button_event";
    doc["count"] = buttonPressCount;
    doc["timestamp"] = millis() / 1000;

    String message;
    serializeJson(doc, message);

    otaLib.broadcastMessage(message);

    // 触发API活动指示
    StatusIndicator *statusIndicator = otaLib.getStatusIndicator();
    if (statusIndicator)
    {
        statusIndicator->triggerApiActivity();
    }
}

/**
 * 设置自定义API路由
 */
void setupCustomRoutes()
{
    WebServerManager *webServer = otaLib.getWebServerManager();

    // 添加自定义API路由 - 获取按钮状态
    /*
    webServer->on("/api/button-status", HTTP_GET, [](AsyncWebServerRequest *request) {
      StaticJsonDocument<100> doc;
      doc["button_count"] = buttonPressCount;
      doc["button_state"] = buttonState == LOW ? "pressed" : "released";

      String response;
      serializeJson(doc, response);

      request->send(200, "application/json", response);
    });

    // 添加自定义API路由 - LED控制
    webServer->on("/api/led", HTTP_POST, [](AsyncWebServerRequest *request) {
      // 处理LED控制请求
      // 这里需要处理请求体中的JSON参数
    });
    */
}