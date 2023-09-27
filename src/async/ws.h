#pragma once
#include "../config.hpp"  // Подключение конфигурационного файла
#include "../macro.hpp"   // Подключение файла с макросами

// Проверка условной компиляции
#ifdef GH_ESP_BUILD
#ifdef GH_NO_WS
class HubWS {
   public:
}; // Пустой класс для случая, если WebSocket не нужен
#else

#include <Arduino.h>            // Arduino API
#include <ESPAsyncWebServer.h>  // Асинхронный веб-сервер

#include "../utils/stats.h"     // Утилиты статистики

class HubWS {
    // ============ PROTECTED =============
   protected:
    // Конструктор
    HubWS() : server(GH_WS_PORT), ws("/") {
        server.addHandler(&ws);  // Добавление WebSocket обработчика
    }

    // Чисто виртуальные методы для переопределения
    virtual void parse(char* url, GHconn_t from) = 0;
    virtual void sendEvent(GHevent_t state, GHconn_t from) = 0;

    // Инициализация WebSocket
    void beginWS() {
        // Установка обработчика событий для WebSocket
        ws.onEvent([this](GH_UNUSED AsyncWebSocket* server, GH_UNUSED AsyncWebSocketClient* client, AwsEventType etype, void* arg, uint8_t* data, size_t len) {
            switch (etype) {
                case WS_EVT_CONNECT: // когда клиент вошел в систему,
                    sendEvent(GH_CONNECTED, GH_WS); // Подключение установлено
                    break;

                case WS_EVT_DISCONNECT: //когда клиент вышел из системы
                    sendEvent(GH_DISCONNECTED, GH_WS); // Подключение разорвано
                    break;

                case WS_EVT_ERROR: //при получении сообщения об ошибке от клиента
                    sendEvent(GH_ERROR, GH_WS); // Произошла ошибка
                    break;
				
                case WS_EVT_DATA: { //когда от клиента получен пакет данных.
                    AwsFrameInfo* ws_info = (AwsFrameInfo*)arg;
                    // Проверка типа и длины данных
                    if (ws_info->final && ws_info->index == 0 && ws_info->len == len && ws_info->opcode == WS_TEXT) {
						// Сохранение clientID может привести к гонке данных, если несколько клиентов подключаются одновременно. 
						// В этом случае стоит использовать механизмы синхронизации...
                        clientID = client->id(); // Запоминаем ID клиента
                        parse((char*)data, GH_WS); // Парсим данные
                    }
                } break;

                case WS_EVT_PONG:
                    break; // Пустая операция для события ответ на запрос ping
        });

        server.begin();  // Запуск сервера
    }

    // Остановка WebSocket
    void endWS() {
        server.end();
    }

    // Чистка клиентов WebSocket
    void tickWS() {
        ws.cleanupClients();
    }

    // Отправка сообщения всем клиентам
    void sendWS(const String& answ) {
        ws.textAll(answ.c_str());
    }

    // Отправка сообщения конкретному клиенту
    void answerWS(String& answ) {
        ws.text(clientID, answ.c_str());
    }

    // ============ PRIVATE =============
   private:
    AsyncWebServer server;  // Асинхронный веб-сервер
    AsyncWebSocket ws;      // Асинхронный WebSocket
    uint32_t clientID = 0;  // ID текущего клиента
};
#endif
#endif
