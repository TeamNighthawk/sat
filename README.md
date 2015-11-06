satsolv
=======
satsolv is a Boolean Satisfiability (SAT) solver that parses input files given
in the standard DIMACS format.

Running
-------
To run satsolv simply compile with make and then invoke the satsolv executable that
is put in the `bin` directory.

```
make
bin/satsolv <inputfile>
```

Testing
-------
Our test suite uses differential testing to compare the output of satsolv against
the well known MiniSat solver. To invoke this test suite do simply invoke the `test`
target in the Makefile.

```
make test
```

Group Members
-------------
* Jonathan Whitaker - jon.b.whitaker@gmail.com
* Dalton Wallace - daltonbwallace@gmail.com
* Jay Tuckett - 12crown4@gmail.com
* Tarik Courdy - freedomfighter1986@gmail.com

Todos
-----
* Make `test` target in the Makefile
