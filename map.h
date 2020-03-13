#ifndef _MAP_H_ 
#define _MAP_H_

#include "rgbcolor.h"  // RGBA型を利用できるようにする
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/param.h>


#define MAP_LIST_MAX 30  // マップの最大保存量
#define MAP_TOP_MAX 30  // map_topの最大値 
#define BACKET_POWER 5 // バケツの強さを変更する（後々これを変数に変更する可能性あり）

// マップの移動を構造体型で指定
typedef struct{
    int now_print;
    int top_data;
}map_struct;

// map_listの初期化
void map_list_init(int  pixel_num, RGBA copyer[pixel_num][pixel_num]);

// 1マスコピー
void copy_piece(RGBA *copyer, RGBA *copyee);

// 入れ替えを行う
void int_switch(int *a, int *b);

// map_listをcopyする
// copyer:被コピー者, copyee:コピーする側
void map_list_copy(int pixel_num, RGBA copyer[pixel_num][pixel_num], RGBA copyee[pixel_num][pixel_num]);

// マップを1マス分描く
// 明確にするため関数化した
void print_map(int pixel_num, RGBA map_data_list[pixel_num][pixel_num]);

// mapで保存するためにコピーする
void normal_color_copy(RGBA *src, RGBA *dst, RGBA *out);

// マップポジションを加算する
void map_position_add(map_struct *a);

// マップポジションを減算する
void map_position_sub(map_struct *a);

// カレントディレクトリの取得
int get_path(char path[MAXPATHLEN]);

// csvファイルに書き込みを行う
int file_write(int pixel_num, RGBA map[pixel_num][pixel_num]);

// csvファイルから読み込みを行う
int file_read(int pixel_num, RGBA map[pixel_num][pixel_num]);

// RGBA構造体同士の比較
int comparison_rgba(RGBA a, RGBA b);

// maptopが配列のオーバーフローを起こさないようにする処理
void check_maptop_over(int pixel_num, map_struct *map_position, RGBA map_data_list[MAP_TOP_MAX][pixel_num][pixel_num]);

// もしzなどを使ってnowとtopの位置がずれていた場合topの位置を修正
void top_now_position_revision(int pixel_num, map_struct *map_position, RGBA map_data_list[MAP_TOP_MAX][pixel_num][pixel_num]);

#endif // _MAP_H_
