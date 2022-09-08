/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 07/07/2022
 * Description : a command line tool to calculate quality level using NIMA model
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes

#include <QApplication>
#include <QDebug>
#include <iostream>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/dnn.hpp"
#include <filesystem>

// Local includes

using namespace std;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

	cv::Mat img = cv::imread(argv[1]);
    if (img.empty()) {
        qDebug() << "Parse image to calculate quality score";
        qDebug() << "Usage: <image file>";
        return -1;
    }
	cv::dnn::Net net = cv::dnn::readNetFromTensorflow(argv[2]);
    if (net.empty()) {
        qDebug() << "Parse net to calculate quality score";
        qDebug() << "Usage: <image file>";
        return -1;
    }

	cv::Mat blob = cv::dnn::blobFromImage(img, 1, cv::Size(224, 224), cv::Scalar(0, 0, 0), false, false);
	net.setInput(blob);
	cv::Mat out = net.forward();
    float tmp[10] = {0,1,2,3,4,5,6,7,8,9};
    
    cv::Mat scale(1, 10, out.type(), tmp);
    cv::Scalar score = cv::sum(out.mul(scale));
    
    std::cout << "score : " << score[0] << "\n";
}
