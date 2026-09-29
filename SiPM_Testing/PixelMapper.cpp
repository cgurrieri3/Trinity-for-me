#include <iostream>
#include <fstream>


/*
Create a Pixel Map as a LUT for linear stage to sweep
trough all pixels in a given configuration.
The mapping is relative to the first pixel (ideally pixel 0)
Numbering follows scheme described in pixel map for SPB 2h
this numbering scheme is demonstrated below schematically
 ________________  ________________
|[35][39][43][47]||[51][55][59][63]|
|[34][38][42][46]||[50][54][58][62]|
|[33][37][41][45]||[49][53][57][61]|
|[32][36][40][44]||[48][52][56][60]|
 ⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻  ⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻
 ________________  ________________ 
|[03][07][11][15]||[19][23][27][31]|
|[02][06][10][14]||[18][22][26][30]|
|[01][05][09][13]||[17][21][25][29]|
|[00][04][08][12]||[16][20][24][28]|
 ⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻  ⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻ 

 Where each block represents a matrix in this configuration. 
 By specifying the number of rows and columns the full camera can
 be mapped.

 Will take the starting position to be Pixel 0
*/

int PixelMapper (){
	int matrix_row, matrix_col, pix_row, pix_col;
	double pix_sz, pix_pad;
	ofstream pixMap("PixeLMap.txt");

	matrix_row = 2; //Number of Matrix Rows
	matrix_col = 2; //Number of Matrix Columns

	pix_row = 4; //Rows Of pixels per Matrix;
	pix_col = 4; // Columns of pixels per Matrix

	pix_sz = 6.0; //Size of SiPM in mm
	pix_pad = 0.2; //Space between pixels mm


	int npix;
	double x_coord, y_coord;
	x_coord = 0;
	y_coord = 0;

	cout<<"Pixel#\t"<< "X (mm)\t"<<"Y (mm)"<<endl;
	pixMap<<"#PixelNo. "<< "X (mm) "<<"Y (mm)"<<endl;
	for (int i = 0; i<matrix_row; i++){
		y_coord = i*pix_row*pix_sz+i*(pix_row+1)*pix_pad;
		for (int j=0;j<matrix_col;j++){
			x_coord = j*pix_col*pix_sz + j*(pix_col+1)*pix_pad;
			for (int k=0; k<pix_col;k++){

				for (int l=0; l<pix_row;l++){
					npix = l + pix_row*k+pix_row*pix_col*j+pix_row*pix_col*matrix_col*i;
					

					cout<<npix<<"\t"<<x_coord<<"\t"<<y_coord<<"\t"<<endl;
					pixMap<<npix<<" "<<x_coord<<" "<<y_coord<<"\t"<<endl;
					y_coord += (pix_sz+pix_pad);
				}
				y_coord = i*pix_row*pix_sz+i*(pix_row+1)*pix_pad;
				x_coord += pix_sz+pix_pad;
			}
			
		}
	}
	pixMap.close();

	return 0;

}