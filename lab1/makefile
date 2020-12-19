#target: ingredient[s]
#	recipe

fastsort: fastsort.c
	gcc fastsort.c -o fastsort -Wall -Werror

generate: generate.c
	gcc generate.c -o generate -Wall -Werror

run_fast: fastsort
	./fastsort -i inputfile -o outputfile
