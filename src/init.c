/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smamalig <smamalig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 22:44:23 by smamalig          #+#    #+#             */
/*   Updated: 2025/05/05 09:37:46 by smamalig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft_gl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>
#include <X11/keysym.h>

// avoid strlen calls by providing length
void	buf_write(char *buffer, int *pos, const char *s, int len)
{
	while (len--)
		buffer[(*pos)++] = *s++;
}

void set_conio_terminal_mode() {
	struct termios new_settings;
	tcgetattr(0, &new_settings);
	new_settings.c_lflag &= ~ICANON; // Disable canonical mode
	new_settings.c_lflag &= ~ECHO;   // Disable echo
	tcsetattr(0, TCSANOW, &new_settings);
}

t_ft_gl	*ft_gl_init()
{
	t_ft_gl	*gl_ptr = malloc(sizeof(t_ft_gl));

	struct winsize t;
	ioctl(0, TIOCGWINSZ, &t);

	gl_ptr->real_width = t.ws_col;
	gl_ptr->real_height = t.ws_row;
	gl_ptr->width = t.ws_col * 2;
	gl_ptr->height = t.ws_row * 4;
	gl_ptr->render_pos = 0;
	gl_ptr->pixel_buffer = calloc(t.ws_col * t.ws_row, sizeof(char));
	gl_ptr->color_buffer = calloc(t.ws_col * t.ws_row, sizeof(int));
	gl_ptr->render_buffer = calloc(8 * t.ws_col * t.ws_row, sizeof(char));
	write(1, "\e[?25l", 6);
	set_conio_terminal_mode();
	return (gl_ptr);
}

void reset_terminal_mode() {
	struct termios new_settings;
	tcgetattr(0, &new_settings);
	new_settings.c_lflag |= ICANON;  // Enable canonical mode
	new_settings.c_lflag |= ECHO;    // Enable echo
	tcsetattr(0, TCSANOW, &new_settings);
}

int kbhit() {
	struct termios oldt, newt;
	int oldf;
	tcgetattr(0, &oldt);
	newt = oldt;
	newt.c_lflag &= ~ICANON;
	newt.c_lflag &= ~ECHO;
	tcsetattr(0, TCSANOW, &newt);
	oldf = fcntl(0, F_GETFL, 0);
	fcntl(0, F_SETFL, oldf | O_NONBLOCK);

	int ch = getchar();

	tcsetattr(0, TCSANOW, &oldt);
	fcntl(0, F_SETFL, oldf);

	if(ch != EOF) {
		ungetc(ch, stdin);
		return 1;
	}

	return 0;
}

int	ft_gl_loop(t_ft_gl *gl_ptr)
{
	if (!gl_ptr || !gl_ptr->loop_hook)
		return (1);
	while (1)
	{
		if (gl_ptr->key_hook && kbhit()) {
			char ch = getchar();
			if (ch == '\x04')
				break;
			if (ch == '\e' && getchar() == '[') {
				char arrow = getchar();
				if (arrow == 'A')
					gl_ptr->key_hook(XK_Up, gl_ptr->key_hook_param);
				else if (arrow == 'B')
					gl_ptr->key_hook(XK_Down, gl_ptr->key_hook_param);
				else if (arrow == 'C')
					gl_ptr->key_hook(XK_Right, gl_ptr->key_hook_param);
				else if (arrow == 'D')
					gl_ptr->key_hook(XK_Left, gl_ptr->key_hook_param);
				else
					goto end;
			}
			else 
				gl_ptr->key_hook(ch, gl_ptr->key_hook_param);
		}
		gl_ptr->loop_hook(gl_ptr->loop_hook_param);
	}
end:
	return (0);
}

void	ft_gl_destroy(t_ft_gl *gl_ptr)
{
	free(gl_ptr->pixel_buffer);
	free(gl_ptr->color_buffer);
	write(1, "\e[?25h", 6);
	reset_terminal_mode();
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

void	print_braille(char *buffer, int *pos, int x)
{
	int	utf8;

	utf8 = 0x80A0E2;
	utf8 |= (x & 0x3F) << 16;
	utf8 |= (x & 0xC0) << 2;
	buf_write(buffer, pos, (char *)&utf8, 3);
}

void	ft_put_color(char *buffer, int *pos, int r, int g, int b)
{
	buf_write(buffer, pos, "\e[38;2;", 7);
	char buf[10];
	int i = 9;
	do {
		buf[i--] = '0' + r % 10;
		r /= 10;
	} while (r);
	buf_write(buffer, pos, buf + i + 1, 9 - i);
	buf_write(buffer, pos, ";", 1);
	i = 9;
	do {
		buf[i--] = '0' + g % 10;
		g /= 10;
	} while(g);
	buf_write(buffer, pos, buf + i + 1, 9 - i);
	buf_write(buffer, pos, ";", 1);
	i = 9;
	do {
		buf[i--] = '0' + b % 10;
		b /= 10;
	} while (b);
	buf_write(buffer, pos, buf + i + 1, 9 - i);
	buf_write(buffer, pos, "m", 1);
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
	while (*text) {
		gl_ptr->pixel_buffer[real_y * gl_ptr->real_width + real_x] = 0;
		ft_put_char(*text++);
	}
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
	return (0);
}

void	ft_gl_swap_buffers(t_ft_gl *gl_ptr)
{
	char	buffer[gl_ptr->real_width * gl_ptr->real_height * 8];
	int		pos;
	
	buf_write(buffer, &pos, "\e[1;1H", 6);
	
	pos = 0;
	for (int j = 0; j < gl_ptr->real_height; j++)
	{
		for (int i = 0; i < gl_ptr->real_width; i++)
		{
			int color = gl_ptr->color_buffer[j * gl_ptr->real_width + i];
			int pixel = gl_ptr->pixel_buffer[j * gl_ptr->real_width + i];

			if (!pixel)
			{
				buf_write(buffer, &pos, " ", 1);
				continue;
			}
			
			ft_put_color(buffer, &pos, color >> 16, (color >> 8) & 0xff, color & 0xff);
			print_braille(buffer, &pos, pixel);
		}
		// buf_write(buffer, &pos, "\n", 1);
	}
	write(1, buffer, pos);
}
