#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>
#include <math.h>
#include "driver/i2c.h"

#include "max30102.h"
#include "ssd1306.h"
#include "font8x8_basic.h"
#include "sdkconfig.h"


static const char *TAG = "MAIN";

void master_init() {
    int i2c_master_port = 0;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,         // select GPIO specific to your project
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,         // select GPIO specific to your project
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,  // select frequency specific to your project
        // .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
    };
}
void sensor_init(){
    int i2c_slave_port_1 = I2C_SLAVE_NUM_1;
    i2c_config_t conf_slave = {
        .sda_io_num = I2C_SLAVE_SDA_IO,          // select GPIO specific to your project
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_SLAVE_SCL_IO,          // select GPIO specific to your project
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .mode = I2C_MODE_SLAVE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = 0x57,      // address of your project
    };

    i2c_dev_t dev;
    memset(&dev, 0, sizeof(i2c_dev_t));
    struct  max30102_record record;
    struct max30102_data data;

    if (max30102_readPartID(&dev) == ESP_OK) {
        ESP_LOGI(TAG, "Found MAX30102!");
    }
    else {
        ESP_LOGE(TAG, "Not found MAX30102");        
    }

    if (max30102_init(0x1F, 4, 2, 1000, 118, 4096, &record, &dev) == ESP_OK) {
        ESP_LOGI(TAG, "Init OK");
    }
    else {
        ESP_LOGE(TAG, "Init fail!");
    }
    char data_temp[16] = "";
    unsigned long red;
    unsigned long ir;    while (1)
    {
        ESP_LOGI(TAG, "xin chao");
        max30102_check(&record, &dev); //Check the sensor, read up to 3 samples

        double fred, fir;
        double Spo2 = 0;
        while (max30102_available(&record)) //do we have new data?
        {

            // printf("%d,", max30102_getFIFORed(&record));
            // printf("%d", max30102_getFIFOIR(&record));
            // printf("\n");
            red = max30102_getFIFORed(&record);
            ir = max30102_getFIFOIR(&record);

            memset(data_temp, 0, sizeof(data_temp));
            // sprintf(data_temp, "%lu,%lu\n", red, ir);
            ESP_LOGI(TAG, "IR: %lu, RED: %lu", ir, red);    
            max30102_nextSample(&record); //We're finished with this sample so move to next sample
        }
    }
}
void oled_init(){
    SSD1306_t oled;
    oled._flip = true;
    i2c_master_init(&oled, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
    ssd1306_init(&oled, 128, 64);
    ssd1306_clear_screen(&oled, false);
    ssd1306_display_text(&oled, 0, "Hello", 5, false);

}

void app_main(void)
{
    master_init();
    oled_init();
    sensor_init();

}
