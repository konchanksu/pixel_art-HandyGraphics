#include "opening.h"

//  オープニング画面を描く
int draw_opening(void){
    //　変数の宣言
    double screen_width, screen_height;  // スクリーンの大きさ
    int open_wid;  // ウィンドウid
    int layer_main, layer_sub;  // この関数内でのレイヤー分け
    int cont = true;  // 続きから始めるかどうか
    int roop = true;  // ループ
    hgevent *event;  // イベント取得用変数

    // オープニング画面を開く
    HgScreenSize(&screen_width, &screen_height);
    open_wid = HgWOpen((screen_width - 600) / 2, (screen_height - 600) / 2, 600, 600);
    HgWSetTitle(open_wid, "pixel art opening");

    // レイヤーを分ける
    layer_sub = HgWAddLayer(open_wid);
    layer_main = HgWAddLayer(open_wid);

    // デザインの設定
    HgWSetFont(layer_main, HG_T, 80);
    HgWText(layer_main, 90, 370, "PIXEL  ART");

    HgWSetFont(layer_main, HG_T, 40);
    HgWText(layer_main, 230, 200, "load");
    HgWText(layer_main, 230, 150, "new art");

    HgWSetFont(layer_main, HG_T, 15);
    HgWText(layer_main, 260, 110, "press enter");

    HgWSetFillColor(layer_sub, HG_GRAY);
    HgWBoxFill(layer_sub, 225, 200, 170, 45, 0);

    // 取得イベントの制限
    HgWSetEventMask(open_wid, HG_KEY_DOWN);

    while (roop) {
        event = HgEvent();
        // キーボードを押した時の変化
        switch (event->ch) {
            case HG_U_ARROW: //上を押した時
                cont = true;
                HgLClear(layer_sub);
                HgWBoxFill(layer_sub, 225, 200, 170, 45, 0);
                break;

            case HG_D_ARROW: // 下を押した時
                cont = false;
                HgLClear(layer_sub);
                HgWBoxFill(layer_sub, 225, 150, 170, 45, 0);
                break;

            case HG_ENTER: // エンターキーを押した時
                roop = false;
                HgWClose(open_wid);
                return cont;

            case 'q': // 強制終了
                HgWClose(open_wid);
                exit(0);
        }
    }
    return 0;
}
