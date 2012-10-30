#ifndef FACE_RECOGNITION_H
#define FACE_RECOGNITION_H

#include <iostream>
#include <cstdio>
#include "opencv2/opencv.hpp"

#include "face_recognition/AffineTrans.h"
#include "face_recognition/MatrixOper.h"
#include "face_recognition/MemoryOper.h"
#include "pcamodel.h"

//图像大小
#define IMG_WIDTH 640
#define IMG_HEIGHT 480
//
#define NORMAL_FACE_LEFT_EYE_X 16
#define NORMAL_FACE_LEFT_EYE_Y 30
#define NORMAL_FACE_RIGHT_EYE_X 47
#define NORMAL_FACE_RIGHT_EYE_Y 30

using namespace cv;
using namespace std;

typedef struct {
    int    x_left;
    int    y_left;
    int    x_right;
    int    y_right;
} cgt_eye;

typedef struct {
    int left;
    int top;
    int right;
    int bottom;
} cgt_rect;

typedef struct {
    int x;
    int y;
} s_point;

typedef struct {
    int xleft,yleft,xright,yright;
    float confidence;//50.0 to 100.0
} Cgt_Eye;

typedef struct {
    int left,right,top,bottom;
} Cgt_Rect;


class face_recognition
{
private:
    int gallery_people_num;
    char *people_num_file_path, gallery_model_path;
    PCAModel *pca_model;
    double* gallery_face_pca_coeff;
    Cgt_Eye iris_point;
    byte *gray_img;
    byte face_img_gray[1024 * 512];
    Cgt_Rect area;

    int w,h;

    double *load_gallery_data(int& gallery_people_num, PCAModel *pca_model, char *people_num_file_path, char *gallery_model_path)
    {
        FILE* fp = fopen(people_num_file_path,"r");
        fscanf(fp, "%d", &gallery_people_num);
        fclose(fp);

        fp = fopen(gallery_model_path, "rb");
        double* pGalleryFacePCACoeff = new double[pca_model->m_nPCADim * gallery_people_num];
        fread(pGalleryFacePCACoeff, pca_model->m_nPCADim * gallery_people_num, sizeof(double), fp);
        fclose(fp);

        return pGalleryFacePCACoeff;
    }
    double compute_vec_sim(double a[], double b[], int dim)
    {
        int i;
        double norm_a = 0, norm_b = 0, dot_mul = 0;
        for (i = 0; i < dim; i++)
            norm_a += a[i] * a[i];
        norm_a = sqrt(norm_a);
        for (i = 0; i < dim; i++)
            norm_b += b[i] * b[i];
        norm_b = sqrt(norm_b);
        for (i = 0; i < dim; i++)
            dot_mul += a[i] * b[i];
        return dot_mul / (norm_a * norm_b);
    }
public:
    face_recognition()
    {
    }

    int init_func(byte *face_img_g)
    {
        this->gray_img = face_img_g;
        this->pca_model = new PCAModel("PCAModel.bin"); //读取人脸识别PCA模型
        this->gallery_face_pca_coeff = load_gallery_data(gallery_people_num, pca_model, "GalleryPeopleNum.ini", "GalleryPCACoeff.bin"); //读取罪犯人脸库
    }
    int release_func()
    {
        delete pca_model;
        delete [] gallery_face_pca_coeff;
    }

