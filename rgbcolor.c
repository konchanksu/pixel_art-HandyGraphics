// 定義が書かれているヘッダファイル
#include "rgbcolor.h"

// RGBの値を送り,それらの中の最小値をminとする
// minの初期値がrgb_color->redとなっている
void RGB_min_check(RGBA *rgb_color, double *min){
    if (*min > rgb_color->green) *min = rgb_color->green;
    if (*min > rgb_color->blue) *min = rgb_color->blue;
}


// RGBの値を送り,それらの中の最大値をmaxとする. idを最大値の色のidに指定する
// maxの初期値がrgb_color->redとなっている
void RGB_max_check(RGBA *rgb_color, double *max, int *id){
    if (*max < rgb_color->green) *max = rgb_color->green, *id = RGB_ID_GREEN;
    if (*max < rgb_color->blue) *max = rgb_color->blue, *id = RGB_ID_BLUE;
}


// rgbをhsvに変換する
void RGB_change_to_HSV(HSV *hsv_color, RGBA *rgb_color){
    double max = rgb_color->red;
    double min = rgb_color->red;
    int max_id = RGB_ID_RED;

    // rgbの最大値, 最小値を求める
    RGB_min_check(rgb_color, &min);
    RGB_max_check(rgb_color, &max, &max_id);
    if (rgb_color->green == rgb_color->blue && rgb_color->red == rgb_color->blue) max_id = RGB_ID_NONE;
    // printf("max:%f min:%f id:%d\n", max, min, max_id);

    // 色相を求める
    switch(max_id){
        // 赤が最大
        case RGB_ID_RED:
            hsv_color->hue = 60.0 * ((rgb_color->green - rgb_color->blue) / (max - min));
            break;
        // 緑が最大
        case RGB_ID_GREEN:
            hsv_color->hue = 60.0 * ((rgb_color->blue - rgb_color->red) / (max - min)) + 120.0;
            break;
        // 青が最大
        case RGB_ID_BLUE:
            hsv_color->hue = 60.0 * ((rgb_color->red - rgb_color->green) / (max - min)) + 240.0;
            break;
        // 全て等しい
        case RGB_ID_NONE:
            hsv_color->hue = 0;
            break;
    }

    // hueが0を下回った際に360を加算する
    if (hsv_color->hue < 0) hsv_color->hue += CIRCLE_ANGLE_MAX;

    // 彩度を求める 最大255
    hsv_color->saturation = (max - min) / max * RGB_MAX;

    // 明度を求める 最大255
    hsv_color->value = max;
    // printf("r:%f g:%f b:%f\n", rgb_color->red, rgb_color->green, rgb_color->blue);
}


// HSVをRGBに変換する
void HSV_change_to_RGB(HSV *hsv_color, RGBA *rgb_color){
    double max, min;
    max = hsv_color->value;
    min = max - ((hsv_color->saturation / RGB_MAX) * max);
    // hueの変化でrgbを変化させる
    // 60度ずつで計算を変化させる
    switch ((int) (hsv_color->hue / 60.0)) {
        case 0:
            rgb_color->red = max;
            rgb_color->green = (hsv_color->hue / 60.0) * (max - min) + min;
            rgb_color->blue = min;
            break;
        case 1:
            rgb_color->red = ((120 - hsv_color->hue) / 60.0) * (max - min) + min;
            rgb_color->green = max;
            rgb_color->blue = min;
            break;
        case 2:
            rgb_color->red = min;
            rgb_color->green = max;
            rgb_color->blue = ((hsv_color->hue - 120.0) / 60.0) * (max - min) + min;
            break;
        case 3:
            rgb_color->red = min;
            rgb_color->green = ((240.0 - hsv_color->hue) / 60.0) * (max - min) + min;
            rgb_color->blue = max;
            break;
        case 4:
            rgb_color->red = ((hsv_color->hue - 240.0) / 60.0) * (max - min) + min;
            rgb_color->green = min;
            rgb_color->blue = max;
            break;
        case 5:
            rgb_color->red = max;
            rgb_color->green = min;
            rgb_color->blue = ((360.0 - hsv_color->hue) / 60.0) * (max - min) + min;
            break;
    }
}


// 中央の濃淡を変化させる四角を描く関数
void draw_SV_box(HSV *hsv_color, RGBCLay lay){
    RGBA rgba_color; // rgbaカラーを設定する
    HSV tmp_hsv; // 一時的な保管先
    tmp_hsv.hue = hsv_color->hue; // 一時的な保管先hueをコピーする;

    // 四角を描く
    // s, vのループで描く
    for(double s = RGB_MIN; s < RGB_MAX; s += RGB_MAX / RGB_COUNT_BOX){
        for(double v = RGB_MIN; v < RGB_MAX; v += RGB_MAX / RGB_COUNT_BOX){
            tmp_hsv.saturation = s;
            tmp_hsv.value = v;
            // tmpをrgb方式へ変換
            HSV_change_to_RGB(&tmp_hsv, &rgba_color);

            // 描画
            HgWSetFillColor(lay.draw_layer,
                            HgRGB(rgba_color.red / RGB_MAX, rgba_color.green / RGB_MAX, rgba_color.blue / RGB_MAX));
            HgWBoxFill(lay.draw_layer, SV_BOX_LIGHT_BOTTOM + s * SV_BOX_WIDTH / RGB_MAX,
                       SV_BOX_LIGHT_BOTTOM + v * SV_BOX_WIDTH / RGB_MAX,
                       SV_BOX_WIDTH / RGB_COUNT_BOX, SV_BOX_WIDTH / RGB_COUNT_BOX, 0);
        }
    }
}


// 外側の虹色の円を描く
void draw_hue_circle(HSV *hsv_color, RGBCLay lay) {
    RGBA rgb_color; // rgbaカラーを設定する
    rgb_color.red = 0, rgb_color.green = 0, rgb_color.blue = 0;

    // saturation, valueは最大値で固定
    hsv_color->saturation = RGB_MAX, hsv_color->value = RGB_MAX;

    // 角度を1度ずつずらすごとにhueを1ずらして表示する
    for (double i = 0; i < CIRCLE_ANGLE_MAX; i++) {
        hsv_color->hue = i;                        // hのみ動かす
        HSV_change_to_RGB(hsv_color, &rgb_color);  // rgbに変換する
        double x = cos(i / 180 * M_PI);            // xの座標(半径1)
        double y = sin(i / 180 * M_PI);            // yの座標(半径1)

        // 線の色を変更してhueのみのサークルを描く
        HgWSetColor(lay.draw_layer,
                    HgRGB(rgb_color.red / RGB_MAX, rgb_color.green / RGB_MAX, rgb_color.blue / RGB_MAX));

        // 中央からxyに線を引く
        HgWLine(lay.draw_layer, 0, 0, x * H_CIRCLE_RAD_OUT, y * H_CIRCLE_RAD_OUT);
    }

    // 内側を白で覆う
    HgWSetFillColor(lay.draw_layer, HG_WHITE);
    HgWCircleFill(lay.draw_layer, 0, 0, H_CIRCLE_RADIUS, 0);
}


// 箱の中で動かす円を描く
void draw_move_circle(int x, int y, int move_layer) {
    HgLClear(move_layer);
    HgWSetColor(move_layer, HG_WHITE);
    HgWCircle(move_layer, x, y, MINI_CIRCLE_SIZE);
    HgWSetColor(move_layer, HG_BLACK);
    HgWCircle(move_layer, x, y, MINI_CIRCLE_SIZE - 1);
}


// 現在の色が確認できる箱を作成する
void draw_now_colorbox(RGBCLay lay, RGBA rgb_now_color) {
    // 右上に箱を作成する
    HgLClear(lay.now_color_box);
    HgWSetFillColor(lay.now_color_box,
                    HgRGBA(rgb_now_color.red / RGB_MAX, rgb_now_color.green / RGB_MAX, rgb_now_color.blue / RGB_MAX,
                           rgb_now_color.alpha));
    HgWBoxFill(lay.now_color_box, H_CIRCLE_RAD_OUT / sqrt(2) + 20, H_CIRCLE_RAD_OUT / sqrt(2) + 20, 30, 30, 1);
}


