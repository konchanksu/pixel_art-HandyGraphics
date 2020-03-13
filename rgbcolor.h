/*
 * カラーサークルを描く際に使用する関数や定数の定義ファイル
 * rgbcolor.h
*/
#ifndef _RGBCOLOR_H_
#define _RGBCOLOR_H_

// 読み込むヘッダファイルの指定
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <handy.h>

#define RGB_MAX 255 // RGB型の最大値を表す
#define RGB_MIN 0 // RGB型の最小値を示す

// HSVからRGBへの変換時に使用するid
#define RGB_ID_NONE -1 
#define RGB_ID_RED 0
#define RGB_ID_BLUE 1
#define RGB_ID_GREEN 2

#define RGB_COUNT_BOX 15.0 // 内側の箱の描画数を20とする
#define WIN_RGBCIR_SIZE 250.0 // ウィンドウのサイズを250とする

// 原点をwindowの中央とする
#define H_CIRCLE_RADIUS 60.0 // 虹色のサークルの内側の半径
#define H_CIRCLE_RAD_OUT 80.0 // 虹色のサークルの幅
// 色の濃淡を変化させる箱の左下の座標（正方形なためx, yが一致する）
#define SV_BOX_LIGHT_BOTTOM (double)(-1 * H_CIRCLE_RADIUS / sqrt(2)) // マイナスになる
#define SV_BOX_WIDTH (double)(-2 * SV_BOX_LIGHT_BOTTOM)

#define CIRCLE_ANGLE_MAX 360.0 // 円の一周を指定する
#define MINI_CIRCLE_SIZE 4 // 小さい色決定用の円の半径

// hsv型の設定
typedef struct {
    double hue;
    double saturation;
    double value;
} HSV;

// rgba型の設定
typedef struct {
    double red;
    double green;
    double blue;
    double alpha;
} RGBA;

// RGBCLay型の設定
// 描画をする際に分ける
typedef struct {
    int hue_move;
    int box_move;
    int alpha_move;
    int now_color_box;
    int draw_layer;
} RGBCLay;

// rgb3つの最小値を確認し,minに代入する
void RGB_min_check(RGBA *rgb_color, double *min);

// rgb3つの最大値を確認し,maxに代入する
void RGB_max_check(RGBA *rgb_color, double *max, int *id);

// RGB形式から,HSV形式へと変換する
void RGB_change_to_HSV(HSV *hsv_color, RGBA *rgb_color);

// HSV形式から,RGB形式へと変換する
void HSV_change_to_RGB(HSV *hsv_color, RGBA *rgb_color);

// 中央の濃淡を変化させる四角を描く関数
void draw_SV_box(HSV *hsv_color, RGBCLay lay);

// 外側の虹色の円を描く
void draw_hue_circle(HSV *hsv_color, RGBCLay lay);

// 動かす円を描く
void draw_move_circle(int x, int y, int move_layer);

// 現在の色が確認できる箱を作成する
void draw_now_colorbox(RGBCLay lay, RGBA rgb_now_color);

// 透明度を操作する円を作成する
void draw_alpha_circle(int x, RGBCLay lay);

// hueを操作する円を描く
void draw_hue_move_circle(RGBCLay lay, HSV *hsv_color);

// カラーサークルを表示する関数
void draw_color_circle(RGBA rgb_now_color, RGBCLay layerdata);

// saturationやvalueの値を変更し描画する
void sat_value_move(RGBA *rgb_color, HSV *hsv_color, RGBCLay lay, hgevent *event);

// alphaの値を変更し描画する
void alpha_move(RGBA *rgb_color, HSV *hsv_color, RGBCLay lay, hgevent *event);

// hueの値を変更し描画する
void hue_move(RGBA *rgb_color, HSV *hsv_color, RGBCLay lay, hgevent *event, int window);

// 実際の動作を指定する
void color_circle_move(RGBA *rgb_color, HSV *hsv_color, RGBCLay lay, hgevent *event, int window);

//  スポイト機能を使用した際のサークルの動きをつける
void spoit_move_circle(RGBA *rgb_color, HSV *hsv_color, RGBCLay lay);

#endif // _RGBCOLOR_H_
