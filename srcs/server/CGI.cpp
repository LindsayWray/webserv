//
// Created by Kester kas De rooij on 3/31/22.
//
#include "server.hpp"
#include <unistd.h>

#define BUFFER_SIZE 128

void	clean_up(char *str)
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

size_t	ft_strnlen(const char *s)
{
    size_t i;

    i = 0;
    while (s[i] && s[i] != '\n')
        i++;
    return (i);
}

void			ft_memccpy_gnl(char *dest, char *src, char c)
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

char			*ft_strdup_gnl(char *str)
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

std::string CGI( webserv::locationData *location, char** env ){
    HTTPResponseMessage response;
    int ret = EXIT_FAILURE, status, pipes[2], stdin_copy = dup(STDIN_FILENO);
    pid_t pid;
    std::string ret_str, reqPath = location->root;
    char* line;
    reqPath.append(location->cgi_param);
    char* args[3] = { strdup("/usr/bin/python"), strdup(reqPath.c_str()), NULL };

    if ( pipe(pipes) != 0 )
        std::cerr << "pipe failed" << std::endl;
    pid = fork();
    if ( pid < 0 ){
        return NULL;
    } else if ( pid == 0 ){
        close( pipes[0] );
        dup2( pipes[1], STDOUT_FILENO );
        close( pipes[1] );
        if ( ( ret = execve( args[0], args, env ) ) < 0 )
            std::cerr << "file open went wrong" << std::endl;
        exit( ret );
    } else {
        waitpid( pid, &status, 0 );
        close( pipes[1] );
        while ( get_next_line(pipes[0], reinterpret_cast<char **>(&line)) ) { //get_next_line(STDIN_FILENO, reinterpret_cast<char **>(&line)) // getline( std::cin, line )
            ret_str.append(line);
            ret_str.append("\n");
        }
        dup2(stdin_copy, 0);
        if ( WIFEXITED(status) )
            ret = WIFEXITED(status);
    }
    std::cout << ret_str;
    free( args[0] );
    free( args[1] );
    return ret_str;
}

HTTPResponseMessage responseFromCGI(webserv::httpData* config, webserv::locationData *location ) {
	HTTPResponseMessage response;
    std::string line;
    std::string body("");

    body = CGI( location, config->env );
    return response.addStatus(HTTPResponseMessage::OK )
            .addLength(body.length())
            .addBody(body)
            .addType("text/plain");
}
