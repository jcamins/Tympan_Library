/*
  CalibrateOutputAndInput

  Created: Chip Audette, OpenAudio, Oct 2024
  Purpose: Measure the signal level of the analog input (pink jack).  If you know 
    the voltage of the input signal, then you can compare it to the Tympan-reported
    digital signal level, which will allow you to compute the calibration coefficient!

    This example generates a sine wave and outputs it from the black headphone jack.
    You can measure the voltage of this signal using a volt meter.  You can then feed
    the signal back into the Tympan's input (pink) jack to perform the calibration.

    This example includes a mode for automatically stepping up through many test
    frequencies.  See the options available in the Serial Monitor menu.  Send
    an "h" (without quotes) in the Serial Monitor to see the help menu.

    This example also lets you record the audio to the SD card for analysis on your
    PC or Mac.

    Remember that the Tympan's audio codec chip (AIC) can provide both filtering
    and additional gain before it digitizes the analog sigal.  So, if using this
    example to calibrate the Tympan inputs, be sure that you know whether your own
    program is using the AIC's filtering and gain differently than being used here.

  For Tympan Rev D, program in Arduino IDE as a Teensy 3.6.
  For Tympan Rev E and F, program in Arduino IDE as a Teensy 4.1.

  MIT License.  use at your own risk.
*/

// Include all the of the needed libraries
#include <Tympan_Library.h>
#include "SerialManager.h"
#include "State.h"        

//set the sample rate and block size
const float       sample_rate_Hz = 44100.0f ;   //24000 or 44117 or 96000 (or other frequencies in the table in AudioOutputI2S_F32)
const int         audio_block_samples = 128;    //do not make bigger than AUDIO_BLOCK_SAMPLES from AudioStream.h (which is 128)  Must be 128 for SD recording.
AudioSettings_F32 audio_settings(sample_rate_Hz, audio_block_samples);

// Create the audio objects and then connect them
Tympan            myTympan(TympanRev::F,audio_settings);   //do TympanRev::D or TympanRev::E or TympanRev::F
#include "AudioProcessing.h"  //see here for audio objects, connections, and configuration functions

// Create classes for controlling the system, espcially via USB Serial and via the App        
SerialManager     serialManager;     //create the serial manager for real-time control (via USB or App)
State             myState(&audio_settings, &myTympan, &serialManager); //keeping one's state is useful for the App's GUI

// Be aware that this calibration program can output steady tones or it can automatically step the tones across frequencies
#include "TestToneControl.h"   //see here for the relevant functions for managing the changing test tones


// ///////////////// Main setup() and loop() as required for all Arduino programs

// define the setup() function, the function that is called once when the device is booting
void setup() {
  //begin the serial comms (for debugging)
  myTympan.beginBothSerial(); delay(500);
  while (!Serial && (millis() < 2000UL)) delay(5); //wait for 2 seconds to see if USB Serial comes up (try not to miss messages!)
  myTympan.println("CalibrateOutputAndInput: Starting setup()...");
  Serial.println("Sample Rate (Hz): " + String(audio_settings.sample_rate_Hz));
  Serial.println("Audio Block Size (samples): " + String(audio_settings.audio_block_samples));

  //allocate the dynamically re-allocatable audio memory
  AudioMemory_F32(100, audio_settings); 

  //activate the Tympan audio hardware
  myTympan.enable();        // activate the flow of audio
  myTympan.volume_dB(0.0);  // headphone amplifier
  myTympan.setHPFonADC(true, myState.adc_hp_cutoff_Hz, sample_rate_Hz); //set DC-blocking filter on AIC

  //Select the input that we will use 
  setConfiguration(myState.input_source);      //use the default that is written in State.h 

  //Setup the level measuring
  setCalcLevelTimeWindow(myState.calcLevel_timeWindow_sec);
  Serial.println("Setup: Setting time windows for level measurements to " + String(myState.calcLevel_timeWindow_sec,4) + " sec");

  //prepare the SD writer for the format that we want and any error statements
  audioSDWriter.setSerial(&myTympan);         //the library will print any error info to this serial stream (note that myTympan is also a serial stream)
  audioSDWriter.setNumWriteChannels(2);       //this is also the built-in defaullt, but you could change it to 4 (maybe?), if you wanted 4 channels.
  Serial.println("Setup: SD configured for " + String(audioSDWriter.getNumWriteChannels()) + " channels.");

  //Setup the output signal
  setOutputChan(myState.output_chan);
  switchTestToneMode(myState.current_test_tone_mode);

  //End of setup
  Serial.println("Setup: complete."); 
  serialManager.printHelp();
} //end setup()


// define the loop() function, the function that is repeated over and over for the life of the device
void loop() {

  //respond to Serial commands
  if (Serial.available()) serialManager.respondToByte((char)Serial.read());   //USB Serial

  //service the SD recording
  audioSDWriter.serviceSD_withWarnings(i2s_in); //For the warnings, it asks the i2s_in class for some info

  //service the LEDs...blink slow normally, blink fast if recording
  myTympan.serviceLEDs(millis(),audioSDWriter.getState() == AudioSDWriter::STATE::RECORDING); 

  //periodically print the CPU and Memory Usage
  if (myState.enable_printCpuToUSB) myState.printCPUandMemory(millis(), 3000); //print every 3000msec  (method is built into TympanStateBase.h, which myState inherits from)

  //check to see what test mode we're in
  if (myState.current_test_tone_mode == State::TONE_MODE_STEPPED_FREQUENCY) {  //are we doing stepped tones?

    //update the stepped dones
    serviceSteppedToneTest(millis());  //update the tones

  } else {                                                                     //we are not doing stepped tones
    //periodically print the signal levels
    if (myState.flag_printInputLevelToUSB) printInputSignalLevels(millis(),1000);  //print every 1000 msec

    //periodically print the output levels
    if (myState.flag_printOutputLevelToUSB) printOutputSignalLevels(millis(),1000);  //print every 1000 msec
  }

} //end loop()

// //////////////////////////////////////// Other functions
void printInputSignalLevels(unsigned long cur_millis, unsigned long updatePeriod_millis) {
  static unsigned long lastUpdate_millis = 0UL;
  if ( (cur_millis < lastUpdate_millis) || (cur_millis >= lastUpdate_millis + updatePeriod_millis) ) {
    Serial.print("Input gain = " + String(myState.input_gain_dB,1) + " dB");
    Serial.print(", Measured Input (L,R) = ");
    Serial.print(calcInputLevel_L.getCurrentLevel_dB(),2);
    Serial.print(", ");
    Serial.print(calcInputLevel_R.getCurrentLevel_dB(),2);
    Serial.print(" dB re: input FS");
    Serial.println();

    lastUpdate_millis = cur_millis;    
  }
}

void printOutputSignalLevels(unsigned long cur_millis, unsigned long updatePeriod_millis) {
  static unsigned long lastUpdate_millis = 0UL;
  if ( (cur_millis < lastUpdate_millis) || (cur_millis >= lastUpdate_millis + updatePeriod_millis) ) {
    Serial.print("SineWave, commanded amplitude = " + String(sineWave.getAmplitude(),4));
    Serial.print(", Measured Ouptut = ");
    Serial.print(calcOutputLevel.getCurrentLevel_dB(),2);
    Serial.print(" dB re: output FS");
    Serial.println();

    lastUpdate_millis = cur_millis;    
  }
}


