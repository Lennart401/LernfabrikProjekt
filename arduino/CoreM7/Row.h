#ifndef ROW_H_
#define ROW_H_

struct Row {
    uint64_t timestamp;
    float acc_x;
    float acc_y;
    float acc_z;
    //float realacc_x;
    //float realacc_y;
    //float realacc_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    //float temperature;
    //float distance;
    float quaternion_w;
    float quaternion_x;
    float quaternion_y;
    float quaternion_z;
};

#endif // ROW_H_