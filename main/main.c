#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"
#include <time.h>

#include "ssd1306.h"
#include "font8x8_basic.h"

#define tag "SSD1306"

//---------------------------------------------------------------------------------------------------------------------------
//   _____ _____            _____  _    _ _____ _____  _____ 
//  / ____|  __ \     /\   |  __ \| |  | |_   _/ ____|/ ____|
// | |  __| |__) |   /  \  | |__) | |__| | | || |    | (___  
// | | |_ |  _  /   / /\ \ |  ___/|  __  | | || |     \___ \ 
// | |__| | | \ \  / ____ \| |    | |  | |_| || |____ ____) |
//  \_____|_|  \_\/_/    \_\_|    |_|  |_|_____\_____|_____/ 
//                                                          
//---------------------------------------------------------------------------------------------------------------------------							

//Solecito 32x32px
uint8_t sunny [] = 
{
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xfe, 0x7f, 0xff, 
0xff, 0xfe, 0x7f, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xfc, 0xfe, 0x7f, 0x3f, 0xfc, 0x7e, 0x7e, 0x3f, 
0xfe, 0x3f, 0xfc, 0x7f, 0xff, 0x1f, 0xf8, 0xff, 0xff, 0x90, 0xf9, 0xff, 0xff, 0xe1, 0x7f, 0xff, 
0xff, 0xc2, 0x0f, 0xff, 0xff, 0xc4, 0x07, 0xff, 0xff, 0xc4, 0x03, 0xff, 0xc0, 0xc0, 0xe3, 0x03, 
0xc0, 0xc0, 0xf1, 0x03, 0xff, 0xc4, 0x31, 0xff, 0xff, 0xc2, 0x23, 0xff, 0xff, 0xe1, 0xc3, 0xff, 
0xff, 0xf0, 0x07, 0xff, 0xff, 0x90, 0x0d, 0xff, 0xff, 0x1e, 0x38, 0xff, 0xfe, 0x3f, 0xfc, 0x7f, 
0xfc, 0x7e, 0x7e, 0x3f, 0xfc, 0xfe, 0x7f, 0x3f, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xfe, 0x7f, 0xff, 
0xff, 0xfe, 0x7f, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

//Borrador 32x32px
uint8_t sunnyEraser [] = 
{
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// Bloque metro, 32x8px
uint8_t meterBlock [] = 
{
0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x07, 0xff, 0x80, 0x00, 0x07, 0xff, 0x00, 0x00, 0x03, 0xff, 
0x00, 0x00, 0x03, 0xff, 0x80, 0x00, 0x07, 0xff, 0xc0, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff
};

// Borrador, 32x8
uint8_t eraser [] =
{
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

//---------------------------------------------------------------------------------------------------------------------------
// __      __     _____  _____          ____  _      ______  _____ 
// \ \    / /\   |  __ \|_   _|   /\   |  _ \| |    |  ____|/ ____|
//  \ \  / /  \  | |__) | | |    /  \  | |_) | |    | |__  | (___  
//   \ \/ / /\ \ |  _  /  | |   / /\ \ |  _ <| |    |  __|  \___ \ 
//    \  / ____ \| | \ \ _| |_ / ____ \| |_) | |____| |____ ____) |
//     \/_/    \_\_|  \_\_____/_/    \_\____/|______|______|_____/ 
//                                                          
//---------------------------------------------------------------------------------------------------------------------------	

//Variables de código
bool solecito = false;
bool nivelesDrawn [5] = {false, false, false, false, false};
int niveles[5] = {0, 0, 0, 0, 0};
int lectura = 0;

//Variables display OLED
SSD1306_t dev;
int center, top, bottom;
char lineChar[20];

//Variables ADC1
adc_oneshot_unit_handle_t adc1_handle;

//---------------------------------------------------------------------------------------------------------------------------
//  / ____/ __ \|  __ \|  ____|
// | |   | |  | | |  | | |__   
// | |   | |  | | |  | |  __|  
// | |___| |__| | |__| | |____ 
//  \_____\____/|_____/|______|
//                                                          
//---------------------------------------------------------------------------------------------------------------------------	

void clear_num(SSD1306_t * dev, int page, bool invert)
{
	char space[10];
	memset(space, 0x00, sizeof(space));
	ssd1306_display_text(dev, page, space, sizeof(space), invert);
}

void output_oled()
{
	//Output durísimo de valor - Check
	char valor[16];
	sprintf(valor, "      %04d", lectura);
	clear_num(&dev, center, false);
	ssd1306_contrast(&dev, 0xff);
	ssd1306_display_text(&dev, center, valor, 12, false);

	//Output durísimo de metro de barras - Check
	//Encendido
	if(niveles[0] && !nivelesDrawn[0]){
		ssd1306_bitmaps(&dev, 95, 56, meterBlock, 32, 8, true);
		nivelesDrawn[0] = true;
	}

	if(niveles[1] && !nivelesDrawn[1]){
		ssd1306_bitmaps(&dev, 95, 47, meterBlock, 32, 8, true);
		nivelesDrawn[1] = true;
	}

	if(niveles[2] && !nivelesDrawn[2]){
		ssd1306_bitmaps(&dev, 95, 38, meterBlock, 32, 8, true);
		nivelesDrawn[2] = true;
	}

	if(niveles[3] && !nivelesDrawn[3]){
		ssd1306_bitmaps(&dev, 95, 29, meterBlock, 32, 8, true);
		nivelesDrawn[3] = true;
	}

	if(niveles[4] && !nivelesDrawn[4]){
		ssd1306_bitmaps(&dev, 95, 20, meterBlock, 32, 8, true);
		nivelesDrawn[4] = true;
	}


	//Apagado
	if(!niveles[4] && nivelesDrawn[4]){
		ssd1306_bitmaps(&dev, 95, 20, eraser, 32, 8, true);
		nivelesDrawn[4] = false;
	}

	if(!niveles[3] && nivelesDrawn[3]){
		ssd1306_bitmaps(&dev, 95, 29, eraser, 32, 8, true);
		nivelesDrawn[3] = false;
	}
	if(!niveles[2] && nivelesDrawn[2]){
		ssd1306_bitmaps(&dev, 95, 38, eraser, 32, 8, true);
		nivelesDrawn[2] = false;
	}

	if(!niveles[1] && nivelesDrawn[1]){
		ssd1306_bitmaps(&dev, 95, 47, eraser, 32, 8, true);
		nivelesDrawn[1] = false;
	}

	if(!niveles[0] && nivelesDrawn[0]){
		ssd1306_bitmaps(&dev, 95, 56, eraser, 32, 8, true);
		nivelesDrawn[0] = false;
	}

	//Output durísimo de solecito - Check
	if(niveles[4] && !solecito){
		ssd1306_bitmaps(&dev, 0, 32, sunny, 32, 32, true);
		solecito = true;
	}

	else if(!niveles[4]){
		ssd1306_bitmaps(&dev, 0, 32, sunnyEraser, 32, 32, true);
		solecito = false;
	}

	//Output durísimo de logotipo - Not Check (No sé qué hay que hacer)



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
	top = 2;
	center = 3;
	bottom = 8;
	ESP_LOGI(tag, "INTERFACE is i2c");
	ESP_LOGI(tag, "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
	ESP_LOGI(tag, "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
	ESP_LOGI(tag, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
	i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);

	dev._flip = true;

	
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
	srand(time(NULL)); //Necesario solo para test

	while (1) 
	{
		adc_oneshot_read(adc1_handle, ADC_CHANNEL_4, &lectura);
		printf("%i\n", lectura);
		output();
		vTaskDelay(200);
	}
}
