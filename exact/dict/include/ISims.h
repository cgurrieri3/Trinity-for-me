#ifndef ISIMULATE_H
#define ISIMULATE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <TGraphErrors.h>
#include <TMath.h>

#define BREAKDOWN_V 38.7 //At 0deg C
class ISims{
	/**
	 * \class ISims
	 * \brief Class with methods to interact with data obtained from simulatiosn
	 * 
	 * The simulation package CARE outputs data in a somewhat similar format to ExACT. This class allows converting the data set in order for it to be piped to ExACT modules without the need of developing new algorithms.
	 * */

	public:
		/** 
		 * Public constructor
		 * */
		ISims();
		/** 
		 * Public destructor
		 * */
		~ISims();
		/**
		 * Generalized poisson definition. The purpose is to use it as input for a TF1. Basicallu it is a poisson distribution which accurately takes optical cross-talk into account
		 * 
		 * @param x A double* of where to evaluate the Generalized Poisson
		 * @param par A double* with all the different parameters for a generalized poisson.
		 * par[0] is the average of the poisson distribution
		 * par[1] is the optical cross-talk
		 * 
		 * @return A double with the probability at x
		 * */
		static double GeneralizedPoisson(double *x, double *par);
		/**
		 * A method to obtain the optical cross-talk from lab measurements summarized in the optical cross talk .csv file
		 * Note: Deprecated
		 * 
		 * @param filename A std::string with the name of the file containing the measurements
		 * */
		static std::vector<std::vector<double>> OpticalXTalkExtraction(std::string filename);
		/**
		 * A method to invert the traces obtained from CARE (CARE outputs traces in positive polarity, whereas the data from EUSO-SPB2 is inverted to negative polarity.
		 * 
		 * @param traces A std::vector<std::vecetor<int>*> As obtained from the output of CARE simulations. The std::vector has dimensions m x n where m is the number of pixels and n the number of samples in the trace
		 * 
		 * @return A std::vector<std::vector<int>> with the inverted trace using an arbitrary pedestal offset
		 * */
		static std::vector<std::vector<int>> InvertCARETrace(std::vector<std::vector<int> *> traces);
		/**
		 * A method to invert the traces obtained from CARE (CARE outputs traces in positive polarity, whereas the data from EUSO-SPB2 is inverted to negative polarity.
		 * @see InvertCARETrace(std::vector<std::vector<int> *> traces)
		 * 
		 * @param traces A std::vector<std::vecetor<int>> The std::vector has dimensions m x n where m is the number of pixels and n the number of samples in the trace
		 * 
		 * @return A std::vector<std::vector<int>> with the inverted trace using an arbitrary pedestal offset
		 * */
		static std::vector<std::vector<int>> InvertCARETrace(std::vector<std::vector<int>> traces);

};

#endif