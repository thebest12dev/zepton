#include "StatMonitor.h"
#include <opencv2/opencv.hpp>
#include <utility>
#include <memory>
#include <vector>
namespace {


//    Gdip_MultiLockedBitsSearch(hStride, hScan, hWidth, hHeight, nStride, nScan, nWidth, nHeight
//        , &OutputList: = "", OuterX1 : = 0, OuterY1 : = 0, OuterX2 : = 0, OuterY2 : = 0, Variation : = 0
//        , SearchDirection : = 1, Instances : = 0, LineDelim : = "`n", CoordDelim : = ",")
//    {
//    OutputList: = ""
//        OutputCount : = !Instances
//        InnerX1 : = OuterX1, InnerY1 : = OuterY1
//        InnerX2 : = OuterX2, InnerY2 : = OuterY2
//
//        ; The following part is a rather ugly but working hack that I
//        ; came up with to adjust the variables and their increments
//        ; according to the specified Haystack Search Direction
//        /*
//        Mod(SD,4) = 0 --> iX = 2 , stepX = +0 , iY = 1 , stepY = +1
//        Mod(SD,4) = 1 --> iX = 1 , stepX = +1 , iY = 1 , stepY = +1
//        Mod(SD,4) = 2 --> iX = 1 , stepX = +1 , iY = 2 , stepY = +0
//        Mod(SD,4) = 3 --> iX = 2 , stepX = +0 , iY = 2 , stepY = +0
//        SD <= 4   ------> Vertical preference
//        SD > 4    ------> Horizontal preference
//        */
//        ; Set the index and the step(for both X and Y) to + 1
//        iX : = 1, stepX : = 1, iY : = 1, stepY : = 1
//        ; Adjust Y variables if SD is 2, 3, 6 or 7
//        Modulo : = Mod(SearchDirection, 4)
//        If(Modulo > 1)
//        iY : = 2, stepY : = 0
//        ; adjust X variables if SD is 3, 4, 7 or 8
//        If !Mod(Modulo, 3)
//        iX : = 2, stepX : = 0
//        ; Set default Preference to vertical and Nonpreference to horizontal
//        P : = "Y", N : = "X"
//        ; adjust Preference and Nonpreference if SD is 5, 6, 7 or 8
//        If(SearchDirection > 4)
//        P : = "X", N : = "Y"
//        ; Set the Preference Index and the Nonpreference Index
//        i_P : = i % P%, i_N : = i % N %
//
//        While(!(OutputCount == Instances) && (0 == Gdip_LockedBitsSearch(hStride, hScan, hWidth, hHeight, nStride
//            , nScan, nWidth, nHeight, &FoundX, &FoundY, OuterX1, OuterY1, OuterX2, OuterY2, Variation, SearchDirection)))
//    {
//        OutputCount++
//            OutputList . = LineDelim FoundX CoordDelim FoundY
//            Outer % P % %i_P % : = Found % P % +step % P %
//            Inner % N % %i_N % : = Found % N % +step % N %
//            Inner % P % 1 : = Found % P %
//            Inner % P % 2 : = Found % P % +1
//            While(!(OutputCount == Instances) && (0 == Gdip_LockedBitsSearch(hStride, hScan, hWidth, hHeight, nStride
//                , nScan, nWidth, nHeight, &FoundX, &FoundY, InnerX1, InnerY1, InnerX2, InnerY2, Variation, SearchDirection)))
//        {
//            OutputCount++
//                OutputList . = LineDelim FoundX CoordDelim FoundY
//                Inner % N % %i_N % : = Found % N % +step % N%
//        }
//    }
//OutputList: = SubStr(OutputList, 1 + StrLen(LineDelim))
//OutputCount -= !Instances
//Return OutputCount
//    }
    int Gdip_SetBitmapTransColor(unsigned char* Scan, int Width, int Height, int Stride, unsigned char* Trans, int* MCount)
    {
        int x1, y1, index;
        MCount[0] = 0;
        for (y1 = 0; y1 < Height; y1++) {
            for (x1 = 0; x1 < Width; x1++) {
                // using index to calculate the needle index offset only once
                index = (4 * x1) + (y1 * Stride);
                if (Scan[index + 2] == Trans[2]
                    && Scan[index + 1] == Trans[1]
                    && Scan[index + 0] == Trans[0]) {
                    Scan[index + 3] = 0;
                    MCount[0]++;
                }
            }
        }
        return 0;
    }
    int _Gdip_ImageSearch(int* Foundx, int* Foundy, unsigned char* HayStack, unsigned char* Needle, int nw, int nh, int Stride1, int Stride2, int sx1, int sy1, int sx2, int sy2, int v, int sd)
    {
        int y1, y2, x1, x2, idxN, idxH;
        if (sd == 1) { // [default] top->left->right->bottom (vertical preference)
            for (y1 = sy1; y1 < sy2; y1++) {
                for (x1 = sx1; x1 < sx2; x1++) {
                    for (y2 = 0; y2 < nh; y2++) {
                        for (x2 = 0; x2 < nw; x2++) {
                            idxN = (4 * x2) + (y2 * Stride2); // needle index offset
                            idxH = (4 * (x1 + x2)) + ((y1 + y2) * Stride1); // haystack index offset
                            if (!(Needle[idxN + 3] == 0
                                || Needle[idxN + 2] <= HayStack[idxH + 2] + v
                                && Needle[idxN + 2] >= HayStack[idxH + 2] - v
                                && Needle[idxN + 1] <= HayStack[idxH + 1] + v
                                && Needle[idxN + 1] >= HayStack[idxH + 1] - v
                                && Needle[idxN + 0] <= HayStack[idxH + 0] + v
                                && Needle[idxN + 0] >= HayStack[idxH + 0] - v))
                                goto NoMatch1;
                        }
                    }
                    Foundx[0] = x1; Foundy[0] = y1;
                    return 0;
                NoMatch1:;
                }
            }
        }
        else if (sd == 2) { // bottom->left->right->top (vertical preference)
            for (y1 = sy2 - 1; y1 >= sy1; y1--) {
                for (x1 = sx1; x1 < sx2; x1++) {
                    for (y2 = 0; y2 < nh; y2++) {
                        for (x2 = 0; x2 < nw; x2++) {
                            idxN = (4 * x2) + (y2 * Stride2); // needle index offset
                            idxH = (4 * (x1 + x2)) + ((y1 + y2) * Stride1); // haystack index offset
                            if (!(Needle[idxN + 3] == 0
                                || Needle[idxN + 2] <= HayStack[idxH + 2] + v
                                && Needle[idxN + 2] >= HayStack[idxH + 2] - v
                                && Needle[idxN + 1] <= HayStack[idxH + 1] + v
                                && Needle[idxN + 1] >= HayStack[idxH + 1] - v
                                && Needle[idxN + 0] <= HayStack[idxH + 0] + v
                                && Needle[idxN + 0] >= HayStack[idxH + 0] - v))
                                goto NoMatch2;
                        }
                    }
                    Foundx[0] = x1; Foundy[0] = y1;
                    return 0;
                NoMatch2:;
                }
            }
        }
        else if (sd == 3) { // bottom->right->left->top (vertical preference)
            for (y1 = sy2 - 1; y1 >= sy1; y1--) {
                for (x1 = sx2 - 1; x1 >= sx1; x1--) {
                    for (y2 = 0; y2 < nh; y2++) {
                        for (x2 = 0; x2 < nw; x2++) {
                            idxN = (4 * x2) + (y2 * Stride2); // needle index offset
                            idxH = (4 * (x1 + x2)) + ((y1 + y2) * Stride1); // haystack index offset
                            if (!(Needle[idxN + 3] == 0
                                || Needle[idxN + 2] <= HayStack[idxH + 2] + v
                                && Needle[idxN + 2] >= HayStack[idxH + 2] - v
                                && Needle[idxN + 1] <= HayStack[idxH + 1] + v
                                && Needle[idxN + 1] >= HayStack[idxH + 1] - v
                                && Needle[idxN + 0] <= HayStack[idxH + 0] + v
                                && Needle[idxN + 0] >= HayStack[idxH + 0] - v))
                                goto NoMatch3;
                        }
                    }
                    Foundx[0] = x1; Foundy[0] = y1;
                    return 0;
                NoMatch3:;
                }
            }
        }
        else if (sd == 4) { // top->right->left->bottom (vertical preference)
            for (y1 = sy1; y1 < sy2; y1++) {
                for (x1 = sx2 - 1; x1 >= sx1; x1--) {
                    for (y2 = 0; y2 < nh; y2++) {
                        for (x2 = 0; x2 < nw; x2++) {
                            idxN = (4 * x2) + (y2 * Stride2); // needle index offset
                            idxH = (4 * (x1 + x2)) + ((y1 + y2) * Stride1); // haystack index offset
                            if (!(Needle[idxN + 3] == 0
                                || Needle[idxN + 2] <= HayStack[idxH + 2] + v
                                && Needle[idxN + 2] >= HayStack[idxH + 2] - v
                                && Needle[idxN + 1] <= HayStack[idxH + 1] + v
                                && Needle[idxN + 1] >= HayStack[idxH + 1] - v
                                && Needle[idxN + 0] <= HayStack[idxH + 0] + v
                                && Needle[idxN + 0] >= HayStack[idxH + 0] - v))
                                goto NoMatch4;
                        }
                    }
                    Foundx[0] = x1; Foundy[0] = y1;
                    return 0;
                NoMatch4:;
                }
            }
        }
        if (sd == 5) { // [default] left->top->bottom->right (horizontal preference)
            for (x1 = sx1; x1 < sx2; x1++) {
                for (y1 = sy1; y1 < sy2; y1++) {
                    for (y2 = 0; y2 < nh; y2++) {
                        for (x2 = 0; x2 < nw; x2++) {
                            idxN = (4 * x2) + (y2 * Stride2); // needle index offset
                            idxH = (4 * (x1 + x2)) + ((y1 + y2) * Stride1); // haystack index offset
                            if (!(Needle[idxN + 3] == 0
                                || Needle[idxN + 2] <= HayStack[idxH + 2] + v
                                && Needle[idxN + 2] >= HayStack[idxH + 2] - v
                                && Needle[idxN + 1] <= HayStack[idxH + 1] + v
                                && Needle[idxN + 1] >= HayStack[idxH + 1] - v
                                && Needle[idxN + 0] <= HayStack[idxH + 0] + v
                                && Needle[idxN + 0] >= HayStack[idxH + 0] - v))
                                goto NoMatch5;
                        }
                    }
                    Foundx[0] = x1; Foundy[0] = y1;
                    return 0;
                NoMatch5:;
                }
            }
        }
        else if (sd == 6) { // left->bottom->top->right (horizontal preference)
            for (x1 = sx1; x1 < sx2; x1++) {
                for (y1 = sy2 - 1; y1 >= sy1; y1--) {
                    for (y2 = 0; y2 < nh; y2++) {
                        for (x2 = 0; x2 < nw; x2++) {
                            idxN = (4 * x2) + (y2 * Stride2); // needle index offset
                            idxH = (4 * (x1 + x2)) + ((y1 + y2) * Stride1); // haystack index offset
                            if (!(Needle[idxN + 3] == 0
                                || Needle[idxN + 2] <= HayStack[idxH + 2] + v
                                && Needle[idxN + 2] >= HayStack[idxH + 2] - v
                                && Needle[idxN + 1] <= HayStack[idxH + 1] + v
                                && Needle[idxN + 1] >= HayStack[idxH + 1] - v
                                && Needle[idxN + 0] <= HayStack[idxH + 0] + v
                                && Needle[idxN + 0] >= HayStack[idxH + 0] - v))
                                goto NoMatch6;
                        }
                    }
                    Foundx[0] = x1; Foundy[0] = y1;
                    return 0;
                NoMatch6:;
                }
            }
        }
        else if (sd == 7) { // right->bottom->top->left (horizontal preference)
            for (x1 = sx2 - 1; x1 >= sx1; x1--) {
                for (y1 = sy2 - 1; y1 >= sy1; y1--) {
                    for (y2 = 0; y2 < nh; y2++) {
                        for (x2 = 0; x2 < nw; x2++) {
                            idxN = (4 * x2) + (y2 * Stride2); // needle index offset
                            idxH = (4 * (x1 + x2)) + ((y1 + y2) * Stride1); // haystack index offset
                            if (!(Needle[idxN + 3] == 0
                                || Needle[idxN + 2] <= HayStack[idxH + 2] + v
                                && Needle[idxN + 2] >= HayStack[idxH + 2] - v
                                && Needle[idxN + 1] <= HayStack[idxH + 1] + v
                                && Needle[idxN + 1] >= HayStack[idxH + 1] - v
                                && Needle[idxN + 0] <= HayStack[idxH + 0] + v
                                && Needle[idxN + 0] >= HayStack[idxH + 0] - v))
                                goto NoMatch7;
                        }
                    }
                    Foundx[0] = x1; Foundy[0] = y1;
                    return 0;
                NoMatch7:;
                }
            }
        }
        else if (sd == 8) { // right->top->bottom->left (horizontal preference)
            for (x1 = sx2 - 1; x1 >= sx1; x1--) {
                for (y1 = sy1; y1 < sy2; y1++) {
                    for (y2 = 0; y2 < nh; y2++) {
                        for (x2 = 0; x2 < nw; x2++) {
                            idxN = (4 * x2) + (y2 * Stride2); // needle index offset
                            idxH = (4 * (x1 + x2)) + ((y1 + y2) * Stride1); // haystack index offset
                            if (!(Needle[idxN + 3] == 0
                                || Needle[idxN + 2] <= HayStack[idxH + 2] + v
                                && Needle[idxN + 2] >= HayStack[idxH + 2] - v
                                && Needle[idxN + 1] <= HayStack[idxH + 1] + v
                                && Needle[idxN + 1] >= HayStack[idxH + 1] - v
                                && Needle[idxN + 0] <= HayStack[idxH + 0] + v
                                && Needle[idxN + 0] >= HayStack[idxH + 0] - v))
                                goto NoMatch8;
                        }
                    }
                    Foundx[0] = x1; Foundy[0] = y1;
                    return 0;
                NoMatch8:;
                }
            }
        }
        Foundx[0] = -1; Foundy[0] = -1;
        return -4001;
    }
    int Gdip_LockedBitsSearch(int hStride, unsigned char* hScan, int hWidth, int hHeight, int nStride, unsigned char* nScan, int nWidth, int nHeight, int* x = nullptr, int* y = nullptr, int sx1 = 0, int sy1 = 0, int sx2 = 0, int sy2 = 0, int Variation = 0, int sd = 1) {
        if (sx2 < sy1) {
            return -3001;
        }
        if (sy2 < sy1) {
            return -3002;
        }

        if (sx2 - sx1 == 0) {
            return -3005;
        }
        if (sy2 - sy1 == 0) {
            return -3006;
        }
        return _Gdip_ImageSearch(x, y, hScan, nScan, nWidth, nHeight, hStride, nStride, sx1, sy1, sx2, sy2, Variation, sd);
    }
    struct GDICompatBuffer
    {
        std::vector<uint8_t> buffer;
        uint8_t* data = nullptr;    
        int width = 0;
        int height = 0;
        int stride = 0;             
    };

