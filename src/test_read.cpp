/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_read.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 14:11:47 by corellan          #+#    #+#             */
/*   Updated: 2024/08/26 12:06:32 by corellan         ###   ########.fr       */
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
	ssize_t			result_orig;
	ssize_t			result_ft;
	int				errno_orig;
	int				errno_ft;
	int				nbr;
	std::string 	str_orig;
	std::string 	str_ft;
};

struct FileDescriptors
{
	int	pipes[2];
};

static FileDescriptors	*singleton_fd(void)
{
	static FileDescriptors	file_descriptors;

	return(&file_descriptors);
}

static void	signal_handler(int sig)
{
	if (singleton_fd()->pipes[0] != -1)
		close(singleton_fd()->pipes[0]);
	if (sig == SIGSEGV)
		std::cout << YELLOW << "[CRASH]" << RESET " -> " << RED << "[KO]" << RESET << "\n";
	std::exit(0);
}

static void	create_log(int &nbr, Test &test)
{
	std::ofstream		file;
	std::ostringstream	oss;

	oss << "logs/ft_read" << nbr << ".txt";
	file.open(oss.str(), std::ios_base::out);
	if (file.fail())
	{
		std::cerr << "Error trying to create/modify the file\n";
		return ;
	}
	file << "YOUR FT_READ GOT AS RESULT: " << test.str_ft << "\n";
	file << "THE ORIGINAL ONE GETS: " << test.str_orig << "\n";
	file << "VALUE OF ERRNO OBTAINED BY YOUR FT_READ: " << test.errno_ft << "\n";
	file << "VALUE OF ERRNO OBTAINED BY THE ORIGINAL READ: " << test.errno_orig << "\n";
	file << "THE RETURN VALUE OF YOUR FT_READ IS: " << test.result_ft << "\n";
	file << "THE ORIGINAL GETS: " << test.result_orig << "\n";
	file.close();
	return ;
}

static void	print_test_and_test_number(int &nbr)
{
	std::cout << "Test " << nbr << ": ";
}

static void	read_wrapper(void *buf, size_t count, Test &test, char const *tmp)
{
	if (pipe((singleton_fd())->pipes) == -1)
		std::exit(1);
	write((singleton_fd())->pipes[1], tmp, strlen(tmp));
	close((singleton_fd())->pipes[1]);
	if (test.nbr == 4)
	{
		close((singleton_fd())->pipes[0]);
		(singleton_fd())->pipes[0] = -1;
	}
	test.result_orig = read((singleton_fd())->pipes[0], buf, count);
	test.errno_orig = errno;
	errno = 0;
	if ((singleton_fd())->pipes[0] != -1)
	{
		close((singleton_fd())->pipes[0]);
		if (buf != nullptr)
			test.str_orig = (char *)buf;
	}
	if (buf != nullptr)
		bzero((char *)buf, 14UL);
	if (pipe((singleton_fd())->pipes) == -1)
		std::exit(1);
	write((singleton_fd())->pipes[1], tmp, strlen(tmp));
	close((singleton_fd())->pipes[1]);
	if (test.nbr == 4)
	{
		close((singleton_fd())->pipes[0]);
		(singleton_fd())->pipes[0] = -1;
	}
	test.result_ft = ft_read((singleton_fd())->pipes[0], buf, count);
	test.errno_ft = errno;
	errno = 0;
	if ((singleton_fd())->pipes[0] != -1)
	{
		close((singleton_fd())->pipes[0]);
		if (buf != nullptr)
			test.str_ft = (char *)buf;
	}
	singleton_fd()->pipes[0] = -1;
	singleton_fd()->pipes[1] = -1;
}

static void	process_test(char const *nbr_str)
{
	int		nbr;
	char	buff[14];
	Test	test;

	singleton_fd()->pipes[0] = -1;
	singleton_fd()->pipes[1] = -1;
	bzero(buff, sizeof(buff));
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
	test.result_orig = 0;
	test.result_ft = 0;
	test.errno_orig = 0;
	test.errno_ft = 0;
	test.nbr = nbr;
	print_test_and_test_number(nbr);
	test.str_orig = "";
	test.str_ft = "";
	
	switch (nbr)
	{
	case 1:
		read_wrapper(buff, 12, test, "Hello, World\n");
		break;
	case 2:
		read_wrapper(buff, 0, test, "Hello, World\n");
		break;
	case 3:
		read_wrapper(nullptr, 12, test, "Hello, World\n");
		break;
	case 4:
		read_wrapper(buff, 12, test, "Hello, World\n");
		break;
	default:
		std::cerr << "Error\n";
		break;
	}
	if (test.str_ft == test.str_orig && test.errno_orig == test.errno_ft && \
		test.result_orig == test.result_ft)
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
