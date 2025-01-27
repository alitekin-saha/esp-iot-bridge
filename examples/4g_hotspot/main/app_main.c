/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_bridge.h"
#include "driver/gpio.h"

static esp_err_t esp_storage_init(void)
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    return ret;
}

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName)
{
	printf("%s\n", pcTaskName);
}

volatile bool test = true;
void app_main(void)
{
    esp_storage_init();
    while(test)
    	vTaskDelay(100);
    
    /* Init and register system/core components */
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_bridge_create_all_netif();

    gpio_set_direction(GPIO_NUM_32, GPIO_MODE_OUTPUT);
    ESP_ERROR_CHECK(gpio_set_level(GPIO_NUM_32, 1));

#if defined(CONFIG_BRIDGE_DATA_FORWARDING_NETIF_SOFTAP)
    wifi_config_t wifi_cfg = {
        .ap = {
            .ssid = CONFIG_BRIDGE_SOFTAP_SSID,
            .password = CONFIG_BRIDGE_SOFTAP_PASSWORD,
        }
    };
    esp_bridge_wifi_set_config(WIFI_IF_AP, &wifi_cfg);
#endif
}
