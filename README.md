# mySystemStats
this is a file to monitor the stats of a linux system
## funcions
### bool isInteger(const char* str)
identify whether a char is int
### addStringToList(char* list[], int* count, const char* newString) 
addStringToList
### print_memory_usage(char* memorylist[], int* count) 
print the memory usage and then add to the memorylist
### void getNumCores()
get the number of the cores of the computer
### secondsToDHMS(long seconds, int *days, int *hours, int *minutes, int *secs)
reformat the second (will be used in print last time function)
### printlasttime() 
print the last usage time
### void printCPUList(char* list[], int count) 
print the cpu suage list
### printmemoryList(char* list[], int count)
print the memory list
### calculateCPUUsage(char* cpulist[], int* count)
calculate the cpu usage and put it into cpulist
### displayMemoryandCpu(int tdelay , int samples,int refresh)
print the system stats by the demand
### calculateCPUUsagegraph(double previous,char* cpulist[], int* count)
print the cpu usage and its graph then add to the cpulist
### calculatememorygraph(double previous,char* memorylist[], int* count)
calculate the memory usage and its graph then put it into memorylist
### printuserinfo()
-
print the user info 
like:
login_name terminal host
--------------------------
wushua39 tty2 tty2
### printbasicsysteminfo()-
print the other system info in this way:
System Name: Linux
Machine Name: iits-b473-20
release Name: 6.3.1-060301-generic
Architecture Name: x86_64
version Name: #202304302031 SMP PREEMPT_DYNAMIC Mon May  1 00:41:22 UTC 2023
(for example)
### main(int argc, char **argv)
to understand the input sentence and then perform the report

## usage

To run the program, compile the code using the following command:

```console
$ gcc mySystemStats.c -o mySytemStats
```
And then run the executable:

```console
$ ./mySytemStats
```
## --system
to indicate that only the system usage should be generated
``` console
$ ./mySytemStats --system
```
## --user
to indicate that only the users usage should be generated
``` console
$ ./mySytemStats --user
```
## --graphics
to include graphical output in the cases where a graphical outcome is possible as indicated below.

``` console
$ ./mySytemStats --graphics
```
or
``` console
$ ./mySytemStats --g
```
## --sequential
to indicate that the information will be output sequentially without needing to "refresh" the screen 
``` console
$ ./mySytemStats --sequential
```
## --samples=N
N will indicate how many times the statistics are going to be collected and results will be average and reported based on the N number of repetitions.
If not value is indicated the default value will be 10.
``` console
$ ./mySytemStats --samples=N
```
## --tdelay=T
to indicate how frequently to sample in seconds.
If not value is indicated the default value will be 1 sec
``` console
$ ./mySytemStats --tdelay=T
```
## simplified way to set the delay
just in the order of sample and delay
for example
``` console
$ ./mySytemStats 1 1
```
you can even make it 
``` console
$ ./mySytemStats 1 --system 1
```
both of them will set the sample and delay into 1 and 1

## the output result
The reported "stats" should include:

### running parameters:
        number of samples
        samples frequency
        memory self-utilization (ie. how much memory the monitoring tool is actually using)
### user usage
  report how many users are connected in a given time
  report how many sessions each user is connected to

### system usage
      report how much utilization of the CPU is being done
      report how much utilization of memory is being done (report used and free memory)

      Total memory is the actual physical RAM memory of the computer.
      Virtual memory accounts for the physical memory and swap space together -- swap is the amount of space (usually in disk or any other storage device) assigned by the OS to be used as memory in case of running out of physical space.
      if the --graphics flag is used, generate a graphical representation showing the variation of memory used
### system information
        system name
        machine name
        OS version
        OS release
        machine architecture
        time that the system has been running since its last reboot
# how i solve the problem 
 i look at the provided material and then search on the internet about what the memory and cpu usage means, then i found that i can find the data by reading the files like "/proc/cpuinfo" and "/proc/stat"
 after applying the knowledge of file IO which i learnt on the pcrs and course by the professor, i get the data i want to get.
 after that i began on displaying the data i collected from the lib and files then i solve the problem 




















