/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_write.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 14:11:47 by corellan          #+#    #+#             */
/*   Updated: 2024/08/22 20:48:44 by corellan         ###   ########.fr       */
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
	std::string 	str_orig;
	std::string 	str_ft;
};


static void	create_log(int &nbr, Test &test)
{
	std::ofstream		file;
	std::ostringstream	oss;

	oss << "logs/ft_write" << nbr << ".txt";
	file.open(oss.str(), std::ios_base::out);
	if (file.fail())
	{
		std::cerr << "Error trying to create/modify the file\n";
		return ;
	}
	file << "YOUR FT_WRITE GOT AS RESULT: " << test.str_ft << "\n";
	file << "THE ORIGINAL ONE GETS: " << test.str_orig << "\n";
	file << "VALUE OF ERRNO OBTAINED BY YOUR FT_WRITE: " << test.errno_ft << "\n";
	file << "VALUE OF ERRNO OBTAINED BY THE ORIGINAL WRITE: " << test.errno_orig << "\n";
	file.close();
	return ;
}

static void	print_test_and_test_number(int &nbr)
{
	std::cout << "Test " << nbr << ": ";
}

static void	extract_output(std::string &str_dest, int fd)
{
	char	buffer[4096];
	int		bytes_read;

	bzero(buffer, sizeof(buffer));
	bytes_read = read(fd, buffer, 4095);
	if (bytes_read < 0)
		std::exit(1);
	str_dest = buffer;
}

static void	write_wrapper(int fd, const void *buf, size_t count, Test &test)
{
	int	pipes[2];
	int	backup;

	backup = dup(fd);
	if (pipe(pipes) == -1)
		std::exit(1);
	dup2(pipes[1], fd);
	close(pipes[1]);
	test.result_orig = write(fd, buf, count);
	if (test.result_orig <= 0)
		write(1, "", 1);
	test.errno_orig = errno;
	errno = 0;
	extract_output(test.str_orig, pipes[0]);
	close(pipes[0]);
	dup2(backup, fd);
	close(backup);
	backup = dup(fd);
	if (pipe(pipes) == -1)
		exit(1);
	dup2(pipes[1], fd);
	close(pipes[1]);
	test.result_ft = ft_write(fd, buf, count);
	if (test.result_orig <= 0)
		write(1, "", 1);
	test.errno_ft = errno;
	errno = 0;
	extract_output(test.str_ft, pipes[0]);
	close(pipes[0]);
	dup2(backup, fd);
	close(backup);
}

static void	process_test(char const *nbr_str)
{
	int		nbr;
	Test	test;

	nbr = std::stoi(nbr_str);
	test.result_orig = 0;
	test.result_ft = 0;
	test.errno_orig = 0;
	test.errno_ft = 0;
	print_test_and_test_number(nbr);
	test.str_orig = "";
	test.str_ft = "";
	switch (nbr)
	{
	case 1:
		write_wrapper(1, "Hello, World\n", 13, test);
		break;
	case 2:
		write_wrapper(1, "Hello, World\n", 0, test);
		break;
	case 3:
		write_wrapper(-1, "Hello, World\n", 13, test);
		errno = 0;
		break;
	case 4:
		write_wrapper(1, nullptr, 13, test);
		break;
	default:
		std::cerr << "Error\n";
		break;
	}
	if (test.str_ft == test.str_orig && test.errno_orig == test.errno_ft)
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
