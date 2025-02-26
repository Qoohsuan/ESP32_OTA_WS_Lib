/**
 * StatusIndicator.h
 *
 * 状态指示灯控制模块，用于指示系统状态
 *
 * @file StatusIndicator.h
 * @author MrQ
 * @version 1.0.0
 * @date 2023-04-20
 */

#ifndef STATUS_INDICATOR_H
#define STATUS_INDICATOR_H

#include <Arduino.h>

// 确定是否支持NeoPixel
#if defined(ESP32) || defined(ESP8266)
#define HAS_NEOPIXEL 1
#else
#define HAS_NEOPIXEL 0
#endif

#if HAS_NEOPIXEL
#include <Adafruit_NeoPixel.h>
#endif

// LED状态模式
enum LedMode
{
    LED_BOOT_ANIMATION,  // 开机动画
    LED_AP_MODE,         // AP模式(红色呼吸)
    LED_CONNECTED,       // 已连接(绿色呼吸)
    LED_API_ACTIVE,      // API活动(黄色闪烁)
    LED_OTA_IN_PROGRESS, // OTA进行中(彩虹效果)
    FIRMWARE_SUCCESS,    // 固件更新成功(蓝色闪烁)
    FILESYSTEM_SUCCESS,  // 文件系统更新成功(橙色闪烁)
    LED_ERROR            // 错误状态(红色快闪)
};

/**
 * 状态指示器类
 *
 * 管理LED灯来指示系统状态
 */
class StatusIndicator
{
public:
    /**
     * 构造函数
     *
     * @param ledPin LED引脚号
     */
    StatusIndicator(uint8_t ledPin = 8);

    /**
     * 初始化状态指示器
     */
    void begin();

    /**
     * 更新LED状态
     */
    void handle();

    /**
     * 设置LED模式
     *
     * @param mode LED状态模式
     */
    void setMode(LedMode mode);

    /**
     * 触发API活动指示
     *
     * 当API被调用时，短暂显示API活动状态
     */
    void triggerApiActivity();

    /**
     * 获取当前LED模式
     *
     * @return 当前的LED模式
     */
    LedMode getMode() const;

    /**
     * 使用特定颜色设置LED
     *
     * @param r 红色分量 (0-255)
     * @param g 绿色分量 (0-255)
     * @param b 蓝色分量 (0-255)
     */
    void setColor(uint8_t r, uint8_t g, uint8_t b);

private:
#if HAS_NEOPIXEL
    Adafruit_NeoPixel _pixels; // NeoPixel实例
#endif
    uint8_t _ledPin;                  // LED引脚
    LedMode _currentLedMode;          // 当前LED模式
    unsigned long _ledPreviousMillis; // 上次更新LED状态的时间
    int _breathBrightness;            // 呼吸效果亮度
    bool _breathIncreasing;           // 呼吸效果方向
    bool _blinkState;                 // 闪烁状态
    unsigned long _otaCompleteTime;   // OTA完成时间
    uint8_t _successBlinks;           // 成功闪烁次数
    uint16_t _rainbowHue;             // 彩虹效果色调
    unsigned long _apiActiveStart;    // API活动开始时间
    uint8_t _bootColorIndex;          // 引导动画颜色索引

    // LED效果方法
    void updateBootAnimation();
    void updateAPMode();
    void updateConnected();
    void updateAPIActive();
    void updateOTAProgress();
    void updateFirmwareSuccess();
    void updateFilesystemSuccess();
    void updateError();
};

#endif // STATUS_INDICATOR_H