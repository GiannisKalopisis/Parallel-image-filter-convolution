#include <stdlib.h>
#include <stdio.h>
#include "AllFunctions.h"



 void getInput(double*** H)
{
        int totalH = 0;

        printf("Enter your H elements: \n");
        for(int i = 0; i < 3; i++){
            if (i != 0){
                printf("                       ");
            }
            scanf("%lf %lf %lf", &((*H)[i][0]), &((*H)[i][1]), &((*H)[i][2]));
            totalH += (*H)[i][0];
            totalH += (*H)[i][1];
            totalH += (*H)[i][2];
        }
        
        fflush(stdout);
        
        
        
        printf("H: \n");
        for(int i = 0; i < 3; i++){
            printf("    ");
            for(int j = 0; j < 3; j++){
                printf("%lf ", ((*H)[i])[j]);
            }
            printf("\n");
        }

        //normalization

    if(totalH != 0){
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                (*H)[i][j] = (*H)[i][j] / totalH;
            }
        }
        


    }
}



void SharpenEdges(unsigned char** image_array,unsigned char receivedTopLeftCorner,
			unsigned char receivedTopRightCorner,unsigned char receivedBottomRightCorner,
			unsigned char receivedBottomLeftCorner,unsigned char* receivedTopRow,
			unsigned char* receivedBottomRow,unsigned char* receivedRightColumn,
			unsigned char* receivedLeftColumn,double** H,int localRows,int localColumns,
			unsigned char*** sharpenedImageArray,int myRank)
	
	{	

		unsigned char centerPixel;

		int i=0;
		int j;
		for(j=0; j<localColumns; j++){
			
			centerPixel=0;

			//We are at top left pixel
			if(j==0){				
			
				centerPixel += (unsigned char) ((double) receivedBottomRightCorner)* H[0][0];
				centerPixel += (unsigned char) ((double) receivedBottomRow[j])* H[0][1];
				centerPixel += (unsigned char) ((double) receivedBottomRow[j+1])*H[0][2];
				centerPixel += (unsigned char) ((double) receivedRightColumn[i])*H[1][0];
				centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
				centerPixel += (unsigned char) ((double) image_array[i][j+1])*H[1][2];
				centerPixel += (unsigned char) ((double) receivedRightColumn[i+1])*H[2][0];
				centerPixel += (unsigned char) ((double) image_array[i+1][j])*H[2][1];
				centerPixel += (unsigned char) ((double) image_array[i+1][j+1])*H[2][2];
            }




			//We are at top right pixel
			else if (j==localColumns-1){
				centerPixel += (unsigned char) ((double) receivedBottomRow[j-1])* H[0][0];
				centerPixel += (unsigned char) ((double) receivedBottomRow[j])* H[0][1];
				centerPixel += (unsigned char) ((double) receivedBottomLeftCorner)*H[0][2];
				centerPixel += (unsigned char) ((double) image_array[i][j-1])*H[1][0];
				centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
				centerPixel += (unsigned char) ((double) receivedLeftColumn[i])*H[1][2];
				centerPixel += (unsigned char) ((double) image_array[i+1][j-1])*H[2][0];
				centerPixel += (unsigned char) ((double) image_array[i+1][j])*H[2][1];
				centerPixel += (unsigned char) ((double) receivedLeftColumn[i+1])*H[2][2];
            }

			//We are anywhere in top row
			else{
   
			    centerPixel += (unsigned char) ((double) receivedBottomRow[j-1])* H[0][0];
			    centerPixel += (unsigned char) ((double) receivedBottomRow[j])* H[0][1];
			    centerPixel += (unsigned char) ((double) receivedBottomRow[j+1])*H[0][2];
			    centerPixel += (unsigned char) ((double) image_array[i][j-1])*H[1][0];
			    centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
			    centerPixel += (unsigned char) ((double) image_array[i][j+1])*H[1][2];
			    centerPixel += (unsigned char) ((double) image_array[i+1][j-1])*H[2][0];
			    centerPixel += (unsigned char) ((double) image_array[i+1][j])*H[2][1];
			    centerPixel += (unsigned char) ((double) image_array[i+1][j+1])*H[2][2];


	
							
			}
			
            (*sharpenedImageArray)[i][j] = centerPixel;	
		}

		//From 1 to localRows-1 because we calculate corners when we edit lines
		for(i=1; i<localRows-1; i++){

			centerPixel=0;
		
			j=0;
			
			centerPixel += (unsigned char) ((double) receivedRightColumn[i-1])* H[0][0];
			centerPixel += (unsigned char) ((double) image_array[i-1][j])* H[0][1];
			centerPixel += (unsigned char) ((double) image_array[i-1][j+1])*H[0][2];
			centerPixel += (unsigned char) ((double) receivedRightColumn[i])*H[1][0];
			centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
			centerPixel += (unsigned char) ((double) image_array[i][j+1])*H[1][2];
			centerPixel += (unsigned char) ((double) receivedRightColumn[i+1])*H[2][0];
			centerPixel += (unsigned char) ((double) image_array[i+1][j])*H[2][1];
			centerPixel += (unsigned char) ((double) image_array[i+1][j+1])*H[2][2];
			
			(*sharpenedImageArray)[i][j] = centerPixel;

 

			j=localColumns-1;
	
			centerPixel=0;


//			if(myRank==0){

				//printf("receivedLeftColumn[%d] is: %d\n",i,receivedLeftColumn[i]);
	

    //		}

			centerPixel += (unsigned char) ((double) image_array[i-1][j-1])* H[0][0];
			centerPixel += (unsigned char) ((double) image_array[i-1][j])* H[0][1];
			centerPixel += (unsigned char) ((double) receivedLeftColumn[i-1])*H[0][2];
			centerPixel += (unsigned char) ((double) image_array[i][j-1])*H[1][0];
			centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
			centerPixel += (unsigned char) ((double) receivedLeftColumn[i])*H[1][2];
			centerPixel += (unsigned char) ((double) image_array[i+1][j-1])*H[2][0];
			centerPixel += (unsigned char) ((double) image_array[i+1][j])*H[2][1];
			centerPixel += (unsigned char) ((double) receivedLeftColumn[i+1])*H[2][2];

			(*sharpenedImageArray)[i][j] = centerPixel;

		}




		i=localRows-1;
		for(j=0; j<localColumns; j++){

			centerPixel=0;
			
			if(j==0){
			
				centerPixel += (unsigned char) ((double) receivedRightColumn[i-1])* H[0][0];
				centerPixel += (unsigned char) ((double) image_array[i-1][j])* H[0][1];
				centerPixel += (unsigned char) ((double) image_array[i-1][j+1])*H[0][2];
				centerPixel += (unsigned char) ((double) receivedRightColumn[i])*H[1][0];
				centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
				centerPixel += (unsigned char) ((double) image_array[i][j+1])*H[1][2];
				centerPixel += (unsigned char) ((double) receivedTopRightCorner)*H[2][0];
				centerPixel += (unsigned char) ((double) receivedTopRow[j])*H[2][1];
				centerPixel += (unsigned char) ((double) receivedTopRow[j+1])*H[2][2];
			}
			else if (j==localColumns-1){
				centerPixel += (unsigned char) ((double) image_array[i-1][j-1])* H[0][0];
				centerPixel += (unsigned char) ((double) image_array[i-1][j])* H[0][1];
				centerPixel += (unsigned char) ((double) receivedLeftColumn[i-1])*H[0][2];
				centerPixel += (unsigned char) ((double) image_array[i][j-1])*H[1][0];
				centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
				centerPixel += (unsigned char) ((double) receivedLeftColumn[i])*H[1][2];
				centerPixel += (unsigned char) ((double) receivedTopRow[j-1])*H[2][0];
				centerPixel += (unsigned char) ((double) receivedTopRow[j])*H[2][1];
				centerPixel += (unsigned char) ((double) receivedTopLeftCorner)*H[2][2];				
				
			}
			else{
				centerPixel += (unsigned char) ((double) image_array[i-1][j-1])* H[0][0];
				centerPixel += (unsigned char) ((double) image_array[i-1][j])* H[0][1];
				centerPixel += (unsigned char) ((double) image_array[i-1][j+1])*H[0][2];
				centerPixel += (unsigned char) ((double) image_array[i][j-1])*H[1][0];
				centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
				centerPixel += (unsigned char) ((double) image_array[i][j+1])*H[1][2];
				centerPixel += (unsigned char) ((double) receivedTopRow[j-1])*H[2][0];
				centerPixel += (unsigned char) ((double) receivedTopRow[j])*H[2][1];
				centerPixel += (unsigned char) ((double) receivedTopRow[j+1])*H[2][2];	
							
			}
			(*sharpenedImageArray)[i][j] = centerPixel;
				
		}
		
			
	
	
	}


