/*
 * pixel_art_editor
 * 作成者: konchan
 * 作成日時：2020年1月10日(ver 1.0)
 *
 * 本制作物の目的：
 *  本制作物は京都産業大学情報理工学部にて開講されている授業、基礎プログラミング演習2
 *  にて発表する目的で制作された
 * 
 * 実行方法
 * make
 * (hgcc -o pixel pixel.c opening.c rgbcolor.c keyboard.c map.c)
 *  
 * 本制作物で可能なこと：
 *  タイトルにある通り、ドット絵を制作することができる．比較的自由な色を使用できるた
 *  め、様々な作品を制作できるだろう
 * 
*/ 

#include "pixel.h"

RGBA WHITE = {RGB_MAX, RGB_MAX, RGB_MAX, 1.0};

// --------------------------------------
// メイン関数
// --------------------------------------
int main(void) {
    int i, j;  // カウント変数
    int tmp;  // 一時的な使用
    int move_data_top = 0;
    int pixel_num = 10; // ピクセルの個数
    
    tmp = draw_opening();  // オープニング画面を作成する 
    number_get(&pixel_num);

    double pixel_size = (MAIN_WINDOW / (double)pixel_num); // ピクセル一つあたりのサイズ
    COORD move_data[pixel_num * pixel_num];
    
    // 保存用の名前作成
    char input_key[MAX_KEY_DATA];
    
    // --------------------------------------
    // pixel.cにて使用する変数宣言
    // --------------------------------------
    RGBA tmp_color;  // 一時的に保管する色を用意
    RGBA spoit_color;  // スポイト用の色

    // マップを保存する配列
    RGBA (*map_data_list)[pixel_num][pixel_num] = (RGBA(*)[pixel_num][pixel_num])malloc(sizeof(RGBA) * MAP_TOP_MAX * pixel_num * pixel_num);
    
   
    // ファイルを読み込むかどうかを設定する
    if(tmp == true){
        file_read(pixel_num, map_data_list[0]);  // ファイルを読み込む
    }else{
        map_list_init(pixel_num, map_data_list[0]);  // ファイルの初期化
    }

    // マップの構造を作成する
    map_struct *map_position;
    map_position = (map_struct *)malloc(sizeof(map_struct));  // メモリを確保する
    map_position->now_print = 0;  // 現在塗りが完了している場所を指す
    map_position->top_data = 1;  // 基本的に現在塗り終えている場所の一つ上を指す

    hgevent *event;  // イベントの取得
    int x;  // xのクリック位置の取得
    int y;  // yのクリック位置の取得

    int popup = false;  // ポップアップが開いているかどうか
    GAMEMODE gamemode;  // ゲームモードを変更する変数
    PENSIZE pensize;  // ペンサイズを変更する変数

    // ウィンドウ変数
    int window_main, win_pensize, win_gamemode;  // 中心の大きなウィンドウのid
    double screen_width, screen_height; // スクリーンの幅,高さ

    int roop = true;  // ループ変数
    int grid = false;  // グリッド線変数

    // スクリーンの大きさを取得する
    HgScreenSize(&screen_width, &screen_height);

    // メインのウィンドウ
    window_main = HgWOpen((-MAIN_WINDOW + screen_width) / 2, (-MAIN_WINDOW + screen_height) / 2, MAIN_WINDOW,
                          MAIN_WINDOW);
    HgWSetTitle(window_main, "Pixel Art");
    int layer_main = HgWAddLayer(window_main);
    int layer_sub  = HgWAddLayer(window_main);
    int layer_line_circle = HgWAddLayer(window_main);
    
    // ペンサイズを設定するウィンドウ
    win_pensize = HgWOpen(150.0, 300.0, PEN_WIN_SIZEX, PEN_WIN_SIZEY);
    HgWSetTitle(win_pensize, "PenSize Change");
    int layer_pen_move = HgWAddLayer(win_pensize);  // 動きをつける
    int layer_pen_state = HgWAddLayer(win_pensize);  // 固定で表示する
    
    // ペンモードを変更するウィンドウ
    win_gamemode = HgWOpen(150.0, 150.0, PEN_WIN_SIZEX, PEN_WIN_SIZEY);
    HgWSetTitle(win_gamemode, "GameMode Change");
    int layer_gamemode_move = HgWAddLayer(win_gamemode);  // 動きをつける
    int layer_gamemode_state = HgWAddLayer(win_gamemode);  // 固定で表示する

    // ポップアップ用のウィンドウ
    int window_popup;
    int popup_state;
    int popup_move;

    // --------------------------------------
    // カラーサークル関係の変数宣言
    // --------------------------------------
    RGBA *rgb_color = NULL;  // rgbaでの色のポインタ
    HSV  *hsv_color = NULL;  // hsv色のポインタ

    // 動的にメモリを確保する
    rgb_color = (RGBA*)malloc(sizeof(RGBA));  
    hsv_color = (HSV*) malloc(sizeof(HSV));

    // 色の初期化をする 
    rgb_color->red = RGB_MAX, rgb_color->green = RGB_MIN, rgb_color->blue = RGB_MIN, rgb_color->alpha = 1;  // 赤色に設定する
    RGB_change_to_HSV(hsv_color, rgb_color);

    // ウィンドウの宣言
    int window_color_circle = HgWOpen(150.0, 450.0, 250, 250);
    HgWSetTitle(window_color_circle, "Color Circle");
    HgWCoordinate(window_color_circle, WIN_RGBCIR_SIZE / 2, WIN_RGBCIR_SIZE / 2, 1);

    // レイヤの宣言
    RGBCLay layerdata;  // RGBのカラーサークルで使用するレイヤーをまとめて宣言する
    layerdata.now_color_box = HgWAddLayer(window_color_circle);
    layerdata.draw_layer    = HgWAddLayer(window_color_circle);
    layerdata.hue_move      = HgWAddLayer(window_color_circle);
    layerdata.box_move      = HgWAddLayer(window_color_circle);
    layerdata.alpha_move    = HgWAddLayer(window_color_circle);

    // カラーサークルを作成する
    draw_color_circle(*rgb_color, layerdata);

    // 取得するイベントを指定する
    HgWSetEventMask(window_color_circle, HG_MOUSE_DOWN | HG_MOUSE_DRAG | HG_MOUSE_UP | HG_KEY_DOWN);
    HgWSetEventMask(window_main, HG_MOUSE_DOWN | HG_MOUSE_DRAG | HG_MOUSE_UP | HG_KEY_DOWN);
    HgWSetEventMask(win_pensize, HG_MOUSE_DOWN | HG_MOUSE_DRAG | HG_KEY_DOWN);
    HgWSetEventMask(win_gamemode, HG_MOUSE_DOWN | HG_MOUSE_DRAG | HG_KEY_DOWN);
    
    // 前の描画内容をコピーをする
    map_list_copy(pixel_num, map_data_list[map_position->top_data], map_data_list[map_position->now_print]);
    
     // ペンサイズの変更用の描写を行う関数
    draw_pensize_init(layer_pen_state);
    change_pensize(layer_pen_move, &pensize, 0);

    // ゲームモード変更用の描写を行う関数
    draw_gamemode_init(layer_gamemode_state);
    change_gamemode(layer_gamemode_move, &gamemode, 0);
    

    // 一番初めの描写を行う
    draw_pixel_all(pixel_num, pixel_size, map_data_list[map_position->now_print], layer_main);
   

    // --------------------------------------
    // ループ
    // --------------------------------------
    while(roop){
        event = HgEventNonBlocking();
        // ゲームモードが消しゴムでなくなった時の操作
        if(gamemode != eraser_mode && popup == true){
            HgWClose(window_popup);
            popup = false;     
        }

        // ゲームモードが消しゴムになっている時に行う動作
        if(gamemode == eraser_mode && popup == false){
            // ウィンドウの設定を行う
            popup_eraser(&popup, &window_popup, &popup_state, &popup_move);
        }

        if(event != NULL){
            // キーボードが押された際の操作
            if(event->type == HG_KEY_DOWN){
                switch (event->ch){
                    case 's': // csv形式で描画を保存する
                        file_write(pixel_num, map_data_list[map_position->now_print]);
                        break;
                    case 'q': // qで終了する
                        // 選択肢を作成
                        tmp = HgAlert("終了しますか?", "保存して終了", "保存せずに終了", "キャンセル", NULL);
                        
                        switch (tmp){
                            // 保存して終了
                            case 0:
                                // キーボードの入力を取る
                                if(get_keyboard(input_key) == true){
                                    if(input_key[0] == '\0')
                                        HgWSave(window_main, "sample");
                                    else
                                        HgWSave(window_main, input_key);
                                } else {
                                    break;
                                }

                            // 保存せずに終了
                            case 1:
                                roop = false;
                                break;
                            // キャンセル
                            case 2:
                                break;
                        }          
                        break;
                    
                    case 'c':  // 画面をクリアする
                        HgLClear(layer_main);

                        // topの位置をずらす
                        map_position->top_data = map_position->now_print + 1;

                        // もし最大を超えてしまう場合古いものから消去
                        // コピーをとるタイミングがマウスを上げた時なのでmap_topがMAP_TOP_MAX-1以上になったら発動
                        check_maptop_over(pixel_num, map_position, map_data_list);

                        // 白で上書き
                        map_list_init(pixel_num, map_data_list[map_position->top_data]);  

                        // トップに現在のマップをコピー
                        map_position_add(map_position);
                        map_list_copy(pixel_num, map_data_list[map_position->top_data], map_data_list[map_position->now_print]);
                        break;

                    case 'g':  // グリッド線をオン、オフにする
                        grid = draw_grid(pixel_num, pixel_size, layer_sub, grid);
                        break;
                    
                    case 'y':  // zを取り消す
                        if(map_position->now_print < map_position->top_data - 1){
                            map_position->now_print++;  // nowを1つ上げる
                            draw_pixel_diff(pixel_num, pixel_size, map_data_list[map_position->now_print], map_data_list[map_position->now_print - 1] ,layer_main);
                        }
                        break;

                    case 'z':  // 一つ前の描画を呼び出す
                        if (map_position->now_print > 0) {
                            map_position->now_print--; // nowを1つ下げる
                            draw_pixel_diff(pixel_num, pixel_size, map_data_list[map_position->now_print], map_data_list[map_position->now_print + 1] ,layer_main);
                        }
                        break;
                    
                    case 'm':  // モードを変更する（裏コマンド）
                        gamemode++;
                        if(gamemode > spoit_mode) gamemode = pen_mode;
                        draw_change_gamemode(layer_gamemode_move, (int)gamemode * 50 + 1);
                        break;
                        
                    case 'p':  // ペンサイズを変更する（裏コマンド）
                        pensize++;
                        if(pensize > large) pensize = small;
                        draw_change_pensize(layer_pen_move, (int)pensize * 50 + 1);
                        break;
                }
            }

            // カラーサークルの移動
            if(event->wid == window_color_circle){
                color_circle_move(rgb_color, hsv_color, layerdata, event, window_color_circle);
                // printf("%f %f %f %f\n", rgb_color->red, rgb_color->green, rgb_color->blue, rgb_color->alpha);
            }

            // 描き込む
            else if(event->wid == window_main){
                if(gamemode == pen_mode || gamemode == eraser_mode){
                    // マウスを下ろした状態の処理
                    if(event->type == HG_MOUSE_DOWN || event->type == HG_MOUSE_DRAG){
                        // もしzなどを使ってnowとtopの位置がずれていた場合topの位置を修正
                        top_now_position_revision(pixel_num, map_position, map_data_list);

                        // もし最大を超えてしまう場合古いものから消去
                        check_maptop_over(pixel_num, map_position, map_data_list);

                        x = event->x / pixel_size;  // x座標を取る
                        y = event->y / pixel_size;  // y座標を取る

                        // 描く
                        double tmp_alpha = rgb_color->alpha;  // 一時的に本来のrgbcolor->alphaを格納する

                        // ループはペンサイズによって変更される
                        for(i = (int)-pensize; i <= (int)pensize; i++){
                            for(j = (int)-pensize; j <= (int)pensize; j++){
                                // ペンの質感をちょっと変更してみる
                                rgb_color->alpha = tmp_alpha - ((double)(abs(i) + abs(j)) / 8) * tmp_alpha;
                                // printf("%f\n", rgb_color->alpha);
                                int tmpx = x + i;
                                int tmpy = y + j;
                                // 範囲外のものを描かないようにする
                                if(abs(i) + abs(j) > pensize + 1) continue;
                                if(tmpx < 0 || tmpy < 0 || tmpx >= pixel_num || tmpy >= pixel_num) continue;
                                // ペンを描く
                                if(gamemode == pen_mode){
                                    draw_pixel_art(pixel_size, tmpx, tmpy, rgb_color, layer_main);
                                    // 記録する
                                    normal_color_copy(&map_data_list[map_position->top_data][tmpx][tmpy], rgb_color, &tmp_color);
                                    map_data_list[map_position->top_data][tmpx][tmpy] = tmp_color;
                                }
                                // 消しゴムを描く
                                else if(gamemode == eraser_mode){
                                    draw_pixel_art(pixel_size, tmpx, tmpy, &WHITE, layer_main);
                                    // 記録する
                                    normal_color_copy(&map_data_list[map_position->top_data][tmpx][tmpy], &WHITE, &tmp_color);
                                    map_data_list[map_position->top_data][tmpx][tmpy] = tmp_color;
                                }
                            }
                        }
                        rgb_color->alpha = tmp_alpha;  // 変更した値を元に戻す
                    }
                    // マウスを上げた時の処理
                    else if(event->type == HG_MOUSE_UP){
                        map_position_add(map_position);
                    
                        // print_map(map_data_list[map_position->now_print]);
                        // 前の描画内容をコピーをする
                        map_list_copy(pixel_num, map_data_list[map_position->top_data], map_data_list[map_position->now_print]);
                    }
                }
                // バケツの処理
                if(gamemode == backet_mode){
                    if(event->type == HG_MOUSE_DOWN){
                        // もしzなどを使ってnowとtopの位置がずれていた場合topの位置を修正
                        top_now_position_revision(pixel_num, map_position, map_data_list);

                        // もし最大を超えてしまう場合古いものから消去
                        // コピーをとるタイミングがマウスを上げた時なのでmap_topがMAP_TOP_MAX-1以上になったら発動
                        check_maptop_over(pixel_num, map_position, map_data_list);

                        x = event->x / pixel_size;  // x座標を取る
                        y = event->y / pixel_size;  // y座標を取る

                        if(comparison_rgba(*rgb_color, map_data_list[map_position->top_data][x][y]) == false){
                            move_data_top = 0;  // 探索済みがいくつあるかを判定する
                            draw_pixel_art(pixel_size, x, y, rgb_color, layer_main);  // クリックした位置を塗る
                                            
                            // クリックした位置は探索済み
                            move_data_top++;
                            COORD tmp_coord = {x, y};
                            move_data[0] = tmp_coord;

                            // バケツの動作を行う
                            backet_calc(pixel_num, pixel_size, layer_main, map_data_list[map_position->top_data], tmp_coord, map_data_list[map_position->top_data][x][y], *rgb_color, move_data, move_data_top);

                            // この色はここでしか使用しない
                            RGBA tmprgba = map_data_list[map_position->top_data][x][y];

                            // どこも探索されなかった場合の処理
                            if(comparison_rgba(tmprgba, map_data_list[map_position->top_data][x + 1][y]) == false && comparison_rgba(tmprgba, map_data_list[map_position->top_data][x - 1][y]) == false &&
                                comparison_rgba(tmprgba, map_data_list[map_position->top_data][x][y + 1]) == false && comparison_rgba(tmprgba, map_data_list[map_position->top_data][x][y - 1]) == false){
                                normal_color_copy(&map_data_list[map_position->top_data][x][y], rgb_color, &tmp_color);
                                map_data_list[map_position->top_data][x][y] = tmp_color;
                            }
                            map_data_list[map_position->top_data][x][y] = tmp_color;

                            // z,yのコマンドを使えるようにする
                            map_position_add(map_position);
                                                       
                            // 前の描画内容をコピーをする
                            map_list_copy(pixel_num, map_data_list[map_position->top_data], map_data_list[map_position->now_print]);
                        }
                    }
                }
                // 直線の処理  
                if(gamemode == line_mode){
                    if(event->type == HG_MOUSE_DOWN){
                        x = event->x / pixel_size;  // x座標を取得する
                        y = event->y / pixel_size;  // y座標を取得する

                        COORD data1 = {x, y};
                        // printf("%d %d\n", x, y);
                        draw_line_circle(pixel_size, layer_line_circle, x, y);
                        int mini_roop = true;
                        while(mini_roop){
                            event = HgEvent();
                            if(event->type == HG_MOUSE_DOWN){
                                // もしzなどを使ってnowとtopの位置がずれていた場合topの位置を修正
                                top_now_position_revision(pixel_num, map_position, map_data_list);

                                // もし最大を超えてしまう場合古いものから消去
                                // コピーをとるタイミングがマウスを上げた時なのでmap_topがMAP_TOP_MAX-1以上になったら発動
                                check_maptop_over(pixel_num, map_position, map_data_list);
                                HgLClear(layer_line_circle);

                                x = event->x / pixel_size;
                                y = event->y / pixel_size;
                                // printf("%d %d\n", x, y);

                                COORD data2 = {x, y};
                                draw_line(pixel_num, pixel_size, layer_main, *rgb_color, map_data_list[map_position->top_data], data1, data2, pensize);
                                mini_roop = false;

                                // z,yのコマンドを使えるようにする
                                map_position_add(map_position);

                                // 前の描画内容をコピーをする
                                map_list_copy(pixel_num, map_data_list[map_position->top_data], map_data_list[map_position->now_print]);
                            }else if(event->ch == 'd'){ 
                                HgLClear(layer_line_circle);
                                break;
                            }
                        }
                    }
                }
                // スポイトモードの動きを作成
                if(gamemode == spoit_mode){
                    if(event->type == HG_MOUSE_DOWN){
                        // 配列の座標としてx,yを取得
                        x = event->x / pixel_size;
                        y = event->y / pixel_size;

                        // これポインタどうなってるんだ（ポインタわからんマン）
                        spoit_color = map_data_list[map_position->now_print][x][y];
                        *rgb_color = spoit_color;
                        spoit_move_circle(rgb_color, hsv_color, layerdata);
                    }
                }
            }

            // ペンサイズのウィンドウを選択している時の操作
            else if(event->wid == win_pensize){
                if(event->type == HG_MOUSE_DOWN || event->type == HG_MOUSE_DRAG){
                    x = event->x;
                    change_pensize(layer_pen_move, &pensize, x);
                }
            }

            // ゲームモードのウィンドウをしている時の操作
            else if(event->wid == win_gamemode){
                if(event->type == HG_MOUSE_DOWN || event->type == HG_MOUSE_DRAG){
                    x = event->x;
                    change_gamemode(layer_gamemode_move, &gamemode, x);
                }
            }

            // ポップアップが出現している時の動作
            else if(event->wid == window_popup){
                if(event->type == HG_MOUSE_DOWN || event->type == HG_MOUSE_DRAG){
                    x = event->x;
                    eraser_move_circle(popup_move, x);
                }
            }       
        }
    }

    // 終了時に変数を解放する
    free(map_data_list);
    free(rgb_color);
    free(hsv_color);
    free(map_position);

    // ウィンドウを閉じる
    HgCloseAll();

    return 0;
}


