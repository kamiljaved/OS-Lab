bin_dir ?= bin/

bf ?= ./tmp/batchFile

execute.o: execute.c
	@echo '[Building target: $@]'
	gcc -c -o $(bin_dir)execute.o execute.c -O

helper.o: helper.c
	@echo '[Building target: $@]'
	gcc -c -o $(bin_dir)helper.o helper.c -O

shell.o: shell.c
	@echo '[Building target: $@]'
	gcc -c -o $(bin_dir)shell.o shell.c -O

main.o: main.c
	@echo '[Building target: $@]'
	gcc -c -o $(bin_dir)main.o main.c -O

mysh: main.o execute.o helper.o shell.o
	@echo '[Building target: $@]'
	gcc -o $(bin_dir)mysh $(bin_dir)helper.o $(bin_dir)execute.o $(bin_dir)shell.o $(bin_dir)main.o

new: clean mysh

shell: mysh
	@echo '[Starting Shell...]'
	clear
	./$(bin_dir)mysh

shelln: clean mysh
	@echo '[Starting Shell...]'
	clear
	./$(bin_dir)mysh

shellb: clean mysh
	@echo '[Starting Shell...]'
	clear
	./$(bin_dir)mysh $(bf)

shelld: clean mysh
	@echo '[Starting Shell...]'
	clear
	./$(bin_dir)mysh -d

clean:
	rm -f ./$(bin_dir)*.o ./$(bin_dir)mysh