// 透明度を操作する円を作成する
void draw_alpha_circle(int x, RGBCLay lay) {
    HgLClear(lay.alpha_move);
    HgWSetColor(lay.alpha_move, HG_BLACK);
    HgWCircle(lay.alpha_move, x, -105 ,MINI_CIRCLE_SIZE); // 線を描いてから作成する
}

// hueを操作する円を描く
void draw_hue_move_circle(RGBCLay lay, HSV *hsv_color){
    HgLClear(lay.hue_move);
    HgWCircle(lay.hue_move,
        cos(hsv_color->hue / 180.0 * M_PI) * (H_CIRCLE_RADIUS + H_CIRCLE_RAD_OUT) / 2,
        sin(hsv_color->hue / 180.0 * M_PI) * (H_CIRCLE_RADIUS + H_CIRCLE_RAD_OUT) / 2,
        MINI_CIRCLE_SIZE);
}


// カラーサークルを表示する集合の関数
void draw_color_circle(RGBA rgb_now_color, RGBCLay layerdata) {
    HSV hsv_data; // 実際に用いるhsvの色データ
    RGBA rgb_data; // 実際に用いるrgbの色データ

    // rgbdataの初期化
    rgb_data.red = RGB_MAX, rgb_data.green = RGB_MIN, rgb_data.blue = RGB_MIN;

    // HSVカラーに変換
    RGB_change_to_HSV(&hsv_data, &rgb_data);
    // printf("r:%f g:%f b:%f\n", rgbdata.red, rgbdata.green, rgbdata.blue);
    // printf("h:%f s:%f v:%f\n", hsvdata.hue, hsvdata.saturation, hsvdata.value);

    rgb_data.blue = 1.0; // 青を1にする

    // 下の部分に透明度を変更する箱を表示する
    HgWSetColor(layerdata.draw_layer, HG_BLACK);
    HgWBox(layerdata.draw_layer, -100,
           -110, 200, 10);

    // 取得イベントの設定と線の幅を設定する
    HgWSetEventMask(layerdata.draw_layer, HG_MOUSE_DRAG | HG_MOUSE_DOWN | HG_MOUSE_UP);
    HgWSetWidth(layerdata.draw_layer, 2.2);

    // 描画
    draw_hue_circle(&hsv_data, layerdata);
    draw_SV_box(&hsv_data, layerdata);

    // 描画をクリアする
    HgLClear(layerdata.box_move);

    // hを操作する円
    draw_hue_move_circle(layerdata, &hsv_data);

    // 箱を動かす円
    draw_move_circle(-SV_BOX_LIGHT_BOTTOM, -SV_BOX_LIGHT_BOTTOM, layerdata.box_move);

    // 透明度操作する円
    draw_alpha_circle(100, layerdata);

    // 右上に箱を作成する
    draw_now_colorbox(layerdata, rgb_now_color);
}


// saturation, valueの値を変更し描画する
void sat_value_move(RGBA *rgb_color, HSV *hsv_color, RGBCLay lay, hgevent *event){
    while (true) {
        event = HgEventNonBlocking();
        if (event != NULL && event->type != HG_KEY_DOWN) {
        // 座標を作成する
            double x = event->x - (WIN_RGBCIR_SIZE / 2);
            double y = event->y - (WIN_RGBCIR_SIZE / 2);       

            // 範囲外に出た場合の処理
            if (x > -SV_BOX_LIGHT_BOTTOM) {
                x = -SV_BOX_LIGHT_BOTTOM;
            } else if (x < SV_BOX_LIGHT_BOTTOM) {
                x = SV_BOX_LIGHT_BOTTOM;
            }
            if (y > -SV_BOX_LIGHT_BOTTOM) {
                y = -SV_BOX_LIGHT_BOTTOM;
            } else if (y < SV_BOX_LIGHT_BOTTOM) {
                y = SV_BOX_LIGHT_BOTTOM;
            }

            // 計算
            hsv_color->saturation = RGB_MAX * (x - SV_BOX_LIGHT_BOTTOM) / (-SV_BOX_LIGHT_BOTTOM * 2);
            hsv_color->value = RGB_MAX * (y - SV_BOX_LIGHT_BOTTOM) / (-SV_BOX_LIGHT_BOTTOM * 2);

            // 描画
            HgLClear(lay.box_move);
            HSV_change_to_RGB(hsv_color, rgb_color);
            draw_now_colorbox(lay, *rgb_color);
            
            draw_move_circle(x, y, lay.box_move);
            if(event->type == HG_MOUSE_UP) break; // 終了
        } 
    }               
}


