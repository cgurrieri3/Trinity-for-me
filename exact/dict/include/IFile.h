#ifndef IFILE_H
#define IFILE_H

#include <iostream>
#include <TFile.h>
#include <TTree.h>

/**
 * \class IFile
 * \brief Class used for Merged files I/O 
 * 
 * Used to contain TFile with events.
 * It allows loading the trees and using the data in methods and modules without losing reference to the TFile. 
 * Since ROOT Histograms attach to the TFile, creating it inside a method destroys it once the method exits. This causes SegFaults. In order to avoid this, the class IFile was created that contains all the trees in the raw data file from the CT.
 * Creates a new TFile and keeps it alive, while initializing all trees from a standard DAQ from the CT.
 * 
 * A Tree ID was created to keep reference of the different trees
 * 
 * 0: "HLED"
 * 1: "BiFocal"
 * 2: "Test"
 * 3: "Forced"
 * */

class IFile{
	public:
		TFile *ITFile; /**< TFile object for opening ROOT file */
		TTree *treeHLED; /**< TTree object containing HLED triggered events */
		TTree *treeBiFocal;/**< TTree object containing BiFocal triggered events */
		TTree *treeForced;/**< TTree object containing Forced triggered events */
		TTree *treeTest;/**< TTree object containing Test (singleFocus) triggered events */

		/**
		 * Constructor taking 2 arguments.
		 * @param filename a string matching the file to open
		 * @param mode a string with which modality to use ("READ","UPDATE","RECREATE") same as for ROOT TFile
		 * 
		 * */

		IFile(std::string filename, std::string mode="READ");

		/** 
		 * A destructor
		 * */

		~IFile();

		/**
		 * Public member returning the tree ID
		 * @param treeName a string matching the name of the tree to obtain its ID.\ Not case sensitive, can be hled, bifocal, test, forced
		 * @return an int with the treeID
		 * */

		static int GetTreeID(std::string treeName);
	private:
		/**
		 * Private member used to obtain TTrees from ROOT file
		 * */
		
		void GetTrees();
};
#endif