// グリッド線を描く
int draw_grid(int pixel_num, double pixel_size, int layer, int grid) {
    HgLClear(layer);  // レイヤー描写の全消去
    if (grid == false) {
        HgWSetColor(layer, HgRGB(0.1, 0.1, 0.1));  // 色の指定
        for (int i = 1; i < pixel_num; i++) {
            // 実際に一本ずつ描く
            HgWLine(layer, 0, i * pixel_size, MAIN_WINDOW, i * pixel_size);
            HgWLine(layer, i * pixel_size, 0, i * pixel_size, MAIN_WINDOW);
        }
        return true;
    }
    return false;
}


// 1マスを描く関数
void draw_pixel_art(double pixel_size, int x, int y, RGBA *color, int layer){
    HgWSetFillColor(layer, HgRGBA(color->red / RGB_MAX, color->green / RGB_MAX,
                                  color->blue / RGB_MAX, color->alpha));

    HgWBoxFill(layer, x * pixel_size, y * pixel_size, pixel_size, pixel_size, 0);
}


// 全体を描く関数
void draw_pixel_all(int pixel_num, double pixel_size, RGBA data[pixel_num][pixel_num], int layer){
    for(int i = 0; i < pixel_num; i++){
        for(int j = 0; j < pixel_num; j++){
            draw_pixel_art(pixel_size, i, j, &data[i][j], layer);
        }
    }
}


