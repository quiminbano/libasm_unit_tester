/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_strcpy.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 14:11:47 by corellan          #+#    #+#             */
/*   Updated: 2024/08/26 14:38:05 by corellan         ###   ########.fr       */
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

static void	create_log(const std::string &result_ft, const std::string &result_orig, \
char *ptr_ft, char *ptr_orig, char *real_ptr_ft, char *real_ptr_orig, int &nbr)
{
	std::ofstream		file;
	std::ostringstream	oss;

	oss << "logs/ft_strcpy" << nbr << ".txt";
	file.open(oss.str(), std::ios_base::out);
	if (file.fail())
	{
		std::cerr << "Error trying to create/modify the file\n";
		return ;
	}
	file << "TEST CASE NUMBER " << nbr << ".\n\n";
	if (nbr == 4)
	{
		file << "YOUR FT_STRCPY FUNCTION DIDN'T CRASH WHEN IT SHOULD CRASH.\n\n";
		file << "REMEMBER THAT OVERPROTECTION OF YOUR FUNCTIONS MAKES MORE DIFFICULT FOR YOU TO\n";
		file << "DEBUG YOUR CODE IN CASE OF AN ERROR.\n";
	}
	else
	{
		file << "YOUR FT_STRCPY GOT AS RESULT: " << result_ft << "\n";
		file << "IT MUST GET: " << result_orig << "\n";
		file << "PTR OBTAINED FROM FT_STRCPY: " << ptr_ft << "\n";
		file << "IT MUST GET: " << real_ptr_ft << "\n";
		file << "ORIGINAL FUNCTION RETURN AS PTR: " << ptr_orig << "\n";
		file << "AND IT GETS: " << real_ptr_orig << "\n";
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

static void	process_test(char const *nbr_str)
{
	int			nbr;
	int			*ptr;
	char		result_ft[14];
	char		result_orig[14];
	char		*ptr_ft;
	char		*ptr_orig;
	std::string	str_ft;
	std::string	str_orig;

	signal(SIGSEGV, &signal_handler);
	nbr = std::stoi(nbr_str);
	bzero(result_ft, sizeof(result_ft));
	bzero(result_orig, sizeof(result_orig));
	print_test_and_test_number(nbr);
	switch (nbr)
	{
	case 1:
		ptr_ft = ft_strcpy(result_ft, "Hello, World\n");
		ptr_orig = std::strcpy(result_orig, "Hello, World\n");
		break;
	case 2:
		ptr_ft = ft_strcpy(result_ft, "Hello\0 World\n");
		ptr_orig = std::strcpy(result_orig, "Hello\0 World\n");
		break;
	case 3:
		ptr_ft = ft_strcpy(result_ft, "");
		ptr_orig = std::strcpy(result_orig, "");
		break;
	case 4:
		ptr = get_status();
		*ptr = 1;
		ptr_orig = std::strcpy(result_orig, nullptr);
		break;
	case 5:
		ptr = get_status();
		*ptr = 2;
		ptr_ft = ft_strcpy(result_ft, nullptr);
		break;
	default:
		std::cerr << "Error\n";
		break;
	}
	str_ft = result_ft;
	str_orig = result_orig;
	if (nbr == 4)
	{
		std::cout << YELLOW << "[NO-CRASH]" << RESET " -> ";
		std::cout << RED << "[KO]" << RESET << "\n";
		create_log(str_ft, str_orig, ptr_ft, ptr_orig, &(result_ft[0]), \
			&(result_orig[0]), nbr);
	}
	else if (str_ft == str_orig && (ptr_ft == &(result_ft[0])) && (ptr_orig == &(result_orig[0])))
		std::cout << GREEN << "[OK]" << RESET << "\n";
	else
	{
		std::cout << RED << "[KO]" << RESET << "\n";
		create_log(result_ft, result_orig, ptr_ft, ptr_orig,  &(result_ft[0]), \
			&(result_orig[0]), nbr);
	}
}

int	main(int ac, char **av)
{
	if (ac == 2)
		process_test(av[1]);
	return (0);
}
