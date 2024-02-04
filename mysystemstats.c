#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <utmp.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

int getusage(){
        struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
}
// to fint out how much memory the program is taking up

bool isInteger(const char* str) {
    if (str == NULL || *str == '\0') {
        // Empty string or NULL pointer
        return false;
    }

    // Skip leading whitespace characters
    while (*str == ' ' || *str == '\t') {
        str++;
    }

    // Check for an optional sign
    if (*str == '+' || *str == '-') {
        str++;
    }

    //Check if the remaining characters are digits
    while (*str != '\0') {
        if (!isdigit(*str)) {
            return false;
        }
        str++;
    }

    return true;
}// find if char is int


void addStringToList(char* list[], int* count, const char* newString) {
    if (*count < 100) {
        list[*count] = strdup(newString); // Duplicate the string and store it in the list
        (*count)++;
    } else {
        printf("List is full, cannot add more strings.\n");
    }
}
// to get the final output


double kbToGb(int kilobytes) {
    return (double)kilobytes / (1024.0 * 1024.0);
}
//transfer kb to gb


double get_memory_usage(char* memorylist[], int* count) {
    struct sysinfo info;//get memory repoty
    sysinfo(&info);
    char result[1024];//the result of memory usage at the specific time

    long total_phys_mem = info.totalram / 1024;
    long used_phys_mem = (info.totalram - info.freeram) / 1024;
    long total_virt_mem = (info.totalswap +info.totalram)/ 1024;
    long used_virt_mem = (info.totalswap - info.freeswap+info.totalram-info.freeram) / 1024;


    sprintf(result,"%.2f GB / %.2f GB  -- %.2f GB / %.2f GB",
           (double)used_phys_mem / 1024 / 1024, (double)total_phys_mem / 1024 / 1024,
           (double)used_virt_mem / 1024 / 1024, (double)total_virt_mem/1024/1024);
    addStringToList(memorylist,count,result);// store the sentence in result

    return (double)used_phys_mem / 1024 / 1024;// get the memory used in order to compare with the last time
}



void getNumCores() {
    FILE* file = fopen("/proc/cpuinfo", "r");//open file "/proc/cpuinfo"
    if (!file) {
        perror("Error opening /proc/cpuinfo");
        return ;//check success
    }

    int numCores = 0;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "processor") == line) {
            numCores++;
        }//count the cores
    }

    fclose(file);//close file
    printf("Num cores: %d\n", numCores);//print the result

    return ;
}


void secondsToDHMS(long seconds, int *days, int *hours, int *minutes, int *secs) {
    *days = seconds / 86400;
    seconds %= 86400;
    *hours = seconds / 3600;
    seconds %= 3600;
    *minutes = seconds / 60;
    *secs = seconds % 60;
}// phrase the second


void printlasttime() {
    double uptimeInSeconds;
    FILE *uptimeFile = fopen("/proc/uptime", "r");//open the uptime file
    fscanf(uptimeFile, "%lf", &uptimeInSeconds);// find the seconds

    fclose(uptimeFile);
    time_t currentTime;
    time(&currentTime);//current time
    int roundedsec = (int)uptimeInSeconds;
    time_t bootTime = (time_t)(currentTime - roundedsec);// get the last open time
    struct tm *tmInfo = localtime(&bootTime);

    char timeString[80];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", tmInfo);
    printf("System Last Opened (Boot Time): %s\n", timeString);//print result
}//to print the time that the system was last open


typedef struct {
    unsigned long long user;
    unsigned long long nice;
    unsigned long long system;
    unsigned long long idle;
    unsigned long long iowait;
    unsigned long long irq;
    unsigned long long softirq;
    unsigned long long steal;
    unsigned long long guest;
    unsigned long long guest_nice;
} CPUStats;//cpu stats


void readCPUStats(CPUStats* stats) {
    char line[128];
    FILE* file = fopen("/proc/stat", "r");
    if (file == NULL) {
        perror("Error opening /proc/stat");
        exit(1);//check error
    }

    fgets(line, sizeof(line), file); // Read the first line

    // Parse CPU statistics
    sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
           &stats->user, &stats->nice, &stats->system, &stats->idle,
           &stats->iowait, &stats->irq, &stats->softirq, &stats->steal,
           &stats->guest, &stats->guest_nice);// get the system stats

    fclose(file);//Close the file
}