// 変更地点のみを描く関数
void draw_pixel_diff(int pixel_num, double pixel_size, RGBA data[pixel_num][pixel_num], RGBA before[pixel_num][pixel_num], int layer){
    for(int i = 0; i < pixel_num; i++){
        for(int j = 0; j < pixel_num; j++){
            if(before[i][j].alpha == data[i][j].alpha && before[i][j].red == data[i][j].red
            && before[i][j].green == data[i][j].green && before[i][j].blue == data[i][j].blue) continue;
            // draw_pixel_art(i, j, &WHITE, layer);  // 保存のデータをalpha = 1に固定したので不要
            draw_pixel_art(pixel_size, i, j, &data[i][j], layer);
        }
    }
}


// ペンサイズの変更用の描写を行う関数
void draw_pensize_init(int layer){
    // ペンのサイズの指定
    int r = 5;  // 半径の設定
    double y = PEN_WIN_SIZEY / 2 + 10.0;  // y座標の設定
    double x = PEN_WIN_SIZEX / (2 * PEN_NUM);  // x座標の設定
    // フォントと色の設定
    HgWSetFillColor(layer, HgRGB(0.3, 0.3, 0.3));
    HgWSetFont(layer, HG_T, 20);

    // ループで描写
    for(int i = 0; i < PEN_NUM; i++){
        int print_pensize = i + 1;
        HgWCircleFill(layer, x, y, r, 0);
        HgWText(layer, x - 5.0, 20.0, "%d", print_pensize);
        r += 2;
        x += PEN_WIN_SIZEX / 5;
    }
}


