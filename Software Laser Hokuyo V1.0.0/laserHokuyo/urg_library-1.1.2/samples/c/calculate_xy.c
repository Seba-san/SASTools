/*!
*/

#include "urg_sensor.h"
#include "urg_utils.h"
#include "open_urg_sensor.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
    urg_t urg;
    long *data;
    long max_distance;
    long min_distance;
    long time_stamp;
    int i;
    int n;

    if (open_urg_sensor(&urg, argc, argv) < 0) {
        return 1;
    }

    data = (long *)malloc(urg_max_data_size(&urg) * sizeof(data[0]));
    if (!data) {
        perror("urg_max_index()");
        return 1;
    }

    // \~japanese データ取得
    urg_start_measurement(&urg, URG_DISTANCE, 1, 0);
    n = urg_get_distance(&urg, data, &time_stamp);
    if (n < 0) {
        printf("urg_get_distance: %s\n", urg_error(&urg));
        urg_close(&urg);
        return 1;
    }

    // \~japanese X-Y 座標系の値を出力
    urg_distance_min_max(&urg, &min_distance, &max_distance);
    for (i = 0; i < n; ++i) {
        long distance = data[i];
        double radian;
        long x;
        long y;

        if ((distance < min_distance) || (distance > max_distance)) {
            continue;
        }

        radian = urg_index2rad(&urg, i);
        x = (long)(distance * cos(radian));
        y = (long)(distance * sin(radian));

        printf("%ld, %ld\n", x, y);
    }
    printf("\n");

    // \~japanese 切断
    free(data);
    urg_close(&urg);

#if defined(URG_MSC)
    getchar();
#endif
    return 0;
}
