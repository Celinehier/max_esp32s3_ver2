#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>
#include <esp_timer.h>
#include <esp_vfs_fat.h>
#include <freertos/ringbuf.h>

#include "math.h"
#include "max30102.h"
#include "ssd1306.h"
#include "font8x8_basic.h"


static const char *TAG = "MAIN";

TaskHandle_t readMAXTask_handle = NULL;
double avered = 0;
double aveir = 0;
double sumirrms = 0;
double sumredrms = 0;
int i = 0;
int Num = 100;  //calculate SpO2 by this sampling interval

double ESpO2 = 95.0;     //initial value of estimated SpO2
double FSpO2 = 0.7;      //filter factor for estimated SpO2
double frate = 0.95;     //low pass filter for IR/red LED value to eliminate AC component
#define TIMETOBOOT 3000  // wait for this time(msec) to output SpO2
#define SCALE 88.0       //adjust to display heart beat and SpO2 in the same scale
#define MAX_SPO2 100.0
#define MIN_SPO2 80.0
#define SAMPLING 5       //if you want to see heart beat more precisely , set SAMPLING to 1
#define FINGER_ON 30000  // if red signal is lower than this , it indicates your finger is not on the sensor
#define MINIMUM_SPO2 80.0


void max30102_oled(void *pvParameters)
{
    SSD1306_t dev1;
   	// int center, top, bottom;
	// char lineChar[20];
    i2c_dev_t dev;
    memset(&dev, 0, sizeof(i2c_dev_t));
    dev1._flip = true;

    ESP_ERROR_CHECK(max30102_initDesc(&dev, 0, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO));
	i2c_master_init(&dev1, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
    
    ssd1306_init(&dev1, 128, 64);
    ssd1306_clear_screen(&dev1, false);
	ssd1306_contrast(&dev1, 0x3C);
	ssd1306_display_text(&dev1, 0, "Hieu Le Dinh", 15, false);
    ssd1306_display_text(&dev1, 1, "Hieu Le Dinh", 15, true);

    ESP_LOGI(TAG, "hello");

    
    struct max30102_record record;
    struct max30102_data data;
    ESP_LOGI(TAG, "Hello123 ");

    // if (max30102_readPartID(&dev) == ESP_OK) {
    //     ESP_LOGI(TAG, "Found MAX30102!");
    // }
    // else {
    //     ESP_LOGE(TAG, "Not found MAX30102");        
    // }
    ESP_LOGI(TAG, "Hello123 "); 
    if (max30102_init(0x1F, 4, 2, 1000, 118, 4096, &record, &dev) == ESP_OK) {
        ESP_LOGI(TAG, "Init OK");
    }
    else {
        ESP_LOGE(TAG, "Init fail!");
    }
    uint16_t samplesTaken = 0;
    char data_temp[16] = "";
    unsigned long red;
    unsigned long ir;
        ESP_LOGI(TAG, "Le Dinh Hieu");
    while (1)
    {
        ESP_LOGI(TAG, "xin chao");
        max30102_check(&record, &dev); //Check the sensor, read up to 3 samples

        double fred, fir;
        double Spo2 = 0;
        while (max30102_available(&record)) //do we have new data?
        {
            samplesTaken++;

            // printf("%d,", max30102_getFIFORed(&record));
            // printf("%d", max30102_getFIFOIR(&record));
            // printf("\n");
            red = max30102_getFIFORed(&record);
            ir = max30102_getFIFOIR(&record);

            memset(data_temp, 0, sizeof(data_temp));
            // sprintf(data_temp, "%lu,%lu\n", red, ir);
            // ESP_LOGI(TAG, "IR: %lu, RED: %lu", ir, red);    
            max30102_nextSample(&record); //We're finished with this sample so move to next sample
               
            samplesTaken++;
            fred = (double)red;
            fir = (double)ir;
            avered = avered * frate + (double)red * (1.0 - frate);  //average red level by low pass filter
            aveir = aveir * frate + (double)ir * (1.0 - frate);     //average IR level by low pass filter
            sumredrms += (fred - avered) * (fred - avered);         //square sum of alternate component of red level
            sumirrms += (fir - aveir) * (fir - aveir);  
            if((samplesTaken % 5) == 0){
                float ir_forGraph = 2.0 * (fir - aveir) / aveir * SCALE + (MIN_SPO2 + MAX_SPO2) / 2.0;
                float red_forGraph = 2.0 * (fred - avered) / avered * SCALE + (MIN_SPO2 + MAX_SPO2) / 2.0; 
                if (ir_forGraph > 100.0) {ir_forGraph = 100.0;}
                if (ir_forGraph < 80.0) {ir_forGraph = 80.0;}
                if (red_forGraph > 100.0) {red_forGraph = 100.0;}
                if (red_forGraph < 80.0) {red_forGraph = 80.0;}
                if (ir < FINGER_ON) {
                    ESpO2 = MINIMUM_SPO2;
                    ESP_LOGE(TAG, "Cho tay vao di baby");
                }
            
            }
            if ((samplesTaken % Num) == 0){
                double R = (sqrt(sumredrms) / avered) / (sqrt(sumirrms) / aveir);
                Spo2 = -23.3 * (R - 0.4) + 100;
                i = 0;
                sumirrms = 0.0;
                sumredrms = 0.0;
                char SpO2[10];
                printf(SpO2, "%.2f %", Spo2);
                ssd1306_display_text(&dev1, 2, "hello", 15, true);
                ESP_LOGI(TAG, "SpO2 trong mau la: %f", Spo2);
                break;
            }
            
            max30102_nextSample(&record);
            vTaskDelay(50/portTICK_PERIOD_MS);
            
        }
        
        
    }
}


// Set up I2S Processor configuration


// Set I2S pin configuration


void app_main(void)
{
    // ESP_ERROR_CHECK(i2cdev_init()); 
    ESP_LOGI(TAG, "Hello");
    xTaskCreate(&max30102_oled, "max30102_oled", 2048*2, NULL, 6, NULL); 
}
