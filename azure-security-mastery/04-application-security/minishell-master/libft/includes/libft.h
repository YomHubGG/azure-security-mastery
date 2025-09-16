/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 17:15:22 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/27 14:54:35 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

// General
# include <stdlib.h>
# include <unistd.h>

// LIBFT
# include <stdio.h>
# include <string.h>

// FT_PRINTF
# include "ft_printf.h"
# include <stdarg.h>

// GNL
# include <fcntl.h>

// Garbage Collector
# include "garbage.h"

// Prototypes LIBFT
typedef struct s_list
{
	void				*content;
	struct s_list		*next;
}						t_list;

int						ft_isalpha(int c);
int						ft_isdigit(int c);
int						ft_isalnum(int c);
int						ft_islower(int c);
int						ft_isupper(int c);
int						ft_isascii(int c);
int						ft_isprint(int c);
int						ft_toupper(char c);
int						ft_tolower(char c);
int						ft_strncmp(const char *s1, const char *s2, size_t n);
int						ft_memcmp(const void *s1, const void *s2, size_t n);
int						ft_atoi(const char *nptr);
int						ft_lstsize(t_list *lst);
int						ft_strcmp(const char *s1, const char *s2);

char					*ft_strchr(const char *s, int c);
char					*ft_strrchr(const char *s, int c);
char					*ft_strnstr(const char *big, const char *little,
							size_t len);
char					*ft_strdup(const char *s);
char					*ft_substr(char const *s, unsigned int start,
							size_t len);
char					**ft_split(char const *s, char c);
char					*ft_itoa(int n);
char					*ft_strmapi(char const *s, char (*f)(unsigned int,
								char));
char					*ft_strjoin(char const *s1, char const *s2);
char					*ft_strtrim(char const *s1, char const *set);

void					*ft_memset(void *s, int c, size_t n);
void					ft_bzero(void *s, size_t n);
void					*ft_memcpy(void *dest, const void *src, size_t n);
void					*ft_memmove(void *dest, const void *src, size_t n);
void					*ft_memchr(const void *s, int c, size_t n);
void					*ft_calloc(size_t nmemb, size_t size);
void					ft_striteri(char *s, void (*f)(unsigned int, char *));
void					ft_putchar_fd(char c, int fd);
void					ft_putstr_fd(char *s, int fd);
void					ft_putendl_fd(char *s, int fd);
void					ft_putnbr_fd(int n, int fd);
void					ft_lstadd_front(t_list **lst, t_list *new);
void					ft_lstadd_back(t_list **lst, t_list *new);
void					ft_lstdelone(t_list *lst, void (*del)(void *));
void					ft_lstclear(t_list **lst, void (*del)(void *));
void					ft_lstiter(t_list *lst, void (*f)(void *));

size_t					ft_strlcpy(char *dst, const char *src, size_t size);
size_t					ft_strlen(const char *s);
size_t					ft_strlcat(char *dst, const char *src, size_t size);

t_list					*ft_lstnew(void *content);
t_list					*ft_lstlast(t_list *lst);
t_list					*ft_lstmap(t_list *lst, void *(*f)(void *),
							void (*del)(void *));

int						ft_abs(int abs);

// Prototypes printf

int						ft_printf(const char *format, ...);
int						print_char(char c);
int						print_str(char *str);
int						print_digit(long long n, int base);
int						print_digit_x(long long n, int base);
int						print_ptr(void *p);
int						print_digit_ptr(unsigned long n, int base);

// Prototypes GNL
// Redefine buffer_size if needed with flag -D BUFFER_SIZE=nbr_you_want
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE BUFSIZ
# endif

char					*get_next_line(int fd);
char					*gnl_read_file(int fd, char *stash);
char					*gnl_get_line(char *stash);
size_t					gnl_strlen(const char *s);
char					*gnl_strchr(const char *s, int c);
char					*gnl_free(char *stash, char *buff);
char					*gnl_strjoin(char *stash, char *buffer);
char					*gnl_update_stash(char *old_stash);

#endif