void printCPUList(char* list[], int count) {
    printf("CPU usage List:\n");
    for (int i = 0; i < count; i++) {
        printf("%d: %s\n", i + 1, list[i]);
    }
}// print CPU usage list


void printmemoryList(char* list[], int count) {
    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
    for (int i = 0; i < count; i++) {
        printf("%d: %s\n", i + 1, list[i]);
    }
}// print memory usage list


double calculateCPUUsage(char* cpulist[], int* count) {
    CPUStats prev_stats, curr_stats;
    unsigned long long prev_total_time, curr_total_time;
    char result[1024];

    readCPUStats(&prev_stats);
    prev_total_time = prev_stats.user + prev_stats.nice + prev_stats.system +
                     prev_stats.idle + prev_stats.iowait + prev_stats.irq +
                     prev_stats.softirq + prev_stats.steal + prev_stats.guest +
                     prev_stats.guest_nice;

    sleep(1); // Wait for 1 second

    readCPUStats(&curr_stats);
    curr_total_time = curr_stats.user + curr_stats.nice + curr_stats.system +
                     curr_stats.idle + curr_stats.iowait + curr_stats.irq +
                     curr_stats.softirq + curr_stats.steal + curr_stats.guest +
                     curr_stats.guest_nice;

    // Calculate CPU usage percentage
    unsigned long long total_time_diff = curr_total_time - prev_total_time;
    unsigned long long idle_time_diff = curr_stats.idle - prev_stats.idle;

    double cpu_usage = 100.0 * (total_time_diff - idle_time_diff) / total_time_diff;
    sprintf(result,"CPU usage is %.2f %%", cpu_usage);
    // printf("%s\n", result);
    addStringToList(cpulist,count,result);//add result to list

    return cpu_usage;
}


/*
to print the memory and cpu usage without graphically
*/
void displayMemoryandCpu(int tdelay , int samples,int refresh){


    char* cpulist[100]; // Array to store strings
    int samplecount = 0; // Number of strings in the list
    char* memorylist[100]; // Array to store
    int samplememorycount = 0; //


    
    for (int i = 0; i < samples; i++)
    {
        if (refresh==0)
        {
            /* code */
            printf("\033[H\033[J"); // This clears the screen (Unix-based systems)
        }
    int curusage=getusage();
    double y=get_memory_usage(memorylist,&samplememorycount);
    double x = calculateCPUUsage(cpulist,&samplecount);
    printf("sampels: %d\n", samples);
    printf("---------------------------------------\n");
    printf("tdelay: %d\n", tdelay);
    printf("---------------------------------------\n");
    printf("memory usage: %dkb\n", curusage);
    printf("---------------------------------------\n");
    printmemoryList(memorylist,samplememorycount);
    for (int j = i; j < samples; j++)
    {
        printf("\n");
    }
    printf("---------------------------------------\n");
    printCPUList(cpulist,samplecount);        
    sleep(tdelay);


    }

}


/*
just adding the graphics functions
*/
double calculateCPUUsagegraph(double previous,char* cpulist[], int* count){
    CPUStats prev_stats, curr_stats;
    unsigned long long prev_total_time, curr_total_time;
    char result[1024];

    readCPUStats(&prev_stats);
    prev_total_time = prev_stats.user + prev_stats.nice + prev_stats.system +
                     prev_stats.idle + prev_stats.iowait + prev_stats.irq +
                     prev_stats.softirq + prev_stats.steal + prev_stats.guest +
                     prev_stats.guest_nice;

    sleep(1); // Wait for 1 second

    readCPUStats(&curr_stats);
    curr_total_time = curr_stats.user + curr_stats.nice + curr_stats.system +
                     curr_stats.idle + curr_stats.iowait + curr_stats.irq +
                     curr_stats.softirq + curr_stats.steal + curr_stats.guest +
                     curr_stats.guest_nice;

    // Calculate CPU usage percentage
    unsigned long long total_time_diff = curr_total_time - prev_total_time;
    unsigned long long idle_time_diff = curr_stats.idle - prev_stats.idle;

    double cpu_usage = 100.0 * (total_time_diff - idle_time_diff) / total_time_diff;

        /* code */
    sprintf(result,"CPU usage is %.2f %%", cpu_usage);
    strcat(result,"| | | ");
    for (double i = 0; i < cpu_usage; i++)
    {
        /* code */
        strcat(result,"| ");
    }
    
    addStringToList(cpulist,count,result);
    
    return cpu_usage;
}