// ゲームモードの変更用の背景の描写を行う関数
void draw_gamemode_init(int layer){
    int img[MODE_NUM] = {HgImageLoad(".pencil.png"), HgImageLoad(".eraser.png"), HgImageLoad(".backet.png"),
                         HgImageLoad(".line.png"), HgImageLoad(".spoit.png")};
    for(int i = 0; i < MODE_NUM; i++){
        HgWImageDraw(layer, PEN_WIN_SIZEX / MODE_NUM * i + 10, 20, img[i]);
    }
}

// ペンサイズ変更の表示を行う
void draw_change_pensize(int layer, int x){
    double width = PEN_WIN_SIZEX / PEN_NUM;  // 幅を設定する
    HgWSetFillColor(layer, HgRGB(0.7, 0.7, 0.7));
    HgLClear(layer);
    HgWBoxFill(layer, (int)(x / width) * width, 0.0, width, PEN_WIN_SIZEY, 0);
}


// ペンサイズの変更を行う
void change_pensize(int layer, PENSIZE *pensize, double x){
    double width = PEN_WIN_SIZEX / PEN_NUM;  // 幅を設定する
    // ウィンドウサイズを超えた際の処理
    if(x < 0) x = 0;
    if(x >= PEN_WIN_SIZEX) x = PEN_WIN_SIZEX - 1;
    
    // ペンサイズの切り替え
    switch ((int)(x / width)){
        case small:
            *pensize = small;
            break;
        case small_m:
            *pensize = small_m;
            break;
        case midium:
            *pensize = midium;
            break;
        case midium_l: 
            *pensize = midium_l;
            break;
        case large:
            *pensize = large;
            break;
    }
    // 表示を行う
    draw_change_pensize(layer, x);
}



