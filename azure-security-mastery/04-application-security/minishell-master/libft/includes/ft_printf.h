/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 08:27:13 by ggrisole          #+#    #+#             */
/*   Updated: 2025/04/08 18:00:38 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdarg.h>
# include <stdio.h>
# include <unistd.h>

int	ft_printf(const char *format, ...);
int	ft_dprintf(int fd, const char *format, ...);
int	print_char(char c);
int	print_str(char *str);
int	print_digit(long long n, int base);
int	print_digit_x(long long n, int base);
int	print_ptr(void *p);
int	print_digit_ptr(unsigned long n, int base);

#endif