/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_list_delete_if.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 14:11:47 by corellan          #+#    #+#             */
/*   Updated: 2024/08/30 16:58:07 by corellan         ###   ########.fr       */
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

static int	compare_number(void *a, void *b)
{
	return (reinterpret_cast<long>(a) - reinterpret_cast<long>(b));
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
		free(tmp);
	}
}

static void	list_push_front(t_list **begin, void *content, void (*del)(void *))
{
	t_list	*tmp;
	t_list	*new_node;

	if (!begin)
		return ;
	tmp = (*begin);
	new_node = (t_list *)malloc(sizeof(t_list));
	if (!new_node)
	{
		del(content);
		delete_list(begin, del);
		std::exit(1);
	}
	new_node->data = content;
	new_node->next = tmp;
	(*begin) = new_node;
}

template<typename T>
static void	create_log(std::vector<T> expected, std::vector<T> result, int nbr)
{
	std::ofstream		file;
	std::ostringstream	oss;

	oss << "logs/ft_list_delete_if" << nbr << ".txt";
	file.open(oss.str(), std::ios_base::out);
	if (file.fail())
	{
		std::cerr << "Error trying to create/modify the file\n";
		return ;
	}
	file << "TEST CASE NUMBER " << nbr << ".\n\n";
	file << "YOUR FT_LIST_DELETE_IF GOT AS RESULT:\n";
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

static void	assert_eq(t_list **begin, std::vector<long> vec, int nbr, const void *to_get)
{
	std::string			expected;
	std::string			to_test;
	std::vector<long>	results_list;
	t_list				*tmp;

	expected = static_cast<const char *>(to_get);
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

	expected = "Hello\nHej\n";
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

static int	strcmp_wrapper(void *a, void *b)
{
	return (strcmp(static_cast<const char *>(a), static_cast<const char *>(b)));
}

static void	process_test(char const *nbr_str)
{
	int									nbr;
	char								*str;
	std::vector<long>					vec_t1 = {1, 2, 10, 0, 100};
	std::vector<long>					vec_t2 = {5, 1, 2, 10, 0};
	std::vector<long>					vec_t3 = {5, 1, 2, 0, 100};
	std::vector<long>					vec_t4 = {5, 1, 2, 10, 0, 100};
	std::vector<std::string>			vec_t5 = {"Hello", "Hej"};
	std::vector<long>					vec_t7 = {10};
	std::vector<long>					vec_t8 = {10};

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
		list_push_front(&linked_list, (void *)100, &erase_number);
		list_push_front(&linked_list, (void *)0, &erase_number);
		list_push_front(&linked_list, (void *)10, &erase_number);
		list_push_front(&linked_list, (void *)2, &erase_number);
		list_push_front(&linked_list, (void *)1, &erase_number);
		list_push_front(&linked_list, (void *)5, &erase_number);
		ft_list_remove_if(&linked_list, (void *)5, &compare_number, &erase_number);
		assert_eq(&linked_list, vec_t1, nbr, "1\n2\n10\n0\n100\n");
		delete_list(&linked_list, &erase_number);
		break;
	case 2:
		list_push_front(&linked_list, (void *)100, &erase_number);
		list_push_front(&linked_list, (void *)0, &erase_number);
		list_push_front(&linked_list, (void *)10, &erase_number);
		list_push_front(&linked_list, (void *)2, &erase_number);
		list_push_front(&linked_list, (void *)1, &erase_number);
		list_push_front(&linked_list, (void *)5, &erase_number);
		ft_list_remove_if(&linked_list, (void *)100, &compare_number, &erase_number);
		assert_eq(&linked_list, vec_t2, nbr, "5\n1\n2\n10\n0\n");
		delete_list(&linked_list, &erase_number);
		break;
	case 3:
		list_push_front(&linked_list, (void *)100, &erase_number);
		list_push_front(&linked_list, (void *)0, &erase_number);
		list_push_front(&linked_list, (void *)10, &erase_number);
		list_push_front(&linked_list, (void *)2, &erase_number);
		list_push_front(&linked_list, (void *)1, &erase_number);
		list_push_front(&linked_list, (void *)5, &erase_number);
		ft_list_remove_if(&linked_list, (void *)10, &compare_number, &erase_number);
		assert_eq(&linked_list, vec_t3, nbr, "5\n1\n2\n0\n100\n");
		delete_list(&linked_list, &erase_number);
		break;
	case 4:
		list_push_front(&linked_list, (void *)100, &erase_number);
		list_push_front(&linked_list, (void *)0, &erase_number);
		list_push_front(&linked_list, (void *)10, &erase_number);
		list_push_front(&linked_list, (void *)2, &erase_number);
		list_push_front(&linked_list, (void *)1, &erase_number);
		list_push_front(&linked_list, (void *)5, &erase_number);
		ft_list_remove_if(&linked_list, (void *)50, &compare_number, &erase_number);
		assert_eq(&linked_list, vec_t4, nbr, "5\n1\n2\n10\n0\n100\n");
		delete_list(&linked_list, &erase_number);
		break;
	case 5:
		str = strdup("Hej");
		if (!str)
			return ;
		list_push_front(&linked_list, str, &free);
		str = strdup("Moi");
		if (!str)
		{
			delete_list(&linked_list, &free);
			return ;
		}
		list_push_front(&linked_list, str, &free);
		str = strdup("Hello");
		if (!str)
		{
			delete_list(&linked_list, &free);
			return ;
		}
		list_push_front(&linked_list, str, &free);
		ft_list_remove_if(&linked_list, (void *)"Moi", &strcmp_wrapper, &free);
		assert_eq(&linked_list, vec_t5, nbr);
		delete_list(&linked_list, &free);
		break;
	case 6:
		list_push_front(nullptr, (void *)0, &erase_number);
		ft_list_remove_if(nullptr, (void *)5, &compare_number, &erase_number);
		check_special_cases(&linked_list, nbr);
		break;
	case 7:
		list_push_front(&linked_list, (void *)10, &erase_number);
		ft_list_remove_if(&linked_list, (void *)2, nullptr, &erase_number);
		assert_eq(&linked_list, vec_t4, nbr, "10\n");
		delete_list(&linked_list, &erase_number);
		break;
	case 8:
		list_push_front(&linked_list, (void *)10, &erase_number);
		ft_list_remove_if(&linked_list, (void *)2, &compare_number, nullptr);
		assert_eq(&linked_list, vec_t4, nbr, "10\n");
		delete_list(&linked_list, &erase_number);
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
