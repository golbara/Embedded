// XTronical DAC Audio Library, currently supporting ESP32
// May work with other processors and/or DAC's with or without modifications
// (c) XTronical 2018, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty given
// See www.xtronical.com for documentation
// This software is currently under active development (Jan 2018) and may change and break your code with new versions
// No responsibility is taken for this. Stick with the version that works for you, if you need newer commands from later versions
// you will have to alter your original code


#include "esp32-hal-timer.h";
#include "XT_DAC_Audio.h";
#include "HardwareSerial.h";

// These globals are used because the interrupt routine was a little unhappy and unpredictable
// using objects, with kernal panics etc. I think this is a compiler / memory tracking issue
// For now any vars inside the interrupt are kept as simple globals

volatile uint32_t NextFillPos=0;								// position in buffer of next byte to fill
volatile int32_t NextPlayPos=0;									// position in buffer of next byte to play
volatile int32_t EndFillPos=BUFFER_SIZE;						// position in buffer of last byte+1 that can be filled
volatile uint8_t LastDacValue;									// Next Idx pos in buffer to send to DAC
volatile uint8_t Buffer[BUFFER_SIZE];							// The buffer to store the data that will be sent to the 
volatile uint8_t DacPin;                              			// pin to send DAC data to, presumably one of the DAC pins!
volatile XT_PlayListItem_Class *FirstPlayListItem=0;          	// first play list item to play in linked list
volatile uint16_t BufferUsedCount=0;							// how much buffer used since last buffer fill

// because of the difficulty in passing parameters to interrupt handlers we have a global
// object of this type that points to the object the user creates.
XT_DAC_Audio_Class *XTDacAudioClassGlobalObject;       

// interrupt stuff
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;


// The main interrupt routine called 50,000 times per second
void IRAM_ATTR onTimer() 
{ 
	// Sound playing code, plays whatevers in the buffer.
	
	if(NextPlayPos!=NextFillPos)					// If not up against the next fill position then valid data to play
	{     
		if(LastDacValue!=Buffer[NextPlayPos])		// Send value to DAC only of changed since last value else no need
		{
			// value to DAC has changed, send to actual hardware, else we just leave setting as is as it's not changed
			LastDacValue=Buffer[NextPlayPos];
			dacWrite(DacPin,LastDacValue);			// write out the data				  
		}
		NextPlayPos++;								// Move play pos to next byte in buffer
		if(NextPlayPos==BUFFER_SIZE)				// If gone past end of buffer, 
			NextPlayPos=0;							// set back to beginning
		EndFillPos=NextPlayPos-1;					// Move area where we can fill up to to NextPlayPos
		if(EndFillPos<0)							// check if less than zero, if so then 1 less is 
			EndFillPos=BUFFER_SIZE;					// BUFFER_SIZE (we can fill up to one less than this)
	}
	if(FirstPlayListItem!=0)  // sounds in Q
		BufferUsedCount++;
}

void XT_DAC_Audio_Class::AverageBufferUsage()
{
	// returns the average buffer usage over 50 iterations of your main loop. 
	// Call this routine in your main loop and after 50 calls to this
	// routine it will display the avg buffer memory used via the serial link, 
	// so ensure you have enabled serial comms.
	// This routine should only be used to check how much buffer is being used during your
	// code executing in order to optimise how much buffer you need to reserve.
	
	static bool ResultPrinted=false;
	static uint8_t LoopCount=0;
	
	if((LoopCount==50)&(ResultPrinted==false))
	{
		Serial.print("Avg Buffer Usage : ");
		Serial.print(BufferUsedCount/50);
		Serial.println(" bytes");
		ResultPrinted=true;
	}
	LoopCount++;
}

void XT_DAC_Audio_Class::FillBuffer()
{
	// Fill buffer with the sound to output
	
	if((NextFillPos==BUFFER_SIZE)&(EndFillPos!=0)&(NextPlayPos!=0))
		NextFillPos=0;
	
	// if there are items that need to be played & room for more in buffer
	while((FirstPlayListItem!=0)&(NextFillPos!=EndFillPos)&(NextFillPos!=BUFFER_SIZE))							
	{		
		Buffer[NextFillPos]=MixBytesToPlay();					// Get the byte to play and put into buffer
		NextFillPos++;											// move to next buffer position
		if(NextFillPos!=EndFillPos)
		{
			if(NextFillPos==BUFFER_SIZE)
				NextFillPos=0; 
		}
	}		
} 


XT_DAC_Audio_Class::XT_DAC_Audio_Class(uint8_t TheDacPin, uint8_t TimerNo)
{
  // Using a prescaler of 80 gives a counting frequency of 1,000,000 (1MHz) and using
  // and calling the function every 20 counts of the freqency (the 20 below) means
  // that we will call our onTimer function 50,000 times a second
  FirstPlayListItem=0;
  timer = timerBegin(TimerNo, 80, true);                // use timer 0, prescaler is 80 (divide by 8000), count up
  timerAttachInterrupt(timer, &onTimer, true);          // P3= edge trggered
  timerAlarmWrite(timer, 20, true);                     // will trigger 50,000 times per second, so 50kHz is max freq.
  timerAlarmEnable(timer);                              // enable
  XTDacAudioClassGlobalObject=this;						// set variable to use in ISR
  DacPin=TheDacPin;									// set dac pin to use
  LastDacValue=0x7f;									// set to mid  point
  dacWrite(DacPin,LastDacValue);						// Set speaker to mid point, stops click at start of first sound
}


