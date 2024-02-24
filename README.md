# Algorithms-DGIM
This repository contain a program that uses the DGIM algorim to sum up 1,000,000 integer. DGIM(Datar-Gionis-Indyk-Motwani) is algorithm that are famous in the big data field. All the code are written by myself.


Files:
Run dgim_linux.c and dgim.h on linux system will generate an excutable program called dgim_linux, which will run the DGIM algorithm and print the result. However, before running it, you should provide data files.

Run rand_generator_linux.c will generate the rand_generator_linux excutable program, which will produce 8 data files that feed the DGIM algorithm: 1 file contain 1,000,000 random integers range from [1, 127]; and 7 files contain 1,000,000 bit point, one for each position in the 2's complement form of the first file.
