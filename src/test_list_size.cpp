/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_list_size.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 14:11:47 by corellan          #+#    #+#             */
/*   Updated: 2024/08/30 00:33:05 by corellan         ###   ########.fr       */
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

static void	push_add_front(t_list **begin, void *content, void (*del)(void *))
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
		del(content);
		delete_list(begin, del);
		std::exit(1);
	}
	new_node->data = content;
	new_node->next = tmp;
	(*begin) = new_node;
}

template<typename T>
static void	create_log(int expected, int to_test, std::vector<T> vec, int nbr)
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
	if (nbr == 0)
		file << "YOUR FT_LIST_SIZE RECEIVED A NULL POINTER AS A LIST" << '\n';
	file << "YOUR FT_LIST_SIZE GOT AS RESULT: " << to_test << '\n';
	file << "IT SHOULD GET: " << expected << '\n';
	if (nbr != 0)
	{
		file << "THE LIST CONTAINS: " << '\n';
		for (T &i : vec)
			file << i << " -> ";
		file << "NULL" << '\n';
	}
	file.close();
	return ;
}

static void	print_test_and_test_number(int &nbr)
{
	std::cout << "Test " << nbr << ": ";
}

template<typename T>
static void	assert_eq(int expected, int to_test, std::vector<T> vec, int nbr)
{
	if (to_test == expected)
		std::cout << GREEN << "[OK]\n" << RESET;
	else
	{
		std::cout << RED << "[KO]\n" << RESET;
		create_log<T>(expected, to_test, vec, nbr);
	}
}

static void	process_test(char const *nbr_str)
{
	int							nbr;
	char						*str;
	std::vector<int>			vec_t1 = {0};
	std::vector<int>			vec_t2 = {0, 1, 2, 5, 10, 100};
	std::vector<std::string>	vec_t3 = {"Hello", "Moi", "Hej"};

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
		assert_eq<int>(0, ft_list_size(linked_list), vec_t1, nbr);
		break;
	case 2:
		push_add_front(&linked_list, (void *)0, &erase_number);
		push_add_front(&linked_list, (void *)1, &erase_number);
		push_add_front(&linked_list, (void *)2, &erase_number);
		push_add_front(&linked_list, (void *)5, &erase_number);
		push_add_front(&linked_list, (void *)10, &erase_number);
		push_add_front(&linked_list, (void *)100, &erase_number);
		assert_eq<int>(6, ft_list_size(linked_list), vec_t2, nbr);
		delete_list(&linked_list, &erase_number);
		break;
	case 3:
		str = strdup("Hello");
		if (!str)
			return ;
		push_add_front(&linked_list, str, &free);
		str = strdup("Moi");
		if (!str)
		{
			delete_list(&linked_list, &free);
			return ;
		}
		push_add_front(&linked_list, str, &free);
		str = strdup("Hej");
		if (!str)
		{
			delete_list(&linked_list, &free);
			return ;
		}
		push_add_front(&linked_list, str, &free);
		assert_eq<std::string>(3, ft_list_size(linked_list), vec_t3, nbr);
		delete_list(&linked_list, &free);
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
