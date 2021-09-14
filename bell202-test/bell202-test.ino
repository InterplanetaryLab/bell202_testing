#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// Pin Definitions
#define PPT 6
#define TX_PIN 3

/*
Some baudrates and bell202 breakdown
baud rate 	126bps 	315bps 	630bps 	1225bps
Low frequency 	882Hz 	1575Hz 	3150Hz 	4900Hz
High frequency 	1764Hz 	3150Hz 	6300Hz 	7350Hz
*/

// For now low baudrate is selected 126bps
#define LOW_FREQ  882
#define HIGH_FREQ 1764
#define BIT_FREQ 126

//TX Audio Connections
AudioSynthWaveformSine   low_freq_sine_gen;    
AudioSynthWaveformSine   high_freq_sine_gen;  
AudioOutputI2S           audio_out;
AudioMixer4              mixer;

AudioConnection patch_low_sine(low_freq_sine_gen, 0, mixer, 0);
AudioConnection patch_high_sine(high_freq_sine_gen, 0, mixer, 1);
AudioConnection patch_mixer_output(mixer, 0, audio_out, 0);

// Audio Control Object
AudioControlSGTL5000 audio_shield;

// Delay using Interval Timer
IntervalTimer delay_freq_timer;

//function pre-declarations
void setup_audio();
void modulate_bit(uint8_t bit);
void modulate_byte(uint8_t byte_data);

void setup()
{
	setup_audio();
	Serial.begin(115200);
}

void loop()
{
	if (Serial.available())
	{
		uint8_t key = Serial.read();
		modulate_byte(key);
	}
		
}

// Audio Functions
void setup_audio()
{
	// push to talk configurations
	pinMode(PPT, OUTPUT);
	digitalWrite(PPT, LOW);

	// Audio Memory Config (probably too much memory for now but can be updated later).
	AudioMemory(12);

	// Audio Shield Settings
	audio_shield.enable();
	audio_shield.volume(1);
}

void modulate_bit(uint8_t bit)
{
	uint16_t freq = 0;
	if (bit == 0)
		freq = LOW_FREQ;
	else
		freq = HIGH_FREQ;
	AudioNoInterrupts();
	low_freq_sine_gen.frequency(freq);
	low_freq_sine_gen.amplitude(.5);
	AudioInterrupts();
	delay(1/BIT_FREQ);
	AudioNoInterrupts();
	low_freq_sine_gen.amplitude(0);	
	AudioInterrupts();
}

void modulate_byte(uint8_t byte_data)
{
	uint8_t i;
	for (i=0; i<< 8; i++)
	{
		uint8_t bit = ((byte_data >> i) & 0x1);
		modulate_bit(bit);	
	}
}
