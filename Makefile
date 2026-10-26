NAME    = libasm.a
SRCS    = ft_strlen.s ft_strcpy.s ft_strcmp.s ft_write.s ft_read.s ft_strdup.s ft_list_size.s ft_list_push_front.s ft_atoi_base.s ft_list_remove_if.s ft_list_sort.s
OBJS    = $(SRCS:.s=.o)
NASM    = nasm
NFLAGS  = -f elf64
CC      = cc

all: $(NAME)

$(NAME): $(OBJS)
	ar rcs $(NAME) $(OBJS)

%.o: %.s
	$(NASM) $(NFLAGS) $< -o $@

main.o: main.c
	$(CC) -c $< -o $@

test: main.o $(NAME)
	$(CC) $^ -o $@

clean:
	rm -f $(OBJS) main.o

fclean: clean
	rm -f $(NAME) test

re: fclean all

.PHONY: all clean fclean re