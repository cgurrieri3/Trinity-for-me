#include<iostream>

class IPlotTools{
	public:
		static void FindBin(int pixelID, int *nx, int *ny);
		static int FindPixel(int nx, int ny);
		static int GetMUSICCursorID(int x, int y);
};
