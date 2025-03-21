/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smamalig <smamalig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 22:53:41 by smamalig          #+#    #+#             */
/*   Updated: 2025/03/21 09:38:35 by smamalig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft_gl.h"

#include <unistd.h>
#include <stdint.h>

#include "../../so_long/libmlx/mlx.h"

int	ft_abs(int x)
{
	if (x < 0)
		return -x;
	return x;
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

int	main()
{
	t_ft_gl	*gl_ptr = ft_gl_init();
	ft_gl_clear(gl_ptr);

	ft_line(gl_ptr, 00, 16, 32, 32);
	ft_line(gl_ptr, 32, 32, 64, 16);
	ft_line(gl_ptr, 64, 16, 32, 00);
	ft_line(gl_ptr, 32, 00, 00, 16);
	ft_line(gl_ptr, 00, 16, 00, 48);
	ft_line(gl_ptr, 00, 48, 32, 64);
	ft_line(gl_ptr, 32, 64, 64, 48);
	ft_line(gl_ptr, 64, 48, 64, 16);
	ft_line(gl_ptr, 32, 32, 32, 64);

	ft_gl_text(gl_ptr, "Hello, World!", 20, 16, 0);

	ft_gl_loop(gl_ptr);
	ft_gl_destroy(gl_ptr);
	return (0);
}
