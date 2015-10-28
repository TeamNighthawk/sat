src_dir = src
bin_dir = bin

all: clean
	gcc -g -o ${bin_dir}/satsolv ${src_dir}/satsolv.c

clean:
	rm -f ${bin_dir}/satsolv *.out *.cnf *~
