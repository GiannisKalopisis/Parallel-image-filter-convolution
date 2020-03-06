# Parallel-image-filter-convolution
This project is a parallel image convolution application using MPI and OpenMP.

## Project summary
In this work MPI and OpenMP standards are used for parallel image processing with non-blocking communication between processes and parallel I/O. The rendering of the image is repeated until convergence is achieved. Non-blocking communication overlaps the time lost from communication between processors, with calculations. 

## MPI Datatypes
By using the appropriate MPI Datatype Column to send the first and last pixels of the image without the use of an intermediate buffer, we save time and memory.

## Parallel I/O
Each process reads and writes to the corresponding part of the image, that its assigned to it. This reduces both the time of reading and writing and the communication between the processes as the image does not need to be sent from the master process to the rest and correspondingly at the end to retrieve it.

## Measurements
  - **MPI Grey Image**
  
  |                | 1             | 4          | 9           | 16           | 25           | 36           | 64           |
  | :-------------:|:-------------:|:----------:|:-----------:|:------------:|:------------:|:------------:|:------------:|
  | **Read**           | 3.5745479     | 3.5159042  |3.7380148    |3.9340938     |5.1139713     |5.7315604     |6.3847808     |
  | **Calculations**   | 4.7472401     | 3.3943974  |1.9754175    |1.7649175     |1.6002157     |1.52450783    |1.4991442     |
  | **Write**          | 4.5736383     | 5.5422219  |5.749798     |7.9885803     |9.6555623     |11.5538539    |15.3479368    |
  

  - **MPI RGB Image**
  
  |                | 1             | 4          | 9           | 16           | 25           | 36           | 64           |
  | :-------------:|:-------------:|:----------:|:-----------:|:------------:|:------------:|:------------:|:------------:|
  | **Read**           | 4.0965889     | 4.493452  |4.900173    |5.001458     |5.569235     |6.038952     |7.0346531     |
  | **Calculations**   | 6.335689     | 5.594490  |4.0018496    |3.3654422     |2.8236234     |2.4120980    |2.1567228     |
  | **Write**          | 6.2863584     | 6.9542342  |7.1437362     |9.2345170     |10.5724136     |10.9264563    |13.4623461 |
  
  
  - **MPI+OpenMP Grey Image**
  
  |                | 1             | 4          | 9           | 16           | 25           | 36           |
  | :-------------:|:-------------:|:----------:|:-----------:|:------------:|:------------:|:------------:|
  | **Read**           | 3.9123417     | 4.8022319  |4.646164    |9.523439     |7.7322494     |7.813583     |
  | **Calculations**   | 3.0981239     | 2.6885567  |1.4844748    |1.0073215     |0.7877467     |1.0377608    |
  | **Write**          | 4.9347563     | 5.5878421  |8.1640457     |10.6001583     |10.9791241     |12.0770332    |
  
  
  - **MPI+OpenMP RGB Image**
  
  |                | 1             | 4          | 9           | 16           | 25           | 36           |
  | :-------------:|:-------------:|:----------:|:-----------:|:------------:|:------------:|:------------:|
  | **Read**           | 5.1123525     | 7.0459522  |5.2429888    |6.795392     |7.6580692     |8.7994973     |
  | **Calculations**   | 8.8534159     | 8.1770325  |3.7852966    |2.4146305     |1.5947278     |1.3748322    |
  | **Write**          | 5.7457425     | 6.3164081  |8.4643856     |11.2552024     |14.2507892     |16.0013461    |


## Conclusion
We generally observed that as the number of processes in MPI increased, we saw a decrease of time. However, this decrease was not linear, i.e. the duplication of processes didn't lead to half time. The reasons for this non-linear decrease in time is  the large number of messages due to the increase in border pixels between processes. It was also observed that as we proceeded to convergence, the computation times were reduced due to simplification of calculations. 

Even with the use of OpenMP along with MPI to calculate pixels, we have observed reduce of time, which makes sense since the calculation is assigned 4 threads per process, each machine runs from 1 process according to the machines file, where the number of machine processes are written, and so it takes less computation time since threads are faster and lighter than processes due to their "nature/implementation". But after 25 processes, which are backed by 4 threads, we saw an increase in time again. We generally saw significant improvement with the use of hybrid programming (MPI + OpenMP) and specifically RGB picture. 

We also saw that as processes increased, so did I/O time, which is reasonable since only 1 processor can access the hard disk at a time or for reading either for writting, and thus the competition and race conditions resulting from the processes, delays the program. 

Finally the time between Gray and RGB image is almost three times, which is explained perfectly from the size of the image (it is 3 times larger than Gray since each pixel is made up of 3 bytes).

## Possible extensions
The program can be expanded to various image sizes and types. Also the communication that it can be used for other files such as video, audio etc.

## Compile - Machines

### Compiler Version
The compiler version of gcc must be `4.4 or newer`. Check it by typing `gcc --version`

### mpicc Version
To install mpicc you have to type `apt install mpich`.

### Compilation and Makefile targets

The machines file shows which machines work and how many cores we want to use from each one.

The compile instructions are:
  - `make all` : compile all the program
  - `make exec` : execution at local machine
  - `make mexec` : execution for the machines and cores that are written at the machine file
  - `make clean` : deleting executable and .o files
  
To change the processes running the program you need to change at makefile the -n parameter for either target exec (for local execution) or target mexec. Also in order to run the program with other images you will need to change the file names in the code.

If you do not want to run the program using makefile you can run the command:

```mpiexec -f machines -n X ./project_parallhla -r Y -c Z -i N```

  - X: number of processes
  - Y: number of lines of image
  - Z: number of columns of image
  - N: type of image (Grey/RGB)
  
  
