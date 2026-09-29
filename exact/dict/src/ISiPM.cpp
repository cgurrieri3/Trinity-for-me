#include "ISiPM.h"

ISiPM::ISiPM(){
	
}
ISiPM::ISiPM(int nPixels){
	gain = std::vector<double>(nPixels, 0);
	gainTCorrection = std::vector<double>(nPixels, 0);
	amplToPE = std::vector<double>(nPixels, 0);
	relOverV = std::vector<double>(nPixels, 0);
	pedestal = std::vector<double>(nPixels, 0);
	
}
ISiPM::~ISiPM(){
	
}

void ISiPM::SetGain(std::vector<double> values){
	gain = values;
}
void ISiPM::SetRelOverV(std::vector<double> values){
	relOverV = values;
}
void ISiPM::SetTCorrection(std::vector<double> values){
	gainTCorrection = values;
}
void ISiPM::SetAmplToPE(std::vector<double> values){
	amplToPE = values;
}
void ISiPM::SetPedestal(std::vector<double> values){
	pedestal = values;
}

std::vector<double> ISiPM::GetGain(){
	return gain;
}
std::vector<double> ISiPM::GetRelOverV(){
	return relOverV;
}
std::vector<double> ISiPM::GetTCorrection(){
	return gainTCorrection;
}
std::vector<double> ISiPM::GetAmplToPE(){
	return amplToPE;
}
std::vector<double> ISiPM::GetPedestal(){
	return pedestal;
}
