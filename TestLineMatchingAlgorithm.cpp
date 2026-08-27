/*IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.

 By downloading, copying, installing or using the software you agree to this license.
 If you do not agree to this license, do not download, install,
 copy or use the software.


                          License Agreement
               For Open Source Computer Vision Library

Copyright (C) 2011-2012, Lilian Zhang, all rights reserved.
Copyright (C) 2013, Manuele Tamburrano, Stefano Fabri, all rights reserved.
Third party copyrights are property of their respective owners.

To extract edge and lines, this library implements the EDLines Algorithm and the Edge Drawing detector:
http://www.sciencedirect.com/science/article/pii/S0167865511001772
http://www.sciencedirect.com/science/article/pii/S1047320312000831

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * The name of the copyright holders may not be used to endorse or promote products
    derived from this software without specific prior written permission.

This software is provided by the copyright holders and contributors "as is" and
any express or implied warranties, including, but not limited to, the implied
warranties of merchantability and fitness for a particular purpose are disclaimed.
In no event shall the Intel Corporation or contributors be liable for any direct,
indirect, incidental, special, exemplary, or consequential damages
(including, but not limited to, procurement of substitute goods or services;
loss of use, data, or profits; or business interruption) however caused
and on any theory of liability, whether in contract, strict liability,
or tort (including negligence or otherwise) arising in any way out of
the use of this software, even if advised of the possibility of such damage.
*/


#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <opencv2/opencv.hpp>

#include "LineDescriptor.hh"

using namespace std;


void usage(int argc, char** argv)
{
    cout << "Usage: " << argv[0] << " image1.png image2.png" << endl;
}


