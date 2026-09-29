#include <TROOT.h>
#ifndef IFHBEVENT_H
#define IFHBEVENT_H

#include "Event.h"



// DataType class for all CT Events //

/* 

Please follow the following naming convention rules

Variables:
	- All public variables should start with capital letter I
		e.g. 
		int InumberOfPhotons
	- Only camel casing should be used. No underscores for variable names
		e.g. thisIsCameCasing this_Is_not
	- Please do not make all variables public if there is no need for them
	- Access to variable values should exclusively through Get/Set Methods

Methods:
	- Constructor method
		- The simplest constructor will initialize all variables to 0;
		- It is possible to overload for passing parameters
	- Camel Casing should be observed in naming all methods
	- Only ONE return statement for non-void methods
	- Getter and Setter Methods should be named Get[variableName] Set[variableName]
		e.g. GetNumberOfPhotons()
		- All Gets should return the datatype of the variable. 
		- All Sets should be void
	- Memory Address methods will be named Copy[variableName]
		e.g. int* CopyNumberOfPhotons()
		- Should always return a pointer to the variable in question.
	- Should only be relevant to data format itself. For analysis procedures please 
	use the include and source folders NOT in dict/
*/

class IEvent : public Event{
	public:
		/*EEvent DataType includes the following information

		ULong64_t tb_time; Time as recorded by the TB for the event in 10s of ns
		ULong64_t cobo_time; Time as recorded by the CoBo for the event
		ULong64_t unix_time; Time as recorded by the computer for the event
		Int_t eventType; Deprecated 
	
		vector<vector<Int_t>> signalValue; All traces for the event. contains a vecotr of 512x512
		vector<Int_t> roiMusicID; List of triggered music chips. Exactly 2 IDs are given
		vector<Int_t> roiPixelID; Region of Interest for the event 36 pixelIDs stored */

		IEvent(); //Constructor
		~IEvent(); //Destructor

	protected:
		// Telescope parameters //

		float Iazimuth;
		float Ialtitude;

		//...//

		// Calibration parameters //

		float IcorrectionFactor;

		//...//
};
#endif