// ゲームモード変更の表示を行う
void draw_change_gamemode(int layer, double x){
    double width = PEN_WIN_SIZEX / MODE_NUM; 
    HgWSetFillColor(layer, HgRGB(0.7, 0.7, 0.7));
    HgLClear(layer);
    HgWBoxFill(layer, (int)(x / width) * width, 0.0, width, PEN_WIN_SIZEY, 0);
}


// ゲームモードを変更する
void change_gamemode(int layer, GAMEMODE *gamemode, double x){
    double width = PEN_WIN_SIZEX / MODE_NUM;  // ウィンドウの幅を設定する
    // ウィンドウサイズを超えた際の処理
    if(x < 0) x = 0; 
    if(x >= width * MODE_NUM) x = width * MODE_NUM - 1;
    GAMEMODE tmp = (int)(x / width);

    // モードを切り替える
    switch (tmp){
        case pen_mode:
            *gamemode = pen_mode;
            break;
        case eraser_mode:
            *gamemode = eraser_mode;
            break;
        case backet_mode:
            *gamemode = backet_mode;
            break;
        case line_mode:
            *gamemode = line_mode;
            break;
        case spoit_mode:
            *gamemode = spoit_mode;
            break;
    }
    // 表示を行う
    draw_change_gamemode(layer, x);
}


