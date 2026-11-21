#include "ADDS_21479_EzKit.h"

#include <cycle_count.h>

extern float h[128]; // koeficijenti

static float x_buffer[128] = { 0.0f }; // bafer za odbirke

static int p = 0; // trenutna pozicija u baferu

cycle_t start_count, final_count;

void process_AD1939_samples(uint32_t iid, void *handlerarg)

{

	Receive_ADC_Samples();

// Ucitavanje novog odbirka sa ulaza IN1 u bafer
	x_buffer[p] = Left_Channel_In1;



	START_CYCLE_COUNT(start_count);

//  Racunanje FIR filtra

	float y = 0.0f;

	int i, j;

	for (i = 0; i < 128; i++) {

		j = p - i;

		if (j < 0) {

			j += 128; // Cirkularni pristup baferu

		}

		y += h[i] * x_buffer[j];

	}

	STOP_CYCLE_COUNT(final_count, start_count);

	p++;

	if (p >= 128) {

		p = 0;

	}

//  Slanje filtriranog signala na izlaz OUT2

	Left_Channel_Out2 = y;

	Right_Channel_Out2 = y;

// Loopback za ostale kanale

	Left_Channel_Out1 = Left_Channel_In2;

	Right_Channel_Out1 = Right_Channel_In2;

	Left_Channel_Out3 = Left_Channel_In1;

	Right_Channel_Out3 = Right_Channel_In1;

	Left_Channel_Out4 = Left_Channel_In1;

	Right_Channel_Out4 = Right_Channel_In1;

	Transmit_DAC_Samples();

}
