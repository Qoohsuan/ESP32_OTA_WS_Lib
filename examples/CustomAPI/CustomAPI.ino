/**
 * ESP32_OTA_WS_Lib - 自定义API示例
 *
 * 这个示例演示了如何为ESP32_OTA_WS_Lib库添加自定义API端点，
 * 以实现特定功能和自定义控制逻辑
 *
 * @author MrQ
 * @date 2025.02.26
 */

#include <ESP32_OTA_WS_Lib.h>

// 创建库的实例
ESP32_OTA_WS_Lib otaLib("ESP32_Custom_API", "1.0.0");

// 定义传感器模拟引脚
#define TEMP_SENSOR_PIN A0
#define RELAY_PIN 5

// 全局变量
bool relayState = false;
unsigned long lastSensorUpdate = 0;
float temperature = 0.0;
int lightLevel = 0;

// 函数声明
void setupCustomRoutes();
void updateSensorData();
String getSensorDataJson();

void setup()
{
    // 初始化库
    otaLib.begin();

    // 设置继电器引脚
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);

    // 初始化模拟传感器引脚
    pinMode(TEMP_SENSOR_PIN, INPUT);

    // 配置自定义API路由
    setupCustomRoutes();

    // 获取系统监控器引用并打印信息
    SystemMonitor *sysMonitor = otaLib.getSystemMonitor();
    sysMonitor->printSystemInfo();

    Serial.println("\n自定义API示例已准备就绪");
    Serial.println("访问 http://" + WiFi.localIP().toString() + " 进入Web界面");
    Serial.println("访问 http://" + WiFi.localIP().toString() + "/api/sensors 获取传感器数据");
    Serial.println("访问 http://" + WiFi.localIP().toString() + "/api/relay 控制继电器");
}

void loop()
{
    // 处理库的循环任务
    otaLib.handle();

    // 更新传感器数据
    updateSensorData();

    // 每5秒发送一次传感器数据到WebSocket
    if (millis() - lastSensorUpdate > 5000)
    {
        lastSensorUpdate = millis();
        otaLib.broadcastMessage(getSensorDataJson());
    }
}

/**
 * 设置自定义API路由
 */
void setupCustomRoutes()
{
    // 获取WebServer管理器引用
    WebServerManager *webServer = otaLib.getWebServerManager();

    // 添加传感器数据API
    /*
    webServer->on("/api/sensors", HTTP_GET, [](AsyncWebServerRequest *request) {
      String response = getSensorDataJson();
      request->send(200, "application/json", response);
    });

    // 添加继电器控制API
    webServer->on("/api/relay", HTTP_POST, [](AsyncWebServerRequest *request) {
      // 参数检查
      if (!request->hasParam("state", true)) {
        request->send(400, "application/json", "{\"error\":\"Missing state parameter\"}");
        return;
      }

      // 获取状态参数
      String stateStr = request->getParam("state", true)->value();
      bool newState = (stateStr == "on" || stateStr == "1" || stateStr == "true");

      // 设置继电器状态
      relayState = newState;
      digitalWrite(RELAY_PIN, relayState ? HIGH : LOW);

      // 返回当前状态
      String response = "{\"relay\":\"" + String(relayState ? "on" : "off") + "\"}";
      request->send(200, "application/json", response);

      // WebSocket通知
      StaticJsonDocument<100> doc;
      doc["type"] = "relay_update";
      doc["state"] = relayState ? "on" : "off";

      String message;
      serializeJson(doc, message);
      otaLib.broadcastMessage(message);
    });

    // 添加设备重命名API
    webServer->on("/api/rename", HTTP_POST, [](AsyncWebServerRequest *request) {
      if (!request->hasParam("name", true)) {
        request->send(400, "application/json", "{\"error\":\"Missing name parameter\"}");
        return;
      }

      String newName = request->getParam("name", true)->value();
      // 这里可以将设备名存储到EEPROM等

      request->send(200, "application/json", "{\"success\":true,\"name\":\"" + newName + "\"}");
    });
    */
}

/**
 * 更新传感器数据
 */
void updateSensorData()
{
    // 这里使用模拟值作为示例，在实际应用中应当使用真实传感器
    temperature = 20.0 + (random(0, 100) / 10.0); // 模拟20-30度的温度
    lightLevel = random(0, 1024);                 // 模拟光线传感器
}

/**
 * 获取JSON格式的传感器数据
 */
String getSensorDataJson()
{
    StaticJsonDocument<200> doc;
    doc["type"] = "sensor_data";
    doc["temperature"] = temperature;
    doc["light"] = lightLevel;
    doc["relay"] = relayState ? "on" : "off";
    doc["timestamp"] = millis() / 1000;

    String message;
    serializeJson(doc, message);
    return message;
}