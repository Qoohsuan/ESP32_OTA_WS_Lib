# ESP32 OTA WebSocket Library

ESP32 OTA WebSocket Library (ESP32_OTA_WS_Lib) 是一个为 ESP32、ESP8266 和 RP2040 设备设计的全功能 OTA(空中升级)管理库。它整合了 WiFi 配置、设备扫描、WebSocket 实时通信、按钮事件处理以及通过网页界面更新固件和文件系统等功能。

## 特点

- **网页式 OTA 更新**：通过网页界面轻松更新固件和文件系统
- **WiFi 管理**：支持 WiFi 配置、保存、自动重连和自动 AP 模式
- **WebSocket 通信**：提供实时状态更新、事件通知和日志传输
- **API 路由**：丰富的 API 接口用于设备控制和监控
- **LED 状态指示**：多种状态下的 LED 指示效果（支持 NeoPixel）
- **多平台支持**：兼容 ESP32、ESP8266 和 RP2040 平台

## 安装

### 通过 Arduino 库管理器

1. 打开 Arduino IDE
2. 点击 `工具` -> `管理库...`
3. 搜索 "ESP32_OTA_WS_Lib"
4. 点击安装

### 通过 PlatformIO

1. 在`platformio.ini`中添加：

```ini
lib_deps =
    ESP32_OTA_WS_Lib
```

### 手动安装

1. 下载此仓库
2. 将其解压到 Arduino/libraries 目录或 PlatformIO 项目的 lib 目录

## 快速开始

以下是一个简单的例子，展示如何使用此库进行基本的 OTA 更新：

```cpp
#include <ESP32_OTA_WS_Lib.h>

// 创建库的实例
ESP32_OTA_WS_Lib otaLib("ESP32_OTA_Demo", "1.0.0");

void setup() {
  // 初始化库
  otaLib.begin();

  // 可选：如果你不想使用保存的WiFi凭据，可以手动连接
  // otaLib.getWiFiManager()->connectToWiFi("YourSSID", "YourPassword");

  Serial.println("设备启动完成!");
  Serial.println("访问 http://" + WiFi.localIP().toString() + " 进入Web界面");
}

void loop() {
  // 处理库的循环任务
  otaLib.handle();

  // 你的代码...
}
```

## 模块说明

库由以下主要模块组成：

- **OTAManager**: 处理固件和文件系统的无线更新
- **WiFiManager**: 管理 WiFi 连接、AP 模式和凭据存储
- **WebSocketManager**: 处理 WebSocket 通信，提供实时数据传输
- **WebServerManager**: 管理 Web 服务器和 API 路由
- **SystemMonitor**: 监控系统状态和性能
- **StatusIndicator**: 使用 LED 指示系统状态

## Web 界面

库包含三个主要的 Web 界面：

1. **首页/WiFi 设置页面**：用于设置和管理 WiFi 连接
2. **更新页面**：用于上传和安装新的固件或文件系统
3. **串口监控页面**：实时显示设备日志和状态信息

## API 文档

### 主类

```cpp
ESP32_OTA_WS_Lib(const char* deviceName, const char* firmwareVersion, uint16_t webServerPort, uint16_t wsServerPort, uint8_t ledPin);
bool begin(bool mountFS = true);
void handle();
void enableApiMonitoring(bool enable);
void setRebootInterval(int hours);
void broadcastMessage(const String &message);
```

### OTA 管理器

```cpp
void handleFirmwareUpdate(void* request, const String& filename, size_t index, uint8_t *data, size_t len, bool final);
void handleFilesystemUpdate(void* request, const String& filename, size_t index, uint8_t *data, size_t len, bool final);
```

### WiFi 管理器

```cpp
bool connectToWiFi(const char* ssid, const char* password, uint32_t timeout = 30000);
void startAPMode();
String scanNetworks();
void saveWiFiCredentials(const char* ssid, const char* password);
```

### WebSocket 管理器

```cpp
void broadcastTXT(const String &text);
void sendTXT(uint8_t num, const String &text);
```

### 状态指示器

```cpp
void setMode(LedMode mode);
void triggerApiActivity();
void setColor(uint8_t r, uint8_t g, uint8_t b);
```

## 高级用法示例

请参考 `examples` 目录中的示例，了解更多高级用法：

- **BasicOTA**: 基本的 OTA 更新功能演示
- **CompleteManager**: 展示库的所有功能
- **CustomAPI**: 如何添加自定义 API 端点

## 贡献

欢迎提交问题和改进建议！如果你想为项目做出贡献，请遵循以下步骤：

1. Fork 这个仓库
2. 创建你的特性分支 (`git checkout -b feature/amazing-feature`)
3. 提交你的更改 (`git commit -m 'Add some amazing feature'`)
4. 推送到分支 (`git push origin feature/amazing-feature`)
5. 开启一个 Pull Request

## 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件

## 联系方式

MrQ - mike7830705@gmail.com

项目链接: [https://github.com/QooHsuan/ESP32_OTA_WS_Lib](https://github.com/QooHsuan/ESP32_OTA_WS_Lib)
