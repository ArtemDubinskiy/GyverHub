// Эти директивы подключают конфигурационные файлы.
#include "../config.hpp"
#include "../macro.hpp"

// Если определены директивы GH_ESP_BUILD и GH_NO_MQTT, то используется "пустой" класс
#ifdef GH_ESP_BUILD
#ifdef GH_NO_MQTT
class HubMQTT {
   public:
    void setupMQTT(const char* host, uint16_t port, const char* login = nullptr, const char* pass = nullptr, uint8_t nqos = 0, bool nret = 0) {}
};
#else  // GH_NO_MQTT не определен, поэтому используем реальную реализацию

// Подключаем необходимые библиотеки
#include <Arduino.h>
#include <AsyncMqttClient.h>
#include "../utils/stats.h"

class HubMQTT {
   public:
    // Функции для настройки MQTT соединения
    void setupMQTT(const char* host, uint16_t port, const char* login = nullptr, const char* pass = nullptr, uint8_t nqos = 0, bool nret = 0) {
        if (!strlen(host)) return;  // Проверка на пустой хост
        mqtt.setServer(host, port);  // Установка сервера
        _setupMQTT(login, pass, nqos, nret);  // Приватный метод для дополнительных настроек
    }

    // Перегрузка для IP-адреса
    void setupMQTT(IPAddress ip, uint16_t port, const char* login = nullptr, const char* pass = nullptr, uint8_t nqos = 0, bool nret = 0) {
        mqtt.setServer(ip, port);
        _setupMQTT(login, pass, nqos, nret);
    }

    // Проверка состояния подключения
    bool online() {
        return mqtt.connected();
    }

   protected:
    // Виртуальные методы, должны быть реализованы в наследуемых классах
    virtual void parse(char* url, char* value, GHconn_t from) = 0;
    virtual const char* getPrefix() = 0;
    virtual const char* getID() = 0;
    virtual void sendEvent(GHevent_t state, GHconn_t from) = 0;

    // Методы для управления MQTT соединением
    void beginMQTT() {
        // Здесь идет настройка обработчиков событий для MQTT
        // Например, при успешном подключении, определенные топики подписываются
    }

    // Отключение от MQTT
    void endMQTT() {
        mqtt.disconnect();
    }

    // Дополнительные приватные методы и переменные
   private:
    void _setupMQTT(const char* login, const char* pass, uint8_t nqos, bool nret) {
        // Дополнительные настройки для MQTT
    }
};
#endif
#endif
