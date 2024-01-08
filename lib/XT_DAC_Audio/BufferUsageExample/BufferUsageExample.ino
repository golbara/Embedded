// Playing a digital WAV recording repeatadly using the XTronical DAC Audio library
// prints out to the serial monitor numbers counting up showing that the sound plays 
// independently of the main loop
// See www.xtronical.com for write ups on sound
// You will obviously need the appropriate hardware such as ESP32, audio amp and speaker, see 
// http://www.xtronical.com/basics/audio/dacs-on-esp32/ for the simple build and specifically
// 
// for the write ip on digitised speach/ sounds

#include "SoundData.h";
#include "XT_DAC_Audio.h";

XT_Wav_Class ForceWithYou(Force);     // create an object of type XT_Wav_Class that is used by 
                                      // the dac audio class (below), passing wav data as parameter.
                                      
XT_DAC_Audio_Class DacAudio(25,0);    // Create the main player class object. 
                                      // Use GPIO 25, one of the 2 DAC pins and timer 0

void setup() {
  Serial.begin(115200);
}


void loop() {
  DacAudio.FillBuffer();              //Fill the sound buffer with data
  if(ForceWithYou.Completed)          // if completed playing, play again
    DacAudio.PlayWav(&ForceWithYou);  
  DacAudio.AverageBufferUsage(); 
  delay(1);
}