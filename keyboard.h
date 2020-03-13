#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

// ヘッダファイルの宣言
#include <stdio.h>
#include <handy.h>
#include <stdbool.h>
#include <string.h>

#define WID_WIDTH 300.0
#define WID_HEIGHT 60.0

#define HG_ENTER 0x0d
#define HG_ESCAPE 0x1b
#define HG_BACK 0x7f
#define HG_UNDERBAR 0x5f

#define MAX_KEY_DATA 0x31

// handyを起動する
void keyboard_open(int *window_id, int *lay);

// キーボードの入力を取る
int get_keyboard(char input_key[MAX_KEY_DATA]);

// 数値入力のボタンを設定する
void number_button_open(int *window_id, int *lay);

void number_get(int *number);

#endif // _KEYBOARD_H_
