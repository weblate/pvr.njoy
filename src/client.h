/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "kodi/libXBMC_addon.h"
#include "kodi/libXBMC_pvr.h"

#define DEFAULT_HOST  "127.0.0.1"
#define DEFAULT_PORT  80

extern bool         g_bCreated;           ///< Shows that the Create function was successfully called
extern std::string  g_szUserPath;         ///< The Path to the user directory inside user profile
extern std::string  g_szClientPath;       ///< The Path where this driver is located

/* Client Settings */
extern std::string  g_strHostname;
extern int          g_iPort;

extern ADDON::CHelper_libXBMC_addon *XBMC;
extern CHelper_libXBMC_pvr          *PVR;

