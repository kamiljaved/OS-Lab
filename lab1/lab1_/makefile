bin_dir ?= bin/

generate: generate.c
	@echo 'Building target: $@'
	gcc generate.c -o $(bin_dir)generate -Wall -Werror

dump: dump.c
	@echo 'Building target: $@'
	gcc dump.c -o $(bin_dir)dump -Wall -Werror

fastsort: fastsort.c
	@echo 'Building target: $@'
	gcc fastsort.c -o $(bin_dir)fastsort -Wall -Werror

seed ?= 0
recs ?= 5
inp ?= ./tmp/inputfile
out ?= ./tmp/outputfile
method ?= bubble

test: generate dump fastsort
	./$(bin_dir)generate -s $(seed) -n $(recs) -o $(inp)
	./$(bin_dir)fastsort -i $(inp) -o $(out) -a ${method}

test1: generate dump fastsort
	./$(bin_dir)generate -s $(seed) -n $(recs) -o $(inp)
	./$(bin_dir)fastsort -i $(inp) -o $(out) -a ${method} -p -k

test2: generate dump fastsort
	./$(bin_dir)generate -s $(seed) -n $(recs) -o $(inp)
	./$(bin_dir)fastsort -i $(inp) -o $(out) -a ${method} -p

clean:
	rm -f ./$(bin_dir)generate ./$(bin_dir)dump ./$(bin_dir)fastsort ${inp} ${out}