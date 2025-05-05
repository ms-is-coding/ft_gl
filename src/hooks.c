/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hooks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smamalig <smamalig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 14:21:36 by smamalig          #+#    #+#             */
/*   Updated: 2025/05/01 18:23:45 by smamalig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft_gl.h"

void	ft_gl_loop_hook(t_ft_gl *gl_ptr, void (*hook)(), void *param)
{
	gl_ptr->loop_hook = hook;
	gl_ptr->loop_hook_param = param;
}

void	ft_gl_key_hook(t_ft_gl *gl_ptr, void (*hook)(), void *param)
{
	gl_ptr->key_hook = hook;
	gl_ptr->key_hook_param = param;
}
