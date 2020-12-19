  
# Programming Assignment 3  
  
In this assignment you have to accelerate the grid-solver application discussed in lectures on CPU and GPU. The grid in this assignment is based on a grayscale image.  
  
## Sequential application  
A sequential version of the application is provided, for your convenience, in the directory “sequential”. The `gridSolver` function implements the red-black coloring approach described in the lectures which are more suitable for parallelism. Please read the code carefully, and try to understand it. You have to only accelerate the `gridSolver` function. The *rgb2gray* function is only included to form the grid comprised of single floats. There are no assumptions about the size of the input images, thus the code must be capable of running with color images of any size. The output must match the sequential version; a compare utility is provided to test for this. 2 images are provided for testing. You can download these images from https://drive.google.com/drive/folders/11FDEmVCzRHLLDVMn2fohCGDYEBh3xLUo?usp=sharing. Running the makefile in the "sequential" directory will compile the program. To run the application:  

```  
$ ../bin/sequential <image file>  

```  

## Accelerated application  
  
1- Accelerate the `gridSolver` function using OpenMP and Intel SPMD (or Intel intrinsics). Use the techniques discussed in the lecture 11-12 to reduce the synchronization overhead.  
  
2- Further accelerate the function by improving locality as described in lecture 23-24.  
  
3- Accelerate the function on GPU. Use CUDA streams to hide the copying time of the image with GPU computation.  
  

## Speedup  
  
You have to measure and report several different speedups:  
  
Speedup of CPU implementation = (`gridSolver` time in sequential program) / (`gridSolver` time in CPU acceleration)  
  
Speedup of GPU implementation over sequential = (`gridSolver` time in sequential program) / (`gridSolver` time in GPU)  
  
Speedup of GPU implementation over CPU acceleration = (`gridSolver` time in CPU acceleration / (`gridSolver` time in GPU)  
  
The total time on GPU must include image copying times.  
  
  
Do not include the printing/debugging etc in timing calculations. The speed up results should be reported for both images.  

## Report  
Speedup results are important but we are also interested in the procedure you followed to get those results and  
their analysis. In the report, you need to mention at least the following along with the speedup results:  
  
- How you performed the four steps of Decomposition, Assignment, Orchestration, and Mapping in creating your parallel application?  
- Performance of the GPU kernel indicated on the roofline model.  
- Important points regarding implementation strategy. For instance, how did you improve the locality?  
- Problems/bottlenecks encountered. For instance, how did you tackle the problem of reading/writing to single `diff` variable?  
- Optimizations performed, e.g. the use of shared memory, did it help or not and why did it help?  
- Interesting profiling results to show that the GPU kernel has been optimized sufficiently.  
- Compiler options.  
  
## Submitting the assignment  
Submit a single ZIP archive with the name `prog_assn_3_<your roll number>.zip` containing your working solution. Check the output images after each step and compare them to see if they are correct. Clean the code and do not include images in this archive (as they are too big). Timing calculations and meaningful printing of results should be inside code so that we can verify your reported results by simply running it (without the need to write the timing code ourselves). Put the comments in your code to explain what are you doing, so that we know you understand it.
