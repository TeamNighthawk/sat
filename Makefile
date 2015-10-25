all: clean
	gcc -o satsolv satsolv.c

clean:
	rm -f satsolv *.out *.cnf *~
