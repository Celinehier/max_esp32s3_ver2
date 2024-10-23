/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/ringbuf.h"
// #include "string.h"
// #include "esp_system.h"
// #include "esp_spi_flash.h"
// #include "max30102/max30102.h"
// #include "esp_log.h"

// #define I2C_SDA 5
// #define I2C_SCL 6
// #define I2C_FRQ 100000
// #define I2C_PORT I2C_NUM_0
// // RingbufHandle_t buf_handle_max;
// // static char data_max[400] = "";

// // static const char *TAG = "MAIN";
// max30102_config_t max30102 = {};

// esp_err_t i2c_master_init(i2c_port_t i2c_port){
//     i2c_config_t conf = {};
//     conf.mode = I2C_MODE_MASTER;
//     conf.sda_io_num = I2C_SDA;
//     conf.scl_io_num = I2C_SCL;
//     conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
//     conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
//     conf.master.clk_speed = I2C_FRQ;
//     i2c_param_config(i2c_port, &conf);
//     return i2c_driver_install(i2c_port, I2C_MODE_MASTER, 0, 0, 0);
// }

// void get_bpm(void* param) {
//     printf("MAX30102 Test\n");
//     int BPG;
//     int SpO2;

//     char data_temp[16] = "";
//     max30102_data_t result = {};
//     /*ESP_ERROR_CHECK(max30102_print_registers(&max30102));*/
//     while(true) {
//         //Update sensor, saving to "result"
//         ESP_ERROR_CHECK(max30102_update(&max30102, &result));
//         if(result.pulse_detected) {
//             printf("BEAT\n");
//             printf("BPM: %f | SpO2: %f%%\n", result.heart_bpm, result.spO2);

//         //     BPG = (int) result.heart_bpm;
//         //     SpO2 = (int) result.spO2;

//         //     // ESP_LOGI(TAG, "%d %d", BPG, SpO2);

//         //     memset(data_temp, 0, sizeof(data_temp));
//         //     sprintf(data_temp, "%d, %d\n", BPG, SpO2);
//         //     strcat(data_max, data_temp);
//         // }

//         //Update rate: 100Hz
//         vTaskDelay(10/portTICK_PERIOD_MS);
//         // xRingbufferSend(buf_handle_max, data_max, sizeof(data_max), 2000/portTICK_PERIOD_MS);
//         // memset(data_max, 0, sizeof(data_max));
//     }
// }

// // void receive_oled(void* para){
// //     int BPM;
// //     int SpO2;
// //     while (1)
// //     {
// //         size_t item_size;

// //         char *item = (char *)xRingbufferReceive(buf_handle_max, &item_size, 2000/portTICK_PERIOD_MS);
// //         if(item != NULL){
// //             vRingbufferReturnItem(buf_handle_max, (void *)item);
// //             sscanf(item, "%d, %d", &BPM, &SpO2);
// //             if(BPM > 200){
// //                 ESP_LOGI(TAG, "Cho tay vao");
// //             }
// //             else{
// //             ESP_LOGI(TAG, "BPM: %d | SpO2: %d %%", BPM, SpO2);
// //             }
// //         }
// //     }
    
// // }

// void app_main(void)
// // {
// //     buf_handle_max = xRingbufferCreate(1028 * 6, RINGBUF_TYPE_NOSPLIT);

// //     if(buf_handle_max == NULL) 
// //     {
// //         ESP_LOGE(__func__, "Ring buffers create fail");
// //     }
// //     else
// //     {
// //         ESP_LOGI(__func__, "Ring buffers create OK");
// //     }

//     //Init I2C_NUM_0
//     ESP_ERROR_CHECK(i2c_master_init(I2C_PORT));
//     //Init sensor at I2C_NUM_0
//     ESP_ERROR_CHECK(max30102_init( &max30102, I2C_PORT,
//                    MAX30102_DEFAULT_OPERATING_MODE,
//                    MAX30102_DEFAULT_SAMPLING_RATE,
//                    MAX30102_DEFAULT_LED_PULSE_WIDTH,
//                    MAX30102_DEFAULT_IR_LED_CURRENT,
//                    MAX30102_DEFAULT_START_RED_LED_CURRENT,
//                    MAX30102_DEFAULT_MEAN_FILTER_SIZE,
//                    MAX30102_DEFAULT_PULSE_BPM_SAMPLE_SIZE,
//                    MAX30102_DEFAULT_ADC_RANGE, 
//                    MAX30102_DEFAULT_SAMPLE_AVERAGING,
//                    MAX30102_DEFAULT_ROLL_OVER,
//                    MAX30102_DEFAULT_ALMOST_FULL,
//                    false ));
    
//     //Start test task
//     xTaskCreate(get_bpm, "Get BPM", 1024 * 5, NULL, 1, NULL);
//     // xTaskCreate(receive_oled, "Receive", 1024 * 5, NULL, 1, NULL);
// }
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "max30102/max30102.h"

#define I2C_SDA 5
#define I2C_SCL 6
#define I2C_FRQ 100000
#define I2C_PORT I2C_NUM_0

max30102_config_t max30102 = {};

esp_err_t i2c_master_init(i2c_port_t i2c_port){
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_SDA;
    conf.scl_io_num = I2C_SCL;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_FRQ;
    i2c_param_config(i2c_port, &conf);
    return i2c_driver_install(i2c_port, I2C_MODE_MASTER, 0, 0, 0);
}

void get_bpm(void* param) {
    printf("MAX30102 Test\n");
    max30102_data_t result = {};
    /*ESP_ERROR_CHECK(max30102_print_registers(&max30102));*/
    while(true) {
        //Update sensor, saving to "result"
        ESP_ERROR_CHECK(max30102_update(&max30102, &result));
        if(result.pulse_detected) {
            printf("BEAT\n");
            printf("BPM: %f | SpO2: %f%%\n", result.heart_bpm, result.spO2);
        }
        //Update rate: 100Hz
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}



void app_main(void)
{
    //Init I2C_NUM_0
    ESP_ERROR_CHECK(i2c_master_init(I2C_PORT));
    //Init sensor at I2C_NUM_0
    ESP_ERROR_CHECK(max30102_init( &max30102, I2C_PORT,
                   MAX30102_DEFAULT_OPERATING_MODE,
                   MAX30102_DEFAULT_SAMPLING_RATE,
                   MAX30102_DEFAULT_LED_PULSE_WIDTH,
                   MAX30102_DEFAULT_IR_LED_CURRENT,
                   MAX30102_DEFAULT_START_RED_LED_CURRENT,
                   MAX30102_DEFAULT_MEAN_FILTER_SIZE,
                   MAX30102_DEFAULT_PULSE_BPM_SAMPLE_SIZE,
                   MAX30102_DEFAULT_ADC_RANGE, 
                   MAX30102_DEFAULT_SAMPLE_AVERAGING,
                   MAX30102_DEFAULT_ROLL_OVER,
                   MAX30102_DEFAULT_ALMOST_FULL,
                   false ));
    
    //Start test task
    xTaskCreate(get_bpm, "Get BPM", 8192, NULL, 1, NULL);
}