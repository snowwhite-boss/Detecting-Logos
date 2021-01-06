/**
 * @file MatchTemplate_Demo.cpp
 * @brief Sample code to use the function MatchTemplate
 * @author OpenCV team
 */

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <vector> 

using namespace std;
using namespace cv;

//! [declare]
/// Global Variables
Mat img; Mat templ; Mat result; Mat cropImage; Mat compImage;
const char* comp_window = "Compare Image";
const char* crop_window = "Crop window";
const char* templ_window = "Templ window";

const int group_threshold = 4;
const int max_Method = 5;
struct Match
{
    Point mPosition;
    int mCount = 1;
}match;
vector <Match> matchResult;
//! [declare]

/// Function Headers
Point MatchingMethod(int, void*, int);

/**
 * @function main
 */
int main(int argc, char** argv)
{
    //! [Debug Code]
    argc = 3;
    argv[1] = (char*)"images/letter/1.jpg";
    argv[2] = (char*)"images/logo/4.jpg";
    //! [Debug Code]
    
    if (argc < 2)
    {
        cout << "Not enough parameters" << endl;
        cout << "Usage:\n" << argv[0] << " <image_name> <template_name>" << endl;
        return -1;
    }

    //! [load_image]
    /// Load image and template
    img = imread(argv[1], IMREAD_COLOR);
    templ = imread(argv[2], IMREAD_COLOR);
    //! [exception resize templ]
    while(templ.rows > img.rows || templ.cols > img.cols)
        resize(templ, templ, Size(), 0.5, 0.5);
    //! [exception resize templ]
    if (img.empty() || templ.empty())
    {
        cout << "Can't read one of the images" << endl;
        return EXIT_FAILURE;
    }
    //! [load_image]

    //! [create_windows]
    /// Create windows
    namedWindow(comp_window, WINDOW_NORMAL);
	namedWindow(crop_window, WINDOW_NORMAL);
	namedWindow(templ_window, WINDOW_NORMAL);
	moveWindow(comp_window,  100, 20);
	moveWindow(crop_window,  500, 20);
	moveWindow(templ_window, 900, 20);
    //! [create_windows]

    //! [create_trackbar]
    /// Create Trackbar
//     const char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
//     createTrackbar(trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod);
    //! [create_trackbar]

    //! [find correct position]
	for (int i = 0; i <= max_Method; i++)
    {
        Point pos = MatchingMethod(0, 0, i);
        int j;
        for (j = 0; j < matchResult.size(); j++) {
            if (matchResult[j].mPosition == pos) {
                matchResult[j].mCount++;
                break;
            }
        }
        if (j == matchResult.size()) {
            Match t_match;
            t_match.mPosition = pos;
            t_match.mCount = 1;
            matchResult.push_back(t_match);
        }
    }
    if (matchResult.size() >= group_threshold)
    {
        cout << "Can't find logo in the image" << endl;
        return EXIT_FAILURE;
    }
    int cor_index = 0;
    int max_pick = 0;
    for (int i = 0; i < matchResult.size(); i++)
    {
        if (matchResult[i].mCount > max_pick) {
            cor_index = i;
            max_pick = matchResult[i].mCount;
        }
    }
    Point matchLoc = matchResult[cor_index].mPosition;
    //! [find correct position]
    
	//! [Crop Logo]
    Rect matchRc(matchLoc.x, matchLoc.y, templ.cols, templ.rows);
    cropImage = img(matchRc);
	//! [Crop Logo]
    
    //! [Compare Image]
    compare(cropImage, templ, compImage, CMP_EQ);
    cvtColor(compImage, compImage, COLOR_RGB2GRAY);
    int percentage = countNonZero(compImage);
    //! [Compare Image]
	
    //! [imshow]
	/// Show me what you got
	//rectangle(img, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
	//rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);


	cout << percentage;
	
    imshow(comp_window, compImage);
	imshow(crop_window, cropImage);
	imshow(templ_window, templ);
	//! [imshow]
    //! 
    //! [wait_key]
    waitKey(0);
    return EXIT_SUCCESS;
    //! [wait_key]
}

/**
 * @function MatchingMethod
 * @brief Trackbar callback
 */
Point MatchingMethod(int, void*, int match_method)
{
    //! [copy_source]
    /// Source image to display
//     Mat img_display;
//     img.copyTo(img_display);
    //! [copy_source]

    //! [create_result_matrix]
    /// Create the result matrix
    int result_cols = img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;

    result.create(result_rows, result_cols, CV_32FC1);
    //! [create_result_matrix]

    //! [match_template]
    /// Do the Matching and Normalize
    
    matchTemplate(img, templ, result, match_method);
    //! [match_template]

    //! [normalize]
    normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
    //! [normalize]

    //! [best_match]
    /// Localizing the best match with minMaxLoc
    double minVal; double maxVal; Point minLoc; Point maxLoc;
    Point matchLoc;

    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
    //! [best_match]

    //! [match_loc]
    /// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
    if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
    {
        matchLoc = minLoc;
    }
    else
    {
        matchLoc = maxLoc;
    }
    //! [match_loc]

    

    return matchLoc;
}