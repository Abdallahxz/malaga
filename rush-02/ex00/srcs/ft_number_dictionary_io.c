/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_number_dictionary_io.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aassaad- <aassaad-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/17 21:48:49 by aassaad-          #+#    #+#             */
/*   Updated: 2024/03/17 22:14:35 by aassaad-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_array.h"
#include "ft_boolean.h"
#include "ft_file_utils.h"
#include "ft_is.h"
#include "ft_number_dictionary.h"
#include "ft_split.h"
#include "ft_str.h"
#include "ft_to.h"
#include <fcntl.h>
#include <ft_short_types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void	ft_read_line(t_dict_entry *entry, int fd, t_parse_error *error)
{
	char	buffer[SIZE_1B];
	char	*line;
	UINT	byte_read;
	UINT	total;

	line = NULL;
	total = 0;
	while ((byte_read = read(fd, buffer, SIZE_1B)) > 0)
	{
		if (byte_read == (UINT)-1)
		{
			entry->str = NULL;
			*error = failed;
			break ;
		}
		line = ft_extend_array(line, buffer, total, total + byte_read);
		total += byte_read;
		if (buffer[0] == '\n')
		{
			*error = ft_process_line(entry, line, total);
			byte_read = (UINT)-2;
			break ;
		}
	}
	if (byte_read == 0)
		*error = reached_eof;
	free(line);
}

char	*ft_clean_line(char *str)
{
	int		size;
	char	**split;

	split = ft_split_whitespace(str);
	free(str);
	size = 0;
	while (split[size])
		size++;
	return (ft_str_join(size, split, " "));
}

t_parse_error	ft_process_line(t_dict_entry *entry, char *line, UINT length)
{
	UINT	index;
	char	*number;

	index = 0;
	while (ft_is_number(line[index]))
		index++;
	if (line[index] == '\n')
		return (empty_line);
	if (index == 0 || index >= length)
		return (failed);
	number = ft_str_n_duplicate(line, index);
	while (line[index] == ' ')
		index++;
	if (line[index] != ':')
		return (failed);
	index++;
	while (line[index] == ' ')
		index++;
	entry->value = ft_atoi_strict(number);
	entry->str = ft_clean_line(ft_str_n_duplicate(line + index, length - index
				- 1));
	if (ft_str_length(entry->str) == 0)
		return (failed);
	free(number);
	return (parsing_ok);
}

int	ft_count_valid_line(char *path)
{
	int				fd;
	int				count;
	t_dict_entry	*entry;
	t_parse_error	error;

	count = 0;
	fd = ft_open_file(path);
	if (fd < 0)
		return (INVALID);
	while (true)
	{
		error = parsing_ok;
		entry = malloc(sizeof(t_dict_entry));
		if (!entry)
			return (INVALID);
		ft_read_line(entry, fd, &error);
		if (error == failed)
			return (INVALID);
		if (error == parsing_ok)
			count++;
		if (!entry->str || error == reached_eof)
			break ;
		free(entry);
	}
	ft_close_file(fd);
	return (count);
}

t_bool	ft_load_valid_line(char *path, int size, t_dict *dict)
{
	int				fd;
	int				index;
	t_dict_entry	*entry;
	t_parse_error	error;

	index = 0;
	fd = ft_open_file(path);
	if (fd < 0)
		return (false);
	while (index < size)
	{
		error = parsing_ok;
		entry = &dict->entries[index];
		ft_read_line(entry, fd, &error);
		if (error == failed)
			return (false);
		if (error == parsing_ok)
			index++;
		if ((!entry->str || error == reached_eof) && error != empty_line)
			break ;
	}
	ft_close_file(fd);
	return (true);
}