uint8_t XT_DAC_Audio_Class::MixBytesToPlay()
{
	// goes through sounds, gets the bytes from each and mixes them together, returning a final byte
	
	// mix all sounds together in the list of sounds to play and then output that one mixed value	
	// To mix waves you add the waves together.... That is presuming they are "correct" waves
	// that have both positive and negative peaks and troughs. However these wav files are saved
	// as 8bit unsigned, so therefore there is no negative, we need to adjust them back to having
	// correct positive and ngative peaks and troughs first before doing the "mixing", this is a
	// trivial task
	
	volatile XT_PlayListItem_Class *PlayItem;
	uint8_t ByteToPlay;
	
	PlayItem=FirstPlayListItem;
	//while(PlayItem!=0)
	{
		if(PlayItem->Wav->Completed==false)		
			ByteToPlay=PlayItem->Wav->NextByte();
		PlayItem=PlayItem->NextItem;
	}
	
	return ByteToPlay;
}

void XT_DAC_Audio_Class::PlayWav(XT_Wav_Class *Wav)
{
	
	PlayWav(Wav,true);
}


void XT_DAC_Audio_Class::PlayWav(XT_Wav_Class *Wav,bool Mix)
{

	//if(Mix==false)  // stop all currently playing sounds and just have this one	
	//	StopAllSounds();
	//Serial.println("hh");
	//return;
	// set up this wav to play
	Wav->LastIntCount=0;
	Wav->DataIdx=44;
	Wav->Count=0;
	// fine to here
	// add to list of currently playing wavs
	if(FirstPlayListItem==0) // no items to play in list yet
	{
		FirstPlayListItem=new XT_PlayListItem_Class();
		FirstPlayListItem->Wav=Wav;
		Wav->ParentPlayListItem=FirstPlayListItem;
	}
	else{
		// add to end of list	
	}
	Wav->Completed=false; 					// Will start it playing
}


void XT_DAC_Audio_Class::StopAllSounds()
{
	// stop all sounds and clear the play list
	
	volatile XT_PlayListItem_Class* PlayItem;
	PlayItem=FirstPlayListItem;
	while(PlayItem!=0)
	{
		PlayItem->Wav->Completed=true;
		RemoveFromPlayList(PlayItem);
		PlayItem=FirstPlayListItem;
	}
}


void XT_DAC_Audio_Class::RemoveFromPlayList(volatile XT_PlayListItem_Class *ItemToRemove)
{
	// removes a play item from the play list	
	if(ItemToRemove->PreviousItem!=0)
		ItemToRemove->PreviousItem->NextItem=ItemToRemove->NextItem;
	else
		FirstPlayListItem=ItemToRemove->NextItem;
	if(ItemToRemove->NextItem!=0)
		ItemToRemove->NextItem->PreviousItem=ItemToRemove->PreviousItem;
	delete ItemToRemove;			// free up memory used by this object
}

// Wav Class functions 

XT_Wav_Class::XT_Wav_Class(unsigned char *WavData)
{
  // create a new wav class object
  
  SampleRate=(WavData[25]*256)+WavData[24];  
  DataSize=(WavData[42]*65536)+(WavData[41]*256)+WavData[40]+44;
  IncreaseBy=float(SampleRate)/50000;
  Data=WavData;
  Count=0;
  LastIntCount=0;
  DataIdx=44;
  Completed=true;
}


uint8_t IRAM_ATTR XT_Wav_Class::NextByte()
{
	// Returns the next byte to be played, note that this routine will return values suitable to 
	// be played back at 50,000Hz. Even if this sample is at a lesser rate than that it will be
	// padded out as required so that it will appear to have a 50Khz sample rate
	
	// Note it is up to the calling routine to check if this WAV file has NOT completed playing
	// before calling. If you call it and it has completed playing then it will always return
	// 0x7F (speaker mid point).
	uint16_t IntPartOfCount;
	uint8_t ReturnValue;
	
	if(Completed)
		return 0x7f;	
	
	// increase the counter, if it goes to a new integer digit then write to DAC
	Count+=IncreaseBy; 
	IntPartOfCount=floor(Count);
	ReturnValue=Data[DataIdx];				// by default we return previous value; 
	
	if(IntPartOfCount>LastIntCount)
	{   
		// gone to a new integer of count, we need to send a new value to the DAC 
		LastIntCount=IntPartOfCount; // crashes on this line with panic
		ReturnValue=Data[DataIdx];
		DataIdx++;
		if(DataIdx>=DataSize)  				// end of data, flag end
		{
			Count=0;						// reset frequency counter
			DataIdx=44;						// reset data pointer back to beginning of WAV data
			// remove wav from play list
			Completed=true;  				// mark as completed
			XTDacAudioClassGlobalObject->RemoveFromPlayList(ParentPlayListItem);
		}
	}  
	
	return ReturnValue;
	
}

 


