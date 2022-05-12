NAME		=	webserv
SRCDIR		=	srcs
SRCS		=	Request.cpp HTTPResponseMessage.cpp HTTPResponseMessageContentTypeMap.cpp \
                parentSocket.cpp serverSocket.cpp listeningSocket.cpp clientSocket.cpp socketData.cpp \
                dataStructs.cpp \
                configParser.cpp configParserSetters.cpp configParserLocation.cpp configParserUtils.cpp \
                handler.cpp autoindexing.cpp handlerUtils.cpp \
                CGIResponse.cpp CGIRegister.cpp \
                webserv.cpp webservInit.cpp accepter.cpp responder.cpp \
                main.cpp
OBJDIR		=	objs
VPATH 		=	$(subst $(space),:,$(shell find $(SRCDIR) -type d))
OBJS		=	$(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))
CC			=	clang++
FLAGS		=	-Wall -Wextra -Werror -pedantic -g -fsanitize=address -std=c++2a

all:		$(NAME)

$(NAME):	$(OBJDIR) $(OBJS)
	@$(CC) $(OBJS) $(FLAGS) -o $(NAME)
	@echo "You can now run ./$(NAME), enjoy!"

$(OBJDIR)/%.o:  $(notdir %.cpp)
	@$(CC) $(FLAGS) -c $< -o $@

$(OBJDIR):
	@mkdir -p $(OBJDIR)

echo:
	@echo $(OBJS)

clean:
	@rm -rf $(OBJDIR)

fclean:		clean
	@rm -f $(NAME)

re:			fclean all

.PHONY:	all clean fclean re .cpp.o