    // nPeopleID：从0开始
    // pPCAModel：PCA模型 由InitialFunc得到
    // pFaceImgGray：输入的Face，必须是灰度图 由Face Detection和Eye Location得到
    // pGalleryFacePCACoeff：已经建模的人脸库中的Face的PCA系数 由InitialFunc得到
    int _face_recognition(int people_id, double threshold = 10.0)//PCAModel * pca_model, byte* face_img_gray, double* gallery_face_pca_coeff, int people_id, int gallery_people_num, double threshold/*阈值*/)
    {
        if(people_id >= gallery_people_num)
            return -1;
        double* input_face_pca_coeff = new double[pca_model->m_nPCADim];
        pca_model->ComputePCACoeff(face_img_gray, input_face_pca_coeff);
        double curSim = compute_vec_sim(input_face_pca_coeff, &gallery_face_pca_coeff[people_id * pca_model->m_nPCADim], pca_model->m_nPCADim);
        delete []input_face_pca_coeff;
        if (curSim >= threshold)
            return 1;
        else
            return 0;
    }
    // 根据眼睛定位的结果从原始图像中crop出face image
    void get_face_img()
    {
            double OriginalFaceEyePosition[4];
            double NormalFaceEyePosition[4];
            double AffineMatrix[4];

            OriginalFaceEyePosition[0] = iris_point.xleft;
            OriginalFaceEyePosition[1] = iris_point.yleft;
            OriginalFaceEyePosition[2] = iris_point.xright;
            OriginalFaceEyePosition[3] = iris_point.yright;

            NormalFaceEyePosition[0] = NORMAL_FACE_LEFT_EYE_X;
            NormalFaceEyePosition[1] = NORMAL_FACE_LEFT_EYE_Y;
            NormalFaceEyePosition[2] = NORMAL_FACE_RIGHT_EYE_X;
            NormalFaceEyePosition[3] = NORMAL_FACE_RIGHT_EYE_Y;

            GetAffineMatrix_Pts(AffineMatrix, NormalFaceEyePosition, OriginalFaceEyePosition, 2);
            double dR = sqrt(AffineMatrix[0]*AffineMatrix[0] + AffineMatrix[1]*AffineMatrix[1]);

            s_Point NormReferPt;
            s_Point OriReferPt;
            NormReferPt.x = NORMAL_FACE_LEFT_EYE_X;
            NormReferPt.y = NORMAL_FACE_LEFT_EYE_Y;
            OriReferPt.x = OriginalFaceEyePosition[0];
            OriReferPt.y = OriginalFaceEyePosition[1];

            float cosAngle = AffineMatrix[0] / dR;
            float sinAngle = AffineMatrix[1] / dR;
            AffineTranImg_CenterSizeAngle_1D(face_img_gray, MIN_FACE_WIDTH, MIN_FACE_HEIGHT, NormReferPt,
                gray_img, IMG_WIDTH, IMG_HEIGHT, OriReferPt, sinAngle, cosAngle, dR, 1, 2);

    }
    // 人脸检测 和 眼睛定位
    bool get_face_parameters()
    {
        w = 640, h = 480;
        static CascadeClassifier cascadeFace;
        static CascadeClassifier cascadeEye, cascadeLeftEye, cascadeRightEye;
        String cascadeFaceName = "haarcascade_frontalface_alt.xml";
        String cascadeEyeName = "haarcascade_eye.xml";
        //String cascadeEyeName = "haarcascade_eye_tree_eyeglasses.xml";
        String cascadeRightEyeName = "haarcascade_lefteye_2splits.xml"; //这个左右眼的分类器似乎是针对镜像的人脸图像，这里需要反过来用
        String cascadeLeftEyeName = "haarcascade_righteye_2splits.xml";
        bool useSingleEyeClassifier = true;

        bool nRetCode = false;
        vector<Rect> faces;
        static bool classifierInitialized = false;

        if(classifierInitialized == false)
        {
            //load classifier
            if(!cascadeFace.load( cascadeFaceName))
            {
                fprintf(stderr, "Could not load face classifier cascade!");
                exit(1);
            }
            if(useSingleEyeClassifier)
            {
                if(!cascadeEye.load( cascadeEyeName))
                {
                    fprintf(stderr, "Could not load eye classifier cascade!");
                    exit(1);
                }
            } else {
                if(!cascadeLeftEye.load(cascadeLeftEyeName))
                {
                    fprintf(stderr, "Could not load left eye classifier cascade!");
                    exit(1);
                }
                if(!cascadeRightEye.load(cascadeRightEyeName))
                {
                    fprintf(stderr, "Could not load right eye classifier cascade!");
                    exit(1);
                }
            }

            classifierInitialized = true;
        }
     // 似乎刚打开摄像头时可能w/h<=0，在这里判断一下。
     if((gray_img == NULL) || (w<=0) || (h<=0))
         return false;
     //将输入的gray_image转换为IplImage类型，然后转为Mat
     IplImage* IplOrigImage = cvCreateImageHeader(cvSize(w,h), IPL_DEPTH_8U, 1);
     //IplOrigImage->origin = ~IplOrigImage->origin;  //origin控制图像自上而下还是自下而上
//////////////     cvSetData(IplOrigImage, gray_image, w*1);
     cvSetData(IplOrigImage, gray_img, w*1);
     Mat matImg = IplOrigImage;
     //imshow("flipped img", matImg);
     //cvWaitKey(0);
     //cvDestroyWindow("flipped img");

     cascadeFace.detectMultiScale( matImg, faces, 1.1, 2, CV_HAAR_SCALE_IMAGE, Size(30, 30) );

     if(faces.empty() == false)
     {
      //取最大的脸
      Rect largestFace;
      largestFace.width=0;
      largestFace.height=0;
      for( vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++)
      {
       if( (r->width*r->height) > (largestFace.width*largestFace.height) )
        largestFace = *r;
      }

      // 将最大人脸区域赋给area
      area.left  = largestFace.x;
      area.right  = largestFace.x + largestFace.width;
      area.top  = largestFace.y;
      area.bottom = largestFace.y + largestFace.height;
      ///////////针对最大的脸检测人眼////////////////////////////////
      Mat smallImgROI;
      Rect eyeArea = largestFace;
      eyeArea.height = eyeArea.height/2; //仅对人脸的上半部分检测人眼，以减少错误率
      smallImgROI = matImg(eyeArea);
      Rect leftEyeRect(0,0,0,0), rightEyeRect(0,0,0,0);
      if(useSingleEyeClassifier)
      {
       // 使用双眼一起的单分类器检测眼
       vector<Rect> eyes;
       cascadeEye.detectMultiScale( smallImgROI, eyes, 1.1, 2, CV_HAAR_SCALE_IMAGE, Size(20, 20) );
       if(eyes.size()>=2) //必须至少有两只眼被检出
       {
        vector<Rect>::const_iterator nr = eyes.begin();
        leftEyeRect = *nr;
        nr++;
        rightEyeRect = *nr;
       }
      }
      else
      {
       //使用左右眼分开的两个分类器检测眼
       vector<Rect> leftEye, rightEye;
       cascadeLeftEye.detectMultiScale( smallImgROI, leftEye, 1.1, 2, CV_HAAR_SCALE_IMAGE, Size(20, 20) );
       cascadeRightEye.detectMultiScale( smallImgROI, rightEye, 1.1, 2, CV_HAAR_SCALE_IMAGE, Size(20, 20) );
       if(leftEye.empty() == false)
        leftEyeRect = *(leftEye.begin());
       if(rightEye.empty() == false)
        rightEyeRect = *(rightEye.begin());
      }

      iris_point.xleft = cvRound(largestFace.x + leftEyeRect.x + leftEyeRect.width*0.5);  //左眼中心的x坐标
      iris_point.yleft = cvRound(largestFace.y + leftEyeRect.y + leftEyeRect.height*0.5);  //左眼中心的y坐标
      iris_point.xright = cvRound(largestFace.x + rightEyeRect.x + rightEyeRect.width*0.5);  //右眼中心的x坐标
      iris_point.yright = cvRound(largestFace.y + rightEyeRect.y + rightEyeRect.height*0.5); //右眼中心的y坐标
      nRetCode = true;
      //对眼睛的后期验证：
      //不允许左眼在右眼右边
      if(iris_point.xleft >= iris_point.xright )
       nRetCode = false;
      //不允许眼睛在边界（由于，初始化的值为0，这也意味着如果少于两个眼检测出来，则认为检测失败）
      if( (iris_point.xleft==0) || (iris_point.yleft==0) ||(iris_point.xright==0) || (iris_point.yright==0) )
       nRetCode = false;
      //不允许两只眼上下倾斜过多（也防止一些误检）
      if(abs(iris_point.yright-iris_point.yleft) > (largestFace.width/3) )
       nRetCode = false;
      //不允许两只眼左右间距小于1/4人脸宽度（也防止一些误检）
      if(abs(iris_point.xright-iris_point.xleft) < (largestFace.width/4) )
       nRetCode = false;
      // 输入的gray_image水平是反转的，虽然并不影响这个函数的执行，
      // 但是为了达到左眼是右眼，右眼是左眼，在最后需要把左右眼对调一下。
      int tmpSwap;
      tmpSwap = iris_point.xleft;
      iris_point.xleft = iris_point.xright;
      iris_point.xright = tmpSwap;
      tmpSwap = iris_point.yleft;
      iris_point.yleft = iris_point.yright;
      iris_point.yright = tmpSwap;
      //画出框到的人脸，验证调试用
      Point left_top, right_bottom;
      left_top.x = area.left;
      left_top.y = area.top;
      right_bottom.x = area.right;
      right_bottom.y = area.bottom;
      rectangle(matImg, left_top, right_bottom, CV_RGB(0,255,0), 2, 8, 0);
      cv::imshow("face area", matImg);
      cvWaitKey(0);
      cvDestroyWindow("face area");

     }
     //why?????????????
     //cvReleaseImage(&IplOrigImage);
     return nRetCode;
    }

};

#endif // FACE_RECOGNITION_H