    static inline int Align4(int x)
    {
        return (x + 3) & ~3;
    }

    GDICompatBuffer MatToGDI32BGRA_BottomUp(const cv::Mat& src)
    {
        if (src.empty())
            return {};

        cv::Mat bgra;

        // --- Normalize to BGRA (4 channels) ---
        switch (src.channels())
        {
        case 1:
            cv::cvtColor(src, bgra, cv::COLOR_GRAY2BGRA);
            break;
        case 3:
            cv::cvtColor(src, bgra, cv::COLOR_BGR2BGRA);
            break;
        case 4:
            bgra = src;
            break;
        default:
            throw std::runtime_error("Unsupported channel count");
        }

        GDICompatBuffer out;
        out.width = bgra.cols;
        out.height = bgra.rows;

        const int bytesPerPixel = 4;
        const int rowSize = out.width * bytesPerPixel;

        out.stride = Align4(rowSize);

        out.buffer.resize(out.stride * out.height);

        for (int y = 0; y < out.height; ++y)
        {
            const uint8_t* srcRow = bgra.ptr<uint8_t>(y);
            int dstY = out.height - 1 - y;
            //int dstY = y;
            uint8_t* dstRow = out.buffer.data() + dstY * out.stride;

            memcpy(dstRow, srcRow, rowSize);

            if (out.stride > rowSize)
            {
                memset(dstRow + rowSize, 0, out.stride - rowSize);
            }
        }
        out.data = out.buffer.data();

        return out;
    }