// 消しゴムの初期設定
void eraser_init(int layer){
    // 箱を作成する
    HgWSetFont(layer, HG_T, 20.0);
    HgWBox(layer, 20, 40.0, 210.0, 10.0);
    HgWText(layer, 80.0, 60.0, "transparency");
}


// 消しゴムの透明度を変更する縁を作成する
void eraser_move_circle(int layer, int x){
    // 外にはみ出た時の処理
    if(230 < x) x = 230.0;
    if(20 > x) x = 20.0;
    // 描画する
    HgLClear(layer);
    HgWCircle(layer, x, 45.0, MINI_CIRCLE_SIZE);
    // 透明度を画像に合わせて変更する
    WHITE.alpha = (x - 20.0) / 210.0;
}


// バケツの計算を行う再帰関数
// レイヤー, 現在のマップ, 今の場所, 塗りたい色, 通った場所の保存, 通った場所がいくつ保存されたのか
int backet_calc(int pixel_num, double pixel_size, int layer, RGBA map[pixel_num][pixel_num], COORD now_pos, RGBA src_color, RGBA data_color, COORD data[pixel_num * pixel_num], int data_top){
    COORD move[8] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}}; // 動く方向のタプル
    COORD tmp = now_pos;  // 現在の場所
    RGBA tmp_color;  // 一次的に保存する色
    int a;  // フラグ変数
    for(int i = 0; i < 8; i++){
        a = true;  // フラグの初期化
        tmp = now_pos; // 現在地に戻す
        tmp.x += move[i].x, tmp.y += move[i].y;  // 移動を行う
        if(tmp.x < 0 || tmp.x >= pixel_num || tmp.y < 0 || tmp.y >= pixel_num) continue;  // 外側にはみ出る場合調査を行わない

        // すでに探索を行ったかどうか
        for(int j = 0; j < data_top; j++){
            if(tmp.x == data[j].x && tmp.y == data[j].y){
                a = false;
                break;
            }
        }
        
        // 色の差を見て適しているかどうか
        if(comparison_rgba(src_color, map[tmp.x][tmp.y]) == true && a ==true){
            normal_color_copy(&map[tmp.x][tmp.y], &data_color, &tmp_color);  // 色のコピー
            draw_pixel_art(pixel_size, tmp.x, tmp.y, &tmp_color, layer);  // 描く
            // HgSleep(0.01);
            map[tmp.x][tmp.y] = tmp_color;  // 色を上書きする
            data[data_top] = tmp;  // 探索済みにする
            // printf("%d %d\n", data[data_top].x, data[data_top].y); // 確認用
            data_top++;  // 探索地点を一つ増やす
            // printf("%d\n", data_top);  // 確認用
            data_top = backet_calc(pixel_num, pixel_size, layer, map, tmp, src_color, data_color, data, data_top);  // 次の座標に移る
        }
    }

    return data_top;
}

