#include "ADDS_21479_EzKit.h"

#include <math.h> // sinf, cosf

#define PI_F 3.14159265f

// Koeficijenti FIR filtra (128 koeficijenata)

float h[128];

void main()

{

	int k;

	float sum = 0.0f;

	adi_initComponents();

	initPLL();

	initExternalMemory();

	InitDAI();

	init1939viaSPI();

	enable_SPORT01_MCM_mode();

// Niskopropusni FIR (windowed-sinc, Hamming), fs = 48 kHz, fc = 3 kHz
	{

		float wc = 2.0f * PI_F * 3000.0f / 48000.0f;

		float M = (128 - 1) / 2.0f;

		for (k = 0; k < 128; k++) {

			float n = (float) k - M;

			float s = sinf(wc * n) / (PI_F * n);

			float w = 0.54f - 0.46f * cosf(2.0f * PI_F * (float) k / (128 - 1));

			h[k] = s * w;

			sum += h[k];

		}

		for (k = 0; k < 128; k++) {

			h[k] /= sum; // pojacanje 1 na niskim frekvencijama

		}

	}

// Prijavljivanje prekidne rutine
	adi_int_InstallHandler(ADI_CID_P3I, process_AD1939_samples, 0, true);

	*pSPMCTL0 |= MCEA;
	*pSPMCTL1 |= MCEA;

	for (;;)

	{

		asm("idle;");
		// Procesor spava, sve se radi u prekidu

	}

}