/*
just adding the graphics functions
*/
double calculatememorygraph(double previous,char* memorylist[], int* count){
    struct sysinfo info;
    sysinfo(&info);
    char result[1024];

    long total_phys_mem = info.totalram / 1024;
    long used_phys_mem = (info.totalram - info.freeram) / 1024;
    long total_virt_mem = (info.totalswap +info.totalram)/ 1024;
    long used_virt_mem = (info.totalswap - info.freeswap+info.totalram-info.freeram) / 1024;


    sprintf(result,"%.2f GB / %.2f GB  -- %.2f GB / %.2f GB",
           (double)used_phys_mem / 1024 / 1024, (double)total_phys_mem / 1024 / 1024,
           (double)used_virt_mem / 1024 / 1024, (double)total_virt_mem / 1024 / 1024);
    double res=(double)used_phys_mem / 1024 / 1024;
    strcat(result,"   |");
    if (previous!=-1)
    {
        /* code */
        double diff=res-previous;
        if (diff >0)
        {
            for (double i = 0; i < diff*100; i++)
            {
                /* code */
                strcat(result,"#");
            }
            strcat(result,"*");
            char cur[100];
            sprintf(cur,"(%.2lf)",res);
            strcat(result,cur);
            
        }
        else
        {
            diff=diff *-1;
            for (double i = 0; i < diff*100; i++)
            {
                /* code */
                strcat(result,":");
            }
            strcat(result,"@");
            char cur[100];
            sprintf(cur,"(%lf)",res);
            strcat(result,cur);
        }
        

    }
    addStringToList(memorylist,count,result);

    return res;
}

/*
display int graph
*/
void displaymemoryandcpuingarph(int tdelay , int samples, int sqe){
    double previousmem=-1;
    double previouscpu=-1;
    double x;
    double y;
    char* cpulist[100]; // Array to store strings
    int samplecount = 0; // Number of strings in the list
    char* memorylist[100]; // Array to store
    int samplememorycount = 0; //
    
    for (int i = 0; i < samples; i++)
    {
        if (sqe==0)
        {
            printf("\033[H\033[J"); // This clears the screen (Unix-based systems)
        }
        

    x = calculateCPUUsagegraph(previouscpu,cpulist,&samplecount);
    y=calculatememorygraph(previousmem,memorylist,&samplememorycount);
    previousmem=y;  
    previouscpu=x;
    int curusage=getusage();
    printf("sampels: %d\n", samples);
    printf("---------------------------------------\n");
    printf("tdelay: %d\n", tdelay);
    printf("---------------------------------------\n");
    printf("memory usage: %dkb\n", curusage);
    printf("---------------------------------------\n");
    printmemoryList(memorylist,samplememorycount);
    for (int j = i; j < samples; j++)
    {
        printf("\n");
    }
    
    printf("---------------------------------------\n");
    printCPUList(cpulist,samplecount);        

    sleep(tdelay);
    }

}

/*
get the user info
*/
void printuserinfo(){
    struct utmp *utent;

    setutent(); // Open the utmp file
    printf("%s %-s %s\n", "Login Name", "Terminal", "Host");
    printf("---------------------------------------\n");

    while ((utent = getutent()) != NULL) {
        if (utent->ut_type == USER_PROCESS) {
            printf("%s %s %s\n",
                   utent->ut_user, utent->ut_line, utent->ut_host);
        }
    }

    endutent(); // Close the utmp file

    return ;
}

/*
most of the other things to print 
*/
void printbasicsysteminfo(){
        struct utsname systemInfo;
        struct sysinfo sysInfo;

    if (uname(&systemInfo) != -1) {
        printf("### System Information ###\n");
        printf("System Name: %s\n", systemInfo.sysname);//system name
        printf("Machine Name: %s\n", systemInfo.nodename);//machine name
        printf("release : %s\n", systemInfo.release);//release
        printf("version : %s\n", systemInfo.version);// version
        printf("Architecture Name: %s\n", systemInfo.machine);// architecture name

    } else {
        perror("uname");//check error
    }

}




