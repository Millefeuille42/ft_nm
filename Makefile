######### Sources #########

SOURCES	= 		pkg/ft_string/ft_string_copy.c\
                pkg/ft_string/ft_string_concat.c\
                pkg/ft_string/get_after_n_sep.c\
                pkg/ft_string/ft_strcmp.c\
                pkg/ft_string/ft_to_lower.c\
                pkg/ft_string/ft_strlen.c\
                pkg/ft_string/ft_is_alnum.c\
                pkg/ft_string/ft_split.c\
                pkg/ft_string/ft_is_alpha.c\
                pkg/ft_string/string.c\
                pkg/ft_memory/zeroed_malloc.c\
                pkg/ft_memory/ft_bzero.c\
                pkg/ft_memory/del_array.c\
                pkg/ft_memory/safe_free.c\
                cmd/main.c\


HEADERS	=		 cmd/ft_nm.h\
                 pkg/ft_string/ft_string.h\
                 pkg/ft_memory/ft_memory.h\


HEADERS_DIRECTORIES	=	cmd \
						pkg \

######### Details #########

NAME	=	ft_nm
SOURCES_EXTENSION = c

######### Compilation #########

COMPILE		=	clang
DELETE		=	rm -f

FLAGS		=	-Wall -Werror -Wextra -pedantic -Ofast

######### Additional Paths #########

vpath	%.h $(HEADERS_DIRECTORIES)

# ################################### #
#        DO NOT ALTER FURTHER!        #
# ################################### #

######### Additional Paths #########

vpath	%.o $(OBJS_DIR)
vpath	%.d $(DEPS_DIR)

######### Implicit Macros #########

OBJS_DIR	= .objs/
DEPS_DIR	= .deps/

OBJS	=	$(addprefix $(OBJS_DIR), $(SOURCES:.$(SOURCES_EXTENSION)=.o))
DEPS	=	$(addprefix $(DEPS_DIR), $(SOURCES:.$(SOURCES_EXTENSION)=.d))

#########  Rules  #########

ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

all:	$(OBJS_DIR) $(DEPS_DIR) $(NAME) ## Compile project and dependencies

$(NAME):	$(OBJS) ## Compile project
			$(COMPILE) $(FLAGS) $^ -o $@

clean: clean_deps clean_objs ## Delete object files

fclean:	clean clean_bin ## Delete object files and binary

re:	fclean ## Delete object files and binary, then recompile all
			@make --no-print-directory all

help: ## Print this help
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z_-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

#########  Sub Rules  #########

objs: $(OBJS_DIR) $(DEPS_DIR) $(OBJS) ## Compile object files

clean_deps: ## Delete dependency files and directory
			$(DELETE) -r $(DEPS_DIR)
clean_objs: ## Delete object files and directory
			$(DELETE) -r $(OBJS_DIR)
clean_bin: ## Delete produced binary
			$(DELETE) $(NAME)

#########  Implicit Rules  #########

$(OBJS_DIR):
					@mkdir -p $(OBJS_DIR)

$(DEPS_DIR):
					@mkdir -p $(DEPS_DIR)

$(OBJS_DIR)%.o:	%.$(SOURCES_EXTENSION)
			mkdir -p $(OBJS_DIR)$(dir $<)
			@mkdir -p $(DEPS_DIR)$(dir $<)
			$(COMPILE) $(FLAGS) -MMD -MP -MF $(DEPS_DIR)$*.d -c $< -o $@

.PHONY	:	all clean fclean re help

#########  Includes  #########

-include $(DEPS)
