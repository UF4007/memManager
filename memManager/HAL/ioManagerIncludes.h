//include files
#pragma once



#ifdef ESP_PLATFORM
#define MAX_NVS_PATH 15u
#define POINTER_LENGTH_32
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/timer.h"
#include "string.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gatt_common_api.h"
#include "esp_timer.h"
#include "esp_task_wdt.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_ping.h"
#include "esp_netif.h"
#include "esp_netif_sntp.h"
#include "esp_wifi.h"
#include "esp_sntp.h"
#include "esp_ping.h"
#include "esp_mac.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include <arpa/inet.h>
#include "socket.h"
#include "ping/ping.h"
#include "ping/ping_sock.h"
#include "ESP32-Example/example_ble_server_throughput.h"
#include "ESP32-Example/netif_extern_callback.h"
#include "ESP32-Example/ble_extern_callback/ble_extern_callback.h"
#include "ESP32-Example/wifi_extern_callback/wifi_extern_callback.h"
#include "ESP32-Example/sntp_extern_callback.h"
#endif



#ifdef INC_FREERTOS_H
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"
    #ifndef IO_MULTI_THREAD
        #define IO_MULTI_THREAD
    #endif
#endif



#ifdef _WIN32
	#define NOMINMAX
	#include<windows.h>
	#define MAX_NVS_PATH MAX_PATH
    #ifndef IO_MULTI_THREAD
        #define IO_MULTI_THREAD
    #endif
	#ifdef _WIN64
		#define POINTER_LENGTH_64
	#else
		#define POINTER_LENGTH_32
	#endif
#endif
