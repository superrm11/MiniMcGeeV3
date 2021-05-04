#include "coms.h"

/**
 * initialize the wifi adapter, and start the socket thread
 */
void coms_install()
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "MiniMcGeeV3",
            .ssid_len = 11,
            .channel = 6,
            .password = "testing12345",
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        }
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Init mdns
    mdns_init();
    mdns_hostname_set("minimcgee3");
    mdns_instance_name_set("MiniMcGee Version 3");

    // Allocate data and start the communications thread
    socket_mux = xSemaphoreCreateMutex();
    xTaskCreate(socket_task, "SOCK_TASK", 4096, NULL, 1, socket_task_handle);
}

/**
 * Receive data from the UDP socket through a constantly running task
 */
void socket_task(void* ptr)
{
    // Define the socket as UDP
    int serverfd = socket(AF_INET, SOCK_DGRAM, 0);

    // Define the socket on port 8000, with any ipv4 address (as a server)
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8000);
    int addr_len = sizeof(addr);

    bind(serverfd, (struct sockaddr*) &addr,(socklen_t) addr_len);

    curr_data = malloc(sizeof(sock_buffer_t));
    size_t recv_size;

    while(true)
    {
        sock_buffer_t tmp_buffer;
        
        // If there is data available on the port, store the data in a temp buffer
        if((recv_size = lwip_recvfrom(serverfd, &tmp_buffer, sizeof(sock_buffer_t), 0, (struct sockaddr*) &addr, (socklen_t*) &addr_len)) > 0)
        {
            // If the data received is smaller than the buffer, discard it.
            // Wait a max of 10ms for the mutex to unlock
            if(recv_size == sizeof(sock_buffer_t) && xSemaphoreTake(socket_mux, portTICK_PERIOD_MS * 10))
            {
                (*curr_data) = tmp_buffer;
                xSemaphoreGive(socket_mux);
            }
        }

        // Sleep while waiting for new data
        usleep(1000 * 10); // 10 milliseconds
    }
}