/*
 * AudioFilterbank_F32
 * 
 * Created: Chip Audette (OpenAudio) Aug 2021
 * 
 * Purpose: Ceate filberbank classes that joins the filter design and filter implementation to
 *     make it easier to manage having multiple filters so that you don't have to handle each 
 *     individual filter yourself
 * 
 * MIT License.  Use at your own risk.
 * 
 */
 
#ifndef _AudioFilterbank_f32_h
#define _AudioFilterbank_f32_h

#include <Arduino.h>
#include <AudioFilterFIR_F32.h> 		  //from Tympan_Library
#include <AudioConfigFIRFilterBank_F32.h> //from Tympan_Library
#include <AudioFilterBiquad_F32.h> 		  //from Tympan_Library
#include <AudioConfigIIRFilterBank_F32.h> //from Tympan_Library
#include <SerialManager_UI.h>

#define AudioFilterbankFIR_MAX_NUM_FILTERS 8       //maximum number of filters to allow
#define AudioFilterbankBiquad_MAX_NUM_FILTERS 12      //maximum number of filters to allow
#define AudioFilterbankBiquad_MAX_IIR_FILT_ORDER 6    //oveall desired filter order (note: in Matlab, an "N=3" bandpass is actually a 6th-order filter to be broken up into biquads
#define AudioFilterbankBiquad_COEFF_PER_BIQUAD  6     //3 "b" coefficients and 3 "a" coefficients per biquad

//This class helps manage some of the configuration and state information of the AudioFilterbank classes.
//It is also helpful for managing the GUI on the TympanRemote mobile App.
class AudioFilterbankState {
	public:
		AudioFilterbankState(void) {};
		~AudioFilterbankState(void) { delete crossover_freq_Hz; }
		
		int filter_order = 0;
		
		int set_crossover_freq_Hz(float *freq_Hz, int n_filts);
		float get_crossover_freq_Hz(int Ichan);
		int get_crossover_freq_Hz(float *freq_Hz, int n_filts);

		int set_n_filters(int n);
		int get_n_filters(void) { return n_filters; }
		
		//keep track of the maximum number of filters...the user shouldn't have to worry about this
		int set_max_n_filters(int n);
		int get_max_n_filters(void) { return max_n_filters; }

	protected:
		int max_n_filters = 0;  //should correspond to the length of crossover_freq_Hz
		int n_filters = 0;      //should correspond to however many of the filters are actually being employed by the AuioFilterbank
		float *crossover_freq_Hz;  //this really only needs to be [nfilters-1] in length, but we'll generally allocate [nfilters] just to avoid mistaken overruns
		
		
};

//This is a parent class for the FIR filterbank and IIR (biquad) filterbank.  This class defines an interfaces
//for other classes to interact with the filterbank (regardless of FIR and IIR) and this class holds some
//common methods that should only be defined in one place.
//
//Since this has some purely virtual methods, you cannot instantiate this class directly.
class AudioFilterbankBase_F32 : public AudioStream_F32 {
	public:
		AudioFilterbankBase_F32(void): AudioStream_F32(1,inputQueueArray) { } 
		AudioFilterbankBase_F32(const AudioSettings_F32 &settings) : AudioStream_F32(1,inputQueueArray) { }
		
		virtual void enable(bool _enable = true) { is_enabled = _enable; }
		
		//virtual void begin(void) = 0;  //must implement this in a child class
		virtual void update(void) = 0;  //must implement this in a child class;
		virtual int set_n_filters(int val) = 0;  //must implement this in a child class
		virtual int designFilters(int n_chan, int n_order, float sample_rate_Hz, int block_len, float *crossover_freq) = 0;  //must implement this in a child class
		
		virtual int get_n_filters(void) { return n_filters; }
		
		AudioFilterbankState state;
		
	protected: 
		audio_block_f32_t *inputQueueArray[1];  //required as part of AudioStream_F32.  One input.
		bool is_enabled = false;
		int n_filters = AudioFilterbankFIR_MAX_NUM_FILTERS; //how many filters are actually being used.  Must be less than AudioFilterbankFIR_MAX_NUM_FILTERS

};


