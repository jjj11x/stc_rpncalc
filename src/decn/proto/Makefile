all: div ln exp

div: div_mfp.cpp
	g++ -Wall div_mfp.cpp -o div -lgmp

ln: ln_mfp.cpp
	g++ -Wall ln_mfp.cpp -o ln -lmpfr

exp: exp.cpp
	g++ -Wall exp.cpp -o exp -lmpfr
