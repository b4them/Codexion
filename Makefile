# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bmoura-s <bmoura-s@42Porto.student.com>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/11/02 00:15:52 by bmoura-s          #+#    #+#              #
#    Updated: 2026/11/02 00:15:59 by bmoura-s         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = codexion

INC = -I ./includes

SRCS = src/actions.c \
	src/arbiter.c \
	src/engine.c \
	src/helper.c \
	src/main.c \
	src/parse.c \
	src/queue.c \
	src/routine.c \
	src/time.c

OBJS = ${SRCS:.c=.o}

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread


all: ${NAME}

${NAME}: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o ${NAME}

%.o: %.c
	${CC} ${CFLAGS} ${INC} -c $< -o $@

${OBJS}: includes/codex.h

clean:
	rm -f ${OBJS}

fclean: clean
	rm -f ${NAME}

re: fclean
	${MAKE} all

.PHONY: all clean fclean re
