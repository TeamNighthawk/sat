all: clean
	gcc -g -o satsolv satsolv.c

clean:
	rm -f satsolv *.out *.cnf *~
