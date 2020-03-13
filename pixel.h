/*
　メインのファイルの定義を記したヘッダファイル
  pixel.h
*/

#ifndef _PIXEL_H_
#define _PIXEL_H_

// ヘッダファイル群
#include <stdio.h>
#include <handy.h>
#include <stdbool.h>
#include <stdlib.h>


// 自作ヘッダファイル 
#include "rgbcolor.h"
#include "opening.h"
#include "map.h"
#include "keyboard.h"

// 定数
#define MAIN_WINDOW 600.0  // メインウィンドウサイズ
#define PEN_WIN_SIZEX 250.0  // ペンサイズ変更用のウィンドウのXの大きさ
#define PEN_WIN_SIZEY 100.0  // ペンサイズ変更用のウィンドウのYの大きさ
#define PEN_NUM 5  // ペンサイズの大きさの本数を設定する
#define MODE_NUM 5  // モードの数を指定する

// 座標の構造体
typedef struct COORD{
    int x;  // x座標
    int y;  // y座標
}COORD;

// ゲームモードの列挙型
typedef enum GAMEMODE{
    pen_mode,  // ペンモード
    eraser_mode,  // 消しゴムモード
    backet_mode,  // バケツモード
    line_mode,  // 直線モード
    spoit_mode  // スポイトモード
} GAMEMODE;

// ペンサイズの列挙型
typedef enum PENSIZE{
    small,
    small_m,
    midium,
    midium_l,
    large
} PENSIZE;

// グリッド線の表示を操作する
int draw_grid(int pixel_num, double pixel_size, int layer, int grid);

// ドット絵を描く関数
void draw_pixel_art(double pixel_size, int x, int y, RGBA *color, int layer);

// 全体を描く関数
void draw_pixel_all(int pixel_num, double pixel_size, RGBA data[pixel_num][pixel_num], int layer);

// 変更地点のみを描く関数
void draw_pixel_diff(int pixel_num, double pixel_size, RGBA data[pixel_num][pixel_num], RGBA before[pixel_num][pixel_num], int layer);

// ペンサイズの変更用の描写を行う関数
void draw_pensize_init(int layer);

// ゲームモードの変更用の背景の描写を行う関数
void draw_gamemode_init(int layer);

// ペンサイズ変更の表示を行う
void draw_change_pensize(int layer, int x);

// ペンサイズを変更する関数
void change_pensize(int layer, PENSIZE *pensize, double x);

// ゲームモード変更の表示を行う
void draw_change_gamemode(int layer, double x);

// ゲームモードを変更する関数
void change_gamemode(int layer, GAMEMODE *gamemode, double x);

// 消しゴムの初期設定
void eraser_init(int layer);

// 消しゴムの透明度を変更する縁を作成する
void eraser_move_circle(int layer, int x);

// バケツの計算を行う再帰関数
// レイヤー, 現在のマップ, 今の場所, 塗りたい色, 通った場所の保存, 通った場所がいくつ保存されたのか
int backet_calc(int pixel_num, double pixel_size, int layer, RGBA map[pixel_num][pixel_num], COORD now_pos, RGBA src_color, RGBA data_color, COORD data[pixel_num * pixel_num], int data_top);

// 線を引く処理
void draw_line(int pixel_num, double pixel_size, int layer, RGBA now_color, RGBA map[pixel_num][pixel_num], COORD data1, COORD data2, int pen_size);

// 線を引くときの計算
void draw_line_calcx(int *x, COORD *data, int *data_num, int i);
void draw_line_calcy(int *y, COORD *data, int *data_num, int i);

// ゲームモードが消しゴムモードになっている際のポップアップの動作
void popup_eraser(int *popup, int *window_popup, int *popup_state, int *popup_move);

// 線を引く動作の時の円を描く
void draw_line_circle(double pixel_size, int layer, int x, int y);

#endif // _PIXEL_H_
