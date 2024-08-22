/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_strcmp.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 14:11:47 by corellan          #+#    #+#             */
/*   Updated: 2024/08/22 13:39:43 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <csignal>
#include "colors.hpp"
extern "C"
{
	#include "libasm.h"
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
	if (sig == SIGSEGV && !(*test_status_addr))
		std::cout << YELLOW << "[CRASH]" << RESET " -> " << RED << "[KO]" << RESET << "\n";
	else if (sig == SIGSEGV && (*test_status_addr) == 1)
		std::cout << YELLOW << "[CRASH], " << RESET;
	else if (sig == SIGSEGV && (*test_status_addr) == 2)
		std::cout << YELLOW << "[CRASH]" << RESET " -> " << GREEN << "[OK]\n" << RESET;
	std::exit(0);
}

static void	create_log(int &nbr, int &result_orig, int &result_ft)
{
	std::ofstream		file;
	std::ostringstream	oss;

	if (nbr <= 7)
		oss << "logs/ft_strcmp" << nbr << ".txt";
	else if (nbr == 8)
		oss << "logs/ft_strcmp" << 7 << ".txt";
	else if (nbr == 10)
		oss << "logs/ft_strcmp" << 8 << ".txt";
	file.open(oss.str(), std::ios_base::out);
	if (file.fail())
	{
		std::cerr << "Error trying to create/modify the file\n";
		return ;
	}
	if (nbr <= 7)
		file << "TEST CASE NUMBER " << nbr << ".\n\n";
	else if (nbr == 8)
		file << "TEST CASE NUMBER " << 7 << ".\n\n";
	else if (nbr == 10)
		file << "TEST CASE NUMBER " << 8 << ".\n\n";
	if (nbr == 8 || nbr == 10)
	{
		file << "YOUR FT_STRCMP FUNCTION DIDN'T CRASH WHEN IT SHOULD CRASH.\n\n";
		file << "REMEMBER THAT OVERPROTECTION OF YOUR FUNCTIONS MAKES MORE DIFFICULT FOR YOU TO\n";
		file << "DEBUG YOUR CODE IN CASE OF AN ERROR.\n";
	}
	else
	{
		file << "YOUR FT_STRCMP GOT AS RESULT: " << result_ft << "\n";
		file << "THE ORIGINAL ONE GETS: " << result_orig << "\n";
	}
	file.close();
	return ;
}

static void	print_test_and_test_number(int &nbr)
{
	if (nbr <= 7)
		std::cout << "Test " << nbr << ": ";
	else if (nbr == 9)
		std::cout << "Test " << 8 << ": ";
	if (nbr == 7 || nbr == 9)
		std::cout << "ORIGINAL: ";
	if (nbr == 8 | nbr == 10)
		std::cout << "YOURS: ";
}

static void	process_test(char const *nbr_str)
{
	int			nbr;
	int			*ptr;
	int			result_orig;
	int			result_ft;
	bool		is_correct;

	signal(SIGSEGV, &signal_handler);
	nbr = std::stoi(nbr_str);
	result_orig = 0;
	result_ft = 0;
	is_correct = false;
	print_test_and_test_number(nbr);
	switch (nbr)
	{
	case 1:
		result_ft = ft_strcmp("Hello, World\n", "Hello, World\n");
		result_orig = std::strcmp("Hello, World\n", "Hello, World\n");
		is_correct = (result_ft == 0 && result_orig == 0);
		break;
	case 2:
		result_ft = ft_strcmp("hello, World\n", "Hello, World\n");
		result_orig = std::strcmp("hello, World\n", "Hello, World\n");
		is_correct = (result_ft > 0 && result_orig > 0);
		break;
	case 3:
		result_ft = ft_strcmp("\0hello, World\n", "\0Hello, World\n");
		result_orig = std::strcmp("\0hello, World\n", "\0Hello, World\n");
		is_correct = (result_ft == 0 && result_orig == 0);
		break;
	case 4:
		result_ft = ft_strcmp("Hello, World\n", "Iello, World\n");
		result_orig = std::strcmp("Hello, World\n", "Iello, World\n");
		is_correct = (result_ft < 0 && result_orig < 0);
		break;
	case 5:
		result_ft = ft_strcmp("Hello, World\n", "Hello, World\nini");
		result_orig = std::strcmp("Hello, World\n", "Hello, World\nini");
		is_correct = (result_ft < 0 && result_orig < 0);
		break;
	case 6:
		result_ft = ft_strcmp("Hello, World\n\0a", "Hello, World\n\0and");
		result_orig = std::strcmp("Hello, World\n\0a", "Hello, World\n\0and");
		is_correct = (result_ft == 0 && result_orig == 0);
		break;
	case 7:
		ptr = get_status();
		*ptr = 1;
		result_orig = std::strcmp("Hello\n", nullptr);
		break;
	case 8:
		ptr = get_status();
		*ptr = 2;
		result_ft = ft_strcmp("Hello\n", nullptr);
		break;
	case 9:
		ptr = get_status();
		*ptr = 1;
		result_orig = std::strcmp(nullptr, "Hello\n");
		break;
	case 10:
		ptr = get_status();
		*ptr = 2;
		result_ft = ft_strcmp(nullptr, "Hello\n");
		break;
	default:
		std::cerr << "Error\n";
		break;
	}
	if (nbr == 8 || nbr == 10)
	{
		std::cout << YELLOW << "[NO-CRASH]" << RESET " -> ";
		std::cout << RED << "[KO]" << RESET << "\n";
		create_log(nbr, result_orig, result_ft);
	}
	else if (is_correct == true)
		std::cout << GREEN << "[OK]" << RESET << "\n";
	else
	{
		std::cout << RED << "[KO]" << RESET << "\n";
		create_log(nbr, result_orig, result_ft);
	}
}

int	main(int ac, char **av)
{
	if (ac == 2)
		process_test(av[1]);
	return (0);
}
