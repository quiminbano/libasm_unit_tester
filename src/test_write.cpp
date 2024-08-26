/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_write.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: corellan <corellan@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 14:11:47 by corellan          #+#    #+#             */
/*   Updated: 2024/08/26 10:47:14 by corellan         ###   ########.fr       */
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
	int	backup;
	int	original_fd;
};

static FileDescriptors	*singleton_fd(void)
{
	static FileDescriptors	file_descriptors;

	return(&file_descriptors);
}

static void	signal_handler(int sig)
{
	if (singleton_fd()->backup != -1)
	{
		close(singleton_fd()->pipes[0]);
		dup2(singleton_fd()->backup, singleton_fd()->original_fd);
		close(singleton_fd()->backup);
	}
	if (sig == SIGSEGV)
		std::cout << YELLOW << "[CRASH]" << RESET " -> " << RED << "[KO]" << RESET << "\n";
	std::exit(0);
}

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
	file << "THE RETURN VALUE OF YOUR FT_WRITE IS: " << test.result_ft << "\n";
	file << "THE ORIGINAL GETS: " << test.result_orig << "\n";
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

static int	get_errno_value(int fd, const void *buf, size_t count, ssize_t (*w_funct)(int, const void *, size_t))
{
	pid_t	return_fork;
	pid_t	own_pid;
	int		return_errno;
	int		extract_return;

	own_pid = getpid();
	return_fork = fork();
	if (return_fork == -1)
	{
		std::cerr << "Error trying to perform a fork operation" << '\n';
		std::exit(1);
	}
	if (return_fork == 0)
	{
		signal(SIGSEGV, SIG_DFL);
		close(fd);
		(*w_funct)(fd, buf, count);
		return_errno = errno;
		errno = 0;
		exit(return_errno);
	}
	waitpid(return_fork, &extract_return, 0);
	if (WTERMSIG(extract_return))
	{
		kill(own_pid, SIGSEGV);
		return (1);
	}
	return_errno = (int)WEXITSTATUS(extract_return);
	return (return_errno);
}

static ssize_t	get_return_value(int fd, const void *buf, size_t count, ssize_t (*w_funct)(int, const void *, size_t))
{
	pid_t	return_fork;
	pid_t	own_pid;
	ssize_t	return_function;
	int		extract_return;

	own_pid = getpid();
	return_fork = fork();
	if (return_fork == -1)
	{
		std::cerr << "Error trying to perform a fork operation" << '\n';
		std::exit(1);
	}
	if (return_fork == 0)
	{
		signal(SIGSEGV, SIG_DFL);
		close(fd);
		return_function = (*w_funct)(fd, buf, count);
		exit(return_function);
	}
	waitpid(return_fork, &extract_return, 0);
	if (WTERMSIG(extract_return))
	{
		kill(own_pid, SIGSEGV);
		return (1);
	}
	return_function = (ssize_t)WEXITSTATUS(extract_return);
	if (return_function == 255)
		return_function = -1;
	return (return_function);
}

static void	process_closed_fd(int fd, const void *buf, size_t count, Test &test)
{
	test.result_orig = get_return_value(fd, buf, count, &write);
	test.errno_orig = get_errno_value(fd, buf, count, &write);
	test.result_ft = get_return_value(fd, buf, count, &ft_write);
	test.errno_ft = get_errno_value(fd, buf, count, &ft_write);
	test.str_orig = "";
	test.str_ft = "";
}

static void	write_wrapper(int fd, const void *buf, size_t count, Test &test)
{
	singleton_fd()->original_fd = fd;
	if (test.nbr == 5)
	{
		process_closed_fd(fd, buf, count, test);
		return ;
	}
	(singleton_fd()->backup) = dup(fd);
	if (pipe((singleton_fd())->pipes) == -1)
		std::exit(1);
	dup2((singleton_fd())->pipes[1], fd);
	close((singleton_fd())->pipes[1]);
	test.result_orig = write(fd, buf, count);
	test.errno_orig = errno;
	errno = 0;
	if (test.result_orig <= 0)
		write(fd, "", 1);
	extract_output(test.str_orig, (singleton_fd())->pipes[0]);
	close((singleton_fd())->pipes[0]);
	dup2((singleton_fd())->backup, fd);
	close((singleton_fd())->backup);
	(singleton_fd())->backup = dup(fd);
	if (pipe((singleton_fd())->pipes) == -1)
		exit(1);
	dup2((singleton_fd())->pipes[1], fd);
	close((singleton_fd())->pipes[1]);
	test.result_ft = ft_write(fd, buf, count);
	test.errno_ft = errno;
	errno = 0;
	if (test.result_orig <= 0)
		write(fd, "", 1);
	extract_output(test.str_ft, (singleton_fd())->pipes[0]);
	close((singleton_fd())->pipes[0]);
	dup2((singleton_fd())->backup, fd);
	close((singleton_fd())->backup);
	(singleton_fd())->backup = -1;
}

static void	process_test(char const *nbr_str)
{
	int		nbr;
	Test	test;

	singleton_fd()->pipes[0] = -1;
	singleton_fd()->pipes[1] = -1;
	singleton_fd()->backup = -1;
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
	case 5:
		write_wrapper(1, "Hello, World\n", 13, test);
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
