#pragma once

#include <afxwin.h>

#include <vector>
#include <map>
#include <cstdint>

#include <util/common/math/complex.h>
#include <util/common/plot/plot.h>
#include <util/common/math/fft.h>
#include <util/common/math/vec.h>
#include <util/common/math/raster.h>
#include <util/common/geom/geom.h>

#include <opencv2/opencv.hpp>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif // !M_PI
#ifndef M_E
#define M_E 2.7182818284590452353602874713527
#endif // !M_PI


namespace model
{

    /*****************************************************/
    /*                     params                        */
    /*****************************************************/

    struct parameters
    {
        size_t count;
        bool random_size;
    };

    inline parameters make_default_parameters()
    {
        parameters p =
        {
            3
        };
        return p;
    }

    inline plot::drawable::ptr_t make_bmp_plot(CBitmap & b)
    {
        return plot::custom_drawable::create([&b] (CDC & dc, const plot::viewport & vp)
        {
            if (!b.m_hObject) return;
            CDC memDC; memDC.CreateCompatibleDC(&dc);
            memDC.SelectObject(&b);
            dc.SetStretchBltMode(HALFTONE);
            auto wh = b.GetBitmapDimension();
            dc.StretchBlt(vp.screen.xmin, vp.screen.ymin,
                          vp.screen.width(), vp.screen.height(),
                          &memDC, 0, 0, wh.cx, wh.cy, SRCCOPY);
        });
    }

    /*****************************************************/
    /*                     data                          */
    /*****************************************************/

    struct bitmap
    {
        cv::Mat mat;

        bitmap & to_cbitmap(CBitmap & bmp)
        {
            std::vector < COLORREF > buf(mat.rows * mat.cols);
            for (size_t i = 0; i < mat.rows; ++i)
            for (size_t j = 0; j < mat.cols; ++j)
            {
                if (mat.channels() == 1)
                {
                    float v = mat.at < float > (i, j);
                    BYTE c = (BYTE) (v * 255);
                    buf[mat.cols * i + j] = RGB(c, c, c);
                }
                else
                {
                    auto v = mat.at < cv::Vec3b > (i, j);
                    BYTE c[3] = { (BYTE) (v[0]), (BYTE) (v[1]), (BYTE) (v[2]) };
                    buf[mat.cols * i + j] = RGB(c[0], c[1], c[2]);
                }
            }
            bmp.DeleteObject();
            bmp.CreateBitmap(mat.cols, mat.rows, 1, sizeof(COLORREF) * 8, (LPCVOID) buf.data());
            bmp.SetBitmapDimension(mat.cols, mat.rows);
            return *this;
        }
        bitmap & from_file(const std::string & path)
        {
            auto img = cv::imread(path);
            cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
            mat.create(img.rows, img.cols, CV_32F);
            for (size_t i = 0; i < mat.rows; ++i)
            for (size_t j = 0; j < mat.cols; ++j)
            {
                mat.at < float > (i, j) = img.at < std::uint8_t > (i, j) / 255.0;
            }
            return *this;
        }
        bitmap & to_file(const std::string & path)
        {
            cv::Mat out;
            cv::cvtColor(mat, out, cv::COLOR_GRAY2BGR);
            out *= 255;
            cv::imwrite(path, out);
            return *this;
        }
    };

    /*****************************************************/
    /*                     model                         */
    /*****************************************************/

    struct model_data
    {
        parameters params;
        CBitmap csource;
        bitmap source;
        CBitmap cpatterns[3];
        bitmap patterns[3];
        bitmap patternsDisplay[3];
        CBitmap cmask;
        bitmap mask;
    };

    /*****************************************************/
    /*                     algo                          */
    /*****************************************************/

    class segmentation_helper
    {
    public:

        struct autoconfig
        {
        };

