#include "keyboard.h"

// handyを起動する
void keyboard_open(int *window_id, int *lay){
    double width, height;  // 高さ,　幅の指定
    HgScreenSize(&width, &height);
    width /= 2, height /= 2; // 中心座標を取得

    // ウィンドウの設定 
    *window_id = HgWOpen(width - WID_WIDTH, height - WID_HEIGHT, WID_WIDTH * 2, WID_HEIGHT * 2);
    *lay = HgWAddLayer(*window_id);
    
    HgWSetTitle(*window_id, "KeyBoard");  // タイトルの設定
    HgWSetFont(*lay, HG_T, 30.0);
    HgWText(*lay, 180.0, WID_HEIGHT * 2 - 40.0, "Please enter image name");
}


int get_keyboard(char input_key[MAX_KEY_DATA]){
    // ウィンドウの起動
    int window_id;
    int layer_state;
    int layer_move;

    input_key[0] = '\0'; 

    keyboard_open(&window_id, &layer_state);
    
    layer_move = HgWAddLayer(window_id);
    HgWSetFont(layer_move, HG_T, 40.0);

    // キーボード入力履歴の設定
    int key_times = 0;

    // キーボード入力用の変数の設定
    char key_id[2];
    key_id[1] = '\0';

    // 入力の無限ループ
    while(true){
        key_id[0] = HgWGetChar(window_id);
        
        if(key_id[0] == HG_ESCAPE){
            HgWClose(window_id);
            return false;           
        } else if(key_id[0] == HG_BACK || key_id[0] == 'H'){
            input_key[key_times - 1] = '\0';
            HgLClear(layer_move);
            HgWText(layer_move, 10.0, 30.0, input_key);
            key_times--;
        } else if(key_id[0] == HG_ENTER || key_id[0] == 'J'){
            HgWClose(window_id);
            return true;
        }

        if(key_times < MAX_KEY_DATA - 2){
            // printf("%s\n", input_key);
            if((key_id[0] >= 'a' && key_id[0] <= 'z') || (key_id[0] >= '0' && key_id[0] <= '9') || key_id[0] == HG_UNDERBAR){
                strcat(input_key, key_id);
                HgLClear(layer_move);
                HgWText(layer_move, 10.0, 30.0, input_key);
                key_times++;
            }
            
        }
    }
}

// 数値入力のボタンを設定する
void number_button_open(int *window_id, int *lay){
    double width, height;  // 高さ,　幅の指定
    HgScreenSize(&width, &height);
    width /= 2, height /= 2; // 中心座標を取得

    // ウィンドウの設定 
    *window_id = HgWOpen(width - WID_WIDTH, height - WID_HEIGHT, WID_WIDTH * 2, WID_HEIGHT * 2);
    *lay = HgWAddLayer(*window_id);
    
    HgWSetTitle(*window_id, "InputNumber");  // タイトルの設定

    HgWSetFillColor(*window_id, HgRGB(0.1, 0.1, 0.9));
    HgWBoxFill(*window_id, 30, 20, 100, 80, 0);

    HgWSetFillColor(*window_id, HgRGB(0.9, 0.1, 0.1));
    HgWBoxFill(*window_id, WID_WIDTH * 2 - 130, 20, 100, 80, 0);
}


void number_get(int *number){
    int window, lay, lay2;
    number_button_open(&window, &lay);

    lay2 = HgWAddLayer(window);

    HgWSetFont(lay, HG_T, 30.0);
    HgWText(lay, WID_WIDTH - 90.0, 60.0, "pixel size get"); 

    HgWSetFont(lay2, HG_T, 30.0);
    HgWText(lay2, WID_WIDTH - 20.0, 20.0, "%d", *number); 
    HgWSetEventMask(window, HG_MOUSE_DOWN | HG_KEY_DOWN);

    while(true){
        hgevent *event = HgEvent();
        if(event->type == HG_MOUSE_DOWN){
            if(event->x >= 30 && event->x <= 130 && event->y >= 20 && event->y <= 100 && *number >= 6){
                *number -= 5;
                HgLClear(lay2);
                HgWText(lay2, WID_WIDTH - 20.0, 20.0, "%d", *number); 
            }else if(event->x >= WID_WIDTH * 2 - 130 && event->x <= WID_WIDTH * 2 - 30 && event->y >= 20 && event->y <= 100 && *number <= 99){
                *number += 5;
                HgLClear(lay2);
                HgWText(lay2, WID_WIDTH - 20.0, 20.0, "%d", *number); 
            }
        }else if(event->type == HG_KEY_DOWN && (event->ch == 'q' || event->ch == HG_ENTER || event->ch == HG_ESCAPE)){
            break;
        }
    }

    HgWClose(window);
}
