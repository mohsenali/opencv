/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2008-2012, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#ifndef __SFT_OCTAVE_HPP__
#define __SFT_OCTAVE_HPP__

#include <opencv2/ml/ml.hpp>
#include <sft/common.hpp>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
namespace sft
{
struct ICF
{
    ICF(int x, int y, int w, int h, int ch) : bb(cv::Rect(x, y, w, h)), channel(ch) {}

    bool operator ==(ICF b)
    {
        return bb == b.bb && channel == b.channel;
    }

    bool operator !=(ICF b)
    {
        return bb != b.bb || channel != b.channel;
    }


    float operator() (const cv::Mat& integrals, const cv::Size& model) const
    {
        int step = model.width + 1;

        const int* ptr = integrals.ptr<int>(0) + (model.height * channel + bb.y) * step + bb.x;

        int a = ptr[0];
        int b = ptr[bb.width];

        ptr += bb.height * step;

        int c = ptr[bb.width];
        int d = ptr[0];

        return (float)(a - b + c - d);
    }

private:
    cv::Rect bb;
    int channel;

    friend void write(cv::FileStorage& fs, const std::string&, const ICF& f);
    friend std::ostream& operator<<(std::ostream& out, const ICF& f);
};

void write(cv::FileStorage& fs, const std::string&, const ICF& f);
std::ostream& operator<<(std::ostream& out, const ICF& m);

using cv::FeaturePool;
using cv::Dataset;

class ICFFeaturePool : public cv::FeaturePool
{
public:
    ICFFeaturePool(cv::Size model, int nfeatures);

    virtual int size() const { return (int)pool.size(); }
    virtual float apply(int fi, int si, const cv::Mat& integrals) const;
    virtual void preprocess(cv::InputArray _frame, cv::OutputArray _integrals) const;
    virtual void write( cv::FileStorage& fs, int index) const;

    virtual ~ICFFeaturePool();

private:

    void fill(int desired);

    cv::Size model;
    int nfeatures;

    std::vector<ICF> pool;

    static const unsigned int seed = 0;

    cv::ICFPreprocessor preprocessor;

    enum { N_CHANNELS = 10 };
};



class ScaledDataset : public Dataset
{
public:
    ScaledDataset(const sft::string& path, const int octave);

    virtual cv::Mat get(SampleType type, int idx) const;
    virtual int available(SampleType type) const;
    virtual ~ScaledDataset();

private:
    svector pos;
    svector neg;
};
}

#endif