// 線を引くときの計算
void draw_line_calcx(int *x, COORD *data, int *data_num, int i){
    data->x = *x;
    data->y = i;
    *data_num += 1;
}

void draw_line_calcy(int *y, COORD *data, int *data_num, int i){
    data->x = i;
    data->y = *y;
    *data_num += 1;
}


// 直線を引く処理
void draw_line(int pixel_num, double pixel_size, int layer, RGBA now_color, RGBA map[pixel_num][pixel_num], COORD data1, COORD data2, int pen_size){
    int surplus;  // 剰余
    int div;  // 二つの点の差の商
    int i, j;  // ループ変数
    int data_num = 0;
    int tmpx, tmpy;  // 一時的な座標の使用
    int line = true;  // lineをひとつ多く描くかどうかの判断

    COORD change_pixel_data[10000];
    
    if((data1.x != data2.x || data1.y != data2.y) && abs(data1.x - data2.x) != 0 && abs(data1.y - data2.y) != 0){
        // xの幅の方が大きい場合
        if(abs(data1.x - data2.x) > abs(data1.y - data2.y)){  // 絶対値
            div = (abs(data1.x - data2.x) + 1) / (abs(data1.y - data2.y) + 1) ;
            surplus = (abs(data1.x - data2.x) + 1) % (abs(data1.y - data2.y) + 1);
            // printf("div %d\nsur %d\n", div, surplus);
            // それぞれの場合分け(y, xそれぞれの大小で判断)
            tmpx = data1.x;  // xの値を変化させるための変数
            if(data1.y > data2.y){
                for(i = data1.y; i >= data2.y; i--){
                    line = true;
                    for(j = 0; j < div; j++){            
                        if(surplus > 0 && line == true){
                            draw_line_calcx(&tmpx, &change_pixel_data[data_num], &data_num, i);
                            surplus--;
                            line = false;
                            if(data1.x > data2.x) tmpx--; 
                            else tmpx++;
                        }
                        draw_line_calcx(&tmpx, &change_pixel_data[data_num], &data_num, i);
                        if(data1.x > data2.x) tmpx--;
                        else tmpx++;
                    }
                }
            }else{
                for(i = data1.y; i <= data2.y; i++){
                    line = true;
                    for(j = 0; j < div; j++){            
                        if(surplus > 0 && line == true){
                            draw_line_calcx(&tmpx, &change_pixel_data[data_num], &data_num, i);
                            surplus--;
                            line = false;
                            if(data1.x > data2.x) tmpx--;
                            else tmpx++;
                        }
                        draw_line_calcx(&tmpx, &change_pixel_data[data_num], &data_num, i);
                        if(data1.x > data2.x) tmpx--;
                        else tmpx++;
                    }
                }
            }
        }

        // yの幅の方が大きい場合
        else{
            div = (abs(data1.y - data2.y) + 1) / (abs(data1.x - data2.x) + 1);
            surplus = (abs(data1.y - data2.y) + 1) % (abs(data1.x - data2.x) + 1);
            // printf("div %d\nsur %d\n", div, surplus);
            // それぞれの場合分け(y, xそれぞれの大小で判断)
            tmpy = data1.y;  // yの値を変化させるための変数
            if(data1.x > data2.x){
                for(i = data1.x; i >= data2.x; i--){
                    line = true;
                    for(j = 0; j < div; j++){
                        if(surplus > 0 && line == true){
                            draw_line_calcy(&tmpy, &change_pixel_data[data_num], &data_num, i);
                            surplus--;
                            line = false;
                            if(data1.y > data2.y) tmpy--;
                            else tmpy++;
                        }
                        draw_line_calcy(&tmpy, &change_pixel_data[data_num], &data_num, i);
                        if(data1.y > data2.y) tmpy--;
                        else tmpy++;
                    }
                }
            }else{
                for(i = data1.x; i <= data2.x; i++){
                    line = true;
                    for(j = 0; j < div; j++){
                        if(surplus > 0 && line == true){
                            draw_line_calcy(&tmpy, &change_pixel_data[data_num], &data_num, i);
                            surplus--;
                            line = false;
                            if(data1.y > data2.y) tmpy--;
                            else tmpy++;
                        }
                        draw_line_calcy(&tmpy, &change_pixel_data[data_num], &data_num, i);
                        if(data1.y > data2.y) tmpy--;
                        else tmpy++;
                    }
                }
            }
        }  
    }

    // もしも上下の直線だった場合
    else if(data1.x - data2.x == 0){
        if(data1.y > data2.y) int_switch(&data1.y, &data2.y);
        for(i = data1.y; i <= data2.y; i++){
            change_pixel_data[data_num].x = data1.x, change_pixel_data[data_num].y = i;
            data_num++;
        }
    }

    // もしも左右の直線だった場合
    else if(data1.y - data2.y == 0){
        if(data1.x > data2.x) int_switch(&data1.x, &data2.x);
        for(i = data1.x; i <= data2.x; i++){
            change_pixel_data[data_num].x = i, change_pixel_data[data_num].y = data1.y;
            data_num++;
        }
    }

    RGBA tmp_color;  // 一時的な色
    int data_add = data_num;  // データを除外するための機能じゃい
    // 表示とデータの保存を行う
    for(i = 0; i < data_num; i++){
        // printf("%d %d\n", change_pixel_data[i].x, change_pixel_data[i].y);
        for(int j = -pen_size; j <= pen_size; j++){
            for(int k = -pen_size; k <= pen_size; k++){
                // 範囲外であれば確認をしない
                if(abs(j) + abs(k) > pen_size) continue;
                if(change_pixel_data[i].x + j < 0 || change_pixel_data[i].y + k < 0 || change_pixel_data[i].x + j >= pixel_num || change_pixel_data[i].y + k >= pixel_num)
                    continue;

                for(int roop = 0; roop < data_add; roop++){
                    // 被りがあればブレイク
                    if(change_pixel_data[i].x + j == change_pixel_data[roop].x && change_pixel_data[i].y + k == change_pixel_data[roop].y && (k != 0 || j != 0)) break;
                    // 被りがなかった場合ブレイクせずに描く
                    if(roop == data_add - 1){
                        draw_pixel_art(pixel_size, change_pixel_data[i].x + j, change_pixel_data[i].y + k, &now_color, layer);
                        normal_color_copy(&map[change_pixel_data[i].x + j][change_pixel_data[i].y + k], &now_color, &tmp_color);
                        map[change_pixel_data[i].x + j][change_pixel_data[i].y + k] = tmp_color;
                        change_pixel_data[data_add].x = change_pixel_data[i].x + j, change_pixel_data[data_add].y = change_pixel_data[i].y + k;
                        data_add++;
                        break;
                    }
                }
            }
        }
    }
}

