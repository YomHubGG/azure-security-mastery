/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 14:18:19 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/11 21:30:31 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
#include "libft.h"

int	print_char_fd(char c, int fd)
{
	return (write(fd, &c, 1));
}

int	print_str_fd(char *str, int fd)
{
	int	counter;

	counter = 0;
	if (!str)
		str = "(null)";
	while (*str)
	{
		counter += print_char_fd(*str, fd);
		str++;
	}
	return (counter);
}

int	print_digit_fd(long long n, int base, int fd)
{
	int		counter;
	char	*digits;

	digits = "0123456789abcdef";
	counter = 0;
	if (n < 0)
	{
		counter += print_char_fd('-', fd);
		n = -n;
	}
	if (n >= base)
		counter += print_digit_fd(n / base, base, fd);
	counter += print_char_fd(digits[n % base], fd);
	return (counter);
}

int	print_format_fd(const char *specifier, va_list ap, int fd)
{
	int	counter;
	int	i;

	i = 0;
	counter = 0;
	if (specifier[i] == 'c')
		counter += print_char_fd((char)va_arg(ap, int), fd);
	else if (specifier[i] == 's')
		counter += print_str_fd(va_arg(ap, char *), fd);
	else if (specifier[i] == 'd' || specifier[i] == 'i')
		counter += print_digit_fd((long)va_arg(ap, int), 10, fd);
	else if (specifier[i] == '%')
		counter += print_char_fd('%', fd);
	return (counter);
}

int	ft_dprintf(int fd, const char *format, ...)
{
	va_list	ap;
	int		counter;
	int		i;

	i = 0;
	counter = 0;
	va_start(ap, format);
	while (format[i])
	{
		if (format[i] == '%')
		{
			i++;
			counter += print_format_fd(&format[i], ap, fd);
		}
		else
			counter += print_char_fd(format[i], fd);
		i++;
	}
	va_end(ap);
	return (counter);
}
