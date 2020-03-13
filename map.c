#include "map.h"

// マップを初期化する
void map_list_init(int pixel_num, RGBA copyer[pixel_num][pixel_num]){
    for(int i = 0; i < pixel_num; i++){
        for(int j = 0; j < pixel_num; j++){
            copyer[i][j].red = RGB_MAX;
            copyer[i][j].green = RGB_MAX;
            copyer[i][j].blue = RGB_MAX;
            copyer[i][j].alpha = 1;
        } 
    }
}


// 入れ替えを行う
void int_switch(int *a, int *b){
    int tmp = *a;
    *a = *b;
    *b = tmp;
}


// 1マスコピー
void copy_piece(RGBA *copyer, RGBA *copyee){
    copyer->red = copyee->red;
    copyer->green = copyee->green;
    copyer->blue = copyee->blue;
    copyer->alpha = copyee->alpha;
}


// map_listをcopyする
// copyer:被コピー者, copyee:コピーする側
void map_list_copy(int pixel_num, RGBA copyer[pixel_num][pixel_num], RGBA copyee[pixel_num][pixel_num]){
    for(int i = 0; i < pixel_num; i++){
        for(int j = 0; j < pixel_num; j++){
            copy_piece(&copyer[i][j], &copyee[i][j]);
        } 
    }
}


// map表示
void print_map(int pixel_num, RGBA map_data_list[][pixel_num]){
    for(int i = 0; i < pixel_num; i++){
        for(int j = 0; j < pixel_num; j++){
            printf("%f %f %f %f\n", map_data_list[i][j].alpha, map_data_list[i][j].red, map_data_list[i][j].green, map_data_list[i][j].blue);
        }
    }
}


// 色のコピー
void normal_color_copy(RGBA *src, RGBA *dst, RGBA *out){
    out->red = dst->red * dst->alpha + src->red * (1 - dst->alpha);
    out->green = dst->green * dst->alpha + src->green * (1 - dst->alpha);
    out->blue = dst->blue * dst->alpha + src->blue * (1 - dst->alpha);
    out->alpha = 1.0; // mapに保存される色は常に透明度を1に設定し考慮しないことにする
}


// マップポジションを加算する
void map_position_add(map_struct *a){
    a->top_data++, a->now_print++;
}


// マップポジションを減算する
void map_position_sub(map_struct *a){
    a->top_data--, a->now_print--;
}


int get_path(char path[MAXPATHLEN]){
	if(!getcwd(path, MAXPATHLEN))
        //  printf("%s\n", path);
        return false;
    return true;
}


// csvファイルに書き込みを行う
int file_write(int pixel_num, RGBA map[pixel_num][pixel_num]){
    FILE *fp;
    int tmp;
    char path[MAXPATHLEN];
    get_path(path);
    strcat(path, "/.save.csv");

    if ((fp = fopen(path, "w")) == NULL) {
		tmp = HgAlert("保存できませんでした", "OK", NULL, NULL);
        if(tmp == 0)
		    return false;	// 書き込みに失敗した
	}

    fprintf(fp, "%d\n", pixel_num);

    for(int i = 0; i < pixel_num; i++){
        for(int j = 0; j < pixel_num; j++){
            // 赤　緑　青 透明度の順で保存する
            // printf("%f %f %f %f", map[i][j].red, map[i][j].green, map[i][j].blue, map[i][j].alpha);
            fprintf(fp, "%lf,", map[i][j].red);
            fprintf(fp, "%lf,", map[i][j].green);
            fprintf(fp, "%lf,", map[i][j].blue);
            fprintf(fp, "%lf\n", map[i][j].alpha);
        }
    }

    fclose(fp);
    tmp = HgAlert("保存しました", "OK", NULL, NULL);
    if(tmp == 0)
        return true;  // 書き込みに成功した
    
    return true;
}


// csvファイルから読み込む
int file_read(int pixel_num, RGBA map[pixel_num][pixel_num]){
    FILE *fp;
    int tmp;
    char path[MAXPATHLEN];
    get_path(path);
    strcat(path, "/.save.csv");

    if ((fp = fopen(path, "r")) == NULL) {
		tmp = HgAlert("読み込めませんでした", "OK", NULL, NULL);
        if(tmp == 0)
		    return false;	// 書き込みに失敗した
    }

    fscanf(fp, "%d\n", &tmp);
    if(tmp != pixel_num){
        // printf("%d %d\n", tmp, pixel_num);
        return false;
    }

    // 書き込みを行う
    for(int i = 0; i < pixel_num; i++){
        for(int j = 0; j < pixel_num; j++){
            fscanf(fp, "%lf,", &map[i][j].red);
            fscanf(fp, "%lf,", &map[i][j].green);
            fscanf(fp, "%lf,", &map[i][j].blue);
            fscanf(fp, "%lf\n", &map[i][j].alpha);
        }
    }
    //　ファイルを閉じる
    fclose(fp);
    tmp = HgAlert("読み込みました", "OK", NULL, NULL);
    if(tmp == 0)
        return true;  // 書き込みに成功した

    return true;  // 書き込みに成功した
}

// RGBA構造体同士の比較
int comparison_rgba(RGBA a, RGBA b){
    // alphaは実質的に色の変化がないので含まないようにする
    if(abs((int)(a.red - b.red)) > BACKET_POWER || abs((int)(a.green - b.green)) > BACKET_POWER || abs((int)(a.blue - b.blue)) > BACKET_POWER) return false;
    return true;
}

// maptopが配列のオーバーフローを起こさないようにする処理
void check_maptop_over(int pixel_num, map_struct *map_position, RGBA map_data_list[MAP_TOP_MAX][pixel_num][pixel_num]){
    if(map_position->top_data >= MAP_TOP_MAX - 1){
        for(int i = 0; i < MAP_TOP_MAX - 2; i++){
            map_list_copy(pixel_num, map_data_list[i], map_data_list[i + 1]);
        }
        map_position_sub(map_position);
    }   
}


// もしzなどを使ってnowとtopの位置がずれていた場合topの位置を修正
void top_now_position_revision(int pixel_num, map_struct *map_position, RGBA map_data_list[MAP_TOP_MAX][pixel_num][pixel_num]){
    if(map_position->now_print != map_position->top_data - 1){  
        map_position->top_data = map_position->now_print + 1;
        map_list_copy(pixel_num, map_data_list[map_position->top_data], map_data_list[map_position->now_print]);
    }
}
