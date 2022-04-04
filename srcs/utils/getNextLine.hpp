//
// Created by Kester kas De rooij on 4/4/22.
//

#ifndef WEBSERV_GETNEXTLINE_HPP
#define WEBSERV_GETNEXTLINE_HPP

#define BUFFER_SIZE 128

#include <unistd.h>
#include <stdlib.h>

static void	clean_up(char *str)
{
    size_t	i;
    size_t	j;

    i = 0;
    j = 0;
    while (str[i] && str[i] != '\n' && i < BUFFER_SIZE - 1)
        i++;
    i++;
    while (i < BUFFER_SIZE)
    {
        str[j] = str[i];
        i++;
        j++;
    }
    while (j <= BUFFER_SIZE)
    {
        str[j] = '\0';
        j++;
    }
}

static size_t	ft_strnlen(const char *s)
{
    size_t i;

    i = 0;
    while (s[i] && s[i] != '\n')
        i++;
    return (i);
}

static void			ft_memccpy_gnl(char *dest, char *src, char c)
{
    size_t	i;

    i = 0;
    while (src[i] && src[i] != c)
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

static char			*ft_strdup_gnl(char *str)
{
    size_t	len;
    char	*dest;

    len = (!str) ? 0 : ft_strnlen(str);
    dest = (char*)malloc(sizeof(char) * len + 1);
    dest[len] = '\0';
    while (len)
    {
        len--;
        dest[len] = str[len];
    }
    return (dest);
}

static size_t	ft_strchr_gnl(char *str, char c)
{
    size_t	i;

    i = 0;
    while (str[i])
    {
        if (str[i] == c)
            return (1);
        i++;
    }
    return (0);
}

static char		*ft_strjoin_gnl(char *line, char *temp, int *skip)
{
    size_t	len;
    char	*dest;

    if (!line)
        return (NULL);
    len = ft_strnlen(line) + ft_strnlen(temp);
    dest = (char*)malloc(sizeof(char) * len + 1);
    if (!dest)
        return (NULL);
    ft_memccpy_gnl(dest, line, '\n');
    ft_memccpy_gnl(&dest[ft_strnlen(line)], temp, '\n');
    *skip = ft_strchr_gnl(temp, '\n');
    clean_up(temp);
    free(line);
    return (dest);
}

int				get_next_line(int fd, char **line)
{
    size_t		ret;
    int			skip;
    static char	temp[BUFFER_SIZE];

    skip = 0;
    if (!line || BUFFER_SIZE <= 0 || fd < 0)
        return (-1);
    *line = ft_strdup_gnl("");
    if (*temp)
        *line = ft_strjoin_gnl(*line, temp, &skip);
    while (!skip)
    {
        ret = read(fd, temp, BUFFER_SIZE);
        if (ret < 0)
            return (-1);
        *line = ft_strjoin_gnl(*line, temp, &skip);
        if (!line)
            return (-1);
        if (ret != BUFFER_SIZE)
            break ;
    }
    if (ret != BUFFER_SIZE && !skip)
        return (0);
    return (1);
}

#endif //WEBSERV_GETNEXTLINE_HPP
