## Assignment 3
<sub>by Lior Belenkov (319125134) & Jameel Khalifa (204666085)</sub>

### How to build and run
0. make sure cmake is installed, change to project directory
1. run command: ```cmake -S. -B ./build```
2. run command: ```cmake --build ./build```
3. copy/extract from ./build/myrobot/myrobot file to the working directory
4. copy/extract from ./build/Algo_319125134/libAlgo_319125134.so to to the desired Algorithms directory (or in the same directory as myrobot)
5. copy/extract from ./build/Algo_319125134/libAlgo_319125134.so to to the desired Algorithms directory
7. run my robot with desired Algorithm directory and House directories. **Example command**: 
```./myrobot -algo_path="./algorithms" -house_path="./houses" -num_threads=3```

### Notices
* In simulator_manager.cpp there is constexp **TIME_QUANTUM**. This is the amount of milliseconds we multiply by MaxSteps to calculate the timeout for each house.\
For example: TIME_QUANTUM = 2, MaxSteps in House input is 150. The resulting timeout for each algorithm running on this house will be 2 * 150 = 300 milliseconds.\
You are welcome to change TIME_QUANTUM value, in that case please build the project again - you can solely rebuild the myrobot project (```cmake --build ./build/myrobot```)
* To ensure the program does not dlclose the algorithm libraries during thread operation, which results in **segfault**, the program will wait for all threads to join before dlclose. **HOWEVER** the program will output results such as the summary and output files right after threads either timed out or finished beforehand. So while the program is not done running, it will output correct result files with timeout in mind (elaborated in the excel file).\
For example: If an algorithm has an infinite while loop, after it's timeout the summary will be generated with appropriate score, but the program will still run.
* myrobot will generate all files in the working directory (./), from which you run the and not in the myrobot directory
* -summary_only flag only produces the summary.csv file and .error files