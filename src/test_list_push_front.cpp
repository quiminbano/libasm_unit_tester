/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_list_push_front.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 14:11:47 by corellan          #+#    #+#             */
/*   Updated: 2024/08/30 12:44:33 by corellan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <csignal>
#include <cstdarg>
#include <vector>
#include "colors.hpp"
extern "C"
{
	#include "libasm.h"
	t_list	*linked_list = NULL;
}

static int	*get_status(void)
{
	static int	test_status = 0;

	return (&test_status);
}

extern "C" 
{ 
	void *__real_malloc(size_t size);
}

extern "C"
{
	void	*__wrap_malloc(size_t size)
	{
		if (*(get_status()) == -1)
			return (NULL);
		return (__real_malloc(size));
	}
}

static void	signal_handler(int sig)
{
	if (sig == SIGSEGV)
		std::cout << YELLOW << "[CRASH]" << RESET " -> " << RED << "[KO]\n" << RESET;
	std::exit(0);
}

static void	erase_number(void *nbr)
{
	void *ptr;

	ptr = NULL;
	nbr = ptr;
	(void)nbr;
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

template<typename T>
static void	create_log(std::vector<T> expected, std::vector<T> result, int nbr)
{
	std::ofstream		file;
	std::ostringstream	oss;

	oss << "logs/ft_list_size" << nbr << ".txt";
	file.open(oss.str(), std::ios_base::out);
	if (file.fail())
	{
		std::cerr << "Error trying to create/modify the file\n";
		return ;
	}
	file << "TEST CASE NUMBER " << nbr << ".\n\n";
	file << "YOUR FT_LIST_PUSH_FRONT GOT AS RESULT:\n";
	for (T &i : result)
		file << i << " -> ";
	file << "NULL" << '\n';
	file << "IT SHOULD GET:\n";
	for (T &i : expected)
		file << i << " -> ";
	file << "NULL" << '\n';
	file.close();
	return ;
}

static void	print_test_and_test_number(int &nbr)
{
	std::cout << "Test " << nbr << ": ";
}

static void	assert_eq(t_list **begin, std::vector<long> vec, int nbr)
{
	std::string			expected;
	std::string			to_test;
	std::vector<long>	results_list;
	t_list				*tmp;

	expected = "0\n1\n2\n5\n10\n100\n";
	to_test = "";
	tmp = (*begin);
	while (tmp)
	{
		to_test += std::to_string(reinterpret_cast<long>(tmp->data)) + "\n";
		results_list.push_back(reinterpret_cast<long>(tmp->data));
		tmp = tmp->next;
	}
	if (to_test == expected)
		std::cout << GREEN << "[OK]\n" << RESET;
	else
	{
		std::cout << RED << "[KO]\n" << RESET;
		create_log<long>(vec, results_list, nbr);
	}
}

static void	assert_eq(t_list **begin, std::vector<std::string> vec, int nbr)
{
	std::string					expected;
	std::string					to_test;
	std::string					tmp_str;
	std::vector<std::string>	results_list;
	t_list						*tmp;

	expected = "Hej\nMoi\nHello\n";
	to_test = "";
	tmp = (*begin);
	while (tmp)
	{
		tmp_str.clear();
		to_test.append((char *)tmp->data);
		to_test.push_back('\n');
		tmp_str.append((char *)tmp->data);
		results_list.push_back(tmp_str);
		tmp = tmp->next;
	}
	if (to_test == expected)
		std::cout << GREEN << "[OK]\n" << RESET;
	else
	{
		std::cout << RED << "[KO]\n" << RESET;
		create_log<std::string>(vec, results_list, nbr);
	}
}

static void	check_special_cases(t_list **begin, int nbr)
{
	t_list	*tmp;

	tmp = (*begin);
	if (tmp)
	{
		std::cout << RED << "[KO]\n" << RESET;
		create_log<int>({0}, {0}, nbr);
	}
	else
		std::cout << GREEN << "[OK]\n" << RESET;
}

static void	process_test(char const *nbr_str)
{
	int							nbr;
	char						*str;
	std::vector<long>			vec_t1 = {0, 1, 2, 5, 10, 100};
	std::vector<std::string>	vec_t2 = {"Hej", "Moi", "Hello"};

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
		ft_list_push_front(&linked_list, (void *)100);
		ft_list_push_front(&linked_list, (void *)10);
		ft_list_push_front(&linked_list, (void *)5);
		ft_list_push_front(&linked_list, (void *)2);
		ft_list_push_front(&linked_list, (void *)1);
		ft_list_push_front(&linked_list, (void *)0);
		assert_eq(&linked_list, vec_t1, nbr);
		delete_list(&linked_list, &erase_number);
		break;
	case 2:
		str = strdup("Hello");
		if (!str)
			return ;
		ft_list_push_front(&linked_list, str);
		str = strdup("Moi");
		if (!str)
		{
			delete_list(&linked_list, &free);
			return ;
		}
		ft_list_push_front(&linked_list, str);
		str = strdup("Hej");
		if (!str)
		{
			delete_list(&linked_list, &free);
			return ;
		}
		ft_list_push_front(&linked_list, str);
		assert_eq(&linked_list, vec_t2, nbr);
		delete_list(&linked_list, &free);
		break;
	case 3:
		ft_list_push_front(NULL, (void *)0);
		check_special_cases(&linked_list, nbr);
		break;
	case 4:
		*(get_status()) = -1;
		ft_list_push_front(&linked_list, (void *)88);
		*(get_status()) = 0;
		check_special_cases(&linked_list, nbr);
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
