/**
 * WebSocketManager.h
 *
 * WebSocket通信管理模块
 *
 * @file WebSocketManager.h
 * @author MrQ
 * @version 1.0.0
 * @date 2023-04-20
 */

#ifndef WEBSOCKET_MANAGER_H
#define WEBSOCKET_MANAGER_H

#include <Arduino.h>
#include <WebSockets.h>
#include <WebSocketsServer.h>

/**
 * WebSocket管理器类
 *
 * 管理WebSocket通信，用于实时数据传输
 */
class WebSocketManager
{
public:
    /**
     * 构造函数
     *
     * @param port WebSocket服务器端口
     */
    WebSocketManager(uint16_t port = 81);

    /**
     * 初始化WebSocket服务器
     */
    void begin();

    /**
     * 处理WebSocket事件
     */
    void handle();

    /**
     * 广播文本消息给所有连接的客户端
     *
     * @param text 要发送的文本
     */
    void broadcastTXT(const String &text);

    /**
     * 向特定客户端发送文本消息
     *
     * @param num 客户端的编号
     * @param text 要发送的文本
     */
    void sendTXT(uint8_t num, const String &text);

    /**
     * 广播二进制数据给所有连接的客户端
     *
     * @param payload 要发送的数据
     * @param length 数据长度
     */
    void broadcastBIN(const uint8_t *payload, size_t length);

    /**
     * 向特定客户端发送二进制数据
     *
     * @param num 客户端的编号
     * @param payload 要发送的数据
     * @param length 数据长度
     */
    void sendBIN(uint8_t num, const uint8_t *payload, size_t length);

    /**
     * 设置WebSocket事件回调
     *
     * @param callback 回调函数
     */
    void onEvent(WebSocketsServer::WebSocketServerEvent callback);

    /**
     * 获取连接的客户端数量
     *
     * @return 客户端数量
     */
    size_t getClientCount();

private:
    WebSocketsServer _webSocketServer; // WebSocket服务器实例
    uint16_t _port;                    // 服务器端口

    // WebSocket 事件处理函数
    static void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
};

#endif // WEBSOCKET_MANAGER_H