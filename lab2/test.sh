# name of the module to be tested.
Module_Name=$1
Module_File="$Module_Name.ko"
# test performance with 1, 2, or 4 threads.
Test_Num_Threads="1 2 4"
# test performance for different sizes.
Test_Upper_Bound="100 200 500 1000 2000 4000 8000 10000 20000 40000 100000"

# create files and write header
echo "name: number of primes" > nop.csv
echo "name: unnecessary cross outs" > uco.csv
echo "name: set up time" > sut.csv
echo "name: processing time" > pt.csv
# create output directory
for num_threads in $Test_Num_Threads
do
	for upper_bound in $Test_Upper_Bound
	do
		# clears the dmesg for output
		sudo dmesg --clear
		# install module
		sudo insmod $Module_File num_threads=$num_threads upper_bound=$upper_bound
		# sleep for 2 seconds to wait for the module to complete its work
		sleep 2
		# remove module
		sudo rmmod $Module_Name
		# parse dmesg using grep and then and print to file
		dmesg | grep "total number of primes:" >> nop.csv
		dmesg | grep "unnecessary cross out:" >> uco.csv
		dmesg | grep "time spent for setting up module:" >> sut.csv
		dmesg | grep "time spent for processing primes:" >> pt.csv
	done
done

# done testing
exit 0

