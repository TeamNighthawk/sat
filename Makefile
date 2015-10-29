src_dir = src
bin_dir = bin
test_dir = test
test_files_dir = test/test_files

all: clean
	gcc -g -o ${bin_dir}/satsolv ${src_dir}/satsolv.c

clean:
	rm -f ${bin_dir}/satsolv *.out ${test_files_dir}/*.cnf ${test_dir}/*.pyc
