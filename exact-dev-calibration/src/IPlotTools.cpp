#include "IPlotTools.h"
void IPlotTools::FindBin(int pixelID,int *nx, int *ny)
{
    int MUSIC_ID = pixelID/8;
    int MUSIC_Channel = pixelID%8; 
    int MUSIC_row = MUSIC_ID/16;
    int MUSIC_column = MUSIC_ID%16;

    *ny = 4*MUSIC_row+MUSIC_Channel%4; 
    *nx = 2*MUSIC_column+MUSIC_Channel/4; 
}
int IPlotTools::FindPixel(int nx, int ny)
{
    int pixelID;
    pixelID = (nx*4+(ny%4)+(ny/4)*128);
    return pixelID;
}
int IPlotTools::GetMUSICCursorID(int x, int y)
{
    int MUSIC_column = x/2;
    int MUSIC_row = y/4;
    int MUSIC_ID = MUSIC_column+MUSIC_row*16;

    return MUSIC_ID;  
}