        static autoconfig make_default_config()
        {
            return
            {
            };
        }
    private:
        const parameters & p;
    public:
        segmentation_helper(const parameters & p)
            : p(p)
        {
        }
    public:
        void generate(model_data & d, const autoconfig & cfg) const
        {
            d.patterns[0].mat = cv::Mat::zeros(64, 64, CV_32F);
            d.patterns[1].mat = cv::Mat::zeros(64, 64, CV_32F);
            d.patterns[2].mat = cv::Mat::zeros(64, 64, CV_32F);

            cv::circle(d.patterns[0].mat, { 32, 32 }, 32, { 1 }, CV_FILLED);
            d.patterns[1].mat.setTo(cv::Scalar(1));

            cv::Point path[1][3];
            path[0][0] = { 32, 0 };
            path[0][1] = { 0, 63 };
            path[0][2] = { 63, 63 };
            const cv::Point * ppt[1] = { path[0] };
            int npt[] = { 3 };
            cv::fillPoly(d.patterns[2].mat, ppt, npt, 1, { 1 });

            toColor(d.patterns[0].mat, d.patternsDisplay[0].mat, 2);
            toColor(d.patterns[1].mat, d.patternsDisplay[1].mat, 1);
            toColor(d.patterns[2].mat, d.patternsDisplay[2].mat, 0);

            d.patternsDisplay[0].to_cbitmap(d.cpatterns[0]);
            d.patternsDisplay[1].to_cbitmap(d.cpatterns[1]);
            d.patternsDisplay[2].to_cbitmap(d.cpatterns[2]);

            d.source.mat = cv::Mat::zeros(512, 512, CV_32F);

            for (size_t n = 0; n < p.count; ++n) {
                size_t patt = rand() % 3;
                double alpha = rand() / (RAND_MAX + 1.0);
                double scale = 1 + 0.5 * (2 * rand() / (RAND_MAX + 1.0) - 1);
                cv::Mat img = d.patterns[patt].mat.clone();
                img *= alpha;
                if (p.random_size) cv::resize(img, img, {}, scale, scale);
                size_t r = rand() % (d.source.mat.rows - img.rows);
                size_t c = rand() % (d.source.mat.cols - img.cols);
                img.copyTo(d.source.mat
                    .colRange(c, c + img.cols)
                    .rowRange(r, r + img.rows));
            }

            d.source.to_cbitmap(d.csource);
        }
    private:
        void toColor(const cv::Mat & src, cv::Mat & dst, int c0) const
        {
            src.convertTo(dst, CV_8U, 255);
            cv::cvtColor(dst, dst, CV_GRAY2BGR);
            for (size_t r = 0; r < dst.rows; ++r)
            for (size_t c = 0; c < dst.cols; ++c)
            {
                if (c0 != 0) dst.at<cv::Vec3b>(r, c)[0] = 0;
                if (c0 != 1) dst.at<cv::Vec3b>(r, c)[1] = 0;
                if (c0 != 2) dst.at<cv::Vec3b>(r, c)[2] = 0;
            }
        }
    public:
        void process(model_data & d, const autoconfig & cfg) const
        {
            d.mask.mat = cv::Mat::zeros(d.source.mat.size(), CV_8UC3);

            std::vector<cv::Mat> images(3);

            cv::Mat mask;
            cv::Mat patt;
            if (!d.patterns[0].mat.empty()) {
                d.patterns[0].mat.convertTo(patt, CV_8U, 255);
                cv::erode(d.source.mat, mask, patt);
                cv::flip(patt, patt, 0);
                cv::dilate(mask, mask, patt);
                mask.convertTo(mask, CV_8U, 255);
                images[2] = mask.clone();
            } else
                images[2] = d.mask.mat.clone();
            if (!d.patterns[1].mat.empty()) {
                d.patterns[1].mat.convertTo(patt, CV_8U, 255);
                cv::erode(d.source.mat, mask, patt);
                cv::flip(patt, patt, 0);
                cv::dilate(mask, mask, patt);
                mask.convertTo(mask, CV_8U, 255);
                images[1] = mask.clone();
            } else
                images[1] = d.mask.mat.clone();
            if (!d.patterns[2].mat.empty()) {
                d.patterns[2].mat.convertTo(patt, CV_8U, 255);
                cv::erode(d.source.mat, mask, patt);
                cv::flip(patt, patt, 0);
                cv::dilate(mask, mask, patt);
                mask.convertTo(mask, CV_8U, 255);
                images[0] = mask.clone();
            } else
                images[0] = d.mask.mat.clone();

            cv::merge(images, d.mask.mat);

            d.mask.to_cbitmap(d.cmask);
        }
    };
}
