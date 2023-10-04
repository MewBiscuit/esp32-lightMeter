#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"

#include "ssd1306.h"
#include "font8x8_basic.h"

#define tag "SSD1306"

int niveles[5] = {0, 0, 0, 0, 0};
int lectura = 0;

//Variables display OLED
SSD1306_t dev;
int center, top, bottom;
char lineChar[20];

void output_oled()
{
	//Output durísimo de valor

}

void output_leds()
{
	gpio_set_level(GPIO_NUM_25, niveles[0]);
	gpio_set_level(GPIO_NUM_26, niveles[1]);
	gpio_set_level(GPIO_NUM_27, niveles[2]);
	gpio_set_level(GPIO_NUM_14, niveles[3]);
	gpio_set_level(GPIO_NUM_12, niveles[4]);
}


void output()
{
	niveles[0] = lectura < 3000 ? 1:0; //umbral oscuro
	niveles[1] = lectura < 2500 ? 1:0; //umbral bajo
	niveles[2] = lectura < 2100 ? 1:0; //umbral medio
	niveles[3] = lectura < 1500 ? 1:0; //umbral alto
	niveles[4] = lectura < 1000 ? 1:0; //umbral deslumbrante

	output_leds();
	output_oled();
}

void setup()
{
	//Setup ADC1
	adc_oneshot_unit_handle_t adc1_handle;
	adc_oneshot_unit_init_cfg_t init_config1 = {
    	.unit_id = ADC_UNIT_1,
    	.ulp_mode = ADC_ULP_MODE_DISABLE,
	};
	ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

	adc_oneshot_chan_cfg_t config = {
    	.bitwidth = ADC_BITWIDTH_DEFAULT,
    	.atten = ADC_ATTEN_DB_11,
	};
	ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_4, &config));


	//Setup GPIO para Leds
	gpio_reset_pin(25);
	gpio_reset_pin(26);
	gpio_reset_pin(27);
	gpio_reset_pin(14);
	gpio_reset_pin(12);
	gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT);
	gpio_set_direction(GPIO_NUM_26, GPIO_MODE_OUTPUT);
	gpio_set_direction(GPIO_NUM_27, GPIO_MODE_OUTPUT);
	gpio_set_direction(GPIO_NUM_14, GPIO_MODE_OUTPUT);
	gpio_set_direction(GPIO_NUM_12, GPIO_MODE_OUTPUT);

	// Setup de display OLED
	ESP_LOGI(tag, "INTERFACE is i2c");
	ESP_LOGI(tag, "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
	ESP_LOGI(tag, "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
	ESP_LOGI(tag, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
	i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);

	
	ssd1306_init(&dev, 128, 64);
	ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
	ssd1306_display_text_x3(&dev, center, "Inic...", 5, false);
	vTaskDelay(3000 / portTICK_PERIOD_MS);
	ssd1306_clear_screen(&dev, false);

}


void app_main(void)
{

	setup();

	while (1) 
	{
		lectura = 0; //Leer valor de adc1 -----> adc_oneshot_read()
		printf("%i\n", lectura);
		output();
		vTaskDelay(200);
	}
}