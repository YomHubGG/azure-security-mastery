/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_fctn.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 08:03:33 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/06 15:12:17 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
#include "libft.h"

int	print_str(char *str)
{
	int	i;

	if (!str)
		return (write(1, "(null)", 6));
	i = 0;
	while (str[i])
	{
		print_char(str[i]);
		i++;
	}
	return (i);
}

int	print_digit(long long n, int base)
{
	char	*symbols;
	int		counter;

	symbols = "0123456789abcdef";
	counter = 0;
	if (base == 10 && n < 0)
	{
		counter += print_char('-');
		n = -n;
	}
	if (n >= base)
		counter += print_digit(n / base, base);
	counter += print_char(symbols[n % base]);
	return (counter);
}

int	print_digit_x(long long n, int base)
{
	char	*symbols;
	int		counter;

	symbols = "0123456789ABCDEF";
	counter = 0;
	if (n >= base)
		counter += print_digit_x(n / base, base);
	counter += print_char(symbols[n % base]);
	return (counter);
}

int	print_digit_ptr(unsigned long n, int base)
{
	char	*symbols;
	int		counter;

	symbols = "0123456789abcdef";
	counter = 1;
	if (n >= (unsigned long)base)
		counter += print_digit_ptr(n / base, base);
	print_char(symbols[n % base]);
	return (counter);
}

int	print_ptr(void *p)
{
	int				counter;
	unsigned long	ptr;

	ptr = (unsigned long)p;
	counter = 0;
	if (ptr == 0)
		counter += write(1, "(nil)", 5);
	else
	{
		counter += write(1, "0x", 2);
		counter += print_digit_ptr(ptr, 16);
	}
	return (counter);
}