// ゲームモードが消しゴムモードになっている際のポップアップの動作
void popup_eraser(int *popup, int *window_popup, int *popup_state, int *popup_move){
    // ウィンドウの設定
    *window_popup = HgWOpen(1050.0, 150.0, 250.0, 100.0);
    HgWSetEventMask(*window_popup, HG_MOUSE_DOWN | HG_MOUSE_DRAG | HG_KEY_DOWN);
    HgWSetTitle(*window_popup, "ERASER POWER");
    *popup_state = HgWAddLayer(*window_popup);
    *popup_move = HgWAddLayer(*window_popup);

    // 中身の設定
    eraser_init(*popup_state);
    eraser_move_circle(*popup_move, 230.0);
    *popup = true;
}

// 線を引く動作の時の円を描く
void draw_line_circle(double pixel_size, int layer, int x, int y){
    HgWSetColor(layer, HgRGB(0.3, 0.3, 0.3));
    HgWCircle(layer, (x + 0.5) * pixel_size, (y + 0.5) * pixel_size, MINI_CIRCLE_SIZE);
    HgWSetColor(layer, HgRGB(0.7, 0.7, 0.7));
    HgWCircle(layer, (x + 0.5) * pixel_size, (y + 0.5) * pixel_size, MINI_CIRCLE_SIZE - 1);
}

