src_dir = src
bin_dir = bin
test_dir = test
test_files_dir = test/test_files

all:
	gcc -g -o ${bin_dir}/satsolv ${src_dir}/satsolv.c

test: clean all
	python ${test_dir}/tester.py

clean:
	rm -f ${bin_dir}/satsolv *.out ${test_files_dir}/*.cnf ${test_dir}/*.pyc ${test_dir}/test_results.txt
