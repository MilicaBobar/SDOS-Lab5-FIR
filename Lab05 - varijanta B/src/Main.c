#include "ADDS_21479_EzKit.h"
#include <filter.h>
#include <cycle_count.h>
#include <string.h>
#include <math.h>

// Velicina bloka (1, 16, 64, 256)
#define N 256
#define L 128 // duzina filtra
#define PI_F 3.14159265f

// Koeficijenti u PM memoriji
float pm h[L];

// Delay linija (stanje filtra)
float dm delay_line[L + 1];



cycle_t start_count, final_count;


extern float ping_in[], pong_in[], ping_out[], pong_out[];
extern volatile int block_ready;
extern int use_ping_for_rx;

void main()

{

	int i;
	float sum = 0.0f;



	adi_initComponents();
	initPLL();
	initExternalMemory();
	InitDAI();
	init1939viaSPI();

	enable_SPORT01_MCM_mode();

	adi_int_InstallHandler(ADI_CID_P3I, process_AD1939_samples, 0, true);

	*pSPMCTL0 |= MCEA;
	*pSPMCTL1 |= MCEA;

	// Niskopropusni FIR (windowed-sinc, Hamming), fs = 48 kHz, fc = 3 kHz
	{

		float wc = 2.0f * PI_F * 3000.0f / 48000.0f; // za drugu granicu promijeni 3000.0f

		float M = (L - 1) / 2.0f;

		for (i = 0; i < L; i++) {

			float n = (float) i - M;

			float s = sinf(wc * n) / (PI_F * n);

			float w = 0.54f - 0.46f * cosf(2.0f * PI_F * (float) i / (L - 1));

			h[i] = s * w;

			sum += h[i];

		}

		for (i = 0; i < L; i++) {

			h[i] /= sum; // pojacanje 1 na niskim frekvencijama

		}

	}

	memset(delay_line, 0, sizeof(delay_line));

	for (;;)

	{

		if (block_ready == 1) {

			START_CYCLE_COUNT(start_count);

			// Ako prekid puni 'pong', obradjujemo 'ping' (i obrnuto)
			if (use_ping_for_rx == 0) {
				fir(ping_in, ping_out, h, delay_line, N, L);
			}
			else {
				fir(pong_in, pong_out, h, delay_line, N, L);
			}

			STOP_CYCLE_COUNT(final_count, start_count);
			block_ready = 0;

		}

		asm("idle;");

	}

}
