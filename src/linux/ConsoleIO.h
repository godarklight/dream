/******************************************************************************\
 *
 * Copyright (c) 2013
 *
 * Author(s):
 *  David Flamand
 *
 * Description:
 *  see ConsoleIO.cpp
 *
 ******************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
\******************************************************************************/

#include <termios.h>
#include <signal.h>
#include "../DrmReceiver.h"

class CConsoleIO
{
public:
	static void Enter(CDRMReceiver* pDRMReceiver);
	static void Leave();
	static ERunState Update();
protected:
	static char ETypeRxStatus2char(ETypeRxStatus eTypeRxStatus);
	static CDRMReceiver* pDRMReceiver;
	static unsigned long long time;
	static int mode;
	static int tty;
	static int tty_setup;
	static char screen[];
	static char* screen_ptr;
	static int screen_size;
	static sigset_t sigset;
	static struct termios old_tio;
};
