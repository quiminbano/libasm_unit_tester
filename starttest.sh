#!/bin/bash

function delete_outputs()
{
	if [ -d "outputs" ]; then
		rm -rf outputs
	fi
}

function run_test_function()
{
	if [ ! -d "outputs" ]; then
		mkdir -p outputs
	fi
	echo -e "\033[1;37m\nft_$1\033[0m"
	if c++ -Wall -Wextra -Werror -Wno-nonnull -arch x86_64 -std=c++11 \
	src/test_"$1".cpp -Iinclude -L.. -lasm -o \
	outputs/test_"$1" &> /dev/null ; then
		for i in $(seq 1 "$2"); do
			./outputs/test_"$1" $i
			sleep 0.1
		done
		rm -f outputs/test_"$1"
	else
		echo -e "\033[0;31m[ERROR COMPILING THE TEST]\033[0m"
		sleep 0.1
	fi
}

function run_mandatory()
{
	individualTests=("strlen" "strcpy" "strcmp" "write" "read" "strdup")
	numberOftestsPerTest=(5 5 10 1 1 1)
	index=0
	for instruction in "${individualTests[@]}"; do
		run_test_function "$instruction" "${numberOftestsPerTest[$index]}"
		index=$((index + 1))
	done
}

function run_bonus()
{
	individualTests=("atoi_base" "list_size" "list_push_front" "list_sort" \
	"list_delete_if")
	numberOftestsPerTest=(1 1 1 1 1)
	index=0
	for instruction in "${individualTests[@]}"; do
		run_test_function "$instruction" "${numberOftestsPerTest[$index]}"
		index=$((index + 1))
	done
}

function clean_library()
{
	if ! make fclean -C ..; then
		echo "libasmTester: the tester couldn't run the rule fclean from your Makefile"
		exit 1
	fi
}

function compile_library()
{
	if [ "$1" == "mandatory" ]; then
		make -C ..
	else
		make bonus -C ..
	fi
	if [ $? -ne 0 ]; then
		echo "libasmTester: Invalid compilation of the Makefile"
		exit 1
	fi
}

function detect_test()
{
	globalInstructions=("all" "mandatory" "bonus")
	individualTests=("strlen" "strcpy" "strcmp" "write" "read" "strdup" \
	"atoi_base" "list_size" "list_push_front" "list_sort" "list_delete_if")
	numberOftestsPerTest=(5 5 10 1 1 1 1 1 1 1 1)
	isInGlobal=false
	testName=""
	index=0
	for instruction in "${globalInstructions[@]}"; do
		if [ "$1" == "$instruction" ]; then
			isInGlobal=true
			break
		fi
	done
	if [ "$isInGlobal" == true ] && [ "$1" == "all" ]; then
		compile_library "mandatory"
		run_mandatory
		compile_library "bonus"
		run_bonus
	elif [ "$isInGlobal" == true ] && [ "$1" == "mandatory" ]; then
		compile_library "mandatory"
		run_mandatory
	elif [ "$isInGlobal" == true ] && [ "$1" == "bonus" ]; then
		compile_library "bonus"
		run_bonus
	else
		for instruction in "${individualTests[@]}"; do
			if [ "$instruction" == $1 ]; then
				if [ $index -ge 6 ]; then
					compile_library "bonus"
				else
					compile_library "mandatory"
				fi
				run_test_function "$instruction" "${numberOftestsPerTest[$index]}"
				break
			fi
			index=$((index + 1))
		done
	fi
}

function create_folder_or_clear_logs()
{
	if [ ! -d "logs" ]; then
		mkdir -p logs
	else
		rm -f logs/*
	fi
}

string1=""
isFound=false
create_folder_or_clear_logs
if [ $# -eq 0 ]; then
	string1="all"
elif [ $# -eq 1 ]; then
	string1=$1
else
	echo "libasmTester: Too many parameters"
	exit 1
fi
string1=$(echo "$string1" | tr '[:upper:]' '[:lower:]')
possibleArguments=("all" "mandatory" "bonus" "strlen" "strcpy" "strcmp" \
"write" "read" "strdup" "atoi_base" "list_size" "list_push_front" \
"list_sort" "list_delete_if")

for element in "${possibleArguments[@]}"; do
	if [ "$string1" == "$element" ]; then
		isFound=true
		break
	fi
done
if [ "$isFound" == false ]; then
	echo "libasmTester: invalid argument."
	exit 1
fi
detect_test "$string1"
delete_outputs
clean_library
