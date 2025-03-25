/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smamalig <smamalig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 22:44:23 by smamalig          #+#    #+#             */
/*   Updated: 2025/03/22 16:48:53 by smamalig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft_gl.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>

t_ft_gl	*ft_gl_init()
{
	t_ft_gl	*gl_ptr = malloc(sizeof(t_ft_gl));

	struct winsize t;
	ioctl(0, TIOCGWINSZ, &t);

	gl_ptr->real_width = t.ws_col;
	gl_ptr->real_height = t.ws_row;
	gl_ptr->width = t.ws_col * 2;
	gl_ptr->height = t.ws_row * 4;
	gl_ptr->color = 0xffffff;
	gl_ptr->current_x = 0;
	gl_ptr->current_y = 0;
	gl_ptr->pixel_buffer = calloc(t.ws_col * t.ws_row, sizeof(char));
	gl_ptr->color_buffer = calloc(t.ws_col * t.ws_row, sizeof(int));
	write(1, "\e[?25l", 6);
	return (gl_ptr);
}

int	ft_gl_loop(t_ft_gl *gl_ptr)
{
	(void)gl_ptr;
	while (42)
		;
	return (0);
}

void	ft_gl_destroy(t_ft_gl *gl_ptr)
{
	free(gl_ptr->pixel_buffer);
	free(gl_ptr->color_buffer);
	free(gl_ptr);
}

int	ft_gl_clear(t_ft_gl *gl_ptr)
{
	memset(gl_ptr->pixel_buffer, 0, gl_ptr->real_width * gl_ptr->real_height);
	memset(gl_ptr->color_buffer, 0, gl_ptr->real_width * gl_ptr->real_height * sizeof(int));
	write(1, "\e[3J\e[2J", 8);

	return (0);
}

static inline uint8_t lerp(uint8_t old_c, uint8_t new_c, float t) {
    return new_c * t + old_c * (1. - t);
}

int ft_gl_update_color(t_ft_gl *gl_ptr, int real_x, int real_y, int color) {
    int index = real_y * gl_ptr->real_width + real_x;
    int n = __builtin_popcount(gl_ptr->pixel_buffer[index]);

    if (n == 1) {
        gl_ptr->color_buffer[index] = color;
        return 0;
    }

    uint32_t old_color = gl_ptr->color_buffer[index];

    uint8_t r = lerp((old_color >> 16) & 0xFF, (color >> 16) & 0xFF, 1. / n);
    uint8_t g = lerp((old_color >> 8) & 0xFF, (color >> 8) & 0xFF, 1. / n);
    uint8_t b = lerp(old_color & 0xFF, color & 0xFF, n);

    gl_ptr->color_buffer[index] = (r << 16) | (g << 8) | b;
    return 0;
}

void	print_braille(int x)
{
	int	utf8;

	utf8 = 0x80A0E2;
	utf8 |= (x & 0x3F) << 16;
	utf8 |= (x & 0xC0) << 2;
	write(1, (char *)&utf8, 3);
}

void	ft_put_color(int r, int g, int b)
{
	write(1, "\e[38;2;", 7);
	char buf[10];
	int i = 9;
	do {
		buf[i--] = '0' + r % 10;
		r /= 10;
	} while (r);
	write(1, buf + i + 1, 9 - i);
	write(1, ";", 1);
	i = 9;
	do {
		buf[i--] = '0' + g % 10;
		g /= 10;
	} while(g);
	write(1, buf + i + 1, 9 - i);
	write(1, ";", 1);
	i = 9;
	do {
		buf[i--] = '0' + b % 10;
		b /= 10;
	} while (b);
	write(1, buf + i + 1, 9 - i);
	write(1, "m", 1);
}

void	ft_put_position(int x, int y)
{
	write(1, "\e[", 2);
	char buf[10];
	int i = 9;
	while (y) {
		buf[i--] = '0' + y % 10;
		y /= 10;
	}
	write(1, buf + i + 1, 9 - i);
	write(1, ";", 1);
	i = 9;
	while (x) {
		buf[i--] = '0' + x % 10;
		x /= 10;
	}
	write(1, buf + i + 1, 9 - i);
	write(1, "H", 1);
}

int	ft_gl_update_pixel(t_ft_gl *gl_ptr, int real_x, int real_y)
{
	int index = real_y * gl_ptr->real_width + real_x;
	if (real_y != gl_ptr->current_y || real_x != gl_ptr->current_x)
	{
		ft_put_position(real_x + 1, real_y + 1);
	}
	gl_ptr->current_x = ++real_x;
	gl_ptr->current_y = real_y++;
	if (gl_ptr->current_x == gl_ptr->real_width)
	{
		gl_ptr->current_y++;
		gl_ptr->current_x = 0;
	}
	int	color = gl_ptr->color_buffer[index];
	if (color != gl_ptr->color)
		ft_put_color((color & 0xff0000) >> 16, (color & 0xff00) >> 8, color & 0xff);
	gl_ptr->color = color;
	print_braille(gl_ptr->pixel_buffer[index]);
	return (0);
}

int braille_bitmask(int x, int y) {
	static const uint8_t dot_map[4][2] = {
		{1 << 0, 1 << 3},
		{1 << 1, 1 << 4},
		{1 << 2, 1 << 5},
		{1 << 6, 1 << 7}
	};
	if (y < 4 && x < 2)
		return dot_map[y][x];
	return 0;
}

void	ft_put_char(char c)
{
	write(1, &c, 1);
}

int	ft_gl_text(t_ft_gl *gl_ptr, char *text, int x, int y, int anchor)
{
	int	real_x = x >> 1;
	int real_y = y >> 2;

	(void)anchor;
	ft_put_color(255, 255, 255);
	ft_put_position(real_x + 1, real_y + 1);
	while (*text) {
		gl_ptr->pixel_buffer[real_y * gl_ptr->real_width + real_x] = 0;
		ft_put_char(*text++);
	}
	ft_put_position(1, 1);
	return (0);
}

int	ft_gl_pixel_put(t_ft_gl *gl_ptr, int x, int y, int color)
{

	if (x < 0 || x >= gl_ptr->width || y < 0 || y >= gl_ptr->height)
		return (1);
	int	real_x = x >> 1;
	int real_y = y >> 2;
	gl_ptr->pixel_buffer[real_y * gl_ptr->real_width + real_x] |= braille_bitmask(x & 1, y & 3);
	ft_gl_update_color(gl_ptr, real_x, real_y, color);
	ft_gl_update_pixel(gl_ptr, real_x, real_y);
	return (0);
}
