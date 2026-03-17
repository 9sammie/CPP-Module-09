NAME		=		btc

CXX			=		c++

OBJ_DIR		=		obj

CXXFLAGS	=		-std=c++98 -Wall -Wextra -Werror -MMD -MP

MAKEFLAGS	+=		--no-print-directory -j$(nproc)

################################################################################
###									FILES	 ###
################################################################################

MAKEFILE	=		Makefile

SRC			=		main.cpp BitcoinExchange.cpp

OBJ			=		$(SRC:%.cpp=$(OBJ_DIR)/%.o)

DEP			=		$(OBJ:%.o=%.d)

################################################################################
###									ARCHIVES ###
################################################################################

all: $(NAME)

$(NAME): $(OBJ)
			$(CXX) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: %.cpp $(MAKEFILE) | $(OBJ_DIR)
			$(CXX) $(CFLAGS) -o $@ -c $<

$(OBJ_DIR):
			mkdir -p $(OBJ_DIR)

-include $(DEP)

re: fclean
			$(MAKE) all

fclean: clean
			rm -f $(NAME)

clean: 
			rm -rf $(OBJ_DIR)

.PHONY: all clean fclean re