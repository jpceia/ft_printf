/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpceia <jpceia@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/15 22:52:33 by jpceia            #+#    #+#             */
/*   Updated: 2021/03/29 13:30:57 by jpceia           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int ft_contains(char c, char const *charset)
{
	while (*charset)
		if (c == *charset++)
			return (1);
	return (0);
}

void init_spec(t_spec *spec)
{
	spec->type = 0;
	spec->minus = 0;
	spec->width = 0;
	spec->width_star = 0;
	spec->dot = 0;
	spec->precision = 6;
	spec->precision_star = 0;
}

int parse_spec(const char *fmt, t_spec *spec)
{
	int index;

	if (fmt[0] != '%')
		return (-1);
	index = 1;
	while (fmt[index])
	{
		if (ft_contains(fmt[index], "cspdiuxX%%"))
		{
			spec->type = fmt[index];
			return (index);
		}
		else if (fmt[index] == '-')
			spec->minus = 1;
		else if (fmt[index] == '.')
			spec->dot = 1;
		else if (ft_isdigit(fmt[index]) && spec->dot)
			spec->precision = spec->precision * 10 + (fmt[index] - '0');
		else if (ft_isdigit(fmt[index]) && !spec->dot)
			spec->width = spec->width * 10 + (fmt[index] - '0');
		else if (fmt[index] == '*' && spec->dot)
			spec->precision_star = 1;
		else if (fmt[index] == '*' && !spec->dot)
			spec->width_star = 1;
		else
			return (-1);
		index++;
	}
	return (-1);
}

int adjust_width(char **s, t_spec *spec)
{
	char *holder;
	int n_chars;
	int padding;

	n_chars = ft_strlen(*s);
	padding = 0;
	if (spec->width > n_chars && spec->type != '%')
	{
		holder = *s;
		*s = malloc(spec->width);
		ft_memset(*s, ' ', spec->width);
		if (!spec->minus)
			padding = spec->width - n_chars;
		ft_memcpy(*s + padding, holder, n_chars);
		free(holder);
	}
	return (1);
}

int print_arg(va_list *args, t_spec *spec)
{
	int n_chars;
	char *s;

	if (spec->type == 'c')
	{
		if (!(s = malloc(1)))
			return (-1);
		s[0] = (char)va_arg(*args, int);
	}
	else if (spec->type == 's')
	{
		s = va_arg(*args, char *);
		s = ft_strdup(s ? s : "(null)");
	}
	else if (ft_contains(spec->type, "di"))
		s = ft_lltoa(va_arg(*args, int));
	else if (ft_contains(spec->type, "u"))
		s = ft_lltoa(va_arg(*args, unsigned int));
	else if (spec->type == 'x')
		s = ft_lltoa_base(va_arg(*args, int), "0123456789abcdef");
	else if (spec->type == 'X')
		s = ft_lltoa_base(va_arg(*args, int), "0123456789ABCDEF");
	else if (spec->type == 'p')
		s = ft_ptrtoa(va_arg(*args, void *));
	else if (spec->type == '%')
		s = ft_strdup("\%");
	else
		return (-1);
	adjust_width(&s, spec); // width, zero, minus
	n_chars = ft_strlen(s);
	ft_putstr_fd(s, 1);
	free(s);
	return (n_chars);
}

int parse_spec_width(va_list *args, t_spec *spec)
{
	if (spec->width_star)
		spec->width = va_arg(*args, int);
	return (0);
}

int parse_spec_precision(va_list *args, t_spec *spec)
{
	if (spec->precision_star)
		spec->precision = va_arg(*args, int);
	return (0);
}

int parse_spec_print_arg(const char **fmt, va_list *args)
{
	int index;
	t_spec spec;

	if (**fmt != '%')
		return (-1);
	init_spec(&spec);
	index = parse_spec(*fmt, &spec);
	*fmt = *fmt + 1;
	parse_spec_width(args, &spec);
	parse_spec_precision(args, &spec);
	if (index < 0)
		return (-1);
	*fmt = *fmt + index;
	return (print_arg(args, &spec));
}

int ft_printf(const char *fmt, ...)
{
	va_list args;
	int r;
	int ret;

	va_start(args, fmt);
	ret = 0;
	while (*fmt)
	{
		r = 1;
		if (*fmt == '%')
			r = parse_spec_print_arg(&fmt, &args);
		else
			ft_putchar_fd(*(fmt++), 1);
		if (r < 0)
			return (-1);
		ret += r;
	}
	va_end(args);
	return (ret);
}