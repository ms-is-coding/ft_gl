/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smamalig <smamalig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 22:53:41 by smamalig          #+#    #+#             */
/*   Updated: 2025/03/25 20:45:12 by smamalig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft_gl.h"

#include <unistd.h>
#include <stdint.h>

typedef struct {
    float x, y, z;
} t_vec3;

t_vec3 cube_vertices[8] = {
    {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
    {-1, -1,  1}, {1, -1,  1}, {1, 1,  1}, {-1, 1,  1}
};

int	ft_abs(int x)
{
	if (x < 0)
		return -x;
	return x;
}

t_vec3 project(t_vec3 v, float d, float scale) {
    float factor = scale / (v.z + d);
    t_vec3 p = {v.x * factor, v.y * factor, 0}; // z is ignored after projection
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
			uint8_t r = x0 / 64. * 255.;
			uint8_t g = y0 / 64. * 255.;
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
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Back face
        {4, 5}, {5, 6}, {6, 7}, {7, 4}, // Front face
        {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Connecting edges
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

void fill_triangle(t_ft_gl *gl_ptr, int x0, int y0, int x1, int y1, int x2, int y2, int color) {
    // Compute bounding box
    int min_x = fmin(x0, fmin(x1, x2));
    int max_x = fmax(x0, fmax(x1, x2));
    int min_y = fmin(y0, fmin(y1, y2));
    int max_y = fmax(y0, fmax(y1, y2));

    // Precompute denominator for barycentric coordinates
    float denom = (float)((y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2));

    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            float alpha = ((y1 - y2) * (x - x2) + (x2 - x1) * (y - y2)) / denom;
            float beta  = ((y2 - y0) * (x - x2) + (x0 - x2) * (y - y2)) / denom;
            float gamma = 1.0f - alpha - beta;

            // If inside the triangle, update both buffers
            if (alpha >= 0 && beta >= 0 && gamma >= 0) {
                ft_gl_pixel_put(gl_ptr, x, y, color);  // Set pixel in the braille buffer
            }
        }
    }
}

int	main() {
	t_ft_gl	*gl_ptr = ft_gl_init();
	ft_gl_clear(gl_ptr);
	
	ft_gl_text(gl_ptr, "Hello, World!", 20, 16, 0);

	int	iters = 100;
	while (iters-- > 0) {
		usleep(20000);
		for (int i = 0; i < 8; i++) {
			cube_vertices[i] = rotate_x(cube_vertices[i], 0.01);
			cube_vertices[i] = rotate_y(cube_vertices[i], 0.01);
			cube_vertices[i] = rotate_z(cube_vertices[i], 0.01);
		}
		ft_gl_clear(gl_ptr);
		draw_cube(gl_ptr, cube_vertices, 5., 64., 32, 32);
		ft_gl_text(gl_ptr, "Hello, World!", 32, 10, 0);
	}

	// ft_gl_loop(gl_ptr);
	ft_gl_destroy(gl_ptr);
	return (0);
}
