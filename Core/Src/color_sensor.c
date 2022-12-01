/**
 * @file color_sensor.c
 * @brief File which contain general functions to handle the color sensor.
 * @author Baptiste Cottu
 * @version 0.1
 * @date 27/10/2022
 *
 */

#include "main.h"
#include "color_sensor.h"
#include <string.h>
#include <stdio.h>

extern UART_HandleTypeDef huart1;		// UART de debug
extern TIM_HandleTypeDef htim14;			//<! Timer en imput capture
//extern TIM_HandleTypeDef htim3;			//<! Timer de gestion des filtres

uint32_t color_tab[2];
int color			= 0;
uint32_t freq		= 0;
int tim				= 0;

/**
 * @fn int color_set_output_freq_scaling(h_color_sensor_t * h_color_sensor)
 * @brief Set the scale of the output signal
 *
 * @param h_color_sensor : color_sensor structure
 * @return 0 if failed else 1
 */
void color_setOutputFreqScaling(h_color_sensor_t * h_color_sensor){
	switch(h_color_sensor->ouput_scale){
		case 4:
			// 100%
			GPIO_write(color_S0_GPIO_Port,color_S0_Pin,1);
			GPIO_write(color_S0_GPIO_Port,color_S0_Pin,1);
			break;
		case 3:
			// 20%
			GPIO_write(color_S0_GPIO_Port,color_S0_Pin,1);
			GPIO_write(color_S0_GPIO_Port,color_S0_Pin,0);
			break;
		case 2:
			// 2%
			GPIO_write(color_S0_GPIO_Port,color_S0_Pin,0);
			GPIO_write(color_S0_GPIO_Port,color_S0_Pin,1);
			break;
		case 1:
			// Power_down
			GPIO_write(color_S0_GPIO_Port,color_S0_Pin,0);
			GPIO_write(color_S0_GPIO_Port,color_S0_Pin,0);
			break;
	}
}

/**
 * @fn int color_Disable(h_color_sensor_t * h_color_sensor)
 * @brief disable the sensor by setting to 1 the ENABLE pin
 *
 * @param h_color_sensor : color_sensor structure
 * @return 0 if failed else 1
 */
void color_disable(h_color_sensor_t * h_color_sensor){
	GPIO_write(color_enable_GPIO_Port,color_enable_Pin, 1);
	h_color_sensor->sensor_state=SENSOR_DISABLE;
}

/**
 * @fn int color_Enable(h_color_sensor_t * h_color_sensor)
 * @brief Enable the sensor by setting to 0 the ENABLE pin
 *
 * @param h_color_sensor : color_sensor structure
 * @return 0 if failed else 1
 */
void color_enable(h_color_sensor_t * h_color_sensor){
	GPIO_write(color_enable_GPIO_Port,color_enable_Pin, 0);
	h_color_sensor->sensor_state=SENSOR_ENABLE;
}

/**
 * @fn int color_set_photodiode_type(h_color_sensor_t * h_color_sensor)
 * @brief Set the photoreceptor of the sensor. Each sensor can detect color only one by one.
 * User have to set different types of filters to detect the roght color
 *
 * @param h_color_sensor : color_sensor structure
 * @return 0 if failed else 1
 */
void color_setPhotodiodeType(h_color_sensor_t * h_color_sensor){
	switch(h_color_sensor->color){
		case 4:
			//GREEN
			GPIO_write(color_S2_GPIO_Port,color_S2_Pin,1);
			//GPIO_write(color_S3_GPIO_Port,color_S3_Pin,1);

			break;
		case 3:
			//clear
			GPIO_write(color_S2_GPIO_Port,color_S2_Pin,1);
			//GPIO_write(color_S3_GPIO_Port,color_S3_Pin,0);
			break;
		case 2:
			//BLUE
			GPIO_write(color_S2_GPIO_Port,color_S2_Pin,0);
			//GPIO_write(color_S3_GPIO_Port,color_S3_Pin,1);
			break;
		case 1:
			//RED
			GPIO_write(color_S2_GPIO_Port,color_S2_Pin,0);
			//GPIO_write(color_S3_GPIO_Port,color_S3_Pin,0);
			break;
	}
}

/**
 * @fn uint32_t color_get_value(h_color_sensor_t * h_color_sensor)
 * @brief Update the frequecy value into color_tab[] from a variable set during the interruption.
 *
 * @param h_color_sensor : color_sensor structure
 * @return uint32_t frequency
 */
uint32_t color_getValue(h_color_sensor_t * h_color_sensor){
	freq=h_color_sensor->frequence;
	return h_color_sensor->frequence;
}

