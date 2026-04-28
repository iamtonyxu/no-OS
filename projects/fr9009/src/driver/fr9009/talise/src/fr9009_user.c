/**
 * \file fr9009_user.c
 * \brief Contains FR9009 API default gain table values for Rx and ObsRx channels.  This file contains information which can be modified by the user.
 *
 * Copyright 2022 braridio.
 * Released under the fr9009 API license, for more information see the "LICENSE.txt" file in this zip file.
 */

#include "fr9009_user.h"

/**
 * \brief Default Rx gain table settings
 *
 * Order: {FE table, External Ctl, TIA/ADC, signed Digital Gain/Atten[10:0], Phase offset}
 */
fr9009RxGainTable_t rxGainTable [64] =
{
    {0 , 0, 0, 0x400, 0},   /* Gain index 132 */
    {1 , 0, 0, 0x400, 0},   /* Gain index 133 */
    {2 , 0, 0, 0x400, 0},   /* Gain index 134 */
    {3 , 0, 0, 0x400, 0},   /* Gain index 135 */
    {4 , 0, 0, 0x400, 0},   /* Gain index 136 */
    {5 , 0, 0, 0x400, 0},   /* Gain index 137 */
    {6 , 0, 0, 0x400, 0},   /* Gain index 138 */
    {7 , 0, 0, 0x400, 0},   /* Gain index 139 */
    {8 , 0, 0, 0x400, 0},   /* Gain index 140 */
    {9 , 0, 0, 0x400, 0},   /* Gain index 141 */
    {10, 0, 0, 0x400, 0},   /* Gain index 142 */
    {11, 0, 0, 0x400, 0},   /* Gain index 143 */
    {12, 0, 0, 0x400, 0},   /* Gain index 144 */
    {13, 0, 0, 0x400, 0},   /* Gain index 145 */
    {14, 0, 0, 0x400, 0},   /* Gain index 146 */
    {15, 0, 0, 0x400, 0},   /* Gain index 147 */
    {16, 0, 0, 0x400, 0},   /* Gain index 148 */
    {17, 0, 0, 0x400, 0},   /* Gain index 149 */
    {18, 0, 0, 0x400, 0},   /* Gain index 150 */
    {19, 0, 0, 0x400, 0},   /* Gain index 151 */
    {20, 0, 0, 0x400, 0},   /* Gain index 152 */
    {21, 0, 0, 0x400, 0},   /* Gain index 153 */
    {22, 0, 0, 0x400, 0},   /* Gain index 154 */
    {23, 0, 0, 0x400, 0},   /* Gain index 155 */
    {24, 0, 0, 0x400, 0},   /* Gain index 156 */
    {25, 0, 0, 0x400, 0},   /* Gain index 157 */
    {26, 0, 0, 0x400, 0},   /* Gain index 158 */
    {27, 0, 0, 0x400, 0},   /* Gain index 159 */
    {28, 0, 0, 0x400, 0},   /* Gain index 160 */
    {29, 0, 0, 0x400, 0},   /* Gain index 161 */
    {30, 0, 0, 0x400, 0},   /* Gain index 162 */
    {31, 0, 0, 0x400, 0},   /* Gain index 163 */
    {32, 0, 0, 0x400, 0},   /* Gain index 164 */
    {33, 0, 0, 0x400, 0},   /* Gain index 165 */
    {34, 0, 0, 0x400, 0},   /* Gain index 166 */
    {35, 0, 0, 0x400, 0},   /* Gain index 167 */
    {36, 0, 0, 0x400, 0},   /* Gain index 168 */
    {37, 0, 0, 0x400, 0},   /* Gain index 169 */
    {38, 0, 0, 0x400, 0},   /* Gain index 170 */
    {39, 0, 0, 0x400, 0},   /* Gain index 171 */
    {40, 0, 0, 0x400, 0},   /* Gain index 172 */
    {41, 0, 0, 0x400, 0},   /* Gain index 173 */
    {42, 0, 0, 0x400, 0},   /* Gain index 174 */
    {43, 0, 0, 0x400, 0},   /* Gain index 175 */
    {44, 0, 0, 0x400, 0},   /* Gain index 176 */
    {45, 0, 0, 0x400, 0},   /* Gain index 177 */
    {46, 0, 0, 0x400, 0},   /* Gain index 178 */
    {47, 0, 0, 0x400, 0},   /* Gain index 179 */
    {48, 0, 0, 0x400, 0},   /* Gain index 180 */
    {49, 0, 0, 0x400, 0},   /* Gain index 181 */
    {50, 0, 0, 0x400, 0},   /* Gain index 182 */
    {51, 0, 0, 0x400, 0},   /* Gain index 183 */
    {52, 0, 0, 0x400, 0},   /* Gain index 184 */
    {53, 0, 0, 0x400, 0},   /* Gain index 185 */
    {54, 0, 0, 0x400, 0},   /* Gain index 186 */
    {55, 0, 0, 0x400, 0},   /* Gain index 187 */
    {56, 0, 0, 0x400, 0},   /* Gain index 188 */
    {57, 0, 0, 0x400, 0},   /* Gain index 189 */
    {58, 0, 0, 0x400, 0},   /* Gain index 190 */
    {59, 0, 0, 0x400, 0},   /* Gain index 191 */
    {60, 0, 0, 0x400, 0},   /* Gain index 192 */
    {61, 0, 0, 0x400, 0},   /* Gain index 193 */
    {62, 0, 0, 0x400, 0},   /* Gain index 194 */
    {63, 0, 0, 0x400, 0},   /* Gain index 195 */
};

uint8_t framer_sel_type_rx = 0;/*用于选择framer是否disable link share，即 内部orx与rx map不根据通道开启不同而变化*/
