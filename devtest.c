#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    unsigned int result;
    int fd;     // File descriptor 
    int i, j;   // Loop variables
    
    char input = 0;

    // place the i and j values
    int *num_array = (int*)malloc(2 * sizeof(int));

    // get the raw data from hardware in the form of char array
    char *char_array = (char*)malloc(12);
    
    unsigned int write_res, read_i, read_j;
    
    // Open device file for reading and writing 
    // Use 'open' to open '/dev/multiplier'
    const char dev_name[] = "/dev/multiplier";
    fd = open(dev_name, O_RDWR);
    
    // Handle error opening file 
    if(fd == -1) {
        printf("Failed to open device file!\n");
        return -1;
    }
    
    for(i = 0; i <= 16; i++) {
        for(j = 0; j <= 16; j++) {
            // Write value to registers using char dev 
            // Use write to write i and j to peripheral 
            // populate the array to be written to hardware file
            num_array[0] = i;
            num_array[1] = j;
 
            // debug information
            printf("i = %d d = %d\n", i, j);
            
            //write to hardware file
            write_res = write(fd, num_array, 2 * sizeof(int));
            printf("Response from hardware after writing: bytes written = %u\n", 
			write_res);

            // Read i, j, and result using char dev
            result = read(fd, char_array, 12);
 
            // type cast: char -> int
            int *converted_data = (int*)char_array;

            read_i = converted_data[0];
            read_j = converted_data[1];
            result = converted_data[2];

            // Use read to read from peripheral 
            // print unsigned ints to screen 
            printf("%u * %u = %u\n\r", read_i, read_j, result);
                
            // Validate result 
            if(result == (i*j))
                printf("Result Correct!\n");
            else
                printf("Result Incorrect!\n");
                
            // Read from terminal 
            input = getchar();
            // Continue unless user entered 'q' 
            if(input == 'q') {
                close(fd);
                return 0;
            }
        }
    }
    close(fd);
    return 0;
}
