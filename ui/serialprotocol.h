// NoSpark - OpenEVSE charger firmware
// Copyright (C) 2015 Andre Eisenbach
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// See LICENSE for a copy of the GNU General Public License or see
// it online at <http://www.gnu.org/licenses/>

#pragma once

#define SAPI_VERSION            1


// Status/error codes

#define OK                      0
#define UNKNOWN_COMMAND         1
#define INVALID_PARAMETER       2


// 'E' commands

#define CMD_GET_STATE           1
#define CMD_GET_CHARGE_STATE    2

#define CMD_GET_MAX_CURRENT     10
#define CMD_SET_MAX_CURRENT     11
#define CMD_GET_TIME            12
#define CMD_SET_TIME            13
#define CMD_GET_DATE            14
#define CMD_SET_DATE            15

#define CMD_SET_SLEEP           20

#define CMD_ENABLE_EVENTS       30

#define CMD_GET_SAPI_VERSION    254
