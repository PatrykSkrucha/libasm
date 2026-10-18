#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#define GREEN "\033[32m"
#define RED   "\033[31m"
#define RESET "\033[0m"

size_t	ft_strlen(const char *str);
char*	ft_strcpy(char *restrict dst, const char *restrict src);
int		ft_strcmp(const char *s1, const char *s2);
ssize_t	ft_write(int fd, const void *buf, size_t count);
ssize_t	ft_read(int fd, void *buf, size_t count);
char*	ft_strdup(const char *s);

static int sign(int x){ return (x > 0) - (x < 0); }

int main()
{
	//ft_strlen
	{
		printf(GREEN "ft_strlen" RESET "\n");
		printf("testing len of world \"hello\": %zu\n", ft_strlen("hello"));
		printf("testing len of empty string: %zu\n", ft_strlen(""));
		//printf(RED "this will segfault :(" RESET "\n");
		//printf("testing len of NULL: %zu\n", ft_strlen(NULL));
	}

	//ft_strcpy
	{
		printf(GREEN "\n\nft_strcpy" RESET "\n");
		char dst[6];
		char *src = "Hello";
		ft_strcpy(dst, src);
		printf("should be \"Hello\": %s\n", dst);
	}

	//ft_strcmp
	{
		printf(GREEN "\n\nft_strcmp" RESET "\n");
		

		const char *a[] = {"Hello", "Hello", "Hell", "Hello", "abc", NULL};
		const char *b[] = {"Hello", "World", "Hello", "Hell", "abd", NULL};
		for (int i = 0; a[i]; i++)
		{
			int mine = sign(ft_strcmp(a[i], b[i]));
			int libc = sign(strcmp(a[i], b[i]));
			printf("\"%s\" vs \"%s\": %s\n", a[i], b[i],
				mine == libc ? GREEN "OK" RESET : RED "KO" RESET);
		}
	}

	//ft_write
	{
		printf(GREEN "\n\nft_write" RESET "\n");
		char *str = "Hello, I am using ft_write made in asm, how cool is it!\n";
		ft_write(1, str, ft_strlen(str));
		errno = 0;
		ssize_t r1 = write(-1, "x", 1);
		int e1 = errno;

		errno = 0;
		ssize_t r2 = ft_write(-1, "x", 1);
		int e2 = errno;

		printf("libc:  	    ret=%zd errno=%d (%s)\n", r1, e1, strerror(e1));
		printf("ft_write:   ret=%zd errno=%d (%s)\n", r2, e2, strerror(e2));
	}

	//ft_read
	{
		printf(GREEN "\n\nft_read" RESET "\n");
		char buf[32] = {0};
		int fd = open("Makefile", O_RDONLY);
		ssize_t r = ft_read(fd, buf, 8);
		printf("read ret=%zd [%s]\n", r, buf);
		close(fd);
		errno = 0;
		r = ft_read(-1, buf, 8);
		printf("read bad fd: ret=%zd errno=%d (%s)\n", r, errno, strerror(errno));
	}

	//ft_strdup
	{
		printf(GREEN "\n\nft_strdup" RESET "\n");
		char *str = "Some string";
		char *str2 = ft_strdup(str);
		char *empty = "";
		char *empty2 = ft_strdup(empty);
		printf("original string: %s\nduplicated string: %s\n", str, str2);
		printf("original string: %s\nduplicated string: %s\n", empty, empty2);
		free(str2);
		free(empty2);
	}

	return (0);
}