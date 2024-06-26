/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahn <ahn@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 15:34:10 by moson             #+#    #+#             */
/*   Updated: 2024/05/09 01:30:41 by ahn              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <stdio.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <signal.h>

# define FT_TRUE 1
# define FT_FALSE 0

# define PIPE_READ 0
# define PIPE_WRITE 1

# define SIGNAL_OFFSET 128

// Exit Status
# define SUCCESS 0
# define ERR_UNKNOWN 1
# define ERR_MALLOC_FAILED 2
# define ERR_PIPE_FAILED 3
# define ERR_CLOSE_FAILED 4
# define ERR_FORK_FAILED 5
# define ERR_OPEN_FAILED 6
# define ERR_DUP2_FAILED 7
# define ERR_FILE_NOT_EXIST 8
# define ERR_PERM_DENIED 9
# define ERR_CMD_NOT_FOUND 10
# define ERR_EXECVE_FAILED 11
# define ERR_HEREDOC 12
# define ERR_STAT_FAILED 13
# define ERR_EXEC_DIR 14
# define ERR_SYNTAX 256
# define ERR_QUOTE 257

// Error Message
# define MSG_UNKNOWN "unknown error"
# define MSG_MALLOC_FAILED "malloc failed"
# define MSG_PIPE_FAILED "pipe failed"
# define MSG_CLOSE_FAILED "close failed"
# define MSG_FORK_FAILED "fork failed"
# define MSG_OPEN_FAILED "open failed"
# define MSG_DUP2_FAILED "dup2 failed"
# define MSG_FILE_NOT_EXIST "no such file or directory"
# define MSG_PERM_DENIED "permission denied"
# define MSG_CMD_NOT_FOUND "command not found"
# define MSG_EXECVE_FAILED "execve failed"
# define MSG_HEREDOC "heredoc failed"
# define MSG_STAT_FAILED "stat failed"
# define MSG_EXEC_DIR "is a directory"
# define MSG_SYNTAX "syntax error"
# define MSG_QUOTE "quote error"

# define MSG_UNDEFINED "undefined error"

enum		e_quote_type
{
	SINGLE = 0,
	DOUBLE = 1
};

enum		e_redir_type
{
	I_REDIR = 0,
	O_REDIR = 1,
	A_REDIR = 2,
	HEREDOC = 3
};

//	Extend e_redir_type
enum		e_token_type
{
	PIPE = 4,
	CMD = 5
};

enum		e_fd_rw
{
	READ_FD = 0,
	WRITE_FD = 1
};

enum		e_fd_type
{
	FDTYPE_STD = 0,
	FDTYPE_PIPE = 1
};

//	(void *)list->content
//	type = e_token_type
typedef struct s_token
{
	char	*value;
	int		type;
}			t_token;

//	(void *)list->content
//	default_fdtype = e_fd_type
//	After parse, absolute_path is NULL
typedef struct s_proc
{
	int		default_fdtype[2];
	t_list	*redir_list;
	char	*absolute_path;
	char	**args;
}			t_proc;

//	(void *)list->content
//	redir_type = e_redir_type
typedef struct s_redir
{
	int		redir_type;
	char	*filename;
}			t_redir;

//	(void *)list->content
typedef struct s_env
{
	char	*key;
	char	*value;
}			t_env;

typedef struct s_sh_data
{
	t_list	*env_list;
	int		proc_size;
	int		**fd_pipe;
	int		*child_pid;
	int		*exit_status;
	int		fd_std[3];
	t_list	*hdfile_list;
}			t_sh_data;

t_token		*ft_new_token(void);
void		ft_del_token(void *token);
t_token		*wrap_in_token(void *content);
void		set_token_type(t_token *token);
int			get_token_type(char *str);

t_env		*ft_new_env(void);
void		ft_del_env(void *env);
char		*get_env_value(t_list *env_list, char *key);

t_list		*convert_envp_to_env_list(char **envp);

t_list		*generate_proc_list(t_list *token_list);

t_list		*tokenizer(char *input);
t_list		*split_include_separator(char *s, const char *sep);
t_list		*split_exclude_separator(char *s);
int			is_in_quote(char c, int in_quote[2]);
void		add_sep_in_list(t_list *list, const char *sep, int *i);
void		add_word_in_list(t_list *list, char *s, int i, int *start);

int			check_quote(char *input);
int			check_syntax(t_list *token_list);

char		*expand_string(t_list *env_list, char *str);
void		expand_string_iter(t_list *token_list, t_list *env_list,
				char *(*expand_string)(t_list *, char *), void (*del)(void *));

t_redir		*ft_new_redir(void);
t_redir		*ft_new_redir_init(char *filename, int redir_type);
void		ft_del_redir(void *content);
int			get_redir_type(char *value);
int			is_redirection(int type);

t_proc		*ft_new_proc(void);
void		ft_del_proc(void *content);

t_list		*parser(char *input, t_list *env_list);

int			set_hdfile_list(t_list *proc_list, t_list *hdfile_list);

int			heredoc(t_list *proc_list, t_sh_data *sh_data);
void		replace_hdfilename(t_list *proc_list, t_list *hdfile_list);
void		heredoc_clear(t_list *hdfile_list);

void		display_new_prompt(int signum);
int			prt_err(int exit_code, char *target);
void		exit_wrapper(int exit_code, char *target);

void		prt_err_entrance(int exit_code);

int			set_proc_sh_data(t_sh_data *sh_data, t_list *proc_list);
void		clear_proc_sh_data(t_sh_data *sh_data);
int			executor(t_sh_data *sh_data, t_list *proc_list);

t_sh_data	*ft_new_sh_data(char **envp);

void		init_shell(char **argv, char **envp, t_sh_data **sh_data);
void		print_newline_if_signaled(t_sh_data *sh_data);
void		clear_after_input(char *str, t_sh_data *sh_data, t_list *proc_list);
void		clear_shell(t_sh_data *sh_data);

#endif
