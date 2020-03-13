/*
　オープニング画面を作成する
　opening.h
*/
#ifndef _OPENING_H_
#define _RGBCOLOR_H_

// インクルードファイル
#include <stdio.h>
#include <stdlib.h>
#include <handy.h>
#include <stdbool.h>

/*　注意：エンターキーの識別番号が異なった場合に使用することができない　*/
#define HG_ENTER 0x0d // エンターキーを押す

// オープニング画面を開く
int draw_opening(void);

#endif // _OPENING_H_
