/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 20:00:31 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/06 14:23:25 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
#include "libft.h"

int	print_char(char c)
{
	return (write(1, &c, 1));
}

int	print_format(const char *specifier, va_list ap)
{
	int	counter;
	int	i;

	i = 0;
	counter = 0;
	if (specifier[i] == 'c')
		counter += print_char((char)va_arg(ap, int));
	else if (specifier[i] == 's')
		counter += print_str(va_arg(ap, char *));
	else if (specifier[i] == 'p')
		counter += print_ptr(va_arg(ap, void *));
	else if (specifier[i] == 'd' || specifier[i] == 'i')
		counter += print_digit(va_arg(ap, int), 10);
	else if (specifier[i] == 'u')
		counter += print_digit((long)va_arg(ap, unsigned int), 10);
	else if (specifier[i] == 'x')
		counter += print_digit((long)va_arg(ap, unsigned int), 16);
	else if (specifier[i] == 'X')
		counter += print_digit_x((long)va_arg(ap, unsigned int), 16);
	else if (specifier[i] == 'z' && (specifier[1]) == 'u')
		counter += print_digit((size_t)va_arg(ap, size_t), 10);
	else if (specifier[i] == '%')
		counter += write(1, "%", 1);
	return (counter);
}

static int	handle_percent(const char **format, va_list ap)
{
	int	counter;

	counter = 0;
	(*format)++;
	if (**format)
	{
		if (**format == 'z' && *(*format + 1) == 'u')
		{
			counter += print_digit((size_t)va_arg(ap, size_t), 10);
			(*format)++;
		}
		else
			counter += print_format(*format, ap);
	}
	return (counter);
}

int	ft_printf(const char *format, ...)
{
	va_list	ap;
	int		counter;

	counter = 0;
	va_start(ap, format);
	if (!format)
		return (va_end(ap), -1);
	while (*format)
	{
		if (*format == '%')
			counter += handle_percent(&format, ap);
		else
			counter += write(1, format, 1);
		format++;
	}
	va_end(ap);
	return (counter);
}
// int	ft_printf(const char *format, ...)
// {
// 	va_list	ap;
// 	int		counter;

// 	counter = 0;
// 	va_start(ap, format);
// 	if (!format)
// 		return (va_end(ap), -1);
// 	while (*format)
// 	{
// 		if (*format == '%')
// 		{
// 			format++;
// 			if (*format)
// 			{
// 				if (*format == 'z' && *(format + 1) == 'u')
// 				{
// 					counter += print_digit((size_t)va_arg(ap, size_t), 10);
// 					format++;
// 				}
// 				else
// 					counter += print_format(format, ap);
// 			}
// 		}
// 		else
// 			counter += write(1, format, 1);
// 		format++;
// 	}
// 	va_end(ap);
// 	return (counter);
// }
