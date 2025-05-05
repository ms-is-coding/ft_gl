/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft_gl.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smamalig <smamalig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 22:38:52 by smamalig          #+#    #+#             */
/*   Updated: 2025/05/05 09:40:51 by smamalig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_GL_H
# define LIBFT_GL_H



# define LIBFT_GL_EXPOSE_TERMINAL_API
# ifdef LIBFT_GL_EXPOSE_TERMINAL_API

#  include <stdint.h>

typedef struct s_ft_gl
{
	int			real_width;
	int			real_height;
	int			width;
	int			height;
	int			render_pos;
	uint8_t		*pixel_buffer;
	uint32_t	*color_buffer;
	char		*render_buffer;
	void		*loop_hook_param;
	void		*key_hook_param;
	void		(*loop_hook)(void *param);
	void		(*key_hook)(int keysym, void *param);
}	t_ft_gl;

typedef void (*t_loop_fn)(void *);
typedef void (*t_key_fn)(int, void *);

t_ft_gl		*ft_gl_init();
void		ft_gl_destroy(t_ft_gl *gl_ptr);
void		ft_gl_flush_buffer(t_ft_gl *gl_ptr);
void		ft_gl_swap_buffers(t_ft_gl *gl_ptr);
int			ft_gl_clear(t_ft_gl *gl_ptr);
int			ft_gl_loop(t_ft_gl *gl_ptr);
int			ft_gl_pixel_put(t_ft_gl *gl_ptr, int x, int y, int color);
int			ft_gl_text(t_ft_gl *gl_ptr, char *text, int x, int y, int anchor);

void		ft_gl_loop_hook(t_ft_gl *gl_ptr, void (*func)(void *), void *param);
void		ft_gl_key_hook(t_ft_gl *gl_ptr, void (*func)(int, void *), void *param);

# endif

#endif
