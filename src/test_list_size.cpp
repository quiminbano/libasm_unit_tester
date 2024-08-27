/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_list_size.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 14:11:47 by corellan          #+#    #+#             */
/*   Updated: 2024/08/27 18:35:52 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <csignal>
#include <cstdarg>
#include "colors.hpp"
extern "C"
{
	#include "libasm.h"
}

static void	signal_handler(int sig)
{
	std::cout << YELLOW << "[CRASH]" << RESET " -> " << RED << "[KO]\n" << RESET;
	std::exit(0);
}

static void delete_list(t_list **begin, void (*del)(void *))
{
	t_list	*tmp;

	while ((*begin))
	{
		tmp = (*begin);
		(*begin) = (*begin)->next;
		del(tmp->data);
		delete tmp;
	}
}

static void	push_add_front(t_list **begin, void *content, va_list *args)
{
	t_list	*tmp;
	t_list	*new_node;

	tmp = (*begin);
	try
	{
		new_node = new t_list;
	}
	catch(const std::bad_alloc &e)
	{
		std::cerr << e.what() << '\n';
		va_end(*args);
		std::exit(1);
	}
	new_node->data = content;
	new_node->next = tmp;
	(*begin) = new_node;
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


static void	assert_eq(int to_test, int expected)
{
	if (to_test == expected)
	{
		std::cout << GREEN << "[OK]\n" << RESET;
	}
	else
	{
		std::cout << RED << "[KO]\n" << RESET;
		create_log(nbr, expected, to_test, number_str, base_str);
	}
}


static void	populate_list(t_list **begin, int expected, void const *tmp, ...)
{
	char	*str;
	char	*result;
	bool	is_str;
	bool	is_nbr;
	int		to_test;
	va_list	args;

	str = (char *)tmp;
	va_start(args, tmp);
	is_str = false;
	is_nbr = false;
	for (size_t i = 0; str[i]; i++)
	{
		if (str[i] == 'd')
		{
			is_nbr = true;
			push_add_front(begin, (void *)va_arg(args, int), &args);
		}
		if (str[i] == 's')
		{
			is_str = true;
			result = strdup(va_arg(args, char *));
			if (!result)
			{
				va_end(args);
				delete_list(begin, &free);
				std::exit(1);
			}
			push_add_front(begin, result, &args);
		}
	}
	to_test = ft_list_size(*begin);
	assert_eq(to_test, expected);
	va_end(args);
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