void SharpenEdgesRGB(unsigned char** image_array,unsigned char receivedTopLeftCorner,
			unsigned char receivedTopRightCorner,unsigned char receivedBottomRightCorner,
			unsigned char receivedBottomLeftCorner,unsigned char* receivedTopRow,
			unsigned char* receivedBottomRow,unsigned char* receivedRightColumn,
			unsigned char* receivedLeftColumn,unsigned char receivedTopLeftCornerGreen,unsigned char receivedTopLeftCornerBlue,
            unsigned char receivedTopRightCornerGreen,unsigned char receivedTopRightCornerBlue,
            unsigned char receivedBottomRightCornerGreen,unsigned char receivedBottomRightCornerBlue,unsigned char receivedBottomLeftCornerGreen,
            unsigned char receivedBottomLeftCornerBlue,unsigned char* receivedRightColumnGreen,unsigned char* receivedRightColumnBlue,
            unsigned char* receivedLeftColumnGreen,unsigned char* receivedLeftColumnBlue, double** H,
            int localRows,int localColumns,unsigned char*** sharpenedImageArray,int myRank){

		unsigned char centerPixel;

		int i=0;
		int j;
		for(j=0; j<localColumns; j++){
			
			centerPixel=0;

			//We are at top left pixel
			if(j==0){				
			
				centerPixel += (unsigned char) ((double) receivedBottomRightCorner)* H[0][0];
				centerPixel += (unsigned char) ((double) receivedBottomRow[j])* H[0][1];
				centerPixel += (unsigned char) ((double) receivedBottomRow[j+3])*H[0][2];
				centerPixel += (unsigned char) ((double) receivedRightColumn[i])*H[1][0];
				centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
				centerPixel += (unsigned char) ((double) image_array[i][j+3])*H[1][2];
				centerPixel += (unsigned char) ((double) receivedRightColumn[i+1])*H[2][0];
				centerPixel += (unsigned char) ((double) image_array[i+1][j])*H[2][1];
				centerPixel += (unsigned char) ((double) image_array[i+1][j+3])*H[2][2];
            }

            else if(j==1){

                centerPixel += (unsigned char) ((double) receivedBottomRightCornerGreen)* H[0][0];
				centerPixel += (unsigned char) ((double) receivedBottomRow[j])* H[0][1];
				centerPixel += (unsigned char) ((double) receivedBottomRow[j+3])*H[0][2];
				centerPixel += (unsigned char) ((double) receivedRightColumnGreen[i])*H[1][0];
				centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
				centerPixel += (unsigned char) ((double) image_array[i][j+3])*H[1][2];
				centerPixel += (unsigned char) ((double) receivedRightColumnGreen[i+1])*H[2][0];
				centerPixel += (unsigned char) ((double) image_array[i+1][j])*H[2][1];
				centerPixel += (unsigned char) ((double) image_array[i+1][j+3])*H[2][2];
            }

            else if(j==2){

                centerPixel += (unsigned char) ((double) receivedBottomRightCornerBlue)* H[0][0];
				centerPixel += (unsigned char) ((double) receivedBottomRow[j])* H[0][1];
				centerPixel += (unsigned char) ((double) receivedBottomRow[j+3])*H[0][2];
				centerPixel += (unsigned char) ((double) receivedRightColumnBlue[i])*H[1][0];
				centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
				centerPixel += (unsigned char) ((double) image_array[i][j+3])*H[1][2];
				centerPixel += (unsigned char) ((double) receivedRightColumnBlue[i+1])*H[2][0];
				centerPixel += (unsigned char) ((double) image_array[i+1][j])*H[2][1];
				centerPixel += (unsigned char) ((double) image_array[i+1][j+3])*H[2][2];
			}
			//We are at top right pixel
			else if (j==localColumns-3){
				centerPixel += (unsigned char) ((double) receivedBottomRow[j-3])* H[0][0];
				centerPixel += (unsigned char) ((double) receivedBottomRow[j])* H[0][1];
				centerPixel += (unsigned char) ((double) receivedBottomLeftCorner)*H[0][2];
				centerPixel += (unsigned char) ((double) image_array[i][j-3])*H[1][0];
				centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
				centerPixel += (unsigned char) ((double) receivedLeftColumn[i])*H[1][2];
				centerPixel += (unsigned char) ((double) image_array[i+1][j-3])*H[2][0];
				centerPixel += (unsigned char) ((double) image_array[i+1][j])*H[2][1];
				centerPixel += (unsigned char) ((double) receivedLeftColumn[i+1])*H[2][2];
            }

            else if (j==localColumns-2){
				centerPixel += (unsigned char) ((double) receivedBottomRow[j-3])* H[0][0];
				centerPixel += (unsigned char) ((double) receivedBottomRow[j])* H[0][1];
				centerPixel += (unsigned char) ((double) receivedBottomLeftCornerGreen)*H[0][2];
				centerPixel += (unsigned char) ((double) image_array[i][j-3])*H[1][0];
				centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
				centerPixel += (unsigned char) ((double) receivedLeftColumnGreen[i])*H[1][2];
				centerPixel += (unsigned char) ((double) image_array[i+1][j-3])*H[2][0];
				centerPixel += (unsigned char) ((double) image_array[i+1][j])*H[2][1];
				centerPixel += (unsigned char) ((double) receivedLeftColumnGreen[i+1])*H[2][2];		
            }
            else if (j==localColumns-1){
				centerPixel += (unsigned char) ((double) receivedBottomRow[j-3])* H[0][0];
				centerPixel += (unsigned char) ((double) receivedBottomRow[j])* H[0][1];
				centerPixel += (unsigned char) ((double) receivedBottomLeftCornerBlue)*H[0][2];
				centerPixel += (unsigned char) ((double) image_array[i][j-3])*H[1][0];
				centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
				centerPixel += (unsigned char) ((double) receivedLeftColumnBlue[i])*H[1][2];
				centerPixel += (unsigned char) ((double) image_array[i+1][j-3])*H[2][0];
				centerPixel += (unsigned char) ((double) image_array[i+1][j])*H[2][1];
				centerPixel += (unsigned char) ((double) receivedLeftColumnBlue[i+1])*H[2][2];						
				
			}
			//We are anywhere in top row
			else{

			    centerPixel += (unsigned char) ((double) receivedBottomRow[j-3])* H[0][0];
			    centerPixel += (unsigned char) ((double) receivedBottomRow[j])* H[0][1];
			    centerPixel += (unsigned char) ((double) receivedBottomRow[j+3])*H[0][2];
			    centerPixel += (unsigned char) ((double) image_array[i][j-3])*H[1][0];
			    centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
			    centerPixel += (unsigned char) ((double) image_array[i][j+3])*H[1][2];
			    centerPixel += (unsigned char) ((double) image_array[i+1][j-3])*H[2][0];
			    centerPixel += (unsigned char) ((double) image_array[i+1][j])*H[2][1];
			    centerPixel += (unsigned char) ((double) image_array[i+1][j+3])*H[2][2];
            }

					(*sharpenedImageArray)[i][j] = centerPixel;	
		}

		//From 1 to localRows-1 because we calculate corners when we edit lines
		for(i=1; i<localRows-1; i++){

			centerPixel=0;
		
			j=0;
			
			centerPixel += (unsigned char) ((double) receivedRightColumn[i-1])* H[0][0];
			centerPixel += (unsigned char) ((double) image_array[i-1][j])* H[0][1];
			centerPixel += (unsigned char) ((double) image_array[i-1][j+3])*H[0][2];
			centerPixel += (unsigned char) ((double) receivedRightColumn[i])*H[1][0];
			centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
			centerPixel += (unsigned char) ((double) image_array[i][j+3])*H[1][2];
			centerPixel += (unsigned char) ((double) receivedRightColumn[i+1])*H[2][0];
			centerPixel += (unsigned char) ((double) image_array[i+1][j])*H[2][1];
			centerPixel += (unsigned char) ((double) image_array[i+1][j+3])*H[2][2];
			
			(*sharpenedImageArray)[i][j] = centerPixel;

            centerPixel=0;
            j=1;
			
			centerPixel += (unsigned char) ((double) receivedRightColumnGreen[i-1])* H[0][0];
			centerPixel += (unsigned char) ((double) image_array[i-1][j])* H[0][1];
			centerPixel += (unsigned char) ((double) image_array[i-1][j+3])*H[0][2];
			centerPixel += (unsigned char) ((double) receivedRightColumnGreen[i])*H[1][0];
			centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
			centerPixel += (unsigned char) ((double) image_array[i][j+3])*H[1][2];
			centerPixel += (unsigned char) ((double) receivedRightColumnGreen[i+1])*H[2][0];
			centerPixel += (unsigned char) ((double) image_array[i+1][j])*H[2][1];
			centerPixel += (unsigned char) ((double) image_array[i+1][j+3])*H[2][2];

			(*sharpenedImageArray)[i][j] = centerPixel;

            centerPixel=0;
            j=2;
			centerPixel += (unsigned char) ((double) receivedRightColumnBlue[i-1])* H[0][0];
			centerPixel += (unsigned char) ((double) image_array[i-1][j])* H[0][1];
			centerPixel += (unsigned char) ((double) image_array[i-1][j+3])*H[0][2];
			centerPixel += (unsigned char) ((double) receivedRightColumnBlue[i])*H[1][0];
			centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
			centerPixel += (unsigned char) ((double) image_array[i][j+3])*H[1][2];
			centerPixel += (unsigned char) ((double) receivedRightColumnBlue[i+1])*H[2][0];
			centerPixel += (unsigned char) ((double) image_array[i+1][j])*H[2][1];
			centerPixel += (unsigned char) ((double) image_array[i+1][j+3])*H[2][2];

			(*sharpenedImageArray)[i][j] = centerPixel;

			j=localColumns-3;
	
			centerPixel=0;



			centerPixel += (unsigned char) ((double) image_array[i-1][j-3])* H[0][0];
			centerPixel += (unsigned char) ((double) image_array[i-1][j])* H[0][1];
			centerPixel += (unsigned char) ((double) receivedLeftColumn[i-1])*H[0][2];
			centerPixel += (unsigned char) ((double) image_array[i][j-3])*H[1][0];
			centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
			centerPixel += (unsigned char) ((double) receivedLeftColumn[i])*H[1][2];
			centerPixel += (unsigned char) ((double) image_array[i+1][j-3])*H[2][0];
			centerPixel += (unsigned char) ((double) image_array[i+1][j])*H[2][1];
			centerPixel += (unsigned char) ((double) receivedLeftColumn[i+1])*H[2][2];

			(*sharpenedImageArray)[i][j] = centerPixel;

			j=localColumns-2;
	
			centerPixel=0;



			centerPixel += (unsigned char) ((double) image_array[i-1][j-3])* H[0][0];
			centerPixel += (unsigned char) ((double) image_array[i-1][j])* H[0][1];
			centerPixel += (unsigned char) ((double) receivedLeftColumnGreen[i-1])*H[0][2];
			centerPixel += (unsigned char) ((double) image_array[i][j-3])*H[1][0];
			centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
			centerPixel += (unsigned char) ((double) receivedLeftColumnGreen[i])*H[1][2];
			centerPixel += (unsigned char) ((double) image_array[i+1][j-3])*H[2][0];
			centerPixel += (unsigned char) ((double) image_array[i+1][j])*H[2][1];
			centerPixel += (unsigned char) ((double) receivedLeftColumnGreen[i+1])*H[2][2];

			(*sharpenedImageArray)[i][j] = centerPixel;
			
            j=localColumns-1;
	
			centerPixel=0;



			centerPixel += (unsigned char) ((double) image_array[i-1][j-3])* H[0][0];
			centerPixel += (unsigned char) ((double) image_array[i-1][j])* H[0][1];
			centerPixel += (unsigned char) ((double) receivedLeftColumnBlue[i-1])*H[0][2];
			centerPixel += (unsigned char) ((double) image_array[i][j-3])*H[1][0];
			centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
			centerPixel += (unsigned char) ((double) receivedLeftColumnBlue[i])*H[1][2];
			centerPixel += (unsigned char) ((double) image_array[i+1][j-3])*H[2][0];
			centerPixel += (unsigned char) ((double) image_array[i+1][j])*H[2][1];
			centerPixel += (unsigned char) ((double) receivedLeftColumnBlue[i+1])*H[2][2];

			(*sharpenedImageArray)[i][j] = centerPixel;

		}




		i=localRows-1;
		for(j=0; j<localColumns; j++){

			centerPixel=0;
			
			if(j==0){
			
				centerPixel += (unsigned char) ((double) receivedRightColumn[i-1])* H[0][0];
				centerPixel += (unsigned char) ((double) image_array[i-1][j])* H[0][1];
				centerPixel += (unsigned char) ((double) image_array[i-1][j+3])*H[0][2];
				centerPixel += (unsigned char) ((double) receivedRightColumn[i])*H[1][0];
				centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
				centerPixel += (unsigned char) ((double) image_array[i][j+3])*H[1][2];
				centerPixel += (unsigned char) ((double) receivedTopRightCorner)*H[2][0];
				centerPixel += (unsigned char) ((double) receivedTopRow[j])*H[2][1];
				centerPixel += (unsigned char) ((double) receivedTopRow[j+3])*H[2][2];
			}
            else if(j==1){
				centerPixel += (unsigned char) ((double) receivedRightColumnGreen[i-1])* H[0][0];
				centerPixel += (unsigned char) ((double) image_array[i-1][j])* H[0][1];
				centerPixel += (unsigned char) ((double) image_array[i-1][j+3])*H[0][2];
				centerPixel += (unsigned char) ((double) receivedRightColumnGreen[i])*H[1][0];
				centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
				centerPixel += (unsigned char) ((double) image_array[i][j+3])*H[1][2];
				centerPixel += (unsigned char) ((double) receivedTopRightCornerGreen)*H[2][0];
				centerPixel += (unsigned char) ((double) receivedTopRow[j])*H[2][1];
				centerPixel += (unsigned char) ((double) receivedTopRow[j+3])*H[2][2];           
            
            }
            else if(j==2){
				centerPixel += (unsigned char) ((double) receivedRightColumnBlue[i-1])* H[0][0];
				centerPixel += (unsigned char) ((double) image_array[i-1][j])* H[0][1];
				centerPixel += (unsigned char) ((double) image_array[i-1][j+3])*H[0][2];
				centerPixel += (unsigned char) ((double) receivedRightColumnBlue[i])*H[1][0];
				centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
				centerPixel += (unsigned char) ((double) image_array[i][j+3])*H[1][2];
				centerPixel += (unsigned char) ((double) receivedTopRightCornerBlue)*H[2][0];
				centerPixel += (unsigned char) ((double) receivedTopRow[j])*H[2][1];
				centerPixel += (unsigned char) ((double) receivedTopRow[j+3])*H[2][2];
            }
   
			else if (j==localColumns-3){
				centerPixel += (unsigned char) ((double) image_array[i-1][j-3])* H[0][0];
				centerPixel += (unsigned char) ((double) image_array[i-1][j])* H[0][1];
				centerPixel += (unsigned char) ((double) receivedLeftColumn[i-1])*H[0][2];
				centerPixel += (unsigned char) ((double) image_array[i][j-3])*H[1][0];
				centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
				centerPixel += (unsigned char) ((double) receivedLeftColumn[i])*H[1][2];
				centerPixel += (unsigned char) ((double) receivedTopRow[j-3])*H[2][0];
				centerPixel += (unsigned char) ((double) receivedTopRow[j])*H[2][1];
				centerPixel += (unsigned char) ((double) receivedTopLeftCorner)*H[2][2];				
				
			}
            else if(j==localColumns-2){
                centerPixel += (unsigned char) ((double) image_array[i-1][j-3])* H[0][0];
				centerPixel += (unsigned char) ((double) image_array[i-1][j])* H[0][1];
				centerPixel += (unsigned char) ((double) receivedLeftColumnGreen[i-1])*H[0][2];
				centerPixel += (unsigned char) ((double) image_array[i][j-3])*H[1][0];
				centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
				centerPixel += (unsigned char) ((double) receivedLeftColumnGreen[i])*H[1][2];
				centerPixel += (unsigned char) ((double) receivedTopRow[j-3])*H[2][0];
				centerPixel += (unsigned char) ((double) receivedTopRow[j])*H[2][1];
				centerPixel += (unsigned char) ((double) receivedTopLeftCornerGreen)*H[2][2];
            }
            else if(j==localColumns-1){
				centerPixel += (unsigned char) ((double) image_array[i-1][j-3])* H[0][0];
				centerPixel += (unsigned char) ((double) image_array[i-1][j])* H[0][1];
				centerPixel += (unsigned char) ((double) receivedLeftColumnBlue[i-1])*H[0][2];
				centerPixel += (unsigned char) ((double) image_array[i][j-3])*H[1][0];
				centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
				centerPixel += (unsigned char) ((double) receivedLeftColumnBlue[i])*H[1][2];
				centerPixel += (unsigned char) ((double) receivedTopRow[j-3])*H[2][0];
				centerPixel += (unsigned char) ((double) receivedTopRow[j])*H[2][1];
				centerPixel += (unsigned char) ((double) receivedTopLeftCornerBlue)*H[2][2];				
				
            }
			else{
				centerPixel += (unsigned char) ((double) image_array[i-1][j-3])* H[0][0];
				centerPixel += (unsigned char) ((double) image_array[i-1][j])* H[0][1];
				centerPixel += (unsigned char) ((double) image_array[i-1][j+3])*H[0][2];
				centerPixel += (unsigned char) ((double) image_array[i][j-3])*H[1][0];
				centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
				centerPixel += (unsigned char) ((double) image_array[i][j+3])*H[1][2];
				centerPixel += (unsigned char) ((double) receivedTopRow[j-3])*H[2][0];
				centerPixel += (unsigned char) ((double) receivedTopRow[j])*H[2][1];
				centerPixel += (unsigned char) ((double) receivedTopRow[j+3])*H[2][2];	
							
			}
			(*sharpenedImageArray)[i][j] = centerPixel;
				
		}

}

unsigned char CenterPixel(unsigned char** image_array,int i, int j,int rows,int columns,double** H,int multiplier){
	
	unsigned char centerPixel=0;
	centerPixel += (unsigned char) ((double) image_array[i-1][j-(1*multiplier)])* H[0][0];
	centerPixel += (unsigned char) ((double) image_array[i-1][j])* H[0][1];
	centerPixel += (unsigned char) ((double) image_array[i-1][j+(1*multiplier)])*H[0][2];
	centerPixel += (unsigned char) ((double) image_array[i][j-(1*multiplier)])*H[1][0];
	centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
	centerPixel += (unsigned char) ((double) image_array[i][j+(1*multiplier)])*H[1][2];
	centerPixel += (unsigned char) ((double) image_array[i+1][j-(1*multiplier)])*H[2][0];
	centerPixel += (unsigned char) ((double) image_array[i+1][j])*H[2][1];
	centerPixel += (unsigned char) ((double) image_array[i+1][j+(1*multiplier)])*H[2][2];
	return centerPixel;
}
