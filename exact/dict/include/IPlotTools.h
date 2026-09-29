#ifndef IPLOTTOOLS_H
#define IPLOTTOOLS_H

#include<iostream>
#include <vector>

class IPlotTools{
	/**
	 * \class IPlotTools
	 * \brief Methods to facilitate plotting the camera in ExACT
	 * 
	 * */
	public:
		/**
		 * Public constructor
		 * */
		IPlotTools();
		/**
		 * Public destructor
		 * */
		~IPlotTools();
		/**
		 * A method to find the bin to which a pixel ID belongs to, based on a camera that is 16 rows by 32 columns and follows the EUSO-SPB2 numbering standard
		 * 
		 * @param pixelID An int with the ID of the pixel
		 * @param nx An int* where to store the column bin number
		 * @param ny An int* where to store the row bin number
		 * */
		static void FindBin(int pixelID, int *nx, int *ny);
		/**
		 * A method to calculate the pixel ID corresponding to a certain row and column in the camera
		 * 
		 * @param nx An int with the column number
		 * @param ny An int with the row number
		 * 
		 * @return An int with the pixel number
		 * */
		static int FindPixel(int nx, int ny);
		/**
		 * A method used to calculate the MUSIC ID (trigger cell) over which the mouse is hovering on a ROOT plot of the camera
		 * 
		 * @param x An int with the x coordinate of the cursor
		 * @param y An int with the y coordinate of the cursor
		 * 
		 * @return An int witht the MUSIC ID.
		 * */
		static int FindMUSIC(int nx, int ny);
		static int FindSIAB(int nx, int ny);
		/**
		 * A method used to draw the trigger cell boundaries grouping the pixels connected to the same discriminator together
		 * 
		 * */
		static void DrawMUSICBoundaries();
		static void DrawSIABBoundaries();
	private:
		inline static int MaxNofChannels = 256;
		inline static std::vector<int> mapMUSIC = {17,16,1,0,25,24,9,8,19,18,3,2,27,26,11,10,21,20,5,4,29,28,13,12,23,22,7,6,31,30,15,14};
		inline static std::vector<int> mapSIAB = {12,12,8,8,4,4,0,0,13,13,9,9,5,5,1,1,14,14,10,10,6,6,2,2,15,15,11,11,7,7,3,3};
};
#endif // IPLOTTOOLS_H
