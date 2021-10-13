#define F_CPU 16000000UL
#define BAUD 9600

#include <avr/io.h>
#include <util/setbaud.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <avr/eeprom.h>



#include "DEF_ATMEGA328P.h"
#include "UART.h"
#include "LCD.h"
#include "DHT22.h"
#include "adc_atmega_328.h"


int sensor = 0;
char adc[5];




int main(void)
{
	char printbuff[10];
	float temperatura;
	float humedad;
	
	
	adc_referencia(1); // Para o valor de referencia 1, REF.INTERNA 5V
	adc_iniciar();
	
	uint8_t contador=200;
	
	LCD_init();
	UART_init();
	DHT22_init();
	
    while (1) 
    {	
		
		
		contador++;
		if(contador>= 100.00){			//Para ler o DHT22 a cada 200x10ms = 2000ms e nao utilizar retardos bloqueantes de 2s
			contador=0;
			
			adc_canal(0);
			sensor = adc_start();
			itoa(sensor,adc,10);
			uint8_t status = DHT22_read(&temperatura, &humedad);
			
			if (status)
			{
			
				if (eeprom_is_ready() == 1)
				{	

						
					
					eeprom_write_word(contador,adc);
					printf("\r");
					printf("Luminosidade : %s.2\n\r",eeprom_read_word(contador));
					eeprom_write_float(contador,temperatura);
					printf("Temperatura (C�): %2.2f\n\r", eeprom_read_float(contador) );
					//printf("EEPROM : %2.2f\n\r", eeprom_read_float(contador));
					eeprom_write_float(contador,humedad);
					printf("Umidade : %.2lf\n\r", eeprom_read_float(contador));
					printf("\r");
					//Os valores registrados no terminal virtual, s�o os valores lidos atrav�s do endere�o de memoria correspondente da mem�ria EEPROM.
					
					LCD_clear();
					LCD_printf(" Temp. ");
					dtostrf(temperatura, 2, 2, printbuff);
					LCD_printf(printbuff);
					LCD_printf(" C");
					
					LCD_segunda_linea();
					LCD_printf(" Umi. ");
					dtostrf(humedad, 2, 2, printbuff);
					LCD_printf(printbuff);
					LCD_printf(" %");
				}
				
				
			}
			else
			{
				LCD_clear();
				LCD_printf("Error");
				printf("ERROR\n\r");
				//Caso n�o
				
			}
			
			
		}else{
			_delay_ms(10);
			
		}		
    }
}

