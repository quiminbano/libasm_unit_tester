/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_strdup.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 14:11:47 by corellan          #+#    #+#             */
/*   Updated: 2024/08/27 11:29:41 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <csignal>
#include <cerrno>
#include <unistd.h>
#include <sys/wait.h>
#include "colors.hpp"
extern "C"
{
	#include "libasm.h"
}

struct Test
{
	char			*result_orig;
	char			*result_ft;
	int				nbr;
	bool			null_orig;
	bool			null_ft;
	std::string 	str_orig;
	std::string 	str_ft;
};

static int	*get_status(void);

extern "C" void *__real_malloc(size_t size);

extern "C" void	*__wrap_malloc(size_t size)
{
	if (*(get_status()) == -1)
		return (NULL);
	return (__real_malloc(size));
}

static int	*get_status(void)
{
	static int	test_status = 0;

	return (&test_status);
}

static void	signal_handler(int sig)
{
	int	*test_status_addr;

	test_status_addr = get_status();
	if (sig == SIGSEGV && (*test_status_addr) != 1 && (*test_status_addr) != 2)
		std::cout << YELLOW << "[CRASH]" << RESET " -> " << RED << "[KO]" << RESET << "\n";
	else if (sig == SIGSEGV && (*test_status_addr) == 1)
		std::cout << YELLOW << "[CRASH], " << RESET;
	else if (sig == SIGSEGV && (*test_status_addr) == 2)
		std::cout << YELLOW << "[CRASH]" << RESET " -> " << GREEN << "[OK]\n" << RESET;
	std::exit(0);
}

static void	create_log(int &nbr, Test &test)
{
	std::ofstream		file;
	std::ostringstream	oss;

	if (nbr == 5)
		oss << "logs/ft_strdup" << 4 << ".txt";
	else
		oss << "logs/ft_strdup" << nbr << ".txt";
	file.open(oss.str(), std::ios_base::out);
	if (file.fail())
	{
		std::cerr << "Error trying to create/modify the file\n";
		return ;
	}
	if (nbr != 5)
	{
		file << "YOUR FT_STRDUP GOT AS RESULT: " << test.str_ft << "\n";
		file << "THE ORIGINAL ONE GETS: " << test.str_orig << "\n";
	}
	else
	{
		file << "YOUR FT_STRDUP FUNCTION DIDN'T CRASH WHEN IT SHOULD CRASH.\n\n";
		file << "REMEMBER THAT OVERPROTECTION OF YOUR FUNCTIONS MAKES MORE DIFFICULT FOR YOU TO\n";
		file << "DEBUG YOUR CODE IN CASE OF AN ERROR.\n";
	}
	file.close();
	return ;
}

static void	print_test_and_test_number(int &nbr)
{
	if (nbr != 5)
		std::cout << "Test " << nbr << ": ";
	if (nbr == 4)
		std::cout << "ORIGINAL: ";
	if (nbr == 5)
		std::cout << "YOURS: ";
}

static void	strdup_wrapper(Test &test, char const *str)
{
	if (test.nbr == 4 && !str)
	{
		test.result_orig = strdup(str);
		return ;
	}
	if (test.nbr == 5 && !str)
	{
		test.result_ft = ft_strdup(str);
		free(test.result_ft);
		return ;
	}
	if (test.nbr == 3)
		*(get_status()) = -1;
	test.result_orig = strdup(str);
	if (test.nbr == 3)
		*(get_status()) = 0;
	if (!test.result_orig)
		test.null_orig = true;
	if (test.nbr == 3)
		*(get_status()) = -1;
	test.result_ft = ft_strdup(str);
	if (test.nbr == 3)
		*(get_status()) = 0;
	if (!test.result_ft)
	{
		if (test.result_orig)
			free(test.result_orig);
		test.result_orig = NULL;
		test.null_ft = true;
	}
	if (test.null_orig == false && test.result_orig)
		test.str_orig = test.result_orig;
	if (test.null_ft == false && test.result_ft)
		test.str_ft = test.result_ft;
	if (test.result_orig)
		free(test.result_orig);
	if (test.result_ft)
		free(test.result_ft);
}

static void	process_test(char const *nbr_str)
{
	int		nbr;
	Test	test;

	signal(SIGSEGV, &signal_handler);
	try
	{
		nbr = std::stoi(nbr_str);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return ;
	}
	test.result_orig = nullptr;
	test.result_ft = nullptr;
	test.nbr = nbr;
	test.null_ft = false;
	test.null_orig = false;
	print_test_and_test_number(nbr);
	test.str_orig = "";
	test.str_ft = "";
	switch (nbr)
	{
	case 1:
		strdup_wrapper(test, "Hello, World\n");
		break;
	case 2:
		strdup_wrapper(test, "");
		break;
	case 3:
		strdup_wrapper(test, "Hello, World\n");
		*(get_status()) = 0;		
		break;
	case 4:
		*(get_status()) = 1;
		strdup_wrapper(test, nullptr);
		break;
	case 5:
		*(get_status()) = 2;
		strdup_wrapper(test, nullptr);
		break;
	default:
		std::cerr << "Error\n";
		break;
	}
	if (nbr == 3 && test.null_orig == true && test.null_ft == true)
		std::cout << GREEN << "[OK]" << RESET << "\n";
	else if (nbr == 3 && test.null_orig == true && test.null_ft == false)
		std::cout << RED << "[KO]" << RESET << "\n";
	else if (nbr == 5)
	{
		if (test.result_ft)
			free(test.result_ft);
		test.result_ft = nullptr;
		std::cout << YELLOW << "[NO-CRASH]" << RESET " -> ";
		std::cout << RED << "[KO]" << RESET << "\n";
	}
	else if (test.str_ft == test.str_orig)
		std::cout << GREEN << "[OK]" << RESET << "\n";
	else
	{
		std::cout << RED << "[KO]" << RESET << "\n";
		create_log(nbr, test);
	}
}

int	main(int ac, char **av)
{
	if (ac == 2)
		process_test(av[1]);
	return (0);
}
