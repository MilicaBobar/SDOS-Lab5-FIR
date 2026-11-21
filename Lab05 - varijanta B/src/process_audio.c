#include "ADDS_21479_EzKit.h"
#define N 256

// Ulaz: Left_Channel_In1 (IN1) ili Left_Channel_In2 (IN2)
#define IN_SAMPLE Left_Channel_In2

// Filtrirani izlaz: Out1, Out2, Out3 ili Out4
#define OUT_L Left_Channel_Out1
#define OUT_R Right_Channel_Out1

// Ping-pong baferi
float ping_in[N], pong_in[N];
float ping_out[N], pong_out[N];

// Kontrolne varijable
int buffer_index = 0;

int use_ping_for_rx = 1; // 1 = puni se ping, 0 = puni se pong

volatile int block_ready = 0;

void process_AD1939_samples(uint32_t iid, void *handlerarg)

{

	Receive_ADC_Samples();

	// Svi izlazi prvo dobiju nefiltrirani ulaz (loopback)
	Left_Channel_Out1 = IN_SAMPLE;
	Right_Channel_Out1 = IN_SAMPLE;
	Left_Channel_Out2 = IN_SAMPLE;
	Right_Channel_Out2 = IN_SAMPLE;
	Left_Channel_Out3 = IN_SAMPLE;
	Right_Channel_Out3 = IN_SAMPLE;
	Left_Channel_Out4 = IN_SAMPLE;
	Right_Channel_Out4 = IN_SAMPLE;

	// Spremanje ulaza i slanje prethodno filtriranog blok na izabrani izlaz
	if (use_ping_for_rx) {
		ping_in[buffer_index] = IN_SAMPLE;
		OUT_L = ping_out[buffer_index];
	} else {
		pong_in[buffer_index] = IN_SAMPLE;
		OUT_L = pong_out[buffer_index];
	}

	OUT_R = OUT_L;
	buffer_index++;

	// Bafer pun: zamijeni bafere i javi main petlji
	if (buffer_index >= N) {
		buffer_index = 0;
		use_ping_for_rx = !use_ping_for_rx;
		block_ready = 1;

	}

	Transmit_DAC_Samples();

}
