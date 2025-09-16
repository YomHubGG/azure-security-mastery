/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 13:04:28 by tritter           #+#    #+#             */
/*   Updated: 2025/07/19 14:26:44 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COLOR_H
# define COLOR_H

# define RESET       "\x1b[0m"
# define BOLD        "\x1b[1m"

// Foreground colors
# define RED         "\x1b[31m"
# define GREEN       "\x1b[32m"
# define YELLOW      "\x1b[33m"
# define BLUE        "\x1b[34m"
# define MAGENTA     "\x1b[35m"
# define CYAN        "\x1b[36m"
# define WHITE       "\x1b[37m"

// Bright versions
# define BRIGHT_RED     "\x1b[91m"
# define BRIGHT_GREEN   "\x1b[92m"
# define BRIGHT_YELLOW  "\x1b[93m"
# define BRIGHT_BLUE    "\x1b[94m"
# define BRIGHT_MAGENTA "\x1b[95m"
# define BRIGHT_CYAN    "\x1b[96m"
# define BRIGHT_WHITE   "\x1b[97m"

#endif