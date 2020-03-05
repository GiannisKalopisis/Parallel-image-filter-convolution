#ifndef ALLFUNCTIONS_H
#define ALLFUNCTIONS_H


void getInput(double*** H);

unsigned char CenterPixel(unsigned char** image_array,int i,int j,int rows,int columns,double** H,int multiplier);

void SharpenEdges(unsigned char** image_array,unsigned char receivedTopLeftCorner,
			unsigned char receivedTopRightCorner,unsigned char receivedBottomRightCorner,
			unsigned char receivedBottomLeftCorner,unsigned char* receivedTopRow,
			unsigned char* receivedBottomRow,unsigned char* receivedRightColumn,
			unsigned char* receivedLeftColumn,double** H,int localRows,int localColumns,unsigned char*** sharpenedImageArray,int myRank);

void SharpenEdgesRGB(unsigned char** image_array,unsigned char receivedTopLeftCorner,
			unsigned char receivedTopRightCorner,unsigned char receivedBottomRightCorner,
			unsigned char receivedBottomLeftCorner,unsigned char* receivedTopRow,
			unsigned char* receivedBottomRow,unsigned char* receivedRightColumn,
			unsigned char* receivedLeftColumn,unsigned char receivedTopLeftCornerGreen,unsigned char receivedTopLeftCornerBlue,
            unsigned char receivedTopRightCornerGreen,unsigned char receivedTopRightCornerBlue,
            unsigned char receivedBottomRightCornerGreen,unsigned char receivedBottomRightCornerBlue,unsigned char receivedBottomLeftCornerGreen,
            unsigned char receivedBottomLeftCornerBlue,unsigned char* receivedRightColumnGreen,unsigned char* receivedRightColumnBlue,
            unsigned char* receivedLeftColumnGreen,unsigned char* receivedLeftColumnBlue, double** H,
            int localRows,int localColumns,unsigned char*** sharpenedImageArray,int myRank);


#endif