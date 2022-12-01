/*
 * color_sensor.h
 *
 *  Created on: Oct 21, 2022
 *      Author: bapti
 *
 *
 */

#ifndef SRC_COLOR_SENSOR_H_
#define SRC_COLOR_SENSOR_H_

//liste des Pin en fin de fichier


// Macro necessaires a l'abstraction de la lib HAL

#define color_S2_Pin GPIO_PIN_4
#define color_S2_GPIO_Port GPIOB
#define color_input_freq_Pin GPIO_PIN_15
#define color_input_freq_GPIO_Port GPIOA
#define button_Pin GPIO_PIN_11
#define button_GPIO_Port GPIOI
#define button_EXTI_IRQn EXTI15_10_IRQn
#define color_enable_Pin GPIO_PIN_7
#define color_enable_GPIO_Port GPIOC
#define color_S0_Pin GPIO_PIN_6
#define color_S0_GPIO_Port GPIOC
#define color_S3_Pin GPIO_PIN_7
//#define color_S3_GPIO_Port GPIOG
#define color_S1_Pin GPIO_PIN_6
#define color_S1_GPIO_Port GPIOG

typedef enum tim_mode{
	INPUT_CAPTURE_IT = 1,
	BASE_IT = 2,
	PWM = 3
}tim_mode_t;

typedef enum tim_status{
	START = 1,
	STOP = 0
}tim_status_t;

void GPIO_write(GPIO_TypeDef * gpio_port,uint16_t gpio_pin,GPIO_PinState gpio_PinState );
void timer_handle(TIM_HandleTypeDef htim, tim_mode_t mode, tim_status_t status,uint32_t channel);


// Fonction du capteur couleur


typedef enum color_sensor_color_enum{
	RED =1,
	BLUE=2,
	CLEAR=3,
	GREEN=4
}color_sensor_color_t;

typedef enum color_sensor_output_scale_enum{
	CENT_POUR_CENT=4,
	VINGT_POUR_CENT=3,
	DEUX_POUR_CENT=2,
	POWER_DOWN=1
}color_sensor_output_scale_t;

typedef enum color_sensor_state_enum{
	SENSOR_ENABLE=1,
	SENSOR_DISABLE=0
}color_sensor_state_t;


/**
 * \struct h_color_sensor
 * \brief Color sensor object
 *
 * This object contains all the features of this color sensor. It allow user to manage more than 1 color sensor at a time and deal with them easily
 */

typedef struct h_color_sensor_struct{
	color_sensor_color_t color;
	color_sensor_output_scale_t ouput_scale;
	color_sensor_state_t sensor_state;
	uint32_t frequence;
}h_color_sensor_t;


void color_setOutputFreqScaling(h_color_sensor_t * h_color_sensor);
void color_enable(h_color_sensor_t * h_color_sensor);
void color_disable(h_color_sensor_t * h_color_sensor);
void color_setPhotodiodeType(h_color_sensor_t * h_color_sensor);
uint32_t color_getValue(h_color_sensor_t * h_color_sensor);
int color_handleTim3_IT(h_color_sensor_t * h_color_sensor);
void color_analyse(void);

/*  liste des Pins
 *		color_S0	GPIO_output
 *	    color_S1	GPIO_output
 *		color_S2	GPIO_output
 *		color_S3	GPIO_output
 *		color_enable GPIO_Output
 *		3,3V ou 5V (*2)
 *		GND
 *		color_freq	INPUT_CAPTURE
 *
 *		1 timer en input capture
 *	    Voir si un timer est dispo pour déclencher tout les demi secondes le changelnt de filtre ou alors 4 cycle de IC et on passe au filtre suivant : je gererai ça une fois en programation
 */

#endif /* SRC_COLOR_SENSOR_H_ */
