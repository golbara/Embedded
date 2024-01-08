// XTronical DAC Audio Library, currently supporting ESP32
// May work with other processors and/or DAC's with or without modifications
// (c) XTronical 2018, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty given
// See www.xtronical.com for documentation
// This software is currently under active development (Jan 2018) and may change and break your code with new versions
// No responsibility is taken for this. Stick with the version that works for you, if you need newer commands from later versions
// you will have to alter your original code to work with the new if required.


#define BUFFER_SIZE 600						        // Size of buffer to store data to send to DAC. 3 bytes minimum!
													// 600 bytes (Default) should allow for very slow main loops
													// that repeat only about every 100Hz, If your main loop is much faster
													// than this and the VAST majority will be you can safely reduce this size
													// without any issues down to an absolute minimum of 3 bytes
													// If you experience sound slowing down then you need to increase
													// Buffer memory.
													// If your main loop is very very slow but you want to keep buffer
													// memory low then consider calling the DAC_Audio_Class FillBuffer
													// routine more than once in you main loop. Although to be affective
													// they should not be called in quick succession but farther apart in
													// time on your main loop.
													// Use the routine MaxBufferUsage in your main loop to output the max
													// buffer memory (via serial monitor) that your loop is using, you can
													// then set you buffer memory very precisely

// forward ref for compiler.
class XT_PlayListItem_Class;

// The Main Wave class for sound samples
class XT_Wav_Class
{
  public:      
  uint16_t SampleRate;  
  volatile uint32_t DataSize=0;                         // The last integer part of count
  volatile uint32_t DataIdx=44;
  volatile unsigned char *Data;  
  volatile float IncreaseBy=0;                          // The amount to increase the counter by per call to "onTimer"
  volatile float Count=0;                               // The counter counting up, we check this to see if we need to send
  volatile int32_t LastIntCount=-1;                     // The last integer part of count
  volatile bool Completed=true;
  volatile bool Mix=true;								// Should sound be mixed with others or stop all and play on its own
  volatile XT_PlayListItem_Class *ParentPlayListItem;
  volatile uint8_t LastValue;							// Last value returned from NextByte function
  XT_Wav_Class(unsigned char *WavData);
  uint8_t NextByte();
};


class XT_PlayListItem_Class
{
	// These are the linked list items for the sounds to play through the DAC
	public:
	XT_PlayListItem_Class *PreviousItem=0;							// used in linked list of wavs
	XT_PlayListItem_Class *NextItem=0;								// used in linked list of wavs
	XT_Wav_Class *Wav;
};


class XT_DAC_Audio_Class
{
  // the main class for using the DAC to play sounds
  
	public:
		volatile XT_PlayListItem_Class *LastPlayListItem=0;          	// last play list item to play in linked list

		XT_DAC_Audio_Class(uint8_t TheDacPin, uint8_t TimerNo);

		void FillBuffer();												// Fills the buffer of values to send to the DAC, put in your
																		// main loop, see BUFFER_SIZE comments at top of code.
		uint8_t MixBytesToPlay();
		void PlayWav(XT_Wav_Class *Wav);
		void PlayWav(XT_Wav_Class *Wav,bool Mix);
		void StopAllSounds();
		void RemoveFromPlayList(volatile XT_PlayListItem_Class *ItemToRemove);
		void AverageBufferUsage();
	

};

                                                          