int main(int argc, char** argv)
{
    if (argc < 3)
    {
        usage(argc, argv);
        return -1;
    }


    // =========================================================================
    // Load images
    // =========================================================================

    const std::string imageName1(argv[1]);
    const std::string imageName2(argv[2]);

    cv::Mat leftImage =
        cv::imread(imageName1, cv::IMREAD_GRAYSCALE);

    cv::Mat rightImage =
        cv::imread(imageName2, cv::IMREAD_GRAYSCALE);


    if (leftImage.empty())
    {
        cerr << "Could not open or find image: "
             << imageName1 << endl;
        return -1;
    }

    if (rightImage.empty())
    {
        cerr << "Could not open or find image: "
             << imageName2 << endl;
        return -1;
    }


    // =========================================================================
    // Create color copies for visualization
    // =========================================================================

    cv::Mat leftColorImage;
    cv::Mat rightColorImage;

    cv::cvtColor(
        leftImage,
        leftColorImage,
        cv::COLOR_GRAY2BGR
    );

    cv::cvtColor(
        rightImage,
        rightColorImage,
        cv::COLOR_GRAY2BGR
    );


    // =========================================================================
    // Detect lines + compute LBD descriptors
    // =========================================================================

    LineDescriptor lineDesc;

    ScaleLines linesInLeft;
    ScaleLines linesInRight;

    std::vector<short> matchLeft;
    std::vector<short> matchRight;


    if (lineDesc.GetLineDescriptor(
            leftImage,
            linesInLeft) < 0)
    {
        cerr << "Failed to process left image." << endl;
        return -1;
    }


    if (lineDesc.GetLineDescriptor(
            rightImage,
            linesInRight) < 0)
    {
        cerr << "Failed to process right image." << endl;
        return -1;
    }


    if (linesInLeft.empty() || linesInRight.empty())
    {
        cerr << "No lines detected in one or both images." << endl;
        return -1;
    }


    // =========================================================================
    // Match the LBD descriptors
    // =========================================================================

    if (lineDesc.MatchLineByDescriptor(
            linesInLeft,
            linesInRight,
            matchLeft,
            matchRight,
            LineDescriptor::NearestNeighbor) < 0)
    {
        cerr << "Line matching failed." << endl;
        return -1;
    }


    const std::size_t numberOfMatches =
        std::min(matchLeft.size(), matchRight.size());


    cout << "Number of matched lines: "
         << numberOfMatches
         << endl;


    // =========================================================================
    // Draw all detected lines
    // =========================================================================

    cv::RNG rngDetected(1234);


    for (std::size_t i = 0;
         i < linesInLeft.size();
         ++i)
    {
        if (linesInLeft[i].empty())
            continue;

        const cv::Scalar color(
            rngDetected.uniform(100, 255),
            rngDetected.uniform(100, 255),
            rngDetected.uniform(100, 255)
        );

        const cv::Point startPoint(
            cvRound(linesInLeft[i][0].startPointX),
            cvRound(linesInLeft[i][0].startPointY)
        );

        const cv::Point endPoint(
            cvRound(linesInLeft[i][0].endPointX),
            cvRound(linesInLeft[i][0].endPointY)
        );

        cv::line(
            leftColorImage,
            startPoint,
            endPoint,
            color,
            2,
            cv::LINE_AA
        );
    }


    for (std::size_t i = 0;
         i < linesInRight.size();
         ++i)
    {
        if (linesInRight[i].empty())
            continue;

        const cv::Scalar color(
            rngDetected.uniform(100, 255),
            rngDetected.uniform(100, 255),
            rngDetected.uniform(100, 255)
        );

        const cv::Point startPoint(
            cvRound(linesInRight[i][0].startPointX),
            cvRound(linesInRight[i][0].startPointY)
        );

        const cv::Point endPoint(
            cvRound(linesInRight[i][0].endPointX),
            cvRound(linesInRight[i][0].endPointY)
        );

        cv::line(
            rightColorImage,
            startPoint,
            endPoint,
            color,
            2,
            cv::LINE_AA
        );
    }


    cv::imwrite(
        "LinesInImage1.png",
        leftColorImage
    );

    cv::imwrite(
        "LinesInImage2.png",
        rightColorImage
    );


    // =========================================================================
    // Build side-by-side matching visualization
    // =========================================================================

    cv::Mat leftMatchImage;
    cv::Mat rightMatchImage;

    cv::cvtColor(
        leftImage,
        leftMatchImage,
        cv::COLOR_GRAY2BGR
    );

    cv::cvtColor(
        rightImage,
        rightMatchImage,
        cv::COLOR_GRAY2BGR
    );


    const int outputHeight =
        std::max(
            leftMatchImage.rows,
            rightMatchImage.rows
        );

    const int outputWidth =
        leftMatchImage.cols
        +
        rightMatchImage.cols;


    cv::Mat resultImage(
        outputHeight,
        outputWidth,
        CV_8UC3,
        cv::Scalar(0, 0, 0)
    );


    leftMatchImage.copyTo(
        resultImage(
            cv::Rect(
                0,
                0,
                leftMatchImage.cols,
                leftMatchImage.rows
            )
        )
    );


    rightMatchImage.copyTo(
        resultImage(
            cv::Rect(
                leftMatchImage.cols,
                0,
                rightMatchImage.cols,
                rightMatchImage.rows
            )
        )
    );


    // =========================================================================
    // Draw matched lines and match connections
    // =========================================================================

    cv::RNG rngMatches(12345);


    for (std::size_t pair = 0;
         pair < numberOfMatches;
         ++pair)
    {
        const int lineIDLeft =
            static_cast<int>(matchLeft[pair]);

        const int lineIDRight =
            static_cast<int>(matchRight[pair]);


        if (lineIDLeft < 0 ||
            lineIDRight < 0 ||
            lineIDLeft >= static_cast<int>(linesInLeft.size()) ||
            lineIDRight >= static_cast<int>(linesInRight.size()))
        {
            continue;
        }


        if (linesInLeft[lineIDLeft].empty() ||
            linesInRight[lineIDRight].empty())
        {
            continue;
        }


        const cv::Scalar color(
            rngMatches.uniform(50, 255),
            rngMatches.uniform(50, 255),
            rngMatches.uniform(50, 255)
        );


        // Left matched line
        const cv::Point leftStart(
            cvRound(linesInLeft[lineIDLeft][0].startPointX),
            cvRound(linesInLeft[lineIDLeft][0].startPointY)
        );

        const cv::Point leftEnd(
            cvRound(linesInLeft[lineIDLeft][0].endPointX),
            cvRound(linesInLeft[lineIDLeft][0].endPointY)
        );


        // Right matched line, shifted horizontally
        const cv::Point rightStart(
            cvRound(linesInRight[lineIDRight][0].startPointX)
                + leftMatchImage.cols,
            cvRound(linesInRight[lineIDRight][0].startPointY)
        );

        const cv::Point rightEnd(
            cvRound(linesInRight[lineIDRight][0].endPointX)
                + leftMatchImage.cols,
            cvRound(linesInRight[lineIDRight][0].endPointY)
        );


        cv::line(
            resultImage,
            leftStart,
            leftEnd,
            color,
            3,
            cv::LINE_AA
        );


        cv::line(
            resultImage,
            rightStart,
            rightEnd,
            color,
            3,
            cv::LINE_AA
        );


        // Connect line midpoints
        const cv::Point leftMiddle(
            (leftStart.x + leftEnd.x) / 2,
            (leftStart.y + leftEnd.y) / 2
        );

        const cv::Point rightMiddle(
            (rightStart.x + rightEnd.x) / 2,
            (rightStart.y + rightEnd.y) / 2
        );


        cv::line(
            resultImage,
            leftMiddle,
            rightMiddle,
            color,
            1,
            cv::LINE_AA
        );


        // Match number
        const std::string label =
            std::to_string(pair);


        cv::putText(
            resultImage,
            label,
            leftMiddle,
            cv::FONT_HERSHEY_SIMPLEX,
            0.5,
            color,
            1,
            cv::LINE_AA
        );


        cv::putText(
            resultImage,
            label,
            rightMiddle,
            cv::FONT_HERSHEY_SIMPLEX,
            0.5,
            color,
            1,
            cv::LINE_AA
        );
    }


    // =========================================================================
    // Save results
    // =========================================================================

    cv::imwrite(
        "LBD_matches.png",
        resultImage
    );


    cout << "Saved: LinesInImage1.png" << endl;
    cout << "Saved: LinesInImage2.png" << endl;
    cout << "Saved: LBD_matches.png" << endl;

    cout << "Number of total matches = "
         << numberOfMatches
         << endl;


    // =========================================================================
    // Display
    // =========================================================================

    cv::imshow(
        "LBD Line Matches",
        resultImage
    );

    cout << "Press any key in the image window to exit." << endl;

    cv::waitKey(0);

    return 0;
}