/**
 * @fn int color_handle_tim3_IT(h_color_sensor_t * h_color_sensor)
 * @brief This function is for managing color filters change. To be able to detect the color,
 * user have to set the two filter one after the other and compare the two results
 *
 * @param h_color_sensor : color_sensor structure
 * @return 1
 */
int color_handleTim3_IT(h_color_sensor_t * h_color_sensor){
	if (color==0){
		color_disable(h_color_sensor);
		h_color_sensor->color=GREEN;
		printf("vert\r\n");
		color_setPhotodiodeType(h_color_sensor);
		color_enable(h_color_sensor);
		color=1;
	}
	else if(color==1){
		color_tab[color-1]=color_getValue(h_color_sensor);
		color_disable(h_color_sensor);
		h_color_sensor->color=RED;
		printf("rouge\r\n");
		color_setPhotodiodeType(h_color_sensor);
		color_enable(h_color_sensor);
		color=2;
	}
	else if(color==2){
		color_tab[color-1]=color_getValue(h_color_sensor);
		color_disable(h_color_sensor);
		color=0;
	}
	tim++;
	if(tim==3){
		printf("\r\n");
		color_analyse();
		tim=0;
		timer_handle(htim14, INPUT_CAPTURE_IT,STOP,TIM_CHANNEL_1);
	}
	return 1;
}

/**
 * @fn void color_analyse(void)
 * @brief This function compares the results in color_tab. It occurs after the two mesurements are done.
 *
 * @param h_color_sensor : color_sensor structure
 * @return : Printf the color of the can
 */
void color_analyse(void){
	printf("color_tab[0]=%ld \r\n",color_tab[0]);
	printf("color_tab[1]=%ld \r\n",color_tab[1]);
	if(color_tab[0]>color_tab[1]){
		printf("c'est rouge\r\n");
	}
	else {
		printf("c'est vert\r\n");
	}
	//HAL_TIM_Base_Stop(&htim3);
	//color_Disable();
}





// Fonction d'abstraction de la lib HAL






/**
 * @fn void timer_handle(TIM_HandleTypeDef htim, tim_mode_t mode, tim_status_t status,uint32_t channel)
 * @brief Allow user to activate timer without using the buil-din funtions of STM. If the soft is to run on another hard device,
 * change only the build-in function of this timer_handle() instead of going through the code and change everithing.
 *
 * @param TIM_HandleTypeDef htim : structure of the timer you want to deal with
 * tim_mode_t mode : htim configuration accirding to what is in .ioc file (INPUT_CAPTURE_IT, BASE_IT, PWM)
 * tim_status_t status : same but START,STOP
 * uint32_t channel ; the output channel of htim
 * @return none
 */
void timer_handle(TIM_HandleTypeDef htim, tim_mode_t mode, tim_status_t status,uint32_t channel){
	if(status==START){
		switch(mode){
		case INPUT_CAPTURE_IT:
			HAL_TIM_IC_Start_IT(&htim, channel);
			break;
		case PWM:
			HAL_TIM_PWM_Start(&htim,channel);
			break;
		case BASE_IT:
			HAL_TIM_Base_Start_IT(&htim);
			break;
		default :
			break;
		}
	}
	else if (status==STOP){
		switch(mode){
		case INPUT_CAPTURE_IT:
			HAL_TIM_IC_Stop_IT(&htim, channel);
			break;
		case PWM:
			HAL_TIM_PWM_Stop(&htim,channel);
			break;
		case BASE_IT:
			HAL_TIM_Base_Stop_IT(&htim);
			break;
		default :
			break;
		}
	}
}

/**
 * @fn int GPIO_write(GPIO_TypeDef * gpio_port,uint16_t gpio_pin,GPIO_PinState gpio_PinState )
 * @brief Allow user to set GPIO Pin without using the buil-din funtions of STM. If the soft is to run on another hard device,
 * change only the build-in function of this GPIO_write() instead of going through the code and change everithing.
 *
 * @param GPIO_TypeDef * gpio_port : the port of the GPIO you want to deal with
 * uint16_t gpio_pin : The pin of the GPIO you want to deal with
 * GPIO_PinState gpio_PinState : the state you want your GPIO to have (1 or 0)
 *
 * @return 0 if fail else 1
 */
void GPIO_write(GPIO_TypeDef * gpio_port,uint16_t gpio_pin,GPIO_PinState gpio_PinState ){
	HAL_GPIO_WritePin(gpio_port,gpio_pin,gpio_PinState);
}
