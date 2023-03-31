/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbertozz <tbertozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/30 13:24:57 by tbertozz          #+#    #+#             */
/*   Updated: 2023/03/31 10:17:38 by tbertozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

#ifndef errors_hpp
#define errors_hpp

#define	USERALREADYJOINED 0
#define USERISJOINED 1
#define NOTINCHANNEL 2
#define BANNEDFROMCHANNEL 3
#define TOOMANYCHANNELS 4
#define BADCHANNELKEY 5
#define CHANNELISFULL 6
#define NOSUCHCHANNEL 7
#define USERISBANNED 8
#define BADCHANNELMASK 9
#define USERNOTINCHANNEL -1
#define USERNOTFOUND -1

#endif