// alphaの値を変更し描画する
void alpha_move(RGBA *rgb_color, HSV *hsv_color, RGBCLay lay, hgevent *event){
    while (true) {
        event = HgEventNonBlocking();
            if (event != NULL && event->type != HG_KEY_DOWN) {
                double x = event->x - (WIN_RGBCIR_SIZE / 2); // x座標の取得のみで良い

            // 範囲外に出た場合の処理   
            if (x < -100) {
                x = -100;
            } else if (x > 100) {
                x = 100;
            }
            rgb_color->alpha = (100 + x) / 200;

            // 描画
            draw_now_colorbox(lay, *rgb_color);
            draw_alpha_circle(x, lay);
            if(event->type == HG_MOUSE_UP) break; // 終了
        } 
    }
}

// hueの値を変更すし描画する
void hue_move(RGBA *rgb_color, HSV *hsv_color, RGBCLay lay, hgevent *event, int window){            
    while (true) {
        event = HgEventNonBlocking();
        if (event != NULL && event->type != HG_KEY_DOWN) {
            // 座標を取る
            double x = event->x - (WIN_RGBCIR_SIZE / 2);
            double y = event->y - (WIN_RGBCIR_SIZE / 2);

            // 色の設定
            HgWSetColor(window, HG_BLACK);

            // 計算
            double h = 0;
            double radian = atan(fabs(y) / fabs(x));
            double degree = (radian * 180) / M_PI;

            // 0~90度以外の処理
            if (x < 0) {
                if (y < 0) {
                    h += 180.0;
                } else {
                    h += 90.0;
                    degree = 90.0 - degree;
                }
            } else if (y < 0) {
                h += 270.0;
                degree = 90.0 - degree;
            }

            h += (int) degree;
            hsv_color->hue = h;

            // 描画
            draw_hue_move_circle(lay, hsv_color);
            draw_SV_box(hsv_color, lay);
            HSV_change_to_RGB(hsv_color, rgb_color);
            draw_now_colorbox(lay, *rgb_color);

            if(event->type == HG_MOUSE_UP || event == NULL) break; // 終了
        } 
        
    }
}


