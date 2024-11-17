#include "daisy_petal.h"
#include "daisysp.h"
#include "terrarium.h"

using namespace terrarium;
using namespace daisy;
using namespace daisysp;


#define MAX_SIZE (48000 * 60 * 5) // 5 minutes of floats at 48 khz

DaisyPetal hw;

Led led1, led2;
Switch sw1;
Switch sw2;

float DSY_SDRAM_BSS buf[MAX_SIZE];



Looper looper;
bool play = false;

void Controls();

void processEffects(float input,float& output)
{
if(play)
{
    float inSig=input;
    output=inSig + looper.Process(inSig);
}
else
{
    output=input;
}
}

void AudioCallback(AudioHandle::InputBuffer      in, AudioHandle::OutputBuffer    out, size_t size)
{
	hw.ProcessAllControls();
    Controls();

	for (size_t i = 0; i < size; i++)
	{
        processEffects(in[0][i],out[0][i]);
	}
}

int main(void)
{
	hw.Init();
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAdc();

	led1.Init(hw.seed.GetPin(Terrarium::LED_1),false);
    led2.Init(hw.seed.GetPin(Terrarium::LED_2),false);

    sw1.Init(seed::D25,0.0,Switch::TYPE_MOMENTARY,Switch::POLARITY_NORMAL,Switch::PULL_UP); //FS1
    sw2.Init(seed::D26,0.0,Switch::TYPE_MOMENTARY,Switch::POLARITY_NORMAL,Switch::PULL_UP); //FS2

    looper.Init(buf,MAX_SIZE);
    looper.SetMode(Looper::Mode::NORMAL);
	hw.StartAudio(AudioCallback);
	while(1) {
    }
}

void UpdateButtons()
{
    sw1.Debounce();
    sw2.Debounce();

    if(sw1.RisingEdge())
    {
        play = true;
        looper.TrigRecord();
    }
    
    if(sw1.TimeHeldMs()>1000)
    {
        looper.Clear();
        play = false;
    }

    if(sw2.RisingEdge())
    {
        play = !play;
    }

}

//Deals with analog controls
void Controls()
{
    //drywet = hw.knob[Terrarium::KNOB_1].Process();

    UpdateButtons();

    //leds
    led1.Set(play);
    led2.Set(looper.Recording());

    led1.Update();
    led2.Update();
}



