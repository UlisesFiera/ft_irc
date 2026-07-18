NAME 		= 	ircserv

C++			= 	g++ -std=c++98 -Iinclude/

STD_FLAGS 	= 	-Wall -Wextra -Werror -g

SRCS		= 	src/main.cpp\
				src/client/Client.cpp\
				src/channel/Channel.cpp\
				src/server/EventManager.cpp\
				src/server/Server.cpp\
				src/server/commands/ServerAuth.cpp\
				src/server/commands/ServerInvite.cpp\
				src/server/commands/ServerJoin.cpp\
				src/server/commands/ServerKick.cpp\
				src/server/commands/ServerModer.cpp\
				src/server/commands/ServerNick.cpp\
				src/server/commands/ServerPrivmsg.cpp\
				src/server/commands/ServerTopic.cpp\
				src/server/Socket.cpp\
				src/server/commands/ServerInvite.cpp\
				src/server/commands/ServerJoin.cpp\
				src/server/commands/ServerKick.cpp\
				src/server/commands/ServerMode.cpp\
				src/server/commands/ServerNick.cpp\
				src/server/commands/ServerPrivmsg.cpp\
				src/server/commands/ServerTopic.cpp\
				src/message/Message.cpp\
				src/response/Response.cpp\
				src/utils/printcrlf.cpp\

REMOVE		= 	rm -f ${OBJS}

OBJDIR		= 	objs
OBJS		= 	$(patsubst %.cpp,${OBJDIR}/%.o,${SRCS})

RED_DOT		= 	"\033[31m[●]\033[0m"
GREEN_DOT	= 	"\033[32m[●]\033[0m"
YELLOW_DOT	= 	"\033[33m[●]\033[0m"

SPINNER		= 	for s in "KO" "OK" "KO" "KO" "OK" "KO" "KO" "OK" "KO" "KO" "OK" "KO" "KO" "OK" "KO"; do \
					printf "\r\033[33m[$$s]\033[0m $$MSG..."; \
					sleep 0.05; \
				done;

all: ${NAME}

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@${C++} ${STD_FLAGS} -c $< -o $@

${NAME}: ${OBJS}
	@MSG="building program"; $(SPINNER)
	@${C++} ${SRCS} ${STD_FLAGS} -o ${NAME}
	@printf "\r\033[32m[OK]\033[0m program compiled       \n"

clean:
	@MSG="removing object files"; $(SPINNER)
	@${REMOVE}
	@rm -rf ${OBJDIR}
	@printf "\r\033[32m[OK]\033[0m removed object files         \n"

fclean: clean
	@MSG="removing executable files"; $(SPINNER)
	@rm -f ${NAME}
	@rm -rf ${OBJDIR}
	@printf "\r\033[32m[OK]\033[0m removed executables                \n"
	@rm -f ircserv.dSYM
				
re:	fclean all

run: all
	./ircserv
	
.PHONY:	all clean fclean re
