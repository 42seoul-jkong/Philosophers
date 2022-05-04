# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jkong <jkong@student.42seoul.kr>           +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/05/04 21:58:09 by jkong             #+#    #+#              #
#    Updated: 2022/05/04 21:58:09 by jkong            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

.SUFFIXES: .c .o .h
.PHONY: all clean fclean re bonus

CC = cc
RM = rm -f

OBJECTS_DIR = objs/
HEADER = philo_bonus.h

TARGET = philo_bonus
SRCS = philo_bonus.c
OBJS = $(addprefix $(OBJECTS_DIR), $(SRCS:.c=.o))

C_SANITIZER_FLAGS = address undefined
CFLAGS += $(addprefix -fsanitize=, $(C_SANITIZER_FLAGS))
LDFLAGS += $(addprefix -fsanitize=, $(C_SANITIZER_FLAGS))

C_WARNING_FLAGS = all extra error
CFLAGS += $(addprefix -W, $(C_WARNING_FLAGS))

C_DEBUG_FLAGS = g3
CFLAGS += $(addprefix -, $(C_DEBUG_FLAGS))

all: $(TARGET)
clean:				;	$(RM) -r $(OBJECTS_DIR)
fclean: clean		;	$(RM) $(TARGET)
re: fclean all
bonus: all

$(OBJECTS_DIR):
	mkdir $(OBJECTS_DIR)

$(OBJS): $(HEADER) | $(OBJECTS_DIR)

$(addprefix $(OBJECTS_DIR), %.o): %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(TARGET): $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $^