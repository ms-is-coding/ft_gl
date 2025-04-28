/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   beacon.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smamalig <smamalig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 22:53:41 by smamalig          #+#    #+#             */
/*   Updated: 2025/04/27 21:06:57 by smamalig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft_gl.h"

#include <unistd.h>
#include <stdint.h>

typedef struct {
	float x, y, z;
} t_vec3;

t_vec3 map[] = {
	{-2, 1, -2}, {-1, 1, -2}, {0, 1, -2}, {1, 1, -2}, {2, 1, -2},
	{-2, 1, -1}, {-1, 1, -1}, {0, 1, -1}, {1, 1, -1}, {2, 1, -1},
	{-2, 1,  0}, {-1, 1,  0}, {0, 1,  0}, {1, 1,  0}, {2, 1,  0},
	{-2, 1,  1}, {-1, 1,  1}, {0, 1,  1}, {1, 1,  1}, {2, 1,  1},
	{-2, 1,  2}, {-1, 1,  2}, {0, 1,  2}, {1, 1,  2}, {2, 1,  2},

	{0, 0, -1}, {-1, 0, -1}, {1, 0, -1},
	{0, 0,  0}, {-1, 0,  0}, {1, 0,  0},
	{0, 0,  1}, {-1, 0,  1}, {1, 0,  1},

	{0, -1,  0}
};

t_vec3 vertex_offsets[] = {
	{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1},
	{0, 1, 0}, {1, 1, 0}, {1, 1, 1}, {0, 1, 1},
};


int	ft_abs(int x)
{
	if (x < 0)
		return -x;
	return x;
}

t_vec3 project(t_vec3 v, float d, float scale) {
	float factor = scale / (v.z + d);
	t_vec3 p = {v.x * factor, v.y * factor, 0};
	return p;
}

void ft_line(t_ft_gl *gl_ptr, int x0, int y0, int x1, int y1)
{
	int dx = ft_abs(x1 - x0);
	int dy = ft_abs(y1 - y0);
	int sx = (x0 < x1) ? 1 : -1;
	int sy = (y0 < y1) ? 1 : -1;
	int err = dx - dy;

	while (x0 != x1 || y0 != y1)
	{
		if (x0 >= 0 && x0 < gl_ptr->width && y0 >= 0 && y0 < gl_ptr->height) {
			uint8_t r = x0 / (float)gl_ptr->width * 255.;
			uint8_t g = y0 / (float)gl_ptr->height * 255.;
			uint8_t b = 128.;
			uint32_t color = (r << 16) | (g << 8) | b;
			ft_gl_pixel_put(gl_ptr, x0, y0, color);
		}
		int e2 = 2 * err;
		if (e2 > -dy) { err -= dy; x0 += sx; }
		if (e2 < dx)  { err += dx; y0 += sy; }
	}
}

void draw_cube(t_ft_gl *gl_ptr, t_vec3 *vertices, float d, float scale, int x_offset, int y_offset) {
	int edges[12][2] = {
		{0, 1}, {1, 2}, {2, 3}, {3, 0},
		{4, 5}, {5, 6}, {6, 7}, {7, 4},
		{0, 4}, {1, 5}, {2, 6}, {3, 7},
	};

	for (int i = 0; i < 12; i++) {
		t_vec3 p1 = project(vertices[edges[i][0]], d, scale);
		t_vec3 p2 = project(vertices[edges[i][1]], d, scale);
		ft_line(gl_ptr, p1.x + x_offset, p1.y + y_offset, p2.x + x_offset, p2.y + y_offset);
	}
}

#include <math.h>

t_vec3 rotate_x(t_vec3 v, float angle) {
	float cos_a = cosf(angle);
	float sin_a = sinf(angle);
	return (t_vec3){ v.x, v.y * cos_a - v.z * sin_a, v.y * sin_a + v.z * cos_a };
}

t_vec3 rotate_y(t_vec3 v, float angle) {
	float cos_a = cosf(angle);
	float sin_a = sinf(angle);
	return (t_vec3){ v.x * cos_a + v.z * sin_a, v.y, -v.x * sin_a + v.z * cos_a };
}
t_vec3 rotate_z(t_vec3 v, float angle) {
	float cos_a = cosf(angle);
	float sin_a = sinf(angle);
	return (t_vec3){ v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a, v.z };
}

void transform_vertices(t_vec3* vert, int index, float cube_size)
{
	for (int i = 0; i < 8; i++) {
		vert[i].x = (map[index].x + vertex_offsets[i].x) * cube_size;
		vert[i].y = (map[index].y + vertex_offsets[i].y) * cube_size;
		vert[i].z = (map[index].z + vertex_offsets[i].z) * cube_size;
	}
}

#define MAP_SIZE (int)(sizeof(map) / sizeof(map[0]))

static struct t_camera {
	float theta_x;
	float theta_y;
	float theta_z;
} camera;

#include <termios.h>
#include <X11/keysym.h>

static t_vec3 vert[MAP_SIZE][8];

void	key_hook(t_ft_gl *gl_ptr, int key)
{
	(void)gl_ptr;
	float rot_x = 0;
	float rot_y = 0;
	float rot_z = 0;

	if (key == XK_Up)
		rot_x = -0.1;
	if (key == XK_Down)
		rot_x = 0.1;
	if (key == XK_Left)
		rot_y = 0.1;
	if (key == XK_Right)
		rot_y = -0.1;
	if (key == 'q')
		rot_z = 0.1;
	if (key == 'e')
		rot_z = -0.1;

	for (int i = 0; i < MAP_SIZE; i++) {
		for (int j = 0; j < 8; j++) {
			vert[i][j] = rotate_x(vert[i][j], rot_x);
			vert[i][j] = rotate_y(vert[i][j], rot_y);
			vert[i][j] = rotate_z(vert[i][j], rot_z);
		}
	}
}

void	render(t_ft_gl *gl_ptr)
{
	usleep(20000);
	ft_gl_clear(gl_ptr);
	for (int i = 0; i < MAP_SIZE; i++) {
		draw_cube(gl_ptr, vert[i], 10., 64., gl_ptr->width/2, gl_ptr->height/2);
	}
	ft_gl_flush_buffer(gl_ptr);
}

int	main(void)
{
	(void)camera;
	t_ft_gl	*gl_ptr = ft_gl_init();
	ft_gl_clear(gl_ptr);
	for (int i = 0; i < MAP_SIZE; i++)
	{
		transform_vertices(vert[i], i, 2.);
	}
	ft_gl_key_hook(gl_ptr, key_hook);
	ft_gl_loop_hook(gl_ptr, render);
	ft_gl_loop(gl_ptr);
	ft_gl_clear(gl_ptr);
	ft_gl_flush_buffer(gl_ptr);
	ft_gl_destroy(gl_ptr);
	return (0);
}