    int Gdip_MultiLockedBitsSearch(int hStride, unsigned char* hScan, int hWidth, int hHeight, int nStride, unsigned char* nScan, int nWidth, int nHeight,
        std::vector<int>& OutputList, int OuterX1 = 0, int OuterY1 = 0, int OuterX2 = 0,
        int OuterY2 = 0, int Variation = 0, int SearchDirection = 1, int Instances = 0, std::string LineDelim =
        "\n", std::string CoordDelim = ",") {
        int OutputCount = !Instances;
        //int i = 0;

        int InnerX1 = OuterX1;
        int InnerX2 = OuterX2;
        int InnerY1 = OuterY1;
        int InnerY2 = OuterY2;

        int iX = 1, stepX = 1, iY = 1, stepY = 1;

        int Modulo = SearchDirection % 4;
        if (Modulo > 1) {
            iY = 2;
            stepY = 0;
        }
        if (!(Modulo % 3)) {
            iX = 2;
            stepX = 0;
        }
        char P = 'Y', N = 'X';

        if (SearchDirection > 4) P = 'X', N = 'Y';

        int i_P = iY, i_N = iX;
        if (P == 'X') i_P == iX;
        if (N == 'Y') i_N == iY;


        int FoundX = 0, FoundY = 0;
        while (!(OutputCount == Instances) && (0 == Gdip_LockedBitsSearch(hStride, hScan, hWidth, hHeight, nStride
            , nScan, nWidth, nHeight, &FoundX, &FoundY, OuterX1, OuterY1, OuterX2, OuterY2, Variation, SearchDirection))) {
            OutputCount++;

            OutputList.push_back(FoundX);
            OutputList.push_back(FoundY);
            
            if (P == 'Y' && i_P == 2) OuterY2 = FoundY + stepY;
            if (P == 'Y' && i_P == 1) OuterY1 = FoundY + stepY;
            if (P == 'X' && i_P == 2) OuterX2 = FoundX + stepX;
            if (P == 'X' && i_P == 1) OuterX1 = FoundX + stepX;

            if (N == 'X' && i_N == 2) InnerX2 = FoundX + stepX;
            if (N == 'X' && i_N == 1) InnerX1 = FoundX + stepX;
            if (N == 'Y' && i_N == 2) InnerY2 = FoundY + stepY;
            if (N == 'Y' && i_N == 1) InnerY1 = FoundY + stepY;

            if (P == 'Y') InnerY1 = FoundY;
            if (P == 'X') InnerX1 = FoundX;

            if (P == 'Y') InnerY2 = FoundY + 1;
            if (P == 'X') InnerX2 = FoundX + 1;

            while (!(OutputCount == Instances) && (0 == Gdip_LockedBitsSearch(hStride, hScan, hWidth, hHeight, nStride
                , nScan, nWidth, nHeight, &FoundX, &FoundY, InnerX1, InnerY1, InnerX2, InnerY2, Variation, SearchDirection))) {

                OutputCount++;

                OutputList.push_back(FoundX);
                OutputList.push_back(FoundY);
               // std::cout << FoundX << " " << FoundY << std::endl;
                if (N == 'X' && i_N == 2) InnerX2 = FoundX + stepX;
                if (N == 'X' && i_N == 1) InnerX1 = FoundX + stepX;
                if (N == 'Y' && i_N == 2) InnerY2 = FoundY + stepY;
                if (N == 'Y' && i_N == 1) InnerY1 = FoundY + stepY;
            }
        }
        OutputCount -= !(Instances > 0);
        //std::cout << FoundX << " " << FoundY << std::endl;
        return OutputCount;


    }
    int Gdip_ImageSearch(GDICompatBuffer& pBitmapHaystack, GDICompatBuffer& pBitmapNeedle, std::vector<int>& OutputList, int OuterX1 = 0, int OuterY1 = 0, int OuterX2 = 0, int OuterY2 = 0,
        int Variation = 0, int SearchDirection = 1, int Instances = 1) { 

        if (OuterY2 == 0) {
            OuterY2 = pBitmapHaystack.height;
        }
        if (OuterX2 == 0) {
            OuterX2 = pBitmapHaystack.width;
        }
        if (SearchDirection == 0) {
            SearchDirection = 1;

        }
        int result = Gdip_MultiLockedBitsSearch(pBitmapHaystack.stride, pBitmapHaystack.data, pBitmapHaystack.width, pBitmapHaystack.height, pBitmapNeedle.stride, pBitmapNeedle.data, pBitmapNeedle.width, pBitmapNeedle.height, OutputList, OuterX1, OuterY1, OuterX2, OuterY2, Variation, SearchDirection, Instances);
        return result;
    }
    
    


