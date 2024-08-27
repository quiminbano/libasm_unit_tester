/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_atoi_base.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 14:11:47 by corellan          #+#    #+#             */
/*   Updated: 2024/08/27 15:09:54 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <csignal>
#include <limits>
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
	else
		std::cout << YELLOW << "[CRASH]" << RESET " -> " << GREEN << "[OK]\n" << RESET;
	std::exit(0);
}

static void	create_log(int &nbr, int &expected, int &to_test, std::string &number_str, std::string &base_str)
{
	std::ofstream		file;
	std::ostringstream	oss;

	oss << "logs/ft_atoi_base" << nbr << ".txt";
	file.open(oss.str(), std::ios_base::out);
	if (file.fail())
	{
		std::cerr << "Error trying to create/modify the file\n";
		return ;
	}
	file << "TEST CASE NUMBER " << nbr << ".\n\n";
	if (nbr == 26 || nbr == 27)
	{
		file << "YOUR FT_ATOI_BASE FUNCTION DIDN'T CRASH WHEN IT SHOULD CRASH.\n\n";
		file << "REMEMBER THAT OVERPROTECTION OF YOUR FUNCTIONS MAKES MORE DIFFICULT FOR YOU TO\n";
		file << "DEBUG YOUR CODE IN CASE OF AN ERROR.\n";
	}
	else
	{
		file << "YOUR FT_ATOI_BASE GOT AS RESULT: " << to_test << '\n';
		file << "IT SHOULD GET: " << expected << '\n';
		file << "THE STRING PASSED AS NUMBER WAS: " << number_str << '\n';
		file << "THE STRING PASSED AS BASE WAS: " << base_str << '\n';
	}
	file.close();
	return ;
}

static void	print_test_and_test_number(int &nbr)
{
	std::cout << "Test " << nbr << ": ";
}

static void	assert_eq(int expected, const void *number, const void *base, int nbr)
{
	int			to_test;
	std::string	number_str;
	std::string	base_str;

	number_str = "";
	base_str = "";
	if (nbr == 26 || nbr == 27)
		(*(get_status())) = 1;
	to_test = ft_atoi_base((char *)(number), (char *)(base));
	if (number && base)
	{
		number_str = (char *)number;
		base_str = (char *)base;
	}
	if ((nbr == 26 || nbr == 27))
	{
		std::cout << YELLOW << "[NO-CRASH]" << RESET << " -> " << RED << "[KO]" << RESET << '\n';
		create_log(nbr, expected, to_test, number_str, base_str);
	}
	else if (expected != to_test)
	{
		std::cout << RED << "[KO]" << RESET << '\n';
		create_log(nbr, expected, to_test, number_str, base_str);
	}
	else
		std::cout << GREEN << "[OK]" << RESET << '\n';
}

static void	process_test(char const *nbr_str)
{
	int	nbr;

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
	print_test_and_test_number(nbr);
	switch (nbr)
	{
	case 1:
		assert_eq(2, "10", "01", nbr);
		break;
	case 2:
		assert_eq(-55, "  ++---AAaAAA", "aA", nbr);
		break;
	case 3:
		assert_eq(55, "  ++--AAaAAA", "aA", nbr);
		break;
	case 4:
		assert_eq(55, "  ++--AAaAAA124", "aA", nbr);
		break;
	case 5:
		assert_eq(0, "\0  ++--AAaAAA124", "aA", nbr);
		break;
	case 6:
		assert_eq(std::numeric_limits<int>::max(), "  ++--01111111111111111111111111111111", "01", nbr);
		break;
	case 7:
		assert_eq(std::numeric_limits<int>::min(), "  ++--10000000000000000000000000000000", "01", nbr);
		break;
	case 8:
		assert_eq(511, "0777---", "01234567", nbr);
		break;
	case 9:
		assert_eq(511, "0...---", "0123456.", nbr);
		break;
	case 10:
		assert_eq(511, "/...---", "/123456.", nbr);
		break;
	case 11:
		assert_eq(80383, "/234...---", "/123456.", nbr);
		break;
	case 12:
		assert_eq(-10, "--++++-ba", "abcdefghij", nbr);
		break;
	case 13:
		assert_eq(2024, "cace", "abcdefghij", nbr);
		break;
	case 14:
		assert_eq(7038990, "madonna", "abcdefgmon", nbr);
		break;
	case 15:
		assert_eq(0, "madonna", "0123456789", nbr);
		break;
	case 16:
		assert_eq(2050, "2050", "0123456789", nbr);
		break;
	case 17:
		assert_eq((std::numeric_limits<int>::min() + 2), "2147483650", "0123456789", nbr);
		break;
	case 18:
		assert_eq(1958645160, "HIVE_HELSINKI", "VBCDEFGHIJKLMNS_", nbr);
		break;
	case 19:
		assert_eq(0, "HIVE_HELSINKI", "0123456789ABCDEF", nbr);
		break;
	case 20:
		assert_eq(14, "E_HELSINKI", "0123456789ABCDEF", nbr);
		break;
	case 21:
		assert_eq(0, "100", "010", nbr);
		break;
	case 22:
		assert_eq(0, "100", "   01", nbr);
		break;
	case 23:
		assert_eq(0, "100", "\t\t\t01", nbr);
		break;
	case 24:
		assert_eq(0, "100", "0     1", nbr);
		break;
	case 25:
		assert_eq(0, "100", "01+++", nbr);
		break;
	case 26:
		assert_eq(0, nullptr, "01", nbr);
		break;
	case 27:
		assert_eq(0, "100", nullptr, nbr);
		break;
	default:
		std::cerr << "Error" << '\n';
		break;
	}
}

int	main(int ac, char **av)
{
	if (ac == 2)
		process_test(av[1]);
	return (0);
}
