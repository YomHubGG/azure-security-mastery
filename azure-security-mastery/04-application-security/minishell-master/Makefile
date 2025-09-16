# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/08 17:19:16 by ggrisole          #+#    #+#              #
#    Updated: 2025/07/27 20:25:43 by ggrisole         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# **************************************************************************** #
#                                VARIABLES                                     #
# **************************************************************************** #

NAME            := minishell
CC              := gcc
CFLAGS          := -Wall -Wextra -Werror -g3
READLINE_LIB    := -lreadline -lncurses
READLINE_INC    := -I/usr/include -I/usr/include/readline
RM              := rm -f
MAKEFLAGS      += --no-print-directory

INCLUDES_DIR    := includes
LIBFT_DIR       := libft
LIBFT_INC       := $(LIBFT_DIR)/includes
OBJS_DIR        := objs
INCS            := -I$(INCLUDES_DIR) -I$(LIBFT_INC)

LIBFT_A         := $(LIBFT_DIR)/libft.a
LIBS            := -L$(LIBFT_DIR) -lft

SRC_DIR         := srcs
PARSING_DIR     := $(SRC_DIR)/parsing
AST_DIR			:= $(SRC_DIR)/AST
EXPAND_DIR		:= $(SRC_DIR)/expansion
TOKENIZER_DIR	:= $(PARSING_DIR)/tokenizer
SYNTAX_DIR		:= $(PARSING_DIR)/syntax_checker
ENVAR_DIR		:= $(SRC_DIR)/envar
BUILTINS_DIR	:= $(SRC_DIR)/builtins
GLOBBING_DIR    := $(SRC_DIR)/globbing
EXEC_DIR		:= $(SRC_DIR)/exec
SUBSHELL_DIR	:= $(SRC_DIR)/subshell_handler

SRCS_ENVAR      := env_utils env_manipulation env_display env_conversion init_envp

SRCS_BUILTINS   := builtin_dispatcher builtin_pwd builtin_env builtin_exit \
                   builtin_export builtin_unset builtin_echo builtin_cd
				   
SRCS_MAIN       := main exec_loop signal_handler

SRCS_GLOBBING   := globbing_explore globbing_helper globbing_init globbing_node

SRCS_EXPAND		:= expand_helper expand_helper2 expand_init expand_word \
					expand_error_parsing expand_utils

SRCS_SUBSHELL	:= subshell_handler_init

SRCS_EXEC		:= exec_child_process exec_explore exec_helper exec_init \
					exec_heredoc_handler exec_clear_quote exec_child_tracer \
					exec_wait_child exec_redir_cleaner exec_pipe_cleaner exec_helper2 \
					exec_redir_subwraper exec_heredoc_expand

SRCS_AST		:= ast_creation ast_node_creation_1 ast_node_creation_2 ast_pipe_handler\
					ast_redirection_handler ast_word_handler\
					ast_lor_land_handler ast_parentheses_helper\
					ast_redirection_handler_subwrapper
					
SRCS_TOKENIZER  := tokenizer tokenizer_utils tokenizer_utils_2 tokenizer_utils_3 \
					tokenizer_utils_4 final_tokenizer final_tokenizer_2 \
					final_tokenizer_3 final_tokenizer_4 final_tokenizer_5 \
					final_tokenizer_6 \
					final_tokenizer_screwnorm final_tokenizer_wildcard \
					final_tokenizer_grouping final_tokenizer_grouping_2 \
					run_pipeline_wrapper gc_strdup		
								
SRCS_SYNTAX     := early_error_check_1 early_error_check_2 early_error_check_3 \
					early_error_check_4 early_error_check_5 print_error_1 \
					print_error_2

SRCS            := 	$(addprefix $(SRC_DIR)/, $(addsuffix .c, $(SRCS_MAIN))) \
 					$(addprefix $(TOKENIZER_DIR)/, $(addsuffix .c, $(SRCS_TOKENIZER))) \
                  	$(addprefix $(SYNTAX_DIR)/, $(addsuffix .c, $(SRCS_SYNTAX))) \
					$(addprefix $(AST_DIR)/, $(addsuffix .c, $(SRCS_AST))) \
				    $(addprefix $(EXPAND_DIR)/, $(addsuffix .c, $(SRCS_EXPAND))) \
					$(addprefix $(ENVAR_DIR)/, $(addsuffix .c, $(SRCS_ENVAR))) \
					$(addprefix $(BUILTINS_DIR)/, $(addsuffix .c, $(SRCS_BUILTINS))) \
					$(addprefix $(GLOBBING_DIR)/, $(addsuffix .c, $(SRCS_GLOBBING))) \
					$(addprefix $(EXEC_DIR)/, $(addsuffix .c, $(SRCS_EXEC))) \
					$(addprefix $(SUBSHELL_DIR)/, $(addsuffix .c, $(SRCS_SUBSHELL)))
					
OBJS            := $(SRCS:%.c=$(OBJS_DIR)/%.o)
DEPS            := $(OBJS:.o=.d)

# **************************************************************************** #
#                                RULES                                         #
# **************************************************************************** #

all: $(NAME)

$(NAME): $(LIBFT_A) $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) $(INCS) $(READLINE_INC) $(LIBS) $(READLINE_LIB) -o $@

$(OBJS_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -MMD -MP $(INCS) -c $< -o $@	
	
$(LIBFT_A):
	$(MAKE) -C $(LIBFT_DIR)

valgrind_sans_readline: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all \
		--suppressions=valgrind_ignore.txt \
		./$(NAME)

clean:
	@$(RM) -r $(OBJS_DIR)
fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean
	@$(RM) $(NAME)
re:
	@$(MAKE) fclean
	@$(MAKE) all

-include $(DEPS)

.PHONY: all clean fclean re valgrind_sans_readline

# valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes --suppressions=./valgrind.supp -s ./minishell