// 実際の動作を指定する
void color_circle_move(RGBA *rgb_color, HSV *hsv_color, RGBCLay lay, hgevent *event, int window) {
    if (event != NULL && event->wid == window) {
        if (event->type == HG_MOUSE_DOWN) {
            // 座標に変換する
            double x = event->x - (WIN_RGBCIR_SIZE / 2);
            double y = event->y - (WIN_RGBCIR_SIZE / 2);
            // printf("%f %f\n", x, y);

            // saturationとvalueを操作する
            if (SV_BOX_LIGHT_BOTTOM < x && -SV_BOX_LIGHT_BOTTOM > x && SV_BOX_LIGHT_BOTTOM < y && -SV_BOX_LIGHT_BOTTOM > y) {
                // saturationはx軸，valueはy軸に対応している
                hsv_color->saturation = RGB_MAX * (x - SV_BOX_LIGHT_BOTTOM) / SV_BOX_WIDTH;
                hsv_color->value = RGB_MAX * (y - SV_BOX_LIGHT_BOTTOM) / SV_BOX_WIDTH;
                // printf("%f %f\n", hsv_color->value, hsv_color->saturation);

                // 操作用の丸を動かす
                draw_move_circle(x, y, lay.box_move);
                sat_value_move(rgb_color, hsv_color, lay, event);     
            }

            // alpha(透明度)を操作する
            else if (x >= -115 && x <= 115 && y <= -90 && y >= -110){
                alpha_move(rgb_color, hsv_color, lay, event);
            }

            // hueの値を変更する
            else if(sqrt(x * x + y * y) >= H_CIRCLE_RADIUS && sqrt(x * x + y * y) <= H_CIRCLE_RAD_OUT){
                // クリックする際にまず描画をする
                HgLClear(lay.hue_move);
                HgWCircle(lay.hue_move,
                          cos(hsv_color->hue / 180.0 * M_PI) * (H_CIRCLE_RADIUS + H_CIRCLE_RAD_OUT) / 2,
                          sin(hsv_color->hue / 180.0 * M_PI) * (H_CIRCLE_RADIUS + H_CIRCLE_RAD_OUT) / 2,
                          MINI_CIRCLE_SIZE);

                draw_SV_box(hsv_color, lay);
                HSV_change_to_RGB(hsv_color, rgb_color);
                draw_now_colorbox(lay, *rgb_color);
                
                // hueの値を変更すし描画する
                hue_move(rgb_color, hsv_color, lay, event, window);
            }
        }
    }
}

//  スポイト機能を使用した際のサークルの動きをつける
void spoit_move_circle(RGBA *rgb_color, HSV *hsv_color, RGBCLay lay){
    // hsvでの色の作成
    RGB_change_to_HSV(hsv_color, rgb_color);

    //　アルファ値は1にする　
    draw_alpha_circle(100, lay);

    // 現在色の表示を変更する
    draw_now_colorbox(lay, *rgb_color);

    // svの円を動かす
    double x = hsv_color->saturation * (-SV_BOX_LIGHT_BOTTOM * 2) / RGB_MAX + SV_BOX_LIGHT_BOTTOM;
    double y = hsv_color->value * (-SV_BOX_LIGHT_BOTTOM * 2) / RGB_MAX + SV_BOX_LIGHT_BOTTOM;
    draw_move_circle(x, y, lay.box_move);

    // hueの円を動かす
    draw_hue_move_circle(lay, hsv_color);

    // svの大きい箱の色を変更する
    draw_SV_box(hsv_color, lay);
}


// メインの書き方 ここは保存しない
/*int main(void){
    RGBA *rgb_color = NULL; // rgbのポインタ
    HSV *hsv_color = NULL; // hsv色のポインタ

    rgb_color = (RGBA*)malloc(sizeof(RGBA));
    hsv_color = (HSV*)malloc(sizeof(HSV));

    rgb_color->red = RGB_MAX, rgb_color->green = RGB_MIN, rgb_color->blue = RGB_MIN, rgb_color->alpha = 1;
    RGB_change_to_HSV(hsv_color, rgb_color);

    // 座標の位置を変更する
    int windowid = HgWOpen(400, 400, 250, 250);
    HgWCoordinate(windowid, WIN_RGBCIR_SIZE / 2, WIN_RGBCIR_SIZE / 2, 1);

    // レイヤの宣言
    RGBCLay layerdata;
    layerdata.now_color_box = HgWAddLayer(windowid);
    layerdata.draw_layer = HgWAddLayer(windowid);
    layerdata.hue_move = HgWAddLayer(windowid);
    layerdata.box_move = HgWAddLayer(windowid);
    layerdata.alpha_move = HgWAddLayer(windowid);

    draw_color_circle(*rgb_color, layerdata);
    hgevent *event;
    HgWSetEventMask(windowid, HG_MOUSE_DOWN | HG_MOUSE_DRAG | HG_MOUSE_UP);

    while(true){
        event = HgEventNonBlocking();
        color_circle_move(rgb_color, hsv_color, layerdata, event, windowid);
    }

    free(rgb_color);
    free(hsv_color);

    HgGetChar();
    HgClose();
    return 0;
}*/