    cv::Mat readImage(std::string source) {
        cv::Mat mat = cv::imread(source, cv::IMREAD_UNCHANGED);
        return mat;
    }
}
namespace zepton {
    void StatMonitor::onWalkspeedUpdate(std::function<void(int)> cb) {
        walkspeedUpdate = cb;
    }
    void StatMonitor::setWalkspeed(int cb) {
        walkspeed = cb;
    }
    void StatMonitor::backgroundWorker() {
        GDICompatBuffer digits[9];
        digits[0] = MatToGDI32BGRA_BottomUp(readImage("resources/macro/digit_1.png"));
        digits[1] = MatToGDI32BGRA_BottomUp(readImage("resources/macro/digit_2.png"));
        digits[2] = MatToGDI32BGRA_BottomUp(readImage("resources/macro/digit_3.png"));
        digits[3] = MatToGDI32BGRA_BottomUp(readImage("resources/macro/digit_4.png"));
        digits[4] = MatToGDI32BGRA_BottomUp(readImage("resources/macro/digit_5.png"));
        digits[5] = MatToGDI32BGRA_BottomUp(readImage("resources/macro/digit_6.png"));
        digits[6] = MatToGDI32BGRA_BottomUp(readImage("resources/macro/digit_7.png"));
        digits[7] = MatToGDI32BGRA_BottomUp(readImage("resources/macro/digit_8.png"));
        digits[8] = MatToGDI32BGRA_BottomUp(readImage("resources/macro/digit_9.png"));
        
        cv::Mat hasteMat = cv::imread("resources/macro/haste.png", cv::IMREAD_UNCHANGED);
        cv::Mat melodyMat = cv::imread("resources/macro/melody.png", cv::IMREAD_UNCHANGED);
   
        GDICompatBuffer pBMHaste = MatToGDI32BGRA_BottomUp(hasteMat);
        GDICompatBuffer pBMMelody = MatToGDI32BGRA_BottomUp(melodyMat);
        while (true) {
           
            cv::Mat screenUncropped = ocr.getScreen();
            
            cv::Rect captureRect(0, 22+48, screenUncropped.cols, 38);
            cv::Mat screen = screenUncropped(captureRect);
            GDICompatBuffer pBMArea = MatToGDI32BGRA_BottomUp(screen);

            int x = 0;
            int y = 0;
            int x1 = 0;
            int y1 = 0;
            int hastes = 0;
            for (int i = 0; i < 3; i++) {
               // cv::Rect initialRect(cv::Point(x, 14), cv::Point(screen.cols, screen.rows));
                std::vector<int> outputListInitial;
                int res = Gdip_ImageSearch(pBMArea, pBMHaste, outputListInitial, x, 14, 0, 0, 0, 6);
                if (res != 1) {
                    break;
                }
                
                x = outputListInitial.at(0);
                y = outputListInitial.at(1);
                
                if (Gdip_ImageSearch(pBMArea, pBMMelody, outputListInitial, x+2, 0, x+std::max(16, 2*y-24), y, 12) != 1) {
                    //std::cout << "haste dt!" << std::endl;
                    hastes++;
                    if (hastes == 1) {
                        x1 = x;
                        y1 = 25;
                    }

                    
                }
                x += 2 * y - 14;
            }
            bool cocoHaste = (hastes == 2);
            int stacks = 0;
            if (hastes >= 1) {
                bool t = false;
                for (int i = 0; i < 9; i++) {
                    std::vector<int> v;
                    auto resultStack = Gdip_ImageSearch(pBMArea, digits[i], v, x1 + 2 * y1 - 44, std::max(0, y1 - 18), x1 + 2 * y1 - 14, y1 - 1);
                    //auto resultStack = Gdip_ImageSearch(pBMArea, digits[i], v, 0,0, pBMArea.width, pBMArea.height);
                    
                    if (resultStack == 1) {
                        stacks = i == 0 ? 10 : i+1;
                        t = true;
                        break;
                    }
                }
                if (t == false) {
                    stacks = 1;
                 }
            }
            // actual haste prediction
            int multiplier = ((0.1 * stacks) + 1); //+ (cocoHaste ? 1.5 : 0));
            int updatedWalkspeed = walkspeed * (multiplier == 0 ? 1 : multiplier);
            walkspeedUpdate(updatedWalkspeed);
        }
        return;
    }
    StatMonitor::StatMonitor() {};
    StatMonitor::~StatMonitor() {

    }
    void StatMonitor::startThread() {
        backgroundThread = std::thread(&StatMonitor::backgroundWorker, this);
        backgroundThread.detach();

    }
}