int main(int argc, char **argv) {
    int samples =10;
    int delay =1;
    char ** res;
    int samplefound=0;
    int delayfound=0;
    int printgrapg=0;
    int printsys=0;
    int printuser=0, printgraph=0,printsequential=0;// the init status of what to print


    if (argc == 1) {//print as the most basic form
        displayMemoryandCpu(delay ,samples,0);
        // displaymemoryandcpuingarph(delay ,samples); 
        printf("--------------------------------------------------------\n");
        printuserinfo();
        printf("--------------------------------------------------------\n");
        getNumCores();
        printf("--------------------------------------------------------\n");
        printbasicsysteminfo(); // Display
        printlasttime(); // Display last time
    }
    else{
        int i=1;
        while(i<argc) {
        char *stringPart = strtok(argv[i], "=");
        int intPart;
        char *token; 
        if (strcmp(stringPart, "--system") == 0) {
            printf("This is system.\n");
            printsys=1;
            // displayMemoryandCpu(delay ,samples,1);
            // printbasicsysteminfo(); // Display
            
        }
        else if (strcmp(stringPart, "--sequential") == 0) {
            printsequential=1;
            // displayMemoryandCpu(delay ,samples,0);
            // printuserinfo();
            // printf("--------------------------------------------------------\n");
            // getNumCores();
            // printf("--------------------------------------------------------\n");
            // printbasicsysteminfo(); // Display
            // printlasttime(); // Display last time
        }
        else if ( strcmp(stringPart, "--user") == 0){
            printf("This is user.\n");
            printuser=1;
            // print_sessions_users();
            // printuserinfo();
        } 
        else if(strcmp(stringPart, "--graphics") == 0 ||strcmp(stringPart, "--g") == 0){
            printf("This is graphic.\n");
            printgraph=1;
            // displaymemoryandcpuingarph(delay ,samples,0); 
        }
        else if (strcmp(stringPart, "--tdelay") == 0)
        {
            //change delay
            delay = atoi(strtok(NULL, "")); 
            delayfound = 1;
            printf("tdelay is set to %d seconds.\n", delay);
        }
        else if (strcmp(stringPart,"--samples")==0)
        {
            // change sample
            samples = atoi(strtok(NULL, "")); 
            samplefound=1;
            printf("samples is set to %d \n",samples);
        }
        else if (isInteger(stringPart) ==true)
        {
            if (samplefound ==0)
            {
                // change sample
                samplefound=1;
                samples=atoi(argv[i]);
                printf("samples is set to %d \n",samples);
            }
            else if (samplefound ==1 &&delayfound ==0){
                //change delay
                delayfound=1;
                delay=atoi(argv[i]);
                printf("tdelay is set to %d seconds.\n", delay);
            }
            else
            {
                printf("invalid input.\n");//other
            }
            
            
        }
        else
        {
            printf("invalid input.\n");
            break;
        }
        
        i++;
        }
        //let's see the final answer
        //system only
        if (printsys==1 &&printuser==0)
        {
            /* code */
            if (printgraph==1)
            {
                displaymemoryandcpuingarph(delay,samples,printsequential);
                printf("--------------------------------------------------------\n");
                getNumCores();
            }
            else
            {
                displayMemoryandCpu(delay,samples,printsequential);
                printf("--------------------------------------------------------\n");
                getNumCores();
            }
            
            
        }
        //user only
        else if (printsys==0 &&printuser==1)
        {
            printf("--------------------------------------------------------\n");
            printuserinfo();
        }
        //print both
        else

        {
            if (printgraph==1)//printgraph
            {
                displaymemoryandcpuingarph(delay,samples,printsequential);
                printf("--------------------------------------------------------\n");
                getNumCores();
                printf("--------------------------------------------------------\n");
                printuserinfo();
                printf("--------------------------------------------------------\n");
                printbasicsysteminfo(); // Display
                printlasttime(); // Display last time
            }
            else//not printgraph
            {
                displayMemoryandCpu(delay,samples,printsequential);
                printf("--------------------------------------------------------\n");
                getNumCores();
                printf("--------------------------------------------------------\n");
                printuserinfo();
                printf("--------------------------------------------------------\n");
                printbasicsysteminfo(); // Display
                printlasttime(); // Display last time
            }
        }
        
        
        
    }
    return 0;
}
