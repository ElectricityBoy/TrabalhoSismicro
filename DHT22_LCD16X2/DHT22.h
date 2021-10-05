
#define DHT_DDR DDRD
#define DHT_PORT PORTD
#define DHT_PIN	PIND
#define PIN 2

void DHT22_init(void)
{
	DHT_DDR |= (1<<PIN);		//PIN COMO SALIDA
	DHT_PORT |= (1<<PIN);		//NIVEL ALTO
}

/*		  18ms	   40us	    80us	80us
	¯¯¯¯|_______|¯¯¯¯¯¯¯¯|_______|¯¯¯¯¯¯¯|....datos....
	
	---------PUC---------|----------dht22-------------|      */					

uint8_t DHT22_read(float *dht_temperatura, float *dht_humedad)
{
	uint8_t bits[5];
	uint8_t i,j=0;
	uint8_t contador = 0;
	
	//Paso 1, enviar un puslo en bajo durante18ms 
	
	DHT_PORT &= ~(1<<PIN);		//Nivel bajo
	_delay_ms(18);
	DHT_PORT |= (1<<PIN);		//Nivel alto
	DHT_DDR &= ~(1<<PIN);		//Pin como entrada
	
	//Paso 2 esperamos 20 a 40us hasta que el dht22 envie 0
	contador = 0;
	while(DHT_PIN & (1<<PIN))
	{
		_delay_us(2);
		contador += 2;
		if (contador > 60)
		{
			DHT_DDR |= (1<<PIN);	//Pin como salida
			DHT_PORT |= (1<<PIN);	//Nivel alto
			return 0;
		}	
	}	
	
	//Paso 3 esperamos 80us hasta que el dht22 envie 1
	contador = 0;
	while(!(DHT_PIN & (1<<PIN)))
	{
		_delay_us(2);
		contador += 2;
		if (contador > 100)
		{
			DHT_DDR |= (1<<PIN);	//Pin como salida
			DHT_PORT |= (1<<PIN);	//Nivel alto
			return 0;
		}	
	}
	
	//Paso 4 esperamos 80us hasta que el dht22 envie 0
	contador = 0;
	while(DHT_PIN & (1<<PIN))
	{
		_delay_us(2);
		contador += 2;
		if (contador > 100)
		{
			DHT_DDR |= (1<<PIN);	//Pin como salida
			DHT_PORT |= (1<<PIN);	//Nivel alto
			return 0;
		}
	}
	
	//Paso 5 leemos los 40 bits o 5 bytes
	for (j=0; j<5; j++)
	{
	uint8_t result=0;
		for (i=0; i<8; i++)
		{
			while (!(DHT_PIN & (1<<PIN)));
				_delay_us(35);

			if (DHT_PIN & (1<<PIN))
				result |= (1<<(7-i));
					
			while(DHT_PIN & (1<<PIN));
		}
		bits[j] = result;
	}

	DHT_DDR |= (1<<PIN);	//Pin como salida
	DHT_PORT |= (1<<PIN);	//Nivel alto
	
	//Paso 6 convertir la temperatura y humedad
	
	/*Ejemplo	humedad 25.8  
					258
	*/			
	
	if ((uint8_t) (bits[0] + bits[1] + bits[2] +bits[3]) == bits[4])		//Pregunta por el chekin
	{
		uint16_t rawhumedad = bits[0]<<8 | bits[1];
		uint16_t rawtemperatura = bits[2] <<8 | bits[3];
	
		
		if (rawtemperatura & 0x8000)
		{
			*dht_temperatura = (float)((rawtemperatura & 0x7fff) / 10.0)* -1.0;
		}else{
			*dht_temperatura = (float)(rawtemperatura)/10.0;
		}

		*dht_humedad = (float)(rawhumedad)/10.0;

		return 1;
	}
	return 1;
}