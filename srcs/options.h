#ifndef OPTIONS_H
#define OPTIONS_H

struct ReaderOptions
{
    /* data */
    int resize_x;
    int resize_y;
    ReaderOptions(): resize_x(-1), resize_y(-1){}
    ReaderOptions(int res_x): resize_x(res_x), resize_y(-1){}
    ReaderOptions(int res_x, int res_y): resize_x(res_x), resize_y(res_y){}
    ~ReaderOptions() = default;
};

struct FeatureExtractionOptions
{
};

#endif