//This is the FIR based filterbank.
//You should instantiate this class if you want FIR filters.
class AudioFilterbankFIR_F32 : public AudioFilterbankBase_F32 {
//GUI: inputs:1, outputs:8  //this line used for automatic generation of GUI node  
//GUI: shortName:filterbank_FIR
	public:
		AudioFilterbankFIR_F32(void) : AudioFilterbankBase_F32() { }
		AudioFilterbankFIR_F32(const AudioSettings_F32 &settings) : AudioFilterbankBase_F32(settings) { }

		//virtual void begin(void);
		virtual void update(void);
		virtual int set_n_filters(int val);
		virtual int designFilters(int n_chan, int n_fir, float sample_rate_Hz, int block_len, float *crossover_freq);

		//core classes for designing and implementing the filters
		AudioConfigFIRFilterBank_F32 filterbankDesigner;
		AudioFilterFIR_F32 filters[AudioFilterbankFIR_MAX_NUM_FILTERS]; //every filter instance consumes memory to hold its states, which are numerous for an FIR filter
		
	private:

};

//This is the IIR based (specifically, IIR biquad [aka. second-order sections]) filterbank
//You should instantiate this class if you want Biquad filters.
class AudioFilterbankBiquad_F32 : public AudioFilterbankBase_F32 {
//GUI: inputs:1, outputs:8  //this line used for automatic generation of GUI node  
//GUI: shortName:filterbank_Biquad
	public:
		AudioFilterbankBiquad_F32(void) : AudioFilterbankBase_F32() { }
		AudioFilterbankBiquad_F32(const AudioSettings_F32 &settings) : AudioFilterbankBase_F32(settings) { }

		//virtual void begin(void);
		virtual void update(void);
		virtual int set_n_filters(int val);
		virtual int designFilters(int n_chan, int n_iir, float sample_rate_Hz, int block_len, float *crossover_freq);

		//core classes for designing and implementing the filters
		AudioConfigIIRFilterBank_F32 filterbankDesigner;
		AudioFilterBiquad_F32 filters[AudioFilterbankBiquad_MAX_NUM_FILTERS]; //every filter instance consumes memory to hold its states, which are numerous for an FIR filter
		
	private:

};

// ////////////////////////////////////////////////////////////////////////////////////////////////
//
// UI Versions of the Classes
//
// These versions of the classes add no signal processing functionality.  Instead, they add to the
// classes simply to make it easier to add a menu-based or App-based interface to configure and 
// control the audio-processing classes above.
//
// If you want to add a GUI, you might consider using the classes below instead of the classes above.
// Again, the signal processing is exactly the same either way.
//
// ////////////////////////////////////////////////////////////////////////////////////////////////


class AudioFilterbank_UI : public SerialManager_UI {
	public:
		//AudioFilterbank_UI(void) : SerialManager_UI() {};
		AudioFilterbank_UI(AudioFilterbankBase_F32 *_this_filterbank) : 
			SerialManager_UI(), this_filterbank(_this_filterbank) {};
		
		
		// ///////// here are the methods that you must implement from SerialManager_UI
		virtual void printHelp(void);
		//virtual bool processCharacter(char c); //not used here
		virtual bool processCharacterTriple(char mode_char, char chan_char, char data_char);
		virtual void setFullGUIState(bool activeButtonsOnly = false); 

		float freq_fac = powf(2.0,1.0/12.0);  //how much to multiply the crossover frequency by when shifting up or down
		
	protected:
		AudioFilterbankBase_F32 *this_filterbank = NULL;
		void printChanUpMsg(void);   //used for building the help menu
		void printChanDownMsg(void); //used for building the help menu
		
};

class AudioFilterbankFIR_F32_UI : public AudioFilterbankFIR_F32, public AudioFilterbank_UI {
	public:
		AudioFilterbankFIR_F32_UI(void) : AudioFilterbankFIR_F32(), AudioFilterbank_UI(this) {}
		AudioFilterbankFIR_F32_UI(const AudioSettings_F32 &settings) : AudioFilterbankFIR_F32(settings), AudioFilterbank_UI(this) {};
		

};

class AudioFilterbankBiquad_F32_UI : public AudioFilterbankBiquad_F32, public AudioFilterbank_UI {
	public:
		AudioFilterbankBiquad_F32_UI(void) : AudioFilterbankBiquad_F32(), AudioFilterbank_UI(this) {}
		AudioFilterbankBiquad_F32_UI(const AudioSettings_F32 &settings) : AudioFilterbankBiquad_F32(settings), AudioFilterbank_UI(this) {};
			
};


#endif