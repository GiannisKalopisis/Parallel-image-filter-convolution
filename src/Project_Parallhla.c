#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "AllFunctions.h"



#define TOP_LEFT_CORNER_PIXEL_TAG 0
#define TOP_RIGHT_CORNER_PIXEL_TAG 1
#define BOTTOM_LEFT_CORNER_PIXEL_TAG 2
#define BOTTOM_RIGHT_CORNER_PIXEL_TAG 3
#define TOP_ROW_TAG 4
#define BOTTOM_ROW_TAG 5
#define LEFT_COLUMN_TAG 6
#define RIGHT_COLUMN_TAG 7

#define LEFT_COLUMN_GREEN_TAG 8
#define LEFT_COLUMN_BLUE_TAG 9
#define RIGHT_COLUMN_GREEN_TAG 10
#define RIGHT_COLUMN_BLUE_TAG 11

#define TOP_LEFT_CORNER_GREEN_PIXEL_TAG 12
#define TOP_RIGHT_CORNER_GREEN_PIXEL_TAG 13
#define BOTTOM_LEFT_CORNER_GREEN_PIXEL_TAG 14
#define BOTTOM_RIGHT_CORNER_GREEN_PIXEL_TAG 15
#define TOP_LEFT_CORNER_BLUE_PIXEL_TAG 16
#define TOP_RIGHT_CORNER_BLUE_PIXEL_TAG 17
#define BOTTOM_LEFT_CORNER_BLUE_PIXEL_TAG 18
#define BOTTOM_RIGHT_CORNER_BLUE_PIXEL_TAG 19



int main(int argc, char *argv[])
{

    int myRank, comSize;
    
    unsigned char **image_array;
    int topRow,bottomRow,leftColumn,rightColumn;
	unsigned char **sharpenedImageArray;	
	
    int rows, columns;

    //INPUT FOR GREY OR RGB !!
    MPI_Init (&argc, &argv);      /* starts MPI */

    MPI_Comm_rank (MPI_COMM_WORLD, &myRank);        /* get current process id */
    MPI_Comm_size (MPI_COMM_WORLD, &comSize);        /* get number of processes */
	

		
	double** H=(double**) malloc (3*sizeof(double*));
    for(int i = 0; i < 3; i++){
        H[i]=(double*) malloc(3*sizeof(double));
    }


	int opt;
	int RGB;
	while ((opt = getopt(argc, argv, "r:c:i:")) != -1) {
		
	   switch (opt) {
		   case 'r':
			   rows = atoi(optarg);
			   break;
		   case 'c':
			   columns = atoi(optarg);
			   
			   break;
		   case 'i':
			   RGB = atoi(optarg);
				break;
		   default: 
			   fprintf(stderr, "ERROR BAD ARGUMENTS\n");
			   exit(EXIT_FAILURE);
	   }
	}
    int multiplier=1;
    if (RGB==1){
        multiplier=3;
    }


	MPI_File fh, dest;
	MPI_Status status;

	
	
	double mean;
	double global_time;
	double elapsedComm,elapsedR, elapsedW;
	double startComm, startR, finishComm,finishR, startW, finishW;
    

    if(RGB==1){
        MPI_File_open( MPI_COMM_WORLD, "../images/waterfall_1920_2520.raw", MPI_MODE_RDONLY , MPI_INFO_NULL, &fh );	
    }
    else{
        MPI_File_open( MPI_COMM_WORLD, "../images/waterfall_grey_1920_2520.raw", MPI_MODE_RDONLY , MPI_INFO_NULL, &fh );
    }	
        

    
	double startH[9];
	MPI_Request sendHArray =MPI_REQUEST_NULL;
	MPI_Request recvHArray =MPI_REQUEST_NULL;
	


		if(myRank == 0){
			getInput(&H);
			for (int i = 0; i < 9; i++){
				startH[i] = H[i/3][i%3];
			}
			for (int i = 0; i < 9; i++){

			}
			//MPI_Ibcast(startH, 9, MPI_DOUBLE, 0, MPI_COMM_WORLD, &request);
			for(int i = 1; i<comSize; i++){
			
					MPI_Isend(startH, 9, MPI_DOUBLE, i, 10, MPI_COMM_WORLD, &sendHArray);		
			}			
		}
		else{
			MPI_Irecv(startH, 9, MPI_DOUBLE, 0, 10, MPI_COMM_WORLD, &recvHArray);
				
			
		}

	int procSize = (int) sqrt(comSize);
	int localRows = (int) rows/procSize;
	int localColumns = (int) (multiplier*columns)/procSize;

	unsigned char* data = (unsigned char*) malloc(localRows*localColumns*sizeof(unsigned char));
	
	image_array = (unsigned char **) malloc ((localRows)*sizeof(unsigned char*));
	
	

	MPI_Offset offset=(myRank/procSize)*localRows*(multiplier*columns) + (myRank%procSize)*localColumns;
	
	MPI_File_seek( fh, offset, MPI_SEEK_SET ); 	

    if(RGB==1){	
        MPI_File_open( MPI_COMM_WORLD, "../images/sharpened_RGB.raw", MPI_MODE_WRONLY | MPI_MODE_CREATE , MPI_INFO_NULL, &dest);	
    }
    else{
        MPI_File_open( MPI_COMM_WORLD, "../images/sharpened.raw", MPI_MODE_WRONLY | MPI_MODE_CREATE , MPI_INFO_NULL, &dest);
    }

	
	startR = MPI_Wtime();
	for(int i=0;i<localRows;i++){
		
	  	image_array[i] = &(data[i*localColumns]);
		MPI_File_read(fh, image_array[i], localColumns, MPI_UNSIGNED_CHAR,&status);
	
		
		MPI_File_seek( fh,(procSize-1)*localColumns, MPI_SEEK_CUR ); 
	}


	finishR = MPI_Wtime();	
	

	elapsedR = finishR - startR;
	MPI_Allreduce(&elapsedR, &global_time, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	if (myRank == 0){
		mean = global_time/comSize;
		printf("Mean  time to  read is %lf\n",mean);
	}

	unsigned char receivedTopLeftCorner;
	unsigned char receivedTopRightCorner;
	unsigned char receivedBottomRightCorner;
	unsigned char receivedBottomLeftCorner;
	unsigned char *receivedTopRow = (unsigned char *) malloc(localColumns*sizeof(unsigned char));
	unsigned char *receivedRightColumn = (unsigned char *) malloc((localRows)*sizeof(unsigned char));
	unsigned char *receivedBottomRow = (unsigned char *) malloc(localColumns*sizeof(unsigned char));
	unsigned char *receivedLeftColumn = (unsigned char *) malloc((localRows)*sizeof(unsigned char));

	unsigned char *receivedLeftColumnGreen = (unsigned char *) malloc((localRows)*sizeof(unsigned char));
	unsigned char *receivedLeftColumnBlue = (unsigned char *) malloc((localRows)*sizeof(unsigned char));
	unsigned char *receivedRightColumnGreen = (unsigned char *) malloc((localRows)*sizeof(unsigned char));
	unsigned char *receivedRightColumnBlue = (unsigned char *) malloc((localRows)*sizeof(unsigned char));
	unsigned char receivedTopLeftCornerGreen;
	unsigned char receivedTopLeftCornerBlue;
	unsigned char receivedTopRightCornerGreen;
	unsigned char receivedTopRightCornerBlue;
	unsigned char receivedBottomRightCornerGreen;
	unsigned char receivedBottomRightCornerBlue;
	unsigned char receivedBottomLeftCornerGreen;
	unsigned char receivedBottomLeftCornerBlue;

	



		//TOP OR BOTTOM PROCCESS
		if(myRank/procSize==0){ 
		    topRow=1;
		    bottomRow=0;
		}
		else if(myRank/procSize==procSize-1){
		    topRow=0;
		    bottomRow=1;
		}
		else{
		    topRow=0;
		    bottomRow=0;
		}

		//LEFT OR RIGHT PROCCESS
		if(myRank%procSize==0){
		    leftColumn=1;
		    rightColumn=0;
		}
		else if(myRank%procSize==procSize-1){
		    leftColumn=0;
		    rightColumn=1;
		}
		else{
		    leftColumn=0;
		    rightColumn=0;
		}


		//received buffer creation

		

		//fill buffer with black
		receivedTopLeftCorner = 0;
		receivedTopRightCorner = 0;
		receivedBottomRightCorner = 0;
		receivedBottomLeftCorner = 0;

		if (RGB==1){
		    receivedTopLeftCornerGreen = 0;
		    receivedTopLeftCornerBlue = 0;
		    receivedTopRightCornerGreen = 0;
		    receivedTopRightCornerBlue = 0;
		    receivedBottomRightCornerGreen = 0;
		    receivedBottomRightCornerBlue = 0;
		    receivedBottomLeftCornerGreen = 0;
		    receivedBottomLeftCornerBlue = 0;
		}
		for (int i = 0; i < localColumns; i++){
			receivedTopRow[i] = 0;
			receivedBottomRow[i] = 0;
		}
		for (int i = 0; i < localRows; i++){
			receivedRightColumn[i] = 0;
			receivedLeftColumn[i] = 0;

		    if (RGB ==1){
				receivedRightColumnGreen[i] = 0;
				receivedLeftColumnBlue[i] = 0;
				receivedRightColumnBlue[i] = 0;
				receivedLeftColumnGreen[i] = 0;
		    }
		}
	

		int numberOfRequests;
		MPI_Request* sendRequestArray;
		MPI_Request* receiveRequestArray;

		MPI_Datatype column;
		MPI_Type_vector(localRows,1,localColumns,MPI_UNSIGNED_CHAR,&column);
		MPI_Type_commit(&column);

		
	

		startComm =MPI_Wtime();
		//flag to see if the imaged changed 


		int imageSame = 0;
		int repeats = 0 ;
		while(imageSame == 0 && repeats < 15){

			if(topRow && leftColumn){
				numberOfRequests = 3;
		        if (RGB ==1)
		            numberOfRequests=7;

				sendRequestArray = (MPI_Request*) malloc(numberOfRequests*sizeof(MPI_Request));
				receiveRequestArray = (MPI_Request*) malloc(numberOfRequests*sizeof(MPI_Request));
				//last row
				MPI_Isend(image_array[localRows-1],localColumns,MPI_UNSIGNED_CHAR,myRank+procSize, BOTTOM_ROW_TAG,MPI_COMM_WORLD, &sendRequestArray[0]); 
				//last column
				MPI_Isend(&image_array[0][localColumns-(1*multiplier)],1,column,myRank+1, RIGHT_COLUMN_TAG, MPI_COMM_WORLD, &sendRequestArray[1]);
		        //bottom right pixel
				MPI_Isend(&image_array[localRows-1][localColumns-(1*multiplier)],1,MPI_UNSIGNED_CHAR,myRank+1+procSize, BOTTOM_RIGHT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[2]);
		        

		        if (RGB==1){

		            //last column RGB
		   		    MPI_Isend(&image_array[0][localColumns-2],1,column,myRank+1, RIGHT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &sendRequestArray[3]);
		   		    MPI_Isend(&image_array[0][localColumns-1],1,column,myRank+1, RIGHT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &sendRequestArray[4]);
				    
				  


		            //bottom right pixel RGB
				    MPI_Isend(&image_array[localRows-1][localColumns-2],1,MPI_UNSIGNED_CHAR,myRank+1+procSize, BOTTOM_RIGHT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[5]);
				    MPI_Isend(&image_array[localRows-1][localColumns-1],1,MPI_UNSIGNED_CHAR,myRank+1+procSize, BOTTOM_RIGHT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[6]);
		        }

				//Receive
				//first row of other
				MPI_Irecv(receivedTopRow, localColumns, MPI_UNSIGNED_CHAR, myRank+procSize, TOP_ROW_TAG, MPI_COMM_WORLD, &receiveRequestArray[0]);
				//top left pixel of other
				MPI_Irecv(&receivedTopLeftCorner, 1, MPI_UNSIGNED_CHAR, myRank+1+procSize, TOP_LEFT_CORNER_PIXEL_TAG,MPI_COMM_WORLD, &receiveRequestArray[1]);
				//first column of other
				MPI_Irecv(receivedLeftColumn, localRows, MPI_UNSIGNED_CHAR, myRank+1, LEFT_COLUMN_TAG, MPI_COMM_WORLD, &receiveRequestArray[2]);

		        if (RGB == 1){
		            //top left pixel of other RGB
					MPI_Irecv(&receivedTopLeftCornerGreen, 1, MPI_UNSIGNED_CHAR, myRank+1+procSize, TOP_LEFT_CORNER_GREEN_PIXEL_TAG,MPI_COMM_WORLD, &receiveRequestArray[3]);            
					MPI_Irecv(&receivedTopLeftCornerBlue, 1, MPI_UNSIGNED_CHAR, myRank+1+procSize, TOP_LEFT_CORNER_BLUE_PIXEL_TAG,MPI_COMM_WORLD, &receiveRequestArray[4]);
		            


		            //first column of other RGB
					MPI_Irecv(receivedLeftColumnGreen, localRows, MPI_UNSIGNED_CHAR, myRank+1, LEFT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &receiveRequestArray[5]);
					MPI_Irecv(receivedLeftColumnBlue, localRows, MPI_UNSIGNED_CHAR, myRank+1, LEFT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &receiveRequestArray[6]);
		        }

			}
			else if(topRow && !leftColumn && !rightColumn){
		        numberOfRequests=5;
		        if (RGB == 1)
		            numberOfRequests=13;

			    sendRequestArray = (MPI_Request*) malloc(numberOfRequests*sizeof(MPI_Request));
				receiveRequestArray = (MPI_Request*) malloc(numberOfRequests*sizeof(MPI_Request));
				//first column
				MPI_Isend(&image_array[0][0],1,column,myRank-1, LEFT_COLUMN_TAG, MPI_COMM_WORLD, &sendRequestArray[0]);
		        
				//bottom left pixel
				MPI_Isend(&image_array[localRows-1][0],1,MPI_UNSIGNED_CHAR,myRank-1+procSize, BOTTOM_LEFT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[1]);
				//last row
				MPI_Isend(image_array[localRows-1],localColumns,MPI_UNSIGNED_CHAR,myRank+procSize, BOTTOM_ROW_TAG, MPI_COMM_WORLD, &sendRequestArray[2]);

				//bottom right pixel
				MPI_Isend(&image_array[localRows-1][localColumns-(1*multiplier)],1,MPI_UNSIGNED_CHAR,myRank+1+procSize, BOTTOM_RIGHT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[3]);
				//last column
				MPI_Isend(&image_array[0][localColumns-(1*multiplier)],1,column,myRank+1, RIGHT_COLUMN_TAG, MPI_COMM_WORLD, &sendRequestArray[4]);

		        if (RGB == 1){
		            //first column RGB

				    MPI_Isend(&image_array[0][2],1,column,myRank-1, LEFT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &sendRequestArray[5]);
				    MPI_Isend(&image_array[0][1],1,column,myRank-1, LEFT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &sendRequestArray[6]);


		            //bottom left pixel RGB
				    MPI_Isend(&image_array[localRows-1][1],1,MPI_UNSIGNED_CHAR,myRank-1+procSize, BOTTOM_LEFT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[7]);
				    MPI_Isend(&image_array[localRows-1][2],1,MPI_UNSIGNED_CHAR,myRank-1+procSize, BOTTOM_LEFT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[8]);


		            //bottom right pixel RGB
				    MPI_Isend(&image_array[localRows-1][localColumns-2],1,MPI_UNSIGNED_CHAR,myRank+1+procSize, BOTTOM_RIGHT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[9]);
				    MPI_Isend(&image_array[localRows-1][localColumns-1],1,MPI_UNSIGNED_CHAR,myRank+1+procSize, BOTTOM_RIGHT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[10]);

		            //last column RGB
		   		    MPI_Isend(&image_array[0][localColumns-2],1,column,myRank+1, RIGHT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &sendRequestArray[11]);
		   		    MPI_Isend(&image_array[0][localColumns-1],1,column,myRank+1, RIGHT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &sendRequestArray[12]);
		        }



				//Receive
				//last column of other
				MPI_Irecv(receivedRightColumn, localRows, MPI_UNSIGNED_CHAR, myRank-1, RIGHT_COLUMN_TAG, MPI_COMM_WORLD, &receiveRequestArray[0]);

				//top right pixel of other
				MPI_Irecv(&receivedTopRightCorner, 1, MPI_UNSIGNED_CHAR, myRank+procSize-1,TOP_RIGHT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[1]);
				//first row of other
				MPI_Irecv(receivedTopRow, localColumns, MPI_UNSIGNED_CHAR, myRank+procSize, TOP_ROW_TAG, MPI_COMM_WORLD, &receiveRequestArray[2]);
				//top left pixel of other
				MPI_Irecv(&receivedTopLeftCorner, 1, MPI_UNSIGNED_CHAR, myRank+1+procSize,TOP_LEFT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[3]);
				//first column
				MPI_Irecv(receivedLeftColumn, localRows, MPI_UNSIGNED_CHAR, myRank+1, LEFT_COLUMN_TAG, MPI_COMM_WORLD, &receiveRequestArray[4]);

		        if (RGB==1){
		            //top right pixel of other RGB
					MPI_Irecv(&receivedTopRightCornerGreen, 1, MPI_UNSIGNED_CHAR, myRank+procSize-1,TOP_RIGHT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[5]);
					MPI_Irecv(&receivedTopRightCornerBlue, 1, MPI_UNSIGNED_CHAR, myRank+procSize-1,TOP_RIGHT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[6]);
		            
		            //last column of other RGB
					MPI_Irecv(receivedRightColumnGreen, localRows, MPI_UNSIGNED_CHAR, myRank-1, RIGHT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &receiveRequestArray[7]);
					MPI_Irecv(receivedRightColumnBlue, localRows, MPI_UNSIGNED_CHAR, myRank-1, RIGHT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &receiveRequestArray[8]);



		            //top left pixel of other RGB
					MPI_Irecv(&receivedTopLeftCornerGreen, 1, MPI_UNSIGNED_CHAR, myRank+1+procSize,TOP_LEFT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[9]);
					MPI_Irecv(&receivedTopLeftCornerBlue, 1, MPI_UNSIGNED_CHAR, myRank+1+procSize,TOP_LEFT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[10]);



		            //first column of other RGB
					MPI_Irecv(receivedLeftColumnGreen, localRows, MPI_UNSIGNED_CHAR, myRank+1, LEFT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &receiveRequestArray[11]);
					MPI_Irecv(receivedLeftColumnBlue, localRows, MPI_UNSIGNED_CHAR, myRank+1, LEFT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &receiveRequestArray[12]);
		        }
		
			}
			else if(topRow && rightColumn){
				numberOfRequests=3;
		        if (RGB==1)
		            numberOfRequests=7;

				sendRequestArray = (MPI_Request*) malloc(numberOfRequests*sizeof(MPI_Request));
				receiveRequestArray = (MPI_Request*) malloc(numberOfRequests*sizeof(MPI_Request));
				//first column
				MPI_Isend(&image_array[0][0],1,column,myRank-1, LEFT_COLUMN_TAG, MPI_COMM_WORLD, &sendRequestArray[0]);
				//last row
	   		    MPI_Isend(image_array[localRows-1],localColumns,MPI_UNSIGNED_CHAR,myRank+procSize, BOTTOM_ROW_TAG,MPI_COMM_WORLD, &sendRequestArray[1]);
				//bottom left pixel
	   		    MPI_Isend(&image_array[localRows-1][0],1,MPI_UNSIGNED_CHAR,myRank-1+procSize, BOTTOM_LEFT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[2]);

		        if (RGB == 1){
		        //first column RGB
				    MPI_Isend(&image_array[0][1],1,column,myRank-1, LEFT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &sendRequestArray[3]);
				    MPI_Isend(&image_array[0][2],1,column,myRank-1, LEFT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &sendRequestArray[4]);




		            //bottom left pixel RGB
				    MPI_Isend(&image_array[localRows-1][1],1,MPI_UNSIGNED_CHAR,myRank-1+procSize, BOTTOM_LEFT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[5]);
				    MPI_Isend(&image_array[localRows-1][2],1,MPI_UNSIGNED_CHAR,myRank-1+procSize, BOTTOM_LEFT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[6]);
		        }

				//Receive
				//last column of other
				MPI_Irecv(receivedRightColumn, localRows, MPI_UNSIGNED_CHAR, myRank-1, RIGHT_COLUMN_TAG, MPI_COMM_WORLD, &receiveRequestArray[0]);
				//top right pixel of other
				MPI_Irecv(&receivedTopRightCorner, 1, MPI_UNSIGNED_CHAR, myRank+procSize-1,TOP_RIGHT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[1]);
				//first row of other
				MPI_Irecv(receivedTopRow, localColumns, MPI_UNSIGNED_CHAR, myRank+procSize, TOP_ROW_TAG, MPI_COMM_WORLD, &receiveRequestArray[2]);
		        

		        if (RGB==1){
		            //last row of other RGB
					MPI_Irecv(receivedRightColumnGreen, localRows, MPI_UNSIGNED_CHAR, myRank-1, RIGHT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &receiveRequestArray[3]);
					MPI_Irecv(receivedRightColumnBlue, localRows, MPI_UNSIGNED_CHAR, myRank-1, RIGHT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &receiveRequestArray[4]);



		            //top right of other RGB
					MPI_Irecv(&receivedTopRightCornerGreen, 1, MPI_UNSIGNED_CHAR, myRank+procSize-1,TOP_RIGHT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[5]);
					MPI_Irecv(&receivedTopRightCornerBlue, 1, MPI_UNSIGNED_CHAR, myRank+procSize-1,TOP_RIGHT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[6]);
		        }

			}
			else if(!topRow && !bottomRow && rightColumn){
				numberOfRequests=5;
		        if (RGB==1)
		            numberOfRequests=11;
				sendRequestArray = (MPI_Request*) malloc(numberOfRequests*sizeof(MPI_Request));
				receiveRequestArray = (MPI_Request*) malloc(numberOfRequests*sizeof(MPI_Request));
				//first row
				MPI_Isend(image_array[0],localColumns,MPI_UNSIGNED_CHAR,myRank-procSize, TOP_ROW_TAG, MPI_COMM_WORLD, &sendRequestArray[0]);
				//top left pixel
				MPI_Isend(&image_array[0][0],1,MPI_UNSIGNED_CHAR,myRank-1-procSize,TOP_LEFT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[1]);
				//first column
				MPI_Isend(&image_array[0][0],1,column,myRank-1, LEFT_COLUMN_TAG, MPI_COMM_WORLD, &sendRequestArray[2]);
				//bottom left pixel
				MPI_Isend(&image_array[localRows-1][0],1,MPI_UNSIGNED_CHAR,myRank-1+procSize, BOTTOM_LEFT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[3]);
				//last row
				MPI_Isend(image_array[localRows-1],localColumns,MPI_UNSIGNED_CHAR,myRank+procSize, BOTTOM_ROW_TAG, MPI_COMM_WORLD, &sendRequestArray[4]);

		        if (RGB==1){
		            //top left pixel RGB
				    MPI_Isend(&image_array[0][1],1,MPI_UNSIGNED_CHAR,myRank-1-procSize,TOP_LEFT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[5]);
				    MPI_Isend(&image_array[0][2],1,MPI_UNSIGNED_CHAR,myRank-1-procSize,TOP_LEFT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[6]);




		            //first column RGB
				    MPI_Isend(&image_array[0][1],1,column,myRank-1, LEFT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &sendRequestArray[7]);
				    MPI_Isend(&image_array[0][2],1,column,myRank-1, LEFT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &sendRequestArray[8]);




		            //bottom left pixel RGB
				    MPI_Isend(&image_array[localRows-1][1],1,MPI_UNSIGNED_CHAR,myRank-1+procSize, BOTTOM_LEFT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[9]);
				    MPI_Isend(&image_array[localRows-1][2],1,MPI_UNSIGNED_CHAR,myRank-1+procSize, BOTTOM_LEFT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[10]);
		        }

				//Receive
				//last row of other
				MPI_Irecv(receivedBottomRow, localColumns, MPI_UNSIGNED_CHAR, myRank-procSize, BOTTOM_ROW_TAG, MPI_COMM_WORLD, &receiveRequestArray[0]);
				//bottom right pixel of other
				MPI_Irecv(&receivedBottomRightCorner, 1, MPI_UNSIGNED_CHAR, myRank-1-procSize, BOTTOM_RIGHT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[1]);
				//last column of other
				MPI_Irecv(receivedRightColumn, localRows, MPI_UNSIGNED_CHAR, myRank-1, RIGHT_COLUMN_TAG, MPI_COMM_WORLD, &receiveRequestArray[2]);
				//top right pixel of other
				MPI_Irecv(&receivedTopRightCorner, 1, MPI_UNSIGNED_CHAR, myRank+procSize-1, TOP_RIGHT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[3]);
				//first row of other
				MPI_Irecv(receivedTopRow, localColumns, MPI_UNSIGNED_CHAR, myRank+procSize, TOP_ROW_TAG, MPI_COMM_WORLD, &receiveRequestArray[4]);
		        
		        if (RGB ==1){
		            //bottom right pixel of other RGB
					MPI_Irecv(&receivedBottomRightCornerGreen, 1, MPI_UNSIGNED_CHAR, myRank-1-procSize, BOTTOM_RIGHT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[5]);
					MPI_Irecv(&receivedBottomRightCornerBlue, 1, MPI_UNSIGNED_CHAR, myRank-1-procSize, BOTTOM_RIGHT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[6]);

		            //last column of other RGB
					MPI_Irecv(receivedRightColumnGreen, localRows, MPI_UNSIGNED_CHAR, myRank-1, RIGHT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &receiveRequestArray[7]);
					MPI_Irecv(receivedRightColumnBlue, localRows, MPI_UNSIGNED_CHAR, myRank-1, RIGHT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &receiveRequestArray[8]);

		            //top right pixel of other RGB
					MPI_Irecv(&receivedTopRightCornerGreen, 1, MPI_UNSIGNED_CHAR, myRank+procSize-1, TOP_RIGHT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[9]);
					MPI_Irecv(&receivedTopRightCornerBlue, 1, MPI_UNSIGNED_CHAR, myRank+procSize-1, TOP_RIGHT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[10]);
		        }

			}
			else if(bottomRow && rightColumn){
				numberOfRequests=3;
		        if (RGB == 1)
		            numberOfRequests=7;

				sendRequestArray = (MPI_Request*) malloc(numberOfRequests*sizeof(MPI_Request));
				receiveRequestArray = (MPI_Request*) malloc(numberOfRequests*sizeof(MPI_Request));
				//first row
				MPI_Isend(image_array[0],localColumns, MPI_UNSIGNED_CHAR, myRank-procSize, TOP_ROW_TAG,MPI_COMM_WORLD, &sendRequestArray[0]);
				//top left pixel
				MPI_Isend(&image_array[0][0],1, MPI_UNSIGNED_CHAR, myRank-1-procSize, TOP_LEFT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[1]);
				//first column
				MPI_Isend(&image_array[0][0],1,column,myRank-1, LEFT_COLUMN_TAG, MPI_COMM_WORLD, &sendRequestArray[2]);
		        

		        if (RGB == 1){
		            //top left pixel RGB
		            MPI_Isend(&image_array[0][1],1, MPI_UNSIGNED_CHAR, myRank-1-procSize, TOP_LEFT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[3]);
				    MPI_Isend(&image_array[0][2],1, MPI_UNSIGNED_CHAR, myRank-1-procSize, TOP_LEFT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[4]);
				    //first column RGB
				    MPI_Isend(&image_array[0][1],1,column,myRank-1, LEFT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &sendRequestArray[5]);
				    MPI_Isend(&image_array[0][2],1,column,myRank-1, LEFT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &sendRequestArray[6]);
		        }


				//Receive	
				//last row of other
				MPI_Irecv(receivedBottomRow, localColumns, MPI_UNSIGNED_CHAR, myRank-procSize, BOTTOM_ROW_TAG, MPI_COMM_WORLD, &receiveRequestArray[0]);
				//bottom right pixel of other
				MPI_Irecv(&receivedBottomRightCorner, 1, MPI_UNSIGNED_CHAR, myRank-1-procSize, BOTTOM_RIGHT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[1]);
				//last column of other
				MPI_Irecv(receivedRightColumn, localRows, MPI_UNSIGNED_CHAR, myRank-1, RIGHT_COLUMN_TAG, MPI_COMM_WORLD, &receiveRequestArray[2]);
		        

		        if (RGB == 1){
				//bottom right pixel of other RGB
					MPI_Irecv(&receivedBottomRightCornerGreen, 1, MPI_UNSIGNED_CHAR, myRank-1-procSize, BOTTOM_RIGHT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[3]);
					MPI_Irecv(&receivedBottomRightCornerBlue, 1, MPI_UNSIGNED_CHAR, myRank-1-procSize, BOTTOM_RIGHT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[4]);

		            //last column of other RGB
					MPI_Irecv(receivedRightColumnGreen, localRows, MPI_UNSIGNED_CHAR, myRank-1, RIGHT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &receiveRequestArray[5]);
					MPI_Irecv(receivedRightColumnBlue, localRows, MPI_UNSIGNED_CHAR, myRank-1, RIGHT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &receiveRequestArray[6]);
		        }

			}
			else if(bottomRow && !leftColumn && !rightColumn){
				numberOfRequests=5;
		        if (RGB == 1)
		            numberOfRequests=13;
				sendRequestArray = (MPI_Request*) malloc(numberOfRequests*sizeof(MPI_Request));
				receiveRequestArray = (MPI_Request*) malloc(numberOfRequests*sizeof(MPI_Request));
				//first column
				MPI_Isend(&image_array[0][0],1,column, myRank-1, LEFT_COLUMN_TAG, MPI_COMM_WORLD, &sendRequestArray[0]);
				//top left pixel
				MPI_Isend(&image_array[0][0],1,MPI_UNSIGNED_CHAR, myRank-1-procSize,TOP_LEFT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[1]);
				//top row
				MPI_Isend(image_array[0],localColumns, MPI_UNSIGNED_CHAR, myRank-procSize, TOP_ROW_TAG,MPI_COMM_WORLD, &sendRequestArray[2]);
				//top right pixel
				MPI_Isend(&image_array[0][localColumns-(1*multiplier)],1, MPI_UNSIGNED_CHAR, myRank+1-procSize, TOP_RIGHT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[3]);
				//last column
				MPI_Isend(&image_array[0][localColumns-(1*multiplier)],1,column, myRank+1, RIGHT_COLUMN_TAG, MPI_COMM_WORLD, &sendRequestArray[4]);


		        if (RGB ==1){
		        //first column RGB
				    MPI_Isend(&image_array[0][1],1,column,myRank-1, LEFT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &sendRequestArray[5]);
				    MPI_Isend(&image_array[0][2],1,column,myRank-1, LEFT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &sendRequestArray[6]);

		            //top left pixel RGB
		            MPI_Isend(&image_array[0][1],1, MPI_UNSIGNED_CHAR, myRank-1-procSize, TOP_LEFT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[7]);
				    MPI_Isend(&image_array[0][2],1, MPI_UNSIGNED_CHAR, myRank-1-procSize, TOP_LEFT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[8]);
		        
		            //top right RGB
		            MPI_Isend(&image_array[0][localColumns-2],1, MPI_UNSIGNED_CHAR, myRank+1-procSize, TOP_RIGHT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[9]);                    
		            MPI_Isend(&image_array[0][localColumns-1],1, MPI_UNSIGNED_CHAR, myRank+1-procSize, TOP_RIGHT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[10]);

		            //last column RGB
		   		    MPI_Isend(&image_array[0][1],1,column,myRank+1, RIGHT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &sendRequestArray[11]);
		   		    MPI_Isend(&image_array[0][2],1,column,myRank+1, RIGHT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &sendRequestArray[12]);
		        }                    

				//Receive
				//last column of other
				MPI_Irecv(receivedRightColumn, localRows, MPI_UNSIGNED_CHAR, myRank-1, RIGHT_COLUMN_TAG, MPI_COMM_WORLD, &receiveRequestArray[0]);
				//bottom right pixel of other
				MPI_Irecv(&receivedBottomRightCorner, 1, MPI_UNSIGNED_CHAR, myRank-1-procSize, BOTTOM_RIGHT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[1]);
				//last row of other
				MPI_Irecv(receivedBottomRow, localColumns, MPI_UNSIGNED_CHAR, myRank-procSize, BOTTOM_ROW_TAG, MPI_COMM_WORLD, &receiveRequestArray[2]);
				//bottom left pixel of other
				MPI_Irecv(&receivedBottomLeftCorner, 1, MPI_UNSIGNED_CHAR, myRank+1-procSize, BOTTOM_LEFT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[3]);
				//first column of other
				MPI_Irecv(receivedLeftColumn, localRows, MPI_UNSIGNED_CHAR, myRank+1, LEFT_COLUMN_TAG, MPI_COMM_WORLD, &receiveRequestArray[4]);

		        if (RGB==1){
		        //last column of other RGB
					MPI_Irecv(receivedRightColumnGreen, localRows, MPI_UNSIGNED_CHAR, myRank-1, RIGHT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &receiveRequestArray[5]);
					MPI_Irecv(receivedRightColumnBlue, localRows, MPI_UNSIGNED_CHAR, myRank-1, RIGHT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &receiveRequestArray[6]);
		            //bottom right pixel of other RGB
					MPI_Irecv(&receivedBottomRightCornerGreen, 1, MPI_UNSIGNED_CHAR, myRank-1-procSize, BOTTOM_RIGHT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[7]);
					MPI_Irecv(&receivedBottomRightCornerBlue, 1, MPI_UNSIGNED_CHAR, myRank-1-procSize, BOTTOM_RIGHT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[8]);
		            //bottom left pixel of other RGB
					MPI_Irecv(&receivedBottomLeftCornerGreen, 1, MPI_UNSIGNED_CHAR, myRank+1-procSize, BOTTOM_LEFT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[9]);
					MPI_Irecv(&receivedBottomLeftCornerBlue, 1, MPI_UNSIGNED_CHAR, myRank+1-procSize, BOTTOM_LEFT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[10]);
		            //first column of other RGB
					MPI_Irecv(receivedLeftColumnGreen, localRows, MPI_UNSIGNED_CHAR, myRank+1, LEFT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &receiveRequestArray[11]);
					MPI_Irecv(receivedLeftColumnBlue, localRows, MPI_UNSIGNED_CHAR, myRank+1, LEFT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &receiveRequestArray[12]);
		        }

			}
			else if(bottomRow && leftColumn){
				numberOfRequests=3;
		        if (RGB == 1)
		            numberOfRequests=7;

				sendRequestArray = (MPI_Request*) malloc(numberOfRequests*sizeof(MPI_Request));
				receiveRequestArray = (MPI_Request*) malloc(numberOfRequests*sizeof(MPI_Request));
				//first row
				MPI_Isend(image_array[0],localColumns,MPI_UNSIGNED_CHAR,myRank-procSize, TOP_ROW_TAG, MPI_COMM_WORLD, &sendRequestArray[0]);
				//top right pixel
				MPI_Isend(&image_array[0][localColumns-(1*multiplier)],1,MPI_UNSIGNED_CHAR,myRank+1-procSize, TOP_RIGHT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[1]);
				//last column
				MPI_Isend(&image_array[0][localColumns-(1*multiplier)],1,column,myRank+1, RIGHT_COLUMN_TAG, MPI_COMM_WORLD, &sendRequestArray[2]);

		        if (RGB == 1){
		            //top right pixel RGB
		            MPI_Isend(&image_array[0][localColumns-2],1, MPI_UNSIGNED_CHAR, myRank+1-procSize, TOP_RIGHT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[3]);                    
		            MPI_Isend(&image_array[0][localColumns-1],1, MPI_UNSIGNED_CHAR, myRank+1-procSize, TOP_RIGHT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[4]);


		            //last column RGB
		   		    MPI_Isend(&image_array[0][localColumns-2],1,column,myRank+1, RIGHT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &sendRequestArray[5]);
		   		    MPI_Isend(&image_array[0][localColumns-1],1,column,myRank+1, RIGHT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &sendRequestArray[6]);
		        }      

				//Receive
				//last row of other
				MPI_Irecv(receivedBottomRow, localColumns, MPI_UNSIGNED_CHAR, myRank-procSize, BOTTOM_ROW_TAG, MPI_COMM_WORLD, &receiveRequestArray[0]);
				//bottom left pixel of other
				MPI_Irecv(&receivedBottomLeftCorner, 1, MPI_UNSIGNED_CHAR, myRank+1-procSize, BOTTOM_LEFT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[1]);
				//first column of other
				MPI_Irecv(receivedLeftColumn, localRows, MPI_UNSIGNED_CHAR, myRank+1, LEFT_COLUMN_TAG, MPI_COMM_WORLD, &receiveRequestArray[2]);



		        if (RGB==1){
		            //bottom left pixel of other RGB
					MPI_Irecv(&receivedBottomLeftCornerGreen, 1, MPI_UNSIGNED_CHAR, myRank+1-procSize, BOTTOM_LEFT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[3]);
					MPI_Irecv(&receivedBottomLeftCornerBlue, 1, MPI_UNSIGNED_CHAR, myRank+1-procSize, BOTTOM_LEFT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[4]);


		            //first column of other RGB
					MPI_Irecv(receivedLeftColumnGreen, localRows, MPI_UNSIGNED_CHAR, myRank+1, LEFT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &receiveRequestArray[5]);
					MPI_Irecv(receivedLeftColumnBlue, localRows, MPI_UNSIGNED_CHAR, myRank+1, LEFT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &receiveRequestArray[6]);
		        }

			}
			else if(!bottomRow && !topRow && leftColumn){
				numberOfRequests=5;
		        if (RGB == 1)
		            numberOfRequests=11;

				sendRequestArray = (MPI_Request*) malloc(numberOfRequests*sizeof(MPI_Request));
				receiveRequestArray = (MPI_Request*) malloc(numberOfRequests*sizeof(MPI_Request));
				//first row
				MPI_Isend(image_array[0],localColumns,MPI_UNSIGNED_CHAR,myRank-procSize, TOP_ROW_TAG, MPI_COMM_WORLD, &sendRequestArray[0]);
				//top right pixel
				MPI_Isend(&image_array[0][localColumns-(1*multiplier)],1,MPI_UNSIGNED_CHAR, myRank+1-procSize, TOP_RIGHT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[1]);
				//last column
				MPI_Isend(&image_array[0][localColumns-(1*multiplier)],1,column,myRank+1, RIGHT_COLUMN_TAG, MPI_COMM_WORLD, &sendRequestArray[2]);
				//bottom right pixel
				MPI_Isend(&image_array[localRows-1][localColumns-(1*multiplier)],1,MPI_UNSIGNED_CHAR,myRank+1+procSize, BOTTOM_RIGHT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[3]);
				//last row
				MPI_Isend(image_array[localRows-1],localColumns, MPI_UNSIGNED_CHAR, myRank+procSize, BOTTOM_ROW_TAG, MPI_COMM_WORLD, &sendRequestArray[4]);

		        if (RGB==1){
		            //top right pixel RGB
		            MPI_Isend(&image_array[0][localColumns-2],1, MPI_UNSIGNED_CHAR, myRank+1-procSize, TOP_RIGHT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[5]);                    
		            MPI_Isend(&image_array[0][localColumns-1],1, MPI_UNSIGNED_CHAR, myRank+1-procSize, TOP_RIGHT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[6]);        

		            //last column RGB
		   		    MPI_Isend(&image_array[0][localColumns-2],1,column,myRank+1, RIGHT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &sendRequestArray[7]);
		   		    MPI_Isend(&image_array[0][localColumns-1],1,column,myRank+1, RIGHT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &sendRequestArray[8]); 


		            //bottom right pixel RGB
				    MPI_Isend(&image_array[localRows-1][localColumns-2],1,MPI_UNSIGNED_CHAR,myRank+1+procSize, BOTTOM_RIGHT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[9]);
				    MPI_Isend(&image_array[localRows-1][localColumns-1],1,MPI_UNSIGNED_CHAR,myRank+1+procSize, BOTTOM_RIGHT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[10]);
		        }

				//Receive
				//last row of other
				MPI_Irecv(receivedBottomRow, localColumns, MPI_UNSIGNED_CHAR, myRank-procSize, BOTTOM_ROW_TAG, MPI_COMM_WORLD, &receiveRequestArray[0]);
				//bottom left pixel of other
				MPI_Irecv(&receivedBottomLeftCorner, 1, MPI_UNSIGNED_CHAR, myRank+1-procSize, BOTTOM_LEFT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[1]);
				//first column of other
				MPI_Irecv(receivedLeftColumn, localRows, MPI_UNSIGNED_CHAR, myRank+1, LEFT_COLUMN_TAG, MPI_COMM_WORLD, &receiveRequestArray[2]);
				//top left pixel of other
				MPI_Irecv(&receivedTopLeftCorner, 1, MPI_UNSIGNED_CHAR, myRank+1+procSize, TOP_LEFT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[3]);
				//first row of other
				MPI_Irecv(receivedTopRow, localColumns, MPI_UNSIGNED_CHAR, myRank+procSize, TOP_ROW_TAG, MPI_COMM_WORLD, &receiveRequestArray[4]);


		        if (RGB == 1){
		            //bottom left pixel of other RGB
					MPI_Irecv(&receivedBottomLeftCornerGreen, 1, MPI_UNSIGNED_CHAR, myRank+1-procSize, BOTTOM_LEFT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[5]);
					MPI_Irecv(&receivedBottomLeftCornerBlue, 1, MPI_UNSIGNED_CHAR, myRank+1-procSize, BOTTOM_LEFT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[6]);


		            //first column of other RGB
					MPI_Irecv(receivedLeftColumnGreen, localRows, MPI_UNSIGNED_CHAR, myRank+1, LEFT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &receiveRequestArray[7]);
					MPI_Irecv(receivedLeftColumnBlue, localRows, MPI_UNSIGNED_CHAR, myRank+1, LEFT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &receiveRequestArray[8]);


		            //top left pixel of other RGB
					MPI_Irecv(&receivedTopLeftCornerGreen, 1, MPI_UNSIGNED_CHAR, myRank+1+procSize, TOP_LEFT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[9]);
					MPI_Irecv(&receivedTopLeftCornerBlue, 1, MPI_UNSIGNED_CHAR, myRank+1+procSize, TOP_LEFT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[10]);
		        }
			}
			else{
				numberOfRequests=8;
		        if (RGB == 1)
		            numberOfRequests=20;
				sendRequestArray = (MPI_Request*) malloc(numberOfRequests*sizeof(MPI_Request));
				receiveRequestArray = (MPI_Request*) malloc(numberOfRequests*sizeof(MPI_Request));
				//first column
				MPI_Isend(&image_array[0][0],1,column,myRank-1, LEFT_COLUMN_TAG, MPI_COMM_WORLD, &sendRequestArray[0]);
				//top left pixel
				MPI_Isend(&image_array[0][0],1,MPI_UNSIGNED_CHAR,myRank-1-procSize, TOP_LEFT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[1]);
				//top row
				MPI_Isend(image_array[0],localColumns,MPI_UNSIGNED_CHAR,myRank-procSize, TOP_ROW_TAG,MPI_COMM_WORLD, &sendRequestArray[2]);
				//top right pixel
				MPI_Isend(&image_array[0][localColumns-(1*multiplier)],1, MPI_UNSIGNED_CHAR, myRank+1-procSize, TOP_RIGHT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[3]);
				//last column
				MPI_Isend(&image_array[0][localColumns-(1*multiplier)],1,column,myRank+1, RIGHT_COLUMN_TAG,MPI_COMM_WORLD, &sendRequestArray[4]);
				//bottom right pixel
				MPI_Isend(&image_array[localRows-1][localColumns-(1*multiplier)],1,MPI_UNSIGNED_CHAR,myRank+1+procSize, BOTTOM_RIGHT_CORNER_PIXEL_TAG,MPI_COMM_WORLD, &sendRequestArray[5]);
				//last row
				MPI_Isend(image_array[localRows-1],localColumns,MPI_UNSIGNED_CHAR,myRank+procSize, BOTTOM_ROW_TAG,MPI_COMM_WORLD, &sendRequestArray[6]);
				//bottom left pixel
				MPI_Isend(&image_array[localRows-1][0],1,MPI_UNSIGNED_CHAR, myRank-1+procSize, BOTTOM_LEFT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[7]);


		        if (RGB ==1){
		        //first column RGB
				    MPI_Isend(&image_array[0][1],1,column,myRank-1, LEFT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &sendRequestArray[8]);
				    MPI_Isend(&image_array[0][2],1,column,myRank-1, LEFT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &sendRequestArray[9]);



		            //top left pixel RGB
		            MPI_Isend(&image_array[0][1],1, MPI_UNSIGNED_CHAR, myRank-1-procSize, TOP_LEFT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[10]);
				    MPI_Isend(&image_array[0][2],1, MPI_UNSIGNED_CHAR, myRank-1-procSize, TOP_LEFT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[11]);

		            


		            //top right pixel RGB
		            MPI_Isend(&image_array[0][localColumns-2],1, MPI_UNSIGNED_CHAR, myRank+1-procSize, TOP_RIGHT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[12]);
		            MPI_Isend(&image_array[0][localColumns-1],1, MPI_UNSIGNED_CHAR, myRank+1-procSize, TOP_RIGHT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[13]); 


		            //last column RGB
		   		    MPI_Isend(&image_array[0][localColumns-2],1,column,myRank+1, RIGHT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &sendRequestArray[14]);
		   		    MPI_Isend(&image_array[0][localColumns-2],1,column,myRank+1, RIGHT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &sendRequestArray[15]); 


		            //bottom right pixel RGB
				    MPI_Isend(&image_array[localRows-1][localColumns-2],1,MPI_UNSIGNED_CHAR,myRank+1+procSize, BOTTOM_RIGHT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[16]);
				    MPI_Isend(&image_array[localRows-1][localColumns-1],1,MPI_UNSIGNED_CHAR,myRank+1+procSize, BOTTOM_RIGHT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[17]);


		            //bottom left pixel RGB
				    MPI_Isend(&image_array[localRows-1][1],1,MPI_UNSIGNED_CHAR,myRank-1+procSize, BOTTOM_LEFT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[18]);
				    MPI_Isend(&image_array[localRows-1][2],1,MPI_UNSIGNED_CHAR,myRank-1+procSize, BOTTOM_LEFT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &sendRequestArray[19]);
		        }

				//Receive
				//bottom right pixel of other
				MPI_Irecv(&receivedBottomRightCorner, 1, MPI_UNSIGNED_CHAR, myRank-1-procSize, BOTTOM_RIGHT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[0]);
				//last row of other
				MPI_Irecv(receivedBottomRow, localColumns, MPI_UNSIGNED_CHAR, myRank-procSize, BOTTOM_ROW_TAG, MPI_COMM_WORLD, &receiveRequestArray[1]);
				//bottom left pixel of other
				MPI_Irecv(&receivedBottomRightCorner, 1, MPI_UNSIGNED_CHAR, myRank+1-procSize, BOTTOM_LEFT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[2]);
				//first column of other
				MPI_Irecv(receivedLeftColumn, localRows, MPI_UNSIGNED_CHAR, myRank+1, LEFT_COLUMN_TAG, MPI_COMM_WORLD, &receiveRequestArray[3]);
				//top left pixel of other
				MPI_Irecv(&receivedTopLeftCorner, 1, MPI_UNSIGNED_CHAR, myRank+1+procSize, TOP_LEFT_CORNER_PIXEL_TAG,MPI_COMM_WORLD, &receiveRequestArray[4]);
				//first row of other
				MPI_Irecv(receivedTopRow, localColumns, MPI_UNSIGNED_CHAR, myRank+procSize, TOP_ROW_TAG, MPI_COMM_WORLD, &receiveRequestArray[5]);
				//top right pixel of other
				MPI_Irecv(&receivedTopRightCorner, 1, MPI_UNSIGNED_CHAR, myRank+procSize-1, TOP_RIGHT_CORNER_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[6]);
				//last column of other
				MPI_Irecv(receivedRightColumn, localRows, MPI_UNSIGNED_CHAR, myRank-1, RIGHT_COLUMN_TAG, MPI_COMM_WORLD, &receiveRequestArray[7]);


		        if (RGB == 1){
		            
		            //bottom right pixel of other RGB
					MPI_Irecv(&receivedBottomRightCornerGreen, 1, MPI_UNSIGNED_CHAR, myRank-1-procSize, BOTTOM_RIGHT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[8]);
					MPI_Irecv(&receivedBottomRightCornerBlue, 1, MPI_UNSIGNED_CHAR, myRank-1-procSize, BOTTOM_RIGHT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[9]);


		            //bottom left pixel of other RGB
					MPI_Irecv(&receivedBottomLeftCornerGreen, 1, MPI_UNSIGNED_CHAR, myRank+1-procSize, BOTTOM_LEFT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[10]);
					MPI_Irecv(&receivedBottomLeftCornerBlue, 1, MPI_UNSIGNED_CHAR, myRank+1-procSize, BOTTOM_LEFT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[11]);


		            //first column of other RGB
					MPI_Irecv(receivedLeftColumnGreen, localRows, MPI_UNSIGNED_CHAR, myRank+1, LEFT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &receiveRequestArray[12]);
					MPI_Irecv(receivedLeftColumnBlue, localRows, MPI_UNSIGNED_CHAR, myRank+1, LEFT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &receiveRequestArray[13]);


		            //top left pixel of other RGB
					MPI_Irecv(&receivedTopLeftCornerGreen, 1, MPI_UNSIGNED_CHAR, myRank+1+procSize, TOP_LEFT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[14]);
					MPI_Irecv(&receivedTopLeftCornerBlue, 1, MPI_UNSIGNED_CHAR, myRank+1+procSize, TOP_LEFT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[15]);

		            //top right pixel of other RGB
					MPI_Irecv(&receivedTopRightCornerGreen, 1, MPI_UNSIGNED_CHAR, myRank+procSize-1, TOP_RIGHT_CORNER_GREEN_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[16]);
					MPI_Irecv(&receivedTopRightCornerBlue, 1, MPI_UNSIGNED_CHAR, myRank+procSize-1, TOP_RIGHT_CORNER_BLUE_PIXEL_TAG, MPI_COMM_WORLD, &receiveRequestArray[17]);

		            //last column of other RGB
					MPI_Irecv(receivedRightColumnGreen, localRows, MPI_UNSIGNED_CHAR, myRank-1, RIGHT_COLUMN_GREEN_TAG, MPI_COMM_WORLD, &receiveRequestArray[18]);
					MPI_Irecv(receivedRightColumnBlue, localRows, MPI_UNSIGNED_CHAR, myRank-1, RIGHT_COLUMN_BLUE_TAG, MPI_COMM_WORLD, &receiveRequestArray[19]);
		        }

			}
	

			if(myRank == 0){
				MPI_Waitall(1, &sendHArray, MPI_STATUSES_IGNORE);
			}
			else{
				MPI_Waitall(1, &recvHArray, MPI_STATUSES_IGNORE);	
			
			}
		int k = 0;
		for (int i=0; i<3; i++){
			for(int j=0; j<3; j++){
				H[i][j] = startH[k];
				k++;
			}
		}

		sharpenedImageArray = (unsigned char**) malloc(localRows*sizeof(unsigned char*));
		for(int i = 0; i <localRows; i++){
			sharpenedImageArray[i] = (unsigned char*) malloc(localColumns*sizeof(unsigned char));
		}
        int startpoint=1;
        if(RGB==1){
            startpoint=3;
        }
			unsigned char centerPixel;
            //#pragma omp parallel for schedule(static) collapse(2)
			for(int i=1;i<localRows-1;i++){
				for(int j = startpoint; j < localColumns-startpoint; j++){
					centerPixel=0;
					centerPixel += (unsigned char) ((double) image_array[i-1][j-(1*multiplier)])* H[0][0];
					centerPixel += (unsigned char) ((double) image_array[i-1][j])* H[0][1];
					centerPixel += (unsigned char) ((double) image_array[i-1][j+(1*multiplier)])*H[0][2];
					centerPixel += (unsigned char) ((double) image_array[i][j-(1*multiplier)])*H[1][0];
					centerPixel += (unsigned char) ((double) image_array[i][j])*H[1][1];
					centerPixel += (unsigned char) ((double) image_array[i][j+(1*multiplier)])*H[1][2];
					centerPixel += (unsigned char) ((double) image_array[i+1][j-(1*multiplier)])*H[2][0];
					centerPixel += (unsigned char) ((double) image_array[i+1][j])*H[2][1];
					centerPixel += (unsigned char) ((double) image_array[i+1][j+(1*multiplier)])*H[2][2];
					sharpenedImageArray[i][j] = centerPixel;
				}

			}

			MPI_Waitall(numberOfRequests,receiveRequestArray, MPI_STATUSES_IGNORE);

		    if(RGB==0){
			SharpenEdges(image_array,receivedTopLeftCorner,receivedTopRightCorner,
					receivedBottomRightCorner,receivedBottomLeftCorner,receivedTopRow,
					receivedBottomRow,receivedRightColumn,receivedLeftColumn,
					H,localRows,localColumns,&sharpenedImageArray,myRank);
		    }
		    else{
		    SharpenEdgesRGB(image_array,receivedTopLeftCorner,
		        receivedTopRightCorner,receivedBottomRightCorner,
				receivedBottomLeftCorner,receivedTopRow,
				receivedBottomRow,receivedRightColumn,
				receivedLeftColumn,receivedTopLeftCornerGreen,receivedTopLeftCornerBlue,
		        receivedTopRightCornerGreen,receivedTopRightCornerBlue,
		        receivedBottomRightCornerGreen,receivedBottomRightCornerBlue,receivedBottomLeftCornerGreen,
		        receivedBottomLeftCornerBlue,receivedRightColumnGreen,receivedRightColumnBlue,
		        receivedLeftColumnGreen,receivedLeftColumnBlue,H,
		        localRows,localColumns,&sharpenedImageArray,myRank);
		    }


			MPI_Waitall(numberOfRequests,sendRequestArray, MPI_STATUSES_IGNORE);

			free(sendRequestArray);
			free(receiveRequestArray);

	
	//compare
		int flag = 0;//flag to Send
		int flagCounter = 0; //counter the flags that has receive /

		for(int i=0; i< localRows; i++){
			for(int j=0; j<localColumns; j++){
				if(((image_array[i][j] - 0.9) != (double)sharpenedImageArray[i][j]) || ((double)image_array[i][j] != (sharpenedImageArray[i][j] - 0.9))){
					flag=1;
					break;									
				}
			}
			if(flag==1) break;
		}
		if(flag != 0){	//not the same image				
			for(int i=0; i<localRows; i++){
				for(int j=0; j<localColumns; j++){
					image_array[i][j] = sharpenedImageArray[i][j];
				}
			}
			
			
		}

		MPI_Allreduce(&flag, &flagCounter, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
		if (myRank == 0){
			if (flagCounter == 0){
				imageSame = 1;
			}
			flagCounter=0;
		}
		flag=0;
		repeats ++;

	}

	finishComm=MPI_Wtime(); 
	elapsedComm = finishComm - startComm;

	MPI_Allreduce(&elapsedComm, &global_time, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	if(myRank == 0){
		mean = global_time/comSize;
		printf("Mean  time of process is %lf\n",mean);
		
	}

	MPI_File_seek( dest, offset, MPI_SEEK_SET ); 	

	startW = MPI_Wtime();
	for(int i=0;i<localRows;i++){

		MPI_File_write(dest, sharpenedImageArray[i],localColumns, MPI_UNSIGNED_CHAR,&status);
		MPI_File_seek( dest, (procSize-1)*localColumns, MPI_SEEK_CUR );
	}


	
         
	finishW = MPI_Wtime();	
	
	elapsedW = finishW - startW;
	MPI_Allreduce(&elapsedW, &global_time, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	if (myRank == 0){
		mean = global_time/comSize;
		printf("Mean  time to  write is %lf\n",mean);
	}

	MPI_File_close(&dest);		
	MPI_File_close(&fh);
	for(int i=0;i<3;i++){
		free(H[i]);
	}
	free(H);
	free(receivedTopRow);
	free(receivedRightColumn);
	free(receivedBottomRow);
	free(receivedLeftColumn);
	free(receivedLeftColumnGreen);
	free(receivedLeftColumnBlue);
	free(receivedRightColumnGreen);
	free(receivedRightColumnBlue);
	
	for(int i=0; i<localRows; i++){

		free(sharpenedImageArray[i]);
	}
	free(data);
	free(image_array);
	free(sharpenedImageArray);

	MPI_Finalize();
	

        
    exit(EXIT